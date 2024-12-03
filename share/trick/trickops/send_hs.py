import re, os
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
# I don't know of any cleaner way way to do this.  -Jordan 12/2024
this_trick = os.path.normpath(os.path.join(os.path.dirname(os.path.realpath(__file__)), '../../..'))
class send_hs(object):
    """
    Utility class for parsing simulation diagnostic data at the end of a
    Trick-generated send_hs output file.
    """
    def __init__(self, hs_file=None):
        """
        Initialize this instance.

        >>> sh = send_hs(hs_file=os.path.join(this_trick,"share/trick/trickops/tests/send_hs.nominal"))

        Parameters
        ----------
        hs_file : str
            Path to the send_hs output to read and parse
        """
        self.hs_file = hs_file
        self.num_lines = None
        self._diagnostics = {}         # Internal dict of diagnostic keys and values
        self._missing_diagnostics = [] # List of diagnostics we failed to find
        for k in self.keys():
            self._diagnostics[k] = None

        if hs_file:
            self.parse()

    def is_valid(self):
        """
        Check for validity of the parsed send_hs file. If any expected internal members
        were not able to be parsed, store them in missing_diagnostics and return False.
        If everything was found, return True

        >>> sh = send_hs(hs_file=os.path.join(this_trick,"share/trick/trickops/tests/send_hs.nominal"))
        >>> sh.is_valid()
        True

        Returns
        -------
        True if all expected internal members were parsed. False if any member wasn't found.

        """
        self._missing_diagnostics = [] # Reset the internal list
        for d in self._diagnostics:
            if self._diagnostics[d] is None:
                self._missing_diagnostics.append(d)
        if self._missing_diagnostics:
            return False
        else:
            return True

    def missing_diagnostics(self):
        '''
        Check for validity and return a list of any missing diagnostics that we were
        unable to find in the send_hs output

        >>> sh = send_hs(hs_file=os.path.join(this_trick,"share/trick/trickops/tests/send_hs.nominal"))
        >>> sh.missing_diagnostics()
        []

        Returns
        -------
        list
             All diagnotics that were unable to be parsed
        '''
        self.is_valid()
        return self._missing_diagnostics

    def parse(self, hs_file=None):
        '''
        Parse the content of self.hs_file and assign internal variables for each field found
        if hs_file is given, overwrite self.hs_file with it's value


        >>> sh = send_hs()
        >>> sh.parse(os.path.join(this_trick,"share/trick/trickops/tests/send_hs.nominal"))
        >>> sh2 = send_hs()
        >>> sh2.parse()
        Traceback (most recent call last):
           ...
        RuntimeError: send_hs file: 'None' cannot be read. You must provide a valid send_hs output file.

        Parameters
        ----------
        hs_file : str
            Path to hs_file to parse. If None, self.hs_file is used instead.

        Raises
        ------
        RuntimeError
            If no send_hs file was provided at construction or when calling this function
        '''
        if hs_file:
            self.hs_file = hs_file

        if not self.hs_file or not os.path.isfile(self.hs_file):
            raise RuntimeError(f"send_hs file: '{self.hs_file}' cannot be read."
             " You must provide a valid send_hs output file.")

        self.num_lines = 0
        with open(self.hs_file) as fp:
          # Efficiency guard - we don't do an re pattern match until we've starting parsing the diagnostic msg
          start_reading_diagnostics = False
          for line in fp:
            self.num_lines += 1
            #  NOTE this "attempt match" approach is less efficient but it should be
            # robust to a future ordering change of the lines from Trick's output.
            if start_reading_diagnostics:
              for d in self._diagnostics:
                  if self._diagnostics[d] is None:
                      self._diagnostics[d] = self._attempt_hs_match(d, line)
            # This text precedes the diagnostics output, use it as the trigger to start parsing
            if 'REALTIME SHUTDOWN STATS:' in line:
              start_reading_diagnostics = True
              continue
        # Set validity status immediately after parsing
        self.is_valid()

    def _attempt_hs_match(self, name, text):
        """
        Parameters
        ----------
        hs_file : str
            Path to hs_file to parse. If None, self.hs_file is used instead.
        name : str
            Pattern to match (e.g. SIMULATION START TIME)
        text : str
            Text to search for pattern within

        Returns
        ----------
        float  or None
            Value of name if found, else: None
        """
        name = name.replace('(', '\(').replace(')', '\)')
        m = re.match(name+': +([-]?[0-9]*\.?[0-9]+)', text.strip())
        if m:
            return(float(m.group(1)))
        else:
            return(None)

    def keys(self) -> list:
        """
        Return a list of all possible keys/names that get() can accept This is
        the master list of all diagnostics we search for when we parse the send_hs
        file, and naming should match 1:1 with the output of a Trick simulation

        >>> sh = send_hs(hs_file=os.path.join(this_trick,"share/trick/trickops/tests/send_hs.nominal"))
        >>> sh.keys() #doctest: +ELLIPSIS
        ['ACTUAL INIT TIME', ... 'INVOLUNTARY CONTEXT SWITCHES (RUN)']

        Returns
        -------
        list
            All possible diagnostic names that get() can accept
        """
        return (['ACTUAL INIT TIME',
                 'ACTUAL ELAPSED TIME',
                 'SIMULATION START TIME',
                 'SIMULATION STOP TIME',
                 'SIMULATION ELAPSED TIME',
                 'USER CPU TIME USED',
                 'SYSTEM CPU TIME USED',
                 'SIMULATION / CPU TIME',
                 'INITIALIZATION USER CPU TIME',
                 'INITIALIZATION SYSTEM CPU TIME',
                 'SIMULATION RAM USAGE',
                 'VOLUNTARY CONTEXT SWITCHES (INIT)',
                 'INVOLUNTARY CONTEXT SWITCHES (INIT)',
                 'VOLUNTARY CONTEXT SWITCHES (RUN)',
                 'INVOLUNTARY CONTEXT SWITCHES (RUN)'
               ])

    def get_diagnostic (self, name: str) -> float:
        """
        Get a diagnostic value by it's name or partial name
        The first name matched in the self._diagnostics dict will be returned

        >>> sh = send_hs(hs_file=os.path.join(this_trick,"share/trick/trickops/tests/send_hs.nominal"))
        >>> sh.get_diagnostic('ACTUAL INIT TIME')
        42.606
        >>> sh.get_diagnostic('SIMULATION RAM USAGE')
        1198.867

        Parameters
        ----------
        name : str
            Name or partial name of diagnostic to retrieve

        Returns
        -------
        float
            Value of diagnostic name given

        Raises
        ------
        LookupError
            If name cannot be found in self._diagnostics
        """

        for d in self._diagnostics:
            if name in d:
                return(self._diagnostics[d])
        raise LookupError(f"Unable to get diagnostic '{name}'. Is it spelled correctly?")

    def get(self, name: str) -> float:
        """
        Get a diagnostic value by it's name or partial name. Convienence function that
        calls self.get() directly.

        >>> sh = send_hs(hs_file=os.path.join(this_trick,"share/trick/trickops/tests/send_hs.nominal"))
        >>> sh.get('ACTUAL INIT TIME')
        42.606
        >>> sh.get('SIMULATION RAM USAGE')
        1198.867

        Returns
        -------
        float
            Value of diagnostic name given
        """
        return self.get_diagnostic(name)

    @property
    def diagnostics (self) -> dict:
        return dict(self._diagnostics)

    def get_diagnostics (self) -> dict:
        """
        Get all diagnostics stored in internal self._diagnostics dictionary

        >>> sh = send_hs(hs_file=os.path.join(this_trick,"share/trick/trickops/tests/send_hs.nominal"))
        >>> sh.get_diagnostics() #doctest: +ELLIPSIS
        {'ACTUAL INIT TIME': 42.606, 'ACTUAL ELAPSED TIME': 55.551, ... 14.0}

        Returns
        -------
        dict
            A copy of the internal self._diagnostics dictionary
        """
        return (self.diagnostics)
