"""
Collection of common utility functions useful to software projects needing
scripting for testing. The WorkflowCommon class is intended to be inherited
from and customized on a per-project basis. Use execute_jobs() to manage
subprocesses to get progress bars and curses display logic for free!
"""

import argparse
import curses, textwrap
import pdb, sys, datetime, time, socket, stat
import subprocess, signal, logging
import os, re, collections
import multiprocessing, platform
from contextlib import contextmanager
from ColorStr import ColorStr
from pathlib import Path

# Create a global color printer
printer = ColorStr()

# UTILITY FUNCTIONS
def run_subprocess(command, m_shell=False, m_cwd=None, m_stdout=subprocess.PIPE, m_stderr=subprocess.PIPE):
    """
    Utility method for running a subprocess and returning stdout, stderr, and return code

    >>> result = run_subprocess(command='echo hi', m_shell=subprocess.PIPE, m_stdout=subprocess.PIPE)
    >>> result.code
    0
    >>> result.stdout  #doctest: +ELLIPSIS
    'hi...

    Parameters
    ----------
    command : str
        command to be run in subprocess instance
    m_shell : bool
        passthrough to subprocess shell parameter
    m_cwd : bool
        passthrough to subprocess cwd parameter
    m_stdout : bool
        passthrough to subprocess stdout parameter
    m_stderr : bool
        passthrough to subprocess stderr parameter

    Returns
    -------
    namedtuple
        Collection of process's final exit code, stdout, and stderr
    """
    result = collections.namedtuple("Process", ["code", "stdout", "stderr"])
    p = subprocess.Popen(command, shell=m_shell, cwd=m_cwd, stdout=m_stdout, stderr=m_stderr)
    result.stdout, result.stderr = p.communicate()
    if (result.stdout != None):
        result.stdout = result.stdout.decode(errors='ignore')
    if (result.stderr != None):
        result.stderr = result.stderr.decode(errors='ignore')
    result.code = p.returncode
    return result

def validate_output_file(filename):
    """
    Utility method for validating a file intended to contain some TBD content
    can be created and the subdirectories to that path are created if they
    don't already exist.

    >>> filename = validate_output_file('/tmp/aoeifmeganrsdfalk/b/c/d/foo.txt')
    >>> filename
    '/tmp/aoeifmeganrsdfalk/b/c/d/foo.txt'

    Parameters
    ----------
    filename : str
        Path to file or directory to validate

    Returns
    -------
    str
        Full path to given file or dir
    """
    filename = os.path.abspath(filename)
    os.makedirs(os.path.dirname(filename), exist_ok=True)
    Path(filename).touch(exist_ok=True)
    return (filename)

def tprint(line, color='ENDC', verbosity='INFO'):
    """
    Utility method for writing text to both the log and stdout, with optional
    color for stdout

    Parameters
    ----------
    line : str
        Line of string to both print and log
    color : str
        Color/style of text. Options are listed in Colorstr() class.
    """
    colorLine = printer.colorstr(line, color)
    sys.stdout.write(colorLine+"\n")
    if verbosity == 'INFO':
        logging.info(line)
    elif verbosity == 'DEBUG':
        logging.debug(line)
    elif verbosity == 'ERROR':
        logging.error(line)
    elif verbosity == 'WARNING':
        logging.warning(line)
    else:
        pass

def create_progress_bar(fraction, text):
    """
    Utility method that creates a text-based progress bar

    >>> bar = create_progress_bar(fraction=0.5, text='hello')
    >>> bar
    '[=================================== hello                                     ]'

    Parameters
    ----------
    text : str
        Text string to embed inside progress bar
    fraction : float
        Fraction of 1.0 representing how far progress bar is

    Returns
    -------
    str
        A text-based progress bar string of length 80
    """
    text = ' ' + text + ' '
    length = len(text)
    bar = list('[{0:<78}]'.format('=' * min(78, int(round(fraction * 78)))))
    index = int((len(bar) - length) / 2)
    bar[index : index + length] = text
    return ''.join(bar)

