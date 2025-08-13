"""
Collection of Trick-based utility classes. These classes inherit from generic
functionality provided by WorkflowCommon Provides a consistent framework for
testing multiple sims/runs/analysis/comparisons

Requires: python3 and requirements.txt containing:
  PyYAML         # For reading input yml files
  psutil         # For child process acquisition
"""
import os, sys, threading, socket, abc, time, re, copy, subprocess, hashlib, inspect
from TrickWorkflowYamlVerifier import *  # TODO revisit this import - Jordan

from WorkflowCommon import *
import pprint
# Import Trick natively supported python variable server utilities
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(
  os.path.abspath(inspect.getsourcefile(lambda:0))), '../pymods')))
from trick import variable_server
import pdb

# This global is the result of hours of frustration and debugging. This is only used by doctest
# but appears to be the only solution to the problem of __file__ not being an absolute path in
# some cases for some python versions and how that interacts with this class's os.chdir() when its
# base class initializes. If you attempt to define this_trick locally in the doctest block,
# which was my original attempt, you will find that the value of this_trick is different inside
# the __init__ doctest evaluation compared to any other member function. I believe this is only
# the case when using python version < 3.9, according to the information found here:
#   https://note.nkmk.me/en/python-script-file-path/
# I do not like adding globals to "production code" just to facilitate a testing mechanism, but
# I don't know of any cleaner way way to do this. AND ANOTHER THING! doctest examples are not
# given for any function that prints, because we make use of color printing extensively in this
# module via the tprint function, and doctest is not well suited for handling color output.
#    - Sincerely, a quite ornery Dan Jordan 4/2021
this_trick = os.path.normpath(os.path.join(os.path.dirname(os.path.realpath(__file__)), '../../..'))