def sanitize_cpus(num_cpus, num_tasks, fallback_cpus):
    """
    Bounds num_cpus such that it is:
    - no less than 1
    - no greater than the lesser of:
      - num_tasks
      - the number of logical CPUs (or fallback_cpus if that could
        not be determined)

    Parameters
    ----------
    num_cpus : int
        the value to be sanitized
    num_tasks : int
        the number of runs
    fallback_cpus : int
        the upper bound to use when the number of logical CPUs cannot
        be determined

    Returns
    -------
    (int, str)
        a tuple of:
        - the sanitized value
        - a description of the boundary violated (may be None)
    """
    if num_cpus < 1:
        return (1, 'minimum allowable value')
    else:
        try:
            maximum = multiprocessing.cpu_count()
            source = 'number of logical CPUs'
        except:
            maximum = 16
            source = 'maximum allowed value'

        if num_tasks < maximum:
            maximum = num_tasks
            source = 'number of runs'

        if num_cpus > maximum:
            return (maximum, source)

    return (num_cpus, None)

def unixify_string(string):
    """
    Sanitizes a string making it nice for unix by processing special characters.

    Removes: ()!? and spaces,
    Replaces with underscores: '/' and ' '

    Parameters
    ----------
    string : str
        the string to sanitize

    Returns
    -------
    str
        the sanitized string
    """
    return re.sub("['/]", '_', re.sub('[()!?,]', '', string)).replace(' ', '_')