class TrickWorkflow(WorkflowCommon):
    """
    A Trick Workflow requires a path to your project top-level, path to your
    trick directory and a path to a valid YAML file as input. The yaml file defines
    the sim(s), run(s), valgrind run(s), comparison(s), etc. that are part of the
    project workflow of interest.  A project using Trick should define their own
    <project>Workflow class which inherits from this base class, where all
    project-specific (but not Trick specific) content lives.

    Once TrickWorkflow.__init__() is called, an internal list of Sim() container
    instances are created from the content of the yml file, that can then be
    leveraged as your project sees fit. Some examples:

      # Get lists of jobs created from the content in the YAML file
      builds    = self.get_jobs(kind='build')
      runs      = self.get_jobs(kind='run')
      analysis  = self.get_jobs(kind='analysis')
      valgrind  = self.get_jobs(kind='valgrind')

      # Execute the build jobs, with a max of 3 building simultaneously
      # ret will be 0 if all jobs success, 1 if any fail
      ret = self.execute_jobs(builds, max_concurrent=3)

      # Job statuses are stored internally and can be queried after they've already
      # executed, for example, after execute_jobs() finishes:
      for b in builds:
        print("Build job %s completed with status %d " % (b.name, b.get_status()))
        print("and ran command %s " % (b._command))

      # All jobs store their status internally regardless of whether they have
      # been executed already or not. Jobs that have not yet run have a status
      # of Job.Status.NOT_STARTED. See WorkflowCommon.py for all statuses.
      # Here's an example where we print all states for all jobs:
      for j in (builds + runs + analysis + valgrind):
        print("Job with command '%s' has status %d " % (j._command, j.get_status()))

      # Verbose reporting of container classes is built-in, for example:
      self.report()  # Report all sims, runs, comparisons, etc. recursively
      # Get a sim's report by it's name from the YAML file
      self.get_sim('mysim').report()
      # Note that runs, comparisons, and analysis management classes and/or jobs also
      # support the report() method
    """
    # These are static so Run and Sim classes can access them when needed
    # TODO: It would be nice to consolidate and read this from .yaml_requirements.txt
    # -Jordan 12/2022
    allowed_phase_range = {'min': -1000, 'max': 1000}
    all_possible_phases = range(allowed_phase_range['min'], allowed_phase_range['max']+1)

    def listify_phase(phase=None):
        """
        Given a phase of different potential types, return a list of integers within
        the allowed_phase_range.

        >>> TrickWorkflow.listify_phase(0)
        [0]
        >>> TrickWorkflow.listify_phase(range(0,5))
        [0, 1, 2, 3, 4]

        Parameters
        ----------
        phase : int, list, range, or None
            Phase or phases requested by user

        Returns
        -------
        list of integers which must be equal to or a subset of all_possible_phases

        Raises
        ------
        RuntimeError
            If input is not an int, list of ints, range, or None, or if given values
            are not within TrickWorkflow.all_possible_phases
        """
        # Handle type of phase given
        phases = []  # To be populated with all valid phases requested by user
        if (phase is None):
            phases = list(TrickWorkflow.all_possible_phases)
        elif isinstance(phase, int) and phase in TrickWorkflow.all_possible_phases:
            phases = [phase]
        elif ( (isinstance(phase, list) or isinstance(phase, range)) and
          all(isinstance(p, int) for p in phase) and
          all(p in TrickWorkflow.all_possible_phases for p in phase)):
            phases = list(phase)  # Cast to list to cover range case
        else:
            msg =("ERROR: Given phase %s in listify_given_phase() must be an int,"
                  " list of ints, or range() within the bounds [%s, %s]" % (phase,
                  TrickWorkflow.allowed_phase_range['min'],
                  TrickWorkflow.allowed_phase_range['max']))
            raise RuntimeError(msg)
        return(phases)

    def _find_range_string(string):
        """
        Given a string, return the range in square bracket notation if found
        This does not check for validity of the string, it only finds it.

        >>> TrickWorkflow._find_range_string('SET_hi/RUN_[000-999]/input.py')
        '[000-999]'

        Returns: str or None
           str: string of range notation found
           None: if not found

        Raises
        ------
        RuntimeError
           If more than one range string found
        """
        pattern = "\[\d+-\d+\]"
        if (len(re.findall(pattern, string))) > 1:
          msg = ("ERROR: [min-max] pattern found more than once in %s. Only one instance is"
            " supported." % (string))
          raise RuntimeError(msg)
        m = re.search(pattern, string)
        if m:
          return m.group(0)
        else:
          return None

    def __init__(self, project_top_level, log_dir, trick_dir, config_file, cpus=3, quiet=False):
        """
        Initialize this instance.

        >>> tw = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/', trick_dir=this_trick, config_file=os.path.join(this_trick,"share/trick/trickops/tests/trick_sims.yml"))

        Parameters
        ----------
        project_top_level : str
            Path to the top level of this project a.k.a root directory
        log_dir : str
            Path to directory where all test log output will be written
        trick_dir : str
            Path to root trick directory for this project
        config_file : str
            Path to YAML file describing this Trick Workflow
        cpus : int
            Maximum number of CPUs to use when running parallel jobs
        quiet : bool
            Flag for keeping verbose output to a minimum. Suppresses all progress bars
            when true which is useful for running in a CI system where stdin isn't
            available
        """
        super().__init__(project_top_level=project_top_level, log_dir=log_dir, quiet=quiet)
        self.config_errors =  []        # Contains errors in setup of management classes
        self.compare_errors = False     # True if comparison errors were found
        self.sims = []                  # list of Sim() instances, filled out from config file
        self.config_file = config_file  # path to yml config
        self.cpus = cpus                # Number of CPUs this workflow should use when running
        self.yaml_verifier = TrickWorkflowYamlVerifier(self.config_file)
        # If not found in the config file, these defaults are used
        self.trick_dir = trick_dir
        self.trick_host_cpu = self.get_trick_host_cpu()
        self.env = ''
        self.config = self.yaml_verifier.verify()
        self.parsing_errors = self.yaml_verifier.parsing_errors  # All errors found during parsing
        for e in self.parsing_errors:
            tprint(e, 'DARK_RED')
        self._populate_sims()

    def _populate_sims(self):
        """
        Given a verified self.config, populate the self.sims list of Sim() instances
        Note that all the checking of expected dict key/values has already taken place
        during yaml verification, so we can safely access all dict keys here.
        Furthermore, any sim or run without required key/values have already been purged
        from self.config so we can create all management classes at the sim and run
        levels without worry.
        """
        def cprint(msg, color):
            self.config_errors.append(msg)
            tprint(msg, color)
        self.env = self.config['globals']['env']
        all_sim_paths = [] # Keep a list of all paths for uniqueness check
        for s in self.config:
          if not str(s).startswith('SIM'):  # Ignore everything not starting with SIM
            continue
          if (not os.path.exists(os.path.join(self.project_top_level, self.config[s]['path'])) ):
            cprint("ERROR: %s's 'path' %s not found. Continuing but skipping this entire entry from %s."
                 % (s, self.config[s]['path'], self.config_file), 'DARK_RED')
            continue
          if self.config[s]['path'] in all_sim_paths:
            cprint("ERROR: %s's 'path' is not unique in %s. Continuing but ignoring this sim."
                % (s, self.config_file), 'DARK_RED')
            continue
          # Add the CPU format string (won't fail if there is no CPU placeholder)
          self.config[s]['binary'] = self.config[s]['binary'].format(cpu=self.trick_host_cpu)
          # Add the full path to the build command
          trick_CP=os.path.join(self.trick_dir, "bin/trick-CP")
          if self.config[s]['build_args']:
            trick_CP+=(' ' + self.config[s]['build_args'])
          thisSim = TrickWorkflow.Sim(name=s, sim_dir=self.config[s]['path'],
            description=self.config[s]['description'], labels=self.config[s]['labels'],
            prebuild_cmd=self.env, build_cmd=trick_CP,
            cpus=self.cpus, size=self.config[s]['size'], phase=self.config[s]['phase'],
            log_dir=self.log_dir)
          all_sim_paths.append(self.config[s]['path'])

          all_run_paths = []   # Keep a list of all paths for uniqueness check
          for r in self.config[s]['runs']:
            just_RUN = r.split()[0]                   # Drop arguments after RUN../...py
            just_RUN_dir = os.path.dirname(just_RUN)  # Drop path before     RUN../..py
            # TODO: This check will break generated runs, do we even need to check this at all? - Jordan 2022
            #if not os.path.exists(os.path.join(self.project_top_level, self.config[s]['path'], just_RUN)):
            #  cprint("ERROR: %s's 'run' path %s not found. Continuing but skipping this run "
            #    "from %s." % (s, just_RUN, self.config_file), 'DARK_RED')
            #  continue
            if just_RUN_dir in all_run_paths and self.config[s]['parallel_safety'] == 'strict':
              cprint("ERROR: %s's run directory %s is not unique in %s. With setting "
                     "parallel_safety: strict, you cannot have the same RUN directory listed "
                     "more than once per sim. Continuing but skipping this run." %
                     (s, r, self.config_file), 'DARK_RED')
              continue

            thisRun = TrickWorkflow.Run(sim_dir=self.config[s]['path'], input_file=r,
                          binary= self.config[s]['binary'], prerun_cmd=self.env,
                          returns=self.config[s]['runs'][r]['returns'],
                          valgrind_flags=self.config[s]['runs'][r]['valgrind'],
                          phase=self.config[s]['runs'][r]['phase'], log_dir=self.log_dir)

            # The check for list allows all other non-list types in the yaml file,
            # allowing groups to define their own comparison methodology
            if isinstance(self.config[s]['runs'][r]['compare'], list):
              for cmp in self.config[s]['runs'][r]['compare']:
                lhs, rhs = [ s.strip() for s in cmp.split(' vs.') ]
                thisRun.add_comparison(test_data=lhs, baseline_data=rhs)

            if self.config[s]['runs'][r]['analyze'] is not None:
              thisRun.add_analysis(cmd=self.config[s]['runs'][r]['analyze'])

            theseRuns = []
            try:
              theseRuns = thisRun.multiply()  # If [from-to] notation used, expand runs
            except RuntimeError as e:
              msg = ("ERROR: Unable to multiply run %s in sim %s. Ignoring this run. "
                  "Check for bad [min-max] syntax in run keys in %s and try again.\n  %s"
                  % (r, s, self.config_file, e) )
              cprint(msg, 'DARK_RED')

            for r in theseRuns:
              thisSim.add_run(r)      # Add Run/Runs to this Sim()

            all_run_paths.append(just_RUN_dir)  # Keep track of all runs to check parallel safety

          self.sims.append(thisSim)   # Add Sim() to internal list
        if len(self.sims) < 1:  # At minimum, one valid SIM structure must exist
            msg = ("ERROR: After validating config file, there is insufficient information to continue."
                " Check the syntax in config file %s and try again."
                % (self.config_file) )
            cprint(msg, 'DARK_RED')
            self._cleanup()
            raise RuntimeError(msg)

    def create_test_suite(self):
        """
        Create all Job()s from the internal self.sims structure

        Jobs are created "on-get". This function just gets everything TrickWorkflow
        knows about with respect to Jobs but doesn't execute anything.

        >>> tw = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/', trick_dir=this_trick, config_file=os.path.join(this_trick,"share/trick/trickops/tests/trick_sims.yml"))
        >>> tw.create_test_suite()

        """
        self.get_jobs(kind='build')
        self.get_jobs(kind='run')
        self.get_jobs(kind='analysis')
        self.get_jobs(kind='valgrind')

    def get_trick_host_cpu(self):
        """
        Get a string of the TRICK_HOST_CPU by running trick-gte

        >>> tw = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/', trick_dir=this_trick, config_file=os.path.join(this_trick,"share/trick/trickops/tests/trick_sims.yml"))
        >>> tw.get_trick_host_cpu() is not None
        True

        Returns
        -------
        str or None
            TRICK_HOST_CPU or None if it cannot be determined
        """
        gte_cmd = [os.path.join(self.trick_dir, "bin/trick-gte" ), "TRICK_HOST_CPU"]
        result = run_subprocess(gte_cmd, m_stdout=subprocess.PIPE, m_stderr=subprocess.PIPE)
        self.trick_host_cpu = result.stdout.strip()
        if self.trick_host_cpu == None or self.trick_host_cpu == '':
            tprint("ERROR: Unable to determine TRICK_HOST_CPU, you may encounter problems "
            "if you continue... ", 'DARK_RED')
            self.trick_host_cpu = None
        return self.trick_host_cpu

    def get_sim(self, identifier):
        """
        Get a Sim() instance by unique identifier. This identifier must
        be either the name (top level key) in self.config_file or the path
        parameter

        >>> tw = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/', trick_dir=this_trick, config_file=os.path.join(this_trick,"share/trick/trickops/tests/trick_sims.yml"))
        >>> sim = tw.get_sim('SIM_ball_L1')

        Parameters
        ----------
        identifier : str
            name or path to sim from top level of repo, as written in the YAML file

        Returns
        -------
        Sim() or None
            Instance of Sim() management class matching the path/name given or None
            if one cannot be found

        Raises
        ------
        TypeError
            If identifier is not a str
        """
        if type(identifier) != str:
            raise TypeError('get_sim() only accepts a Sim name or Sim path from project top level')
        for sim in self.sims:
            if sim.sim_dir == identifier or sim.name == identifier:
                return sim
        return None

    def get_sims(self, labels=None):
        """
        Get a list of Sim() instances by label or labels listed in self.config_file

        >>> tw = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/', trick_dir=this_trick, config_file=os.path.join(this_trick,"share/trick/trickops/tests/trick_sims.yml"))
        >>> sims = tw.get_sims(['unit_test'])

        Parameters
        ----------
        labels : str or list or None
            label or labels that each sim must have to be returned by this functionr.
            If None, return all sims

        Returns
        -------
        list
            List of Sim() instances matching the label(s) given or [] if none can be
            found

        Raises
        ------
        TypeError
            If labels is not a str or list
        """
        # If no labels given, just return the full list
        if not labels:
            return self.sims
        sims_found = []
        ls = []

        if type(labels) == str:
            ls = [labels]
        elif type(labels) == list:
            ls = [str(l) for l in labels]
        else:
            raise TypeError('get_sims() only accepts a label string or list of label strings')
        for sim in self.sims:
            if all(l in sim.labels for l in ls):
                sims_found.append(sim)
        return sims_found


    def report(self, indent=''):
        """
        Recursively report all internal information
        >>> tw = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/', trick_dir=this_trick, config_file=os.path.join(this_trick,"share/trick/trickops/tests/trick_sims.yml"))
        """
        [sim.report() for sim in self.sims]

    def status_summary(self):
        """
        Print a summary of all jobs executed, and return 'SUCCESS' if all were successful, 'FAILURE'
        if any job was not successful
        """
        all_builds      = self.get_jobs(kind='build')
        all_runs        = self.get_jobs(kind='run')
        all_analysis    = self.get_jobs(kind='analysis')
        all_comparisons = self.get_comparisons()
        all_valgrind    = self.get_jobs(kind='valgrind')

        executed_builds = [ build for build in all_builds if build.get_status() != Job.Status.NOT_STARTED ]
        executed_runs   = [ run for run in all_runs if run.get_status() != Job.Status.NOT_STARTED ]
        executed_analysis = [ a for a in all_analysis if a.get_status() != Job.Status.NOT_STARTED ]
        executed_comparisons  = [ c for c in all_comparisons if c.status  != Job.Status.NOT_STARTED ]
        executed_valgrind = [ v for v in all_valgrind if v.get_status() != Job.Status.NOT_STARTED ]

        ok_builds       = [ build for build in executed_builds  if build.get_status() == Job.Status.SUCCESS ]
        ok_runs         = [ run for run in executed_runs if run.get_status() == Job.Status.SUCCESS ]
        ok_analysis     = [ a for a in executed_analysis if a.get_status() == Job.Status.SUCCESS ]
        ok_comparisons  = [ c for c in executed_comparisons if c.status  == Job.Status.SUCCESS ]
        ok_valgrind     = [ v for v in executed_valgrind if v.get_status() == Job.Status.SUCCESS ]

        tprint( "SUMMARY:" )
        if executed_builds:
          tprint( "  {0} out of {1} builds succeeded".format(len(ok_builds),len(executed_builds)))
        if executed_runs:
          tprint( "  {0} out of {1} runs succeeded".format(len(ok_runs),len(executed_runs)))
        if executed_analysis:
          tprint( "  {0} out of {1} analyses succeeded".format(len(ok_analysis),len(executed_analysis)))
        if executed_comparisons:
          tprint( "  {0} out of {1} comparisons succeeded".format(len(ok_comparisons),len(executed_comparisons)))
        if executed_valgrind:
          tprint( "  {0} out of {1} valgrind runs succeeded".format(len(ok_valgrind),len(executed_valgrind)))

        if (executed_builds == ok_builds and executed_runs == ok_runs and executed_analysis == ok_analysis
            and executed_comparisons == ok_comparisons and executed_valgrind == ok_valgrind) :
          return 'SUCCESS'
        else:
          return 'FAILURE'

    def compare(self):
        """
        Execute sim.compare() on all sims in self.sims

        >>> tw = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/', trick_dir=this_trick, config_file=os.path.join(this_trick,"share/trick/trickops/tests/trick_sims.yml"))
        >>> tw.compare()
        True

        Returns
        -------
        bool
            True if any comparison failed. False if all were successful.
        """
        return any([sim.compare() for sim in self.sims])

    def get_jobs(self, kind, phase=None):
        """
        Return a list of Jobs() from the self.sims structure of the kind given

        >>> tw = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/', trick_dir=this_trick, config_file=os.path.join(this_trick,"share/trick/trickops/tests/trick_sims.yml"))
        >>> ret = tw.get_jobs(kind='build')
        >>> len(ret)
        56
        >>> ret = tw.get_jobs(kind='run')
        >>> ret = tw.get_jobs(kind='valgrind')
        >>> ret = tw.get_jobs(kind='analysis')

        Parameters
        ----------
        kind : str
            Kind of jobs to return from internal self.sims structure: 'build', 'run', 'valgrind', or 'analysis'
        phase : int, list of ints, or None
            Optional filter to return only jobs of phase or phases given. Analysis jobs
            inherit the phase of the Run() it is associated with

        Returns
        -------
        list
            List of jobs of given kind (and phase if specified)
        """
        # Transform given phase into a list
        phases = TrickWorkflow.listify_phase(phase)

        jobs = []
        if kind == 'build' or kind == 'builds':
            jobs = [ sim.get_build_job() for sim in self.sims if sim.phase in phases ]
        elif kind == 'run' or kind == 'runs':
          for sim in self.sims:
            jobs +=  sim.get_run_jobs(kind='normal', phase=phases)
        elif kind == 'valgrind' or kind == 'valgrinds':
          for sim in self.sims:
            jobs +=  sim.get_run_jobs(kind='valgrind', phase=phases)
        elif kind == 'analysis' or kind == 'analyses' or kind == 'analyze':
          for sim in self.sims:
            jobs +=  sim.get_analysis_jobs(phase=phases)
        else:
            raise TypeError('get_jobs() only accepts kinds: build, run, valgrind, analysis')
        # If these jobs are of type SingleRun and self.quiet is True, tell the jobs to
        # skip the variable server connection logic
        for job in jobs:
            if self.quiet and isinstance(job, SingleRun):
                job.set_use_var_server(False)
        return (jobs)

    def get_comparisons(self):
        """
        Return a list of all Comparison() instances from the self.sims structure

        >>> tw = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/', trick_dir=this_trick, config_file=os.path.join(this_trick,"share/trick/trickops/tests/trick_sims.yml"))
        >>> ret = tw.get_comparisons()
        >>> len(ret)
        1

        Returns
        -------
        list
            List of Comparison objects
        """
        return ([ c for sim in self.sims for run in sim.runs for c in run.comparisons ])

    def get_unique_comparison_dirs(self):
        """
        Loop over all comparisons for all sim runs and return a tuple of unique
        directory comparisons. This is a utility function since many plotting
        tools accept two run directories, not paths to individual log files.
        Note that if a directory doesn't exist, that element of the tuple is None
        see Comparison.get_dirnames()

        >>> tw = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/', trick_dir=this_trick, config_file=os.path.join(this_trick,"share/trick/trickops/tests/trick_sims.yml"))
        >>> ret = tw.get_unique_comparison_dirs()
        >>> len(ret)
        1

        Returns
        -------
        list
           List of (test_dir, baseline_dir) tuples where one or either element
           is None if that directory does not exist
        """
        all_cmp_dirnames = []  # list of tuples ( test_dir, baseline_dir )
        for sim in self.sims:
          for run in sim.runs:
            for cmp in run.comparisons:
              all_cmp_dirnames.append(cmp.get_dirnames())
        # Reduce full list to unique list
        return(list(set(all_cmp_dirnames)))

    def get_koviz_report_job(self, test_dir, baseline_dir, pres=None):
        """
        Given two RUN directories, generate a error/difference report koviz Job() instance
        which when executed will generate a PDF in self.log_dir

        >>> tw = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/', trick_dir=this_trick, config_file=os.path.join(this_trick,"share/trick/trickops/tests/trick_sims.yml"))
        >>> ret = tw.get_koviz_report_job('share/trick/trickops/tests/baselinedata/','share/trick/trickops/tests/testdata/')
        >>> type(ret[0]) is Job
        True
        >>> ret[1] is None
        True

        Parameters
        ----------
        test_dir : str
            path to directory with test logged data
        baseline_dir : str
            path to directory with baseline logged data
        pres : str or None
            passthrough option to koviz's -pres (presentation) option

        Returns
        -------
        Tuple
           (Job() instance for the run directories given or None if error encountered,
            error details or None if successful)
        """
        if os.system(self.env + ' which koviz > /dev/null 2>&1') != 0:
            msg = "ERROR: koviz is not found in PATH. Returning None in get_koviz_report_job()"
            tprint (msg, 'DARK_RED')
            return None, msg
        dirs = [test_dir, baseline_dir]
        for dir in dirs:
           if not os.path.exists(dir):
            msg = "ERROR: %s not found, Returning None in get_koviz_report_job()" % (dir)
            tprint(msg, 'DARK_RED')
            return None, msg
        cmd = (self.env + " koviz -platform offscreen -a")
        if pres:
            cmd+= (" -pres %s " % pres )
        cmd+= (" -pdf %s %s %s" % (os.path.join(self.log_dir,
            (unixify_string(test_dir)+'_vs_' + unixify_string(baseline_dir)) + '.pdf'),
            test_dir, baseline_dir) )
        name='koviz report %s vs. %s' % (test_dir, baseline_dir)
        return(Job(name=name, command=cmd, log_file=os.path.join(self.log_dir,"."+unixify_string(name)),
          expected_exit_status=0), None)

    def get_koviz_report_jobs(self):
        '''
        Loop over all runs for all sims and generate a koviz pdf report job for each unique
        run directory comparison found.

        >>> tw = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/', trick_dir=this_trick, config_file=os.path.join(this_trick,"share/trick/trickops/tests/trick_sims.yml"))
        >>> ret = tw.get_koviz_report_jobs()
        >>> type(ret[0]) is list
        True
        >>> type(ret[0][0]) is Job
        True
        >>> len(ret[1])
        0

        Returns
        -------
        Tuple
            List of 'koviz' commands executed, List of errors
        '''
        koviz_jobs = []
        koviz_errors = []
        all_cmp_dirnames = self.get_unique_comparison_dirs() # List of (test_dir, baseline_dir) tuples
        # Generate koviz reports for the given test_dir, baseline_dir pairs
        for (test_dir, baseline_dir) in all_cmp_dirnames:
          if test_dir and baseline_dir:  # Will be None if dir is empty
            job, error = self.get_koviz_report_job(test_dir, baseline_dir, pres='error')
            if job:
              koviz_jobs.append(job)
            if error:
              koviz_errors.append(error)
        return koviz_jobs, koviz_errors

    class Sim(object):
        """
        Management class for sim content read from yml config file. Each highest-level
        key in the dict read will become a single instance of this management class
        stored in the TrickWorkflow.sims list.
        """
        def __init__(self, name, sim_dir, description=None, labels=[], prebuild_cmd='',
                     build_cmd='trick-CP', cpus=3, size=2200000, phase=0, log_dir='/tmp'):
            """
            Initialize this instance.

            >>> s = TrickWorkflow.Sim(name='alloc', sim_dir=os.path.join(this_trick, 'test/SIM_alloc_test'))
            >>> s.name
            'alloc'

            Parameters
            ----------
            name : str
                Arbitrary name of this simulation
            description : str
                Arbitrary description of this sim
             prebuild_cmd : str
                Optional string to execute immediately before sim build, e.g. env sourcing
             build_cmd : str
                Literal string for build command, defaults to 'trick-CP'
             cpus : int
                Optional Number of CPUs to give via MAKEFLAGS for sim build
             size : int
                Optional estimated size of successful build output file, for progress bars
             phase : int
                Optional ordering phase, typically used to order sim builds when parallel
                execution is not robust
             log_dir: str
                Directory in which log files will be written
            """
            self.name = name               # Name of sim
            self.sim_dir = sim_dir         # Path to sim directory wrt to top level of project
            self.description = description # Description of sim
            self.labels = labels           # Options list of user-specified labels associated w/ this sim
            self.prebuild_cmd = prebuild_cmd # Optional string to execute in shell immediately before building
            self.build_cmd = build_cmd     # Build command for sim
            self.cpus = cpus               # Number of CPUs to use in build
            self.size = size               # Estimated size of successful build output in bytes
            self.phase = phase             # Phase associated with this sim build
            self.log_dir = log_dir         # Directory for which log file should be written
            self.build_job = None          # Contains Build Job instance
            self.runs = []                 # List of normal Run instances
            self.valgrind_runs = []        # List of valgrind Run instances
            self.printer = ColorStr()      # Color printer utility

        def get_build_job( self):
            """
            Create the FileSizeJob(Job) instance if not already created and return it for this Sim

            >>> s = TrickWorkflow.Sim(name='alloc', sim_dir=os.path.join(this_trick, 'test/SIM_alloc_test'))
            >>> s.get_build_job()  #doctest: +ELLIPSIS
            <WorkflowCommon.FileSizeJob ...

            Returns
            -------
            FileSizeJob()
                Instance of FileSizeJob() job for compiling this sim
            """

            if not self.build_job:
                name = 'Build ' + self.sim_dir
                self.build_job = FileSizeJob(name=name,
                    command=("%s cd %s && export MAKEFLAGS=-j%d && %s" %
                    (self.prebuild_cmd, self.sim_dir, self.cpus, self.build_cmd)),
                    log_file=os.path.join(self.log_dir, unixify_string(self.sim_dir)+'_build.txt'),
                    size=self.size )
            return (self.build_job)

        def get_run_jobs( self, kind='normal', phase=None):
            """
            Collect all SingleRun() instances and return them for all sims subject to the filtering
            paramters kind and phase

            >>> s = TrickWorkflow.Sim(name='alloc', sim_dir=os.path.join(this_trick, 'test/SIM_alloc_test'))
            >>> s.get_run_jobs()  # A sim has no runs by default
            []

            Parameters
            -------
            kind : str
                'normal' for normal runs, 'valgrind' for valgrind runs
            phase : int, list of ints, or None
                Get only jobs of the given phase integer or list of integers
                If None (default), return all jobs

            Returns
            -------
            list
                List of SingleRun() job instances associated with kind and phase
            """
            # Transform given phase into a list
            phases = TrickWorkflow.listify_phase(phase)
            all_jobs = []
            if (kind == 'valgrind'):
                all_jobs = [r.get_run_job() for r in self.valgrind_runs if r.phase in phases]
            else:
                all_jobs = [r.get_run_job() for r in self.runs if r.phase in phases]
            return (all_jobs)

        def get_analysis_jobs( self, phase=None):
            """
            Collect all Job() instances for all analysis across all sim runs and valgrind runs

            >>> s = TrickWorkflow.Sim(name='alloc', sim_dir=os.path.join(this_trick, 'test/SIM_alloc_test'))
            >>> s.get_analysis_jobs()  # A sim has no analysis jobs by default
            []

            Parameters
            -------
            phase : int, list of ints, or None
                Get only jobs of the given phase integer or list of integers
                If None (default), return all jobs

            Returns
            -------
            list()
                List of Job() instances for all analyses
            """
            # Transform given phase into a list
            phases = TrickWorkflow.listify_phase(phase)
            return ([r.analysis for r in (self.runs + self.valgrind_runs) if (r.analysis and r.phase in phases) ])

        def get_run(self, input_file):
            """
            Get a Run() instance by unique full input name. This is the full
            string intended to be passed to the binary.

            >>> s = TrickWorkflow.Sim(name='alloc', sim_dir=os.path.join(this_trick, 'test/SIM_alloc_test'))
            >>> s.get_run('RUN_none/input.py')  # This sim has no runs

            Parameters
            ----------
            input : str
                unique full input name

            Returns
            -------
            Run() or None
                Instance of Run() management class matching the input given or None
                if one cannot be found

            Raises
            ------
            TypeError
                If input is not a str
            """
            if type(input_file) != str:
                raise TypeError('get_run() only accepts the unique key representing the entire input to'
                  ' the sim binary. Ex: "RUN_test/input.py --flags-too"')
            for run in self.runs:
                if run.input_file == input_file:
                   return run
            return None

        def get_runs(self):
            """
            Get all Run() instances associated with this sim

            >>> s = TrickWorkflow.Sim(name='alloc', sim_dir=os.path.join(this_trick, 'test/SIM_alloc_test'))
            >>> s.get_runs()  # This sim has no runs
            []

            """
            return self.runs

        def get_valgrind_runs(self):
            """
            Get all Run() instances associated with this sim

            >>> s = TrickWorkflow.Sim(name='alloc', sim_dir=os.path.join(this_trick, 'test/SIM_alloc_test'))
            >>> s.get_valgrind_runs()  # This sim has no runs
            []

            """
            return self.valgrind_runs

        def get_phase(self):
            """
            Get integer phase member

            >>> s = TrickWorkflow.Sim(name='alloc', sim_dir=os.path.join(this_trick, 'test/SIM_alloc_test'))
            >>> s.get_phase()  # Default is zero
            0

            """
            return self.phase

        def set_phase( self, phase):
            """
            Set the phase member variable. Phase is an integer in the range(TrickWorkflow.allowed_phase_range['min'],
            TrickWorkflow.allowed_phase_range['max']) which can be used to order sim builds when a workflow cannot
            support sims building in parallel.

            >>> s = TrickWorkflow.Sim(name='alloc', sim_dir=os.path.join(this_trick, 'test/SIM_alloc_test'))
            >>> s.set_phase(1)
            >>> s.set_phase(-1001) #doctest: +ELLIPSIS
            Traceback (most recent call last):
               ...
            RuntimeError: ERROR: set_phase() for SIM alloc must be an integer between ...

            Parameters
            -------
            phase : int
                phase to change to
            """
            if (not isinstance(phase, int) or phase < TrickWorkflow.allowed_phase_range['min'] or
                phase > TrickWorkflow.allowed_phase_range['max']):
                msg =("ERROR: set_phase() for SIM %s must be an integer between [%s, %s]"
                      % (self.name, TrickWorkflow.allowed_phase_range['min'], TrickWorkflow.allowed_phase_range['max']))
                raise RuntimeError(msg)
            else:
                self.phase = phase

        def add_run( self, run):
            """
            Append a new Run() instance to the internal run lists. Appends to valgrind
            list if run.valgrind_flags is not None, appends to normal run list otherwise

            >>> s = TrickWorkflow.Sim(name='alloc', sim_dir=os.path.join(this_trick, 'test/SIM_alloc_test'))
            >>> r = TrickWorkflow.Run(sim_dir=os.path.join(this_trick, 'test/SIM_alloc_test'), input_file='RUN_test/input.py', binary='S_main_Linux_x86_64.exe')
            >>> s.add_run(r)

            Parameters
            -------
            run : Run()
                Instance to add
            """
            if (run.valgrind_flags):
                self.valgrind_runs.append(run)
            else:
                self.runs.append(run)

        def pop_run( self, input_file):
            """
            Remove a run by its unique self.input_file value

            >>> tw = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/', trick_dir=this_trick, config_file=os.path.join(this_trick,"share/trick/trickops/tests/trick_sims.yml"))
            >>> s = tw.get_sim('SIM_demo_inputfile')
            >>> len(s.runs)
            2
            >>> r = s.pop_run('RUN_test/input.py')
            >>> r.input_file
            'RUN_test/input.py'
            >>> len(s.runs)
            1

            Returns
            -------
            Run()
                Instance in this sim's runs list matching self.input_file
            """
            for i, run in enumerate(self.runs):
              if run.input_file == input_file:
                 return self.runs.pop(i)

        def compare( self):
            """
            Run compare() on all runs for this sim

            >>> s = TrickWorkflow.Sim(name='alloc', sim_dir=os.path.join(this_trick, 'test/SIM_alloc_test'))
            >>> s.compare()  # This sim has no comparisons, which means all comparisons succeed
            False

            Returns
            -------
            bool
                False if all comparisons succeed, True if any failed
            """
            return any([r.compare() for r in self.runs])

        def report(self, indent=''):
            """
            Report this sims information verbosely, ignoring members that are None

            Parameters
            -------
            indent : str
                prepend the report with this custom string

            """
            if self.name:
              tprint(indent + "Name:                 " + self.name)
            else:
              tprint(indent + self.sim_dir)
            if self.description:
              tprint(indent + "Description:          " + str(self.description))
            if self.labels:
              tprint(indent + "Labels:               " + str(', '.join(self.labels)))
            tprint(indent + "  Build: %-20s  %-60s  " %(self.build_job._translate_status()
              if self.build_job else printer.colorstr('NOT RUN', 'DARK_YELLOW'), self.sim_dir))
            if self.runs:
              tprint(indent + "  Runs:")
              for run in self.runs:
                  run.report(indent=indent+'  ')
            if self.valgrind_runs:
              tprint(indent + "  Valgrind Runs:")
              for run in self.valgrind_runs:
                  run.report(indent=indent+'  ')

    class Run(object):
        """
        Management class for run content read from yml config file. Each key in the
        runs: sub-dict will become a single instance of this management class
        """
        def __init__(self, sim_dir, input_file, binary, prerun_cmd = '', returns=0, valgrind_flags=None,
                     phase=0, log_dir='/tmp/'):
            """
            Initialize this instance.

            >>> r = TrickWorkflow.Run(sim_dir=os.path.join(this_trick, 'test/SIM_alloc_test'), input_file='RUN_test/input.py', binary='S_main_Linux_x86_64.exe')
            >>> r.input_file
            'RUN_test/input.py'

            Parameters
            ----------
            sim_dir : str
                path to sim directory from top level of repository for this run
            input_file : str
                literal argument to sim binary representing how this run is initiated
                including command-line arguments
                ex: RUN_test/input.py, or RUN_test/input.py --my-arg
             binary : str
                Name of executable, usually S_main.. but platform specific
             prerun_cmd : str
                Optional string to execute immediately before sim run, e.g. env sourcing
             valgrind_flags : str
                If not None, use these flag and valgrind for this run
             phase : int
                Optional ordering phase, typically used to order sim runs when parallel
                execution is not robust
             log_dir : str
                Directory in which log files will be written
            """
            self.sim_dir = sim_dir        # Path to sim directory wrt to top level of project for this run
            self.prerun_cmd = prerun_cmd  # Optional string to execute in shell immediately before running (env)
            self.input_file = input_file  # Full RUN.../input.py --any-flags --as-well, relative to sim_dir
            self.returns = returns        # Expected exit code on success for this run
            self.valgrind_flags = valgrind_flags  # If not None, this run is to be run in valgrind w/ these flags
            self.phase = phase            # Phase associated with this run
            self.log_dir = log_dir        # Dir where all logged output will go
            self.just_input = self.input_file.split(' ')[0]  # Strip flags if any
            # Derive Just the "RUN_something" part of run_dir_path
            self.just_run_dir = os.path.dirname(self.just_input)
            # Derive Path to run directory wrt to top level of project
            self.run_dir_path = os.path.join(self.sim_dir, self.just_run_dir)
            # Populated later
            self.binary = binary     # Name of binary
            self.run_job = None      # SingleRun Job instance for this run
            self.comparisons = []    # List of comparison objects associated with this run
            self.analysis = None     # Job instance of after-run-completes custom analysis

        def add_comparison(self, test_data, baseline_data):
            """
            Given two data directories, add a new Comparison() instance to self.comparisons list

            >>> r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_test/input.py', binary='S_main_Linux_x86_64.exe')
            >>> r.add_comparison('share/trick/trickops/tests/baselinedata/log_a.csv','share/trick/trickops/tests/testdata/log_a.csv')

            Parameters
            ----------
            test_dir : str
                path to file containing test logged data
            baseline_dir : str
                path to file containing baseline logged data
            """
            comparison = TrickWorkflow.Comparison( test_data, baseline_data );
            self.comparisons.append( comparison )

        def add_analysis(self, cmd):
            """
            Given an analysis command, add it as a post-run analysis for this run

            >>> r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_test/input.py', binary='S_main_Linux_x86_64.exe')
            >>> r.add_analysis('echo analysis goes here')

            Parameters
            ----------
            cmd : str
                literal string representing command to execute post-run
            """
            if self.analysis:
                tprint("WARNING: Overwriting analysis definition for %s's %s" % (self.sim_dir,
                  self.input_file), 'DARK_YELLOW')
            logfile = (os.path.join(self.log_dir, unixify_string(self.sim_dir))
                +'_'+ unixify_string(self.input_file) + '_analysis.txt')
            self.analysis =  Job(name=textwrap.shorten(cmd, width=90), command=self.prerun_cmd + " " +cmd,
              log_file=logfile, expected_exit_status=0)

        def get_phase(self):
            """
            Get integer phase member

            >>> r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_test/input.py', binary='S_main_Linux_x86_64.exe')
            >>> r.get_phase()  # Default is zero
            0

            """
            return self.phase

        def set_phase( self, phase):
            """
            Set the phase member variable. Phase is an integer in the range(TrickWorkflow.allowed_phase_range['min'],
            TrickWorkflow.allowed_phase_range['max']) which can be used to order sim runs when a workflow cannot
            support running sims in parallel.

            >>> r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_test/input.py', binary='S_main_Linux_x86_64.exe')
            >>> r.set_phase(1)
            >>> r.set_phase(-1001) #doctest: +ELLIPSIS
            Traceback (most recent call last):
               ...
            RuntimeError: ERROR: set_phase() for test/SIM_alloc_test/RUN_test/input.py must be an integer between ...

            Parameters
            -------
            phase : int
                phase to change to
            """
            # TODO: This is very similar to Sim.set_phase() in functionality but I didn't want to create
            # a base class just to support this single instance of reducing code duplication
            # -Jordan 12/2022
            if (not isinstance(phase, int) or phase < TrickWorkflow.allowed_phase_range['min'] or
                phase > TrickWorkflow.allowed_phase_range['max']):
                msg =("ERROR: set_phase() for %s/%s must be an integer between [%s, %s]"
                      % (self.sim_dir, self.input_file, TrickWorkflow.allowed_phase_range['min'],
                        TrickWorkflow.allowed_phase_range['max']))
                raise RuntimeError(msg)
            else:
                self.phase = phase

        def compare( self):
            """
            Execute all internal comparisons for this run

            >>> r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_test/input.py', binary='S_main_Linux_x86_64.exe')
            >>> r.compare() # No comparisons means success a.k.a 0
            False

            Returns
            -------
            bool
                False if all comparisons succeed, True if any failed
            """
            return any([c.compare() != Job.Status.SUCCESS for c in self.comparisons])

        def report(self, indent=''):
            """
            Report this run's information verbosely, ignoring members that are None

            Parameters
            -------
            indent : str
                prepend the report with this custom string

            """
            tprint(indent + "  %-20s       %s"  % (self.run_job._translate_status()
              if self.run_job else printer.colorstr('NOT RUN', 'DARK_YELLOW'),
              self.input_file))
            if self.comparisons:
              tprint(indent + "    Run Comparisons:")
              for comparison in self.comparisons:
                  comparison.report(indent=indent+'      ')
            if self.analysis:
              tprint(indent + "    Run Analysis:")
              tprint(indent + "      " + self.analysis.report())

        def get_run_job(self):
            """
            Create if necessary and Return the SingleRun() job instance

            >>> r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_test/input.py', binary='S_main_Linux_x86_64.exe')
            >>> j = r.get_run_job()

            Returns
            -------
            SingleRun()
                SingleRun() Job instance for this run
            """
            if not self.run_job:
                name = 'Run '

                cmd = "%s cd %s && " % (self.prerun_cmd, self.sim_dir)
                sim_name = os.path.basename(os.path.normpath(self.sim_dir))
                logfile = os.path.join(self.log_dir, unixify_string(self.sim_dir))

                if self.valgrind_flags:
                    cmd += ( "valgrind %s --log-file=%s " % (self.valgrind_flags,
                      (os.path.join(self.log_dir, sim_name) +'_valgrind_'
                      + unixify_string(self.input_file) + '.valgrind') ))
                    logfile += '_valgrind'
                    name += 'Valgrind '
                logfile += "_" + unixify_string(self.input_file) + '.txt'
                cmd += (" ./%s %s" % (self.binary, self.input_file))
                name += self.sim_dir + ' ' + self.input_file

                self.run_job = SingleRun(name=name, command=(cmd),
                    expected_exit_status=self.returns, log_file=logfile)
            return (self.run_job)

        def _get_range_list(self, pattern):
            """
            Given a string matching the following pattern:
              [<int1>-<int2>]     - One set of enclosed in square braces
              <int1> < <int2>     - First integer must be less than second integer
              <int1> & <int2> > 0 - Both integers must be positive
              [001-999]           - Leading zeros must be included and consistent
            Return a list(range(int1, int2+1)) with leading zeros maintained in
            string format

            >>> r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_[01-04]/input.py', binary='S_main_Linux_x86_64.exe')
            >>> rl =r._get_range_list('[0-4]')
            >>> len(rl)
            5

            Returns
            -------
            list()
                List of zero-padded integer range as strings
            Raises
            ------
            RuntimeError
               If pattern is unrecognized or contains errors
            """
            must_exist = ['[', ']', '-']
            if any([char not in pattern for char in must_exist]):
              msg = ("ERROR: Pattern %s doesn't match expected syntax of \"[min-max]\"" % (pattern))
              raise RuntimeError(msg)
            min, max = pattern.strip('[').strip(']').split('-')
            if len(min) != len(max):
              msg = ("ERROR: Pattern %s has inconsistent leading zeros." % (pattern))
              raise RuntimeError(msg)
            leading_zeros = int(len(min))
            try:
              min = int(min)
            except ValueError as e:
              msg = ("ERROR: Pattern %s minimum cannot be converted to integer. \n%s" % (pattern, e))
              raise RuntimeError(msg)
            try:
              max = int(max)
            except ValueError as e:
              msg = ("ERROR: Pattern %s maximum cannot be converted to integer. \n%s" % (pattern, e))
              raise RuntimeError(msg)
            if min >= (max):
              msg = ("ERROR: Pattern %s minimum must be less than maximum." % pattern)
              raise RuntimeError(msg)
            range_list = []
            for num in range(min, max+1):
              range_list.append(str(num).zfill(leading_zeros))
            return (list(range_list))


        def multiply(self):
            """
            If [from-to] notation used in input_file, return individual instances
            of Run() for the expanded set. Otherwise, return self. The pattern
            must meet the following criteria for input_file and optionally comparisons:
              [<int1>-<int2>]     - One set of enclosed in square braces
              <int1> < <int2>     - First integer must be less than second integer
              <int1> & <int2> > 0 - Both integers must be positive
              [001-999]           - Leading zeros must be included and consistent

            >>> r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_[01-04]/input.py', binary='S_main_Linux_x86_64.exe')
            >>> runs = r.multiply()
            >>> len(runs)
            4

            Returns
            -------
            list of Run()
                Resultant list of Run() instances after expansion
            Raises
            ------
            RuntimeError
               If expansion of pattern is not possible

            TODO: Note that this approach requires a 1:1 mapping between the [min-max] notation
            specified in the self.input_file and notation in that same run's comparisons. This
            approach does not support range expansion for an input_file with no range pattern
            but which contains a compare range pattern. If this is attempted by the user in
            yaml file, the literal [min-max] is used instead, resulting in (missing) errors.
            This use case could be supported by changing this method and potentially
            Comparison.pattern_replace() so that [min-max] is expanded in comparisons differently
            if self.input file does not contain the range notation. This is left unimplemented
            for now since not only does this add significant complexity but it also would mean
            two separate behaviors for the user with the same notation in yaml, which feels
            like bad mojo. -Jordan 1/2023
            """
            rs = TrickWorkflow._find_range_string(self.input_file)
            if rs is None:
              return [self]
            else:
              range_list = self._get_range_list(rs)
              multiplied_runs = []
              for i in range_list:
                # replace the range string notation with the single run equivalent
                acopy = copy.deepcopy(self)
                acopy.input_file = self.input_file.replace(rs, i)
                # Add comparisons back as multiplied set
                for c in acopy.comparisons:
                  c.pattern_replace(expecting_pattern=rs, replace_with=i)
                multiplied_runs.append(acopy)
              return multiplied_runs

    class Comparison(object):
        """
        Management class for a logged data comparison
        Params:
          test_data:     path to file that represents test data (data generated by a run)
          baseline_data: path to file that represents baseline data for a run
        """
        def __init__(self, test_data, baseline_data):
            """
            Initialize this instance. Environment variables used within the test data and baseline data paths will be expanded.

            >>> c = TrickWorkflow.Comparison('share/trick/trickops/tests/baselinedata/log_a.csv','share/trick/trickops/tests/testdata/log_a.csv')
            >>> c = TrickWorkflow.Comparison('share/trick/trickops/tests/baselinedata/log_a.csv','share/trick/trickops/tests/testdata/${TRICK_HOST_TYPE}/log_a.csv')

            Parameters
            ----------
            test_data : str
                Path to a single test logged data file
            baseline_data : str
                Path to a single baseline logged data file
            """
            self.test_data = os.path.expandvars(test_data)           # Test data file with respect to project top level
            self.baseline_data = os.path.expandvars(baseline_data)   # Baseline data file with respect to project top level
            self.status = Job.Status.NOT_STARTED # Status of comparison
            self.error = None                    # Error details if found
            self.missing = []                    # List of Strings with details of missing files if any

        def compare(self):
            """
            Execute an md5sum hash comparison of self.test_data vs. self.baseline_data.

            >>> c = TrickWorkflow.Comparison('share/trick/trickops/tests/baselinedata/log_a.csv','share/trick/trickops/tests/testdata/log_a.csv')
            >>> c.compare() == Job.Status.FAILED
            True

            Returns
            -------
            Job.Status ENUM
                status of the comparison: Job.Status.SUCCESS on success, Job.Status.FAILED
            """
            for hs in [self.test_data, self.baseline_data]:
              if not os.path.exists(hs)  :
                  self.missing.append(hs)
                  self.status = Job.Status.FAILED
            if self.missing:
                return  self.status
            td = hashlib.new('md5', usedforsecurity=False)
            bd = hashlib.new('md5', usedforsecurity=False)
            td.update(open(self.test_data,'rb').read())
            bd.update(open(self.baseline_data,'rb').read())
            if (td.hexdigest() != bd.hexdigest()):
                self.status = Job.Status.FAILED
            else:
                self.status = Job.Status.SUCCESS
            return  self.status

        def get_status(self):
            """
            Return the status member ENUM

            >>> c = TrickWorkflow.Comparison('share/trick/trickops/tests/baselinedata/log_a.csv','share/trick/trickops/tests/testdata/log_a.csv')
            >>> c.get_status() == Job.Status.NOT_STARTED
            True

            Returns
            -------
            Job.Status ENUM
                status of the comparison: Job.Status.SUCCESS on success, Job.Status.FAILED
                on failure, Job.Status.NOT_STARTED if compare() hasn't been run
            """
            return self.status

        def _translate_status(self):
            """
            Utility function that takes in a status and colors it, for easier reporting
            """
            text, color = {
                Job.Status.NOT_STARTED: ('NOT RUN', 'DARK_YELLOW'),
                Job.Status.SUCCESS: ('OK', 'DARK_GREEN'),
                Job.Status.FAILED: ('FAIL', 'DARK_RED') }[self.get_status()]
            return printer.colorstr(text, color)

        def get_dirnames(self):
            """
            Get a tuple of this comparison's test directory and baseline directory,
            not including the actual filename. If either directory does not exist,
            return None for that element of the tuple

            >>> os.chdir(this_trick)
            >>> c = TrickWorkflow.Comparison('share/trick/trickops/tests/baselinedata/log_a.csv','share/trick/trickops/tests/testdata/log_a.csv')
            >>> c.get_dirnames()
            ('share/trick/trickops/tests/baselinedata', 'share/trick/trickops/tests/testdata')

            Returns
            -------
            tuple
               ( <test_dir>, <baseline_dir> )
            """
            test_dirname = os.path.dirname(self.test_data)
            baseline_dirname = os.path.dirname(self.baseline_data)
            if not os.path.exists(test_dirname):
              test_dirname = None
            if not os.path.exists(baseline_dirname):
              baseline_dirname = None
            return (test_dirname, baseline_dirname)

        def report (self, indent=''):
            """
            Report this comparison's information verbosely

            Parameters
            -------
            indent : str
                prepend the report with this custom string

            """
            string = indent + "%-22s %s"  % (self._translate_status(), self.test_data)
            if self.test_data in self.missing:
              string += printer.colorstr(" (missing)", 'DARK_RED')
            string += " vs. %s"  % (self.baseline_data)
            if self.baseline_data in self.missing:
              string += printer.colorstr(" (missing)", 'DARK_RED')
            tprint(string)

        def pattern_replace(self, expecting_pattern, replace_with):
            """
            Replaces expecting_pattern value in self.test_data and self.baseline_data
            with replace_with value if found. Mostly intended to be used via
            Run.multiply()

            >>> c = TrickWorkflow.Comparison('testdata/RUN_[00-03]/log_a.csv','baseline/RUN_[00-03]/log_a.csv')
            >>> c.pattern_replace(expecting_pattern='[00-03]', replace_with='01')
            >>> c.test_data
            'testdata/RUN_01/log_a.csv'
            >>> c.baseline_data
            'baseline/RUN_01/log_a.csv'

            Raises
            ------
            RuntimeError
               If expected_pattern doesn't match found pattern in test_data or baseline_data
            """
            rsb = TrickWorkflow._find_range_string(self.baseline_data)
            rst = TrickWorkflow._find_range_string(self.test_data)
            if rsb is None and rst is None: # If no patterns, do nothing
              return
            if (rsb and rsb != expecting_pattern) or (rst and rst != expecting_pattern):
              msg = ("ERROR: [min-max] pattern from run (%s) must match pattern in run's"
                " comparisons test (%s) and baseline (%s) sections, if specified."
                % (expecting_pattern, rst, rsb))
              raise RuntimeError(msg)
            if rsb:
              self.baseline_data = self.baseline_data.replace(expecting_pattern, replace_with)
            if rst:
              self.test_data     = self.test_data.replace(expecting_pattern, replace_with)