# CLASSES
class Job(object):
    """
    Manages a given command intended to be run as a subprocess and provides methods
    for getting status information. More specific types of Jobs should inherit from
    this base clasee.
    """
    enums = ['NOT_STARTED', 'RUNNING', 'SUCCESS', 'FAILED']
    Status = collections.namedtuple('Status', enums)(*(range(len(enums))))

    _success_progress_bar = create_progress_bar(1, 'Success')
    _failed_progress_bar = create_progress_bar(1, 'Failed')

    def _translate_status(self):
        """
        Utility function that accepts a Job.Status, colors and formats it with exit
        codes if FAIL.
        """
        text, color = {
            Job.Status.NOT_STARTED: ('NOT RUN', 'DARK_YELLOW'),
            Job.Status.SUCCESS: ('OK', 'DARK_GREEN'),
            Job.Status.FAILED: ('FAIL:' + str(self._expected_exit_status) + '/' +
             str(self._exit_status), 'DARK_RED') }[self.get_status()]
        return printer.colorstr(text, color)

    def __init__(self, name, command, log_file, expected_exit_status=0):
        """
        Initialize this instance.

        Parameters
        ----------
        name : str
            The name of this job.
        command : str
            The command to execute when start() is called.
        log_file : str
            The file to which to write log information.
        """
        self.name = name
        self._command = command
        self.log_file = log_file
        self._log_file = None
        self._process = None
        self._start_time = None
        self._stop_time = None
        self._exit_status = None
        self._expected_exit_status = expected_exit_status

    def start(self):
        """
        Start this job.
        """
        logging.debug('Executing command: ' + self._command)
        self._start_time = time.time()
        self._log_file = open(self.log_file, 'w')
        self._process = subprocess.Popen(
          self._command, stdout=self._log_file, stderr=self._log_file,
          stdin=open(os.devnull, 'r'), shell=True, preexec_fn=os.setsid,
          close_fds=True)

    def get_status(self):
        """
        Get the current Job status.

        Returns
        -------
        Status
            Status.NOT_STARTED
                This job has yet to be started.
            Status.RUNNING
                This job is running.
            Status.SUCCESS
                This job completed with an exit status of zero.
            Status.FAILED
                This job completed with a non-zero exit status.
        """
        if self._process is None:
            return self.Status.NOT_STARTED

        self._exit_status = self._process.poll()
        if self._exit_status is None:
            return self.Status.RUNNING

        if self._stop_time is None:
            self._stop_time = time.time()

        return self.Status.SUCCESS if self._exit_status is self._expected_exit_status else self.Status.FAILED

    def get_expected_exit_status(self):
        return self._expected_exit_status

    def get_exit_status(self):
        return self._exit_status

    def get_status_string_line_count(self):
        """
        Get the constant number of lines in the status string.

        Subclasses should override this if they add lines to the status string.

        Returns
        -------
            The number of lines in the status string.
        """
        return 1

    def get_status_string(self):
        """
        Get a string containing status information.

        -------
        str
            Details of this job's progress.
        """
        status = self.get_status()
        if status is self.Status.NOT_STARTED:
            return self._not_started_string()
        if status is self.Status.RUNNING:
            return self._running_string()
        if status is self.Status.SUCCESS:
            return self._success_string()
        return self._failed_string()

    def _not_started_string(self):
        """
        Get a string to display before this Job has been started.

        Returns
        -------
        str
            A string to be displayed when in the NOT_STARTED state.
        """
        return 'Not started yet'

    def _running_string(self):
        """
        Get a string to display while this Job is running.

        Returns
        -------
        str
            A string to be displayed when in the RUNNING state.
        """
        return 'Elapsed Time: {0:7.1f} sec'.format(
          time.time() - self._start_time)

    def _success_string(self):
        """
        Get a string to display when this Job has finished successfully.

        Returns
        -------
        str
            A string to be displayed when in the SUCCESS state.
        """
        return self._done_string()

    def _failed_string(self):
        """
        Get a string to display when this Job has failed.

        Returns
        -------
        str
            A string to be displayed when in the FAILED state.
        """
        return self._done_string()

    def _done_string(self):
        """
        This class uses the same string for SUCCESS and FAILED, but
        subclasses may differentiate.

        Returns
        -------
        str
            A string to be displayed when this Job is done.
        """
        return 'Total Time:   {0:7.1f} sec'.format(
          self._stop_time - self._start_time)


    def report( self, indent='', width=100):
        """
        Return a colored report string for this job.

        Returns
        -------
        str
            A olored report string
        """
        if self.get_status() ==  Job.Status.NOT_STARTED:
           prepend = printer.colorstr('NOT RUN     ', 'DARK_YELLOW')
        elif self.get_expected_exit_status() ==  self.get_exit_status():
           prepend = printer.colorstr('OK          ', 'DARK_GREEN')
        else:
           prepend = printer.colorstr('FAIL:%-10s ' %
               (str(self.get_exit_status())+'/'+str(self.get_expected_exit_status())) , 'DARK_RED')
        reportStr = indent + prepend + ("  %-s" % ( textwrap.shorten(self.name, width=width-10)))
        return reportStr


    def die(self):
        """
        Immediately kill this Job and all processes in its process group
        by sending them the SIGKILL signal.
        """
        try:
            os.killpg(os.getpgid(self._process.pid), signal.SIGKILL)
            self._process.wait()
        except:
            pass

class FileSizeJob(Job):
    """
    A build job is a subclass of Job() which estimates its progress via file size.
    """
    def __init__(self, name, command, log_file, size):
        """
        Initialize this instance.

        Parameters
        ----------
        name : str
            The name of this job.
        command : str
            The command to execute when start() is called.
        log_file : str
            The file to which to write log information.
        size : int
            The expected number size in bytes of the log_file when the
            build is complete. Used to estimate progress.
        """
        super(FileSizeJob, self).__init__(name, command, log_file)
        self.size = size

    def get_status_string_line_count(self):
        return super(FileSizeJob, self).get_status_string_line_count() + 1

    def _not_started_string(self):
        return super(FileSizeJob, self)._not_started_string() + '\n'

    def _running_string(self):
        progress = os.path.getsize(self._log_file.name) / float(self.size)
        return (super(FileSizeJob, self)._running_string() + '\n' +
          create_progress_bar(progress, '{0:.1f}%'.format(100 * progress)))

    def _success_string(self):
        return (super(FileSizeJob, self)._success_string() + '\n' +
          self._success_progress_bar)

    def _failed_string(self):
        return (super(FileSizeJob, self)._failed_string() + '\n' +
          self._failed_progress_bar)