class SingleRun(Job):
    """
    A single trick simulation run Job. SingleRun's can optionally connect to the
    trick variable server to get progress bar information.
    """
    def __init__(self, name, command, log_file, expected_exit_status=0, use_var_server=True):
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
        self._use_var_server = use_var_server
        self._connected = False
        super().__init__(name=name, command=command, log_file=log_file,
          expected_exit_status=expected_exit_status)

    def set_use_var_server(self, value):
        if (not isinstance(value, bool)):
            msg =("ERROR: SingleRun.set_use_var_server() Requires a True/False value")
            raise RuntimeError(msg)
        else:
            self._use_var_server = value

    def get_use_var_server(self):
        return (self._use_var_server)

    def start(self):
        """
        Start this Simulation job. Attempts a connection to the sim variable server
        in another thread after calling the base class Job() start() method
        """
        super(SingleRun, self).start()
        self._connected = False

        # Finding a sim via PID can take several seconds.
        # Do it on another thread so this method can return immediately.
        # Sims that complete especially quickly may terminate before we
        # can connect, so use a timeout and quit trying if the subprocess
        # has finished.
        def connect():
            import psutil
            while self.get_status() is self.Status.RUNNING:
                # The base-class start() call will have already populated self._process.pid,
                # but that pid may be incompatible with the upcoming find_simulation call
                # if the command spawns child processes (e.g. self._command =
                # 'cd somewhere && ./S_main..' will result in a child of self._process.pid
                # actually running S_main. So here we search self._process.pid's process tree
                # for the first process whose name matches the typical S_main naming pattern:
                # '.*S_main.. and give find_simulation() that PID, defaulting to
                # self._process.pid if not found
                sim_pid = self._process.pid
                try:
                    children = psutil.Process(self._process.pid).children(recursive=True)
                    outerbreak = False
                    for child in children:
                        for i in child.cmdline():
                            if re.search('.*S_main', i):
                                sim_pid = child.pid
                                outerbreak = True
                                break
                        if outerbreak:
                            break
                except Exception as e:
                    sim_pid = self._process.pid
                # Now connect to the sim_pid
                try:
                    self._variable_server = variable_server.find_simulation(
                      pid=sim_pid, timeout=5)
                    self._variable_server.add_variables(
                      *self._create_variables())
                    self._variable_server.set_period(0.1)
                    self._connected = True
                    return
                except socket.timeout:
                    pass
                # If a SingleRun job terminates before the thread can connect to the
                # variable server, Trick's variable_server module throws an IOError
                # with the message: The remote endpoint has closed the connection
                except IOError as e:
                    pass

        if self._use_var_server:
            thread = threading.Thread(target=connect, name='Looking for ' + self.name)
            thread.daemon = True
            thread.start()

    def get_status_string_line_count(self):
        return super(SingleRun, self).get_status_string_line_count() + 1

    def _not_started_string(self):
        return super(SingleRun, self)._not_started_string() + '\n'

    def _running_string(self):
        elapsed_time = super(SingleRun, self)._running_string()

        if self._connected:
            return (elapsed_time + self._connected_string() + '\n' +
              self._connected_bar())

        return (elapsed_time + '\n' +
          create_progress_bar(0, 'Connecting'))

    def _success_string(self):
        text = super(SingleRun, self)._success_string()
        if self._connected:
            text += self._connected_string()
        return text + '\n' + self._success_progress_bar

    def _failed_string(self):
        text = super(SingleRun, self)._failed_string()
        if self._connected:
            text += self._connected_string()
        return text + '\n' + self._failed_progress_bar

    def die(self):
        try:
            self._variable_server.close()
        except:
            pass
        super(SingleRun, self).die()

    def __del__(self):
        try:
            self._variable_server.close()
        except:
            pass

    def _create_variables(self):
        self._tics = variable_server.Variable(
          'trick_sys.sched.time_tics', type_=float)
        self._tics_per_sec = variable_server.Variable(
          'trick_sys.sched.time_tic_value', type_=float)
        self._terminate_time = variable_server.Variable(
          'trick_sys.sched.terminate_time', type_=float)
        return self._tics, self._tics_per_sec, self._terminate_time

    def _connected_string(self):
        return '      {0}       {1}'.format(
          self._sim_time(), self._average_speed())

    def _connected_bar(self):
        if self._terminate_time.value <= 0.0:
          progress =  0.0
        else:
          progress = self._tics.value / self._terminate_time.value
        return create_progress_bar(
          progress, '{0:.1f}%'.format(100 * progress))

    def _sim_time(self):
        """
        Get a string displaying the sim time.

        Returns
        -------
        str
            A string for displaying sim time.
        """
        return 'Sim Time: {0:7.1f} sec'.format(
          self._tics.value / self._tics_per_sec.value)

    def _average_speed(self):
        """
        Get a string displaying the average speedup.

        Returns
        -------
        str
            A string for displaying the ratio of sim time to real time.
        """
        elapsed_time = (
          (self._stop_time if self._stop_time else time.time())
          - self._start_time)
        return 'Average Speed: {0:4.1f} X'.format(
          self._tics.value / self._tics_per_sec.value / elapsed_time)