class WorkflowCommon:
    """
    Base class for a typical software workflow
    Capabilies this base class provides:
      * Storage of global environment associated with this workflow
      * Execute particular tests (Jobs) in parallel and collect their exit codes
      * Provide log directory where output for jobs can be written
      * Get installed packages, host, and other OS information
    """
    def __init__( self, project_top_level, log_dir='/tmp/', log_level=logging.DEBUG, env='', quiet=False ):
        """
        Initialize this instance.

        Parameters
        ----------
        project_top_level : str
            Path to the top level of this project a.k.a root directory
        log_dir : str
            Path to directory where all test log output will be written
        log_file : logging ENUM
            Logging level to use for logging.basicConfig
        env : str
            Literal string representing what should be run before all tests to provide
            the project specific environment. Typically "source .bashrc;" or similar
        quiet : bool
            Flag for keeping verbose output to a minimum. Suppresses all progress bars
            when true which is useful for running in a CI system where stdin isn't
            available
        """
        self.project_top_level = project_top_level
        self.log_dir = log_dir  # Where all logged output will go
        # self.log is where all logging for python script layer goes
        self.log = validate_output_file( os.path.join(self.log_dir,'log.' +
          unixify_string(str(datetime.datetime.now()).replace(':','-')) + '.txt'))
        self.env = env  # Project environment literal string, e.g. "source bashrc"
        self.creation_time = time.time()  # When this instance was created
        self.host_name = socket.gethostname()
        self.platform = sys.platform
        self.this_os = None  # Intended to be retreived on on demand
        # Suppress all normal printing, excluding errors/warnings of course
        if quiet == None or quiet == False:
            self.quiet = False
        else:
            self.quiet = True
        logging.basicConfig(filename=self.log, level=log_level)
        os.chdir(self.project_top_level) # Automatically chdir to top of project

    def _cleanup(self):
        """
        Cleanup action to be used if exception is raised

        Removes the logging file.
        """
        os.remove(self.log)

    def get_installed_packages(self, verbose=False):
        """
        Utility function that returns a list of installed packages queried from the host
        or None if packages cannot be determined

        Parameters
        ----------
        verbose : bool
            Flag for printing verbose output if packages can't be found

        Returns
        -------
        list or None
            A list of installed packages, or None if they cannot be determined
        """
        self.this_os = self.get_operating_system()
        if self.this_os == 'debian':
            list_cmd = "dpkg-query -W -f='${binary:Package}\n'"
        elif self.this_os == 'redhat':
            list_cmd = "rpm -qa "
        else:
            return None

        result = run_subprocess(command=list_cmd, m_shell=True,
                                m_stdout=subprocess.PIPE, m_stderr=subprocess.PIPE)
        if result.code != 0:
            if verbose:
                tprint("ERROR: Trouble finding platform packages, error:\n" + result.stderr, 'DARK_RED')
            return None
        return(result.stdout.strip().split("\n"))

    def get_operating_system(self):
        """
        Get a lowercase string representing the operating system of this machine

        Returns
        -------
        str
            Returns one of these strings: debian, redhat, windows, darwin, unknown
        """
        # Windows is easy
        if platform.system().lower() == 'windows':
            return 'windows'
        # Darwin (Mac OSX) is easy
        if platform.system().lower() == 'darwin':
            return 'darwin'
        # Linux is hard
        # NOTE: sys.platform will not distinguish between flavors of linux and platform.linux_distribution()
        #   doesn't exist on all linux distributions! Also we can't rely on 'import distro' because it's
        #   only pip-installable at the moment. -Jordan 2/2021
        # Determine platform the old fashioned way, by seeing what package manager exists
        rpm_exist = not run_subprocess(command='which rpm', m_shell=True,
            m_stdout=subprocess.PIPE, m_stderr=subprocess.PIPE).code
        apt_exist = not run_subprocess(command='which apt', m_shell=True,
            m_stdout=subprocess.PIPE, m_stderr=subprocess.PIPE).code
        if rpm_exist:
            return 'redhat'
        elif apt_exist:
            return 'debian'
        else:
            return 'unknown'

    def execute_jobs(self, jobs, max_concurrent=None, header=None):
        """
        Run jobs, blocking until all have returned.

        Parameters
        ----------
        jobs : iterable of Job
            The jobs to run.
        max_concurrent : int
            The maximum number of jobs to execute simultaneously.
        header : str
            Header text.

        Returns
        -------
        bool
            True if any job failed or was not run.
            False if all jobs completed successfully.
        """
        if not os.environ.get('TERM') and not self.quiet:
            tprint(
              'The TERM environment variable must be set when the command\n'
              'line option --quiet is not used. This is usually set by one\n'
              "of the shell's configuration files (.profile, .cshrc, etc).\n"
              'However, if this was executed via a non-interactive,\n'
              "non-login shell (for instance: ssh <machine> '<command>'), it\n"
              'may not be automatically set.', 'DARK_RED')
            return True

        num_jobs = len(jobs)
        if max_concurrent is None or max_concurrent < 1:
            max_concurrent = num_jobs

        if header:
            header += '\n'
        else:
            header = ''

        header += (
          'Executing {0} total jobs, running up to {1} simultaneously.\n'
          .format(num_jobs, max_concurrent) +
          'Press CTRL+C to terminate early.\n')

        logging.info(header)

        # Define the meat of this function in an inner function.
        # This inner function will be called via curses.wrapper if
        # status output is enabled. Otherwise, it will be called
        # directly. See below.
        def execute(stdscr=None):

            # stdscr is passed via curses.wrapper
            if stdscr:
                # Turn off the cursor. Not all terminals may support
                # this.
                try:
                    curses.curs_set(False)
                except curses.error:
                    pass

                # Configure colors. Not all terminals may support
                # this.
                try:
                    curses.start_color()
                    curses.use_default_colors()
                    curses.init_pair(1, curses.COLOR_RED, -1)
                    curses.init_pair(2, curses.COLOR_GREEN, -1)
                    use_colors = True
                except curses.error:
                    use_colors = False

                # Cause getch to be non-blocking. The arrow keys and
                # mouse wheel are used to scroll the pad. We don't
                # want to hang if the user doesn't type anything.
                stdscr.timeout(0)

                # Nothing will be displayed without an initial call
                # to refresh.
                stdscr.refresh()

                # Create a pad for the header. It must have enough
                # lines to contain all the content we intend to
                # write. Text longer than the width wraps, consuming
                # extra lines, so pick a realy big width that isn't
                # likely to cause wrapping. We also need a final
                # additional line for the cursor to end on.
                header_pad = curses.newpad(header.count('\n') + 1, 1000)
                header_pad.addstr(header)

                # Create a pad for the status.
                # The total line count is:
                #   all job status strings
                # + a line for each job name
                # + a blank line after each status string
                # + a final line for the cursor to end on
                status_pad = curses.newpad(
                  sum(job.get_status_string_line_count() for job in jobs) +
                    2 * len(jobs) + 1,
                  1000)

                # The top visible status pad line.
                # Used for scrolling.
                top_line = 0
                header_height = header_pad.getmaxyx()[0]
                status_height = status_pad.getmaxyx()[0]

            while any(job.get_status() in
              [job.Status.NOT_STARTED, job.Status.RUNNING]
              for job in jobs):

                # Start waiting jobs if cpus are available
                waitingJobs = [job for job in jobs
                  if job.get_status() is job.Status.NOT_STARTED]

                if waitingJobs:
                    available_cpus = max_concurrent - sum(1 for job in jobs
                      if job.get_status() is job.Status.RUNNING)

                    for i in range(min(len(waitingJobs), available_cpus)):
                        waitingJobs[i].start()

                # display the status if enabled
                if stdscr:
                    status_pad.erase()
                    for i, job in enumerate(jobs):
                        # print the name
                        status_pad.addstr('Job {0:{width}d}/{1}: '.format(
                          i + 1, num_jobs, width=len(str(num_jobs))))
                        status_pad.addstr(job.name + '\n', curses.A_BOLD)

                        # print the status string
                        if use_colors:
                            # color the status string
                            status = job.get_status()
                            if status is job.Status.FAILED:
                                color = curses.color_pair(1)
                            elif status is job.Status.SUCCESS:
                                color = curses.color_pair(2)
                            else:
                                color = curses.color_pair(0)
                            status_pad.addstr(
                              job.get_status_string() + '\n\n', color)
                        else:
                            status_pad.addstr(
                              job.get_status_string() + '\n\n')

                    # handle scrolling
                    while True:
                        key = stdscr.getch()
                        if key == -1:
                            # no input
                            break
                        if key == curses.KEY_UP:
                            top_line -= 1
                        elif key == curses.KEY_DOWN:
                            top_line += 1

                    # prevent scrolling beyond the bounds of status_pad
                    screen_height, screen_width = stdscr.getmaxyx()
                    top_line = max(
                      0,
                      min(top_line,
                          status_height - 2 - (screen_height - header_height)))

                    # Resizing the terminal can cause the actual
                    # screen width or height to become smaller than
                    # what we already got from getmaxyx, resulting
                    # in a curses.error in these calls. Note that
                    # even calling getmaxyx again right here isn't
                    # fool-proof. Resizing is asynchronous (curses
                    # responds to it via a signal handler), so the
                    # size can always change between when we get it
                    # and when we use it. Best to just use what we
                    # have and ignore errors.
                    try:
                        header_pad.noutrefresh(
                          0, 0, 0, 0, screen_height - 1, screen_width - 1)
                        status_pad.noutrefresh(
                          top_line, 0, header_height, 0,
                          screen_height - 1, screen_width - 1)
                    except curses.error:
                        pass
                    curses.doupdate()

                # take a nap
                time.sleep(0.1)
            # When done clear everything, without this subsequent calls
            # to execute_jobs can show previous status bars if the number
            # of jobs is less on the subsequent executions
            if not self.quiet:
                stdscr.clear()

        try:
            if not self.quiet:
                # wrapper takes care of initializing the terminal and
                # restores it to a useable state regardless of how
                # execute exits (even via exception)
                curses.wrapper(execute)
            else:
                # not using curses, just call execute
                execute()

        except BaseException as exception:
            logging.exception('')
            tprint(
              'An exception occurred. See the log for details.\n\n'
              '    ' + repr(exception) + "\n\n"
              'Terminating all jobs. Please wait for cleanup to finish. '
              'CTRL+C may leave orphaned processes.', 'DARK_RED',
              'ERROR')

            # kill all the jobs
            for job in jobs:
                job.die()
            tprint('All jobs terminated.\n', 'DARK_RED')

        # print summary
        summary = 'Job Summary\n'
        for i, job in enumerate(jobs):
            summary += 'Job {0:{width}d}/{1}: {2}\n{3}\n'.format(
              i + 1, num_jobs, job.name, job.get_status_string(),
              width=len(str(num_jobs)))

        logging.info(summary)

        for job in jobs:
            text, color = {
                job.Status.NOT_STARTED: ('was not run', 'GREY40'),
                job.Status.SUCCESS: ('succeeded', 'DARK_GREEN'),
                job.Status.FAILED: ('failed', 'DARK_RED')
            }[job.get_status()]

            text = job.name + ' ' + text

            # Print the summary status even if self.quiet is True
            tprint(text, color)

        return any(job.get_status() is not job.Status.SUCCESS for job in jobs)
