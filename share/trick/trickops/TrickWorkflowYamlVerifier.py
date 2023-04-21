import os, sys, copy
import yaml  # Provided by PyYAML
#from WorkflowCommon import tprint
from pydoc import locate
from exceptions import RequiredParamException, IncorrectYAMLException

this_dir = os.path.abspath(os.path.join(os.path.dirname(os.path.realpath(__file__))))

# This global is the result of hours of frustration and debugging. See comment at the top of
# TrickWorkflow.py for details
this_trick = os.path.normpath(os.path.join(os.path.dirname(os.path.realpath(__file__)), '../../..'))

class TrickWorkflowYamlVerifier():
    """
    This class accepts a project YAML config file which contains a list of all
    possible sims, runs, analyses, comparisons, etc and verifies that the content
    meets the constraints that the TrickWorkflow expects. Specifically, self.config
    is santitized to ensure that it is a dictionary matching the format below

    globals:
      env:                 <-- optional literal string executed before all tests, e.g. env setup
    SIM_abc:               <-- required unique name for sim of interest, must start with SIM
      path:                <-- required SIM path relative to project top level
      description:         <-- optional description for this sim
      labels:              <-- optional list of labels for this sim, can be used to get sims
          - model_x            by label within the framework, or for any other project-defined
          - verification       purpose
      build_args:          <-- optional literal args passed to trick-CP during sim build
      binary:              <-- optional name of sim binary, defaults to S_main_{cpu}.exe
      size:                <-- optional estimated size of successful build output file in bytes
      phase:               <-- optional phase to be used for ordering builds if needed
      parallel_safety:     <-- <loose|strict> strict won't allow multiple input files per RUN dir
      runs:                <-- optional dict of runs to be executed for this sim, where the
        RUN_1/input.py --foo:  dict keys are the literal arguments passed to the sim binary
        RUN_2/input.py:        and the dict values are other run-specific optional dictionaries
        RUN_[10-20]/input.py:  described in indented sections below. Zero-padded integer ranges
                               can specify a set of runs with continuous numbering using
                               [<starting integer>-<ending integer>] notation
              returns: <int>   <---- optional exit code of this run upon completion (0-255). Defaults
                                 to 0
          compare:         <---- optional list of <path> vs. <path> comparison strings to be
            - a vs. b            compared after this run is complete. This is extensible in that
            - d vs. e            all non-list values are ignored and assumed to be used to define
            - ...                an alternate comparison method in a class extending this one
          analyze:         <-- optional arbitrary string to execute as job in bash shell from
                               project top level, for project-specific post-run analysis
          phase:           <-- optional phase to be used for ordering runs if needed
          valgrind:        <-- optional string of flags passed to valgrind for this run.
                               If missing or empty, this run will not use valgrind
    non_sim_extension_example:
      will: be ignored by TrickWorkflow parsing for derived classes to implement as they wish

    Any key matching the above naming schema is checked for type and value validity
    Any key not matching the above naming schema is ignored purposefully to provide
    users of this framework the ability use the YAML file for project-specific use-cases
    """
    def __init__(self, config_file, quiet=False):
        """
        Initialize this instance.

        >>> twyv = TrickWorkflowYamlVerifier(config_file=os.path.join(this_trick,"share/trick/trickops/tests/trick_sims.yml"))
        >>> twyv.parsing_errors
        []
        >>> len(twyv.config.keys())
        57

        """
        self.config_file=config_file
        self.parsing_errors = []                                # List of errors encountered during parsing
        self.config = self._read_config(self.config_file)       # Contains resulting Dict parsed by yaml.load
        self.original_config= copy.deepcopy(self.config)  # Keep a copy of original dict as parsed
        # Contains requirements the format of key/value pairs inside self.config_file
        self.yaml_requirements = self._read_config(os.path.join(this_dir,".yaml_requirements.yml"))
        defaults = self.yaml_requirements.pop('content') # Only used to populate default values, can remove
        self._populate_yaml_requirements(self.yaml_requirements, defaults)

    def _populate_yaml_requirements(self, thedict, defaults):
        """
        Recursively traverse thedict, and overlay thedict values on top
        of defaults dict values. This ensures all non-dict values exist
        as defined in the content: section of .yaml_requirements.yml without
        having to specify them individually in that file
        """
        for k,v in thedict.items():
            if isinstance(v, dict):
                subdicts = False
                for j,u in v.items():
                    if isinstance(u, dict):
                        subdicts = True
                if subdicts:  # If there are subdicts, recurse
                    self._populate_yaml_requirements(v, defaults)
                else:  # No subdicts, overlay values over defaults
                    thedict[k] = TrickWorkflowYamlVerifier.DictVerifier().merge(
                      defaults,thedict[k])

    def _read_config(self, config_file):
        """
        Read the yaml file into a dict and return it

        Parameters
        ----------
        config_file : str
            path to YAML config file to be read

        Returns
        -------
        dict
            dictionary representation of YAML content as parsed by yaml.safe_load()

        Raises
        ------
        RuntimeError
            If config_file cannot be read
        """
        try:
            with open(config_file) as file:
                y = yaml.safe_load(file)
            return y
        except Exception as e:
            msg = ("Unable to parse config file: %s\nERROR: %s\nSee"
              " the TrickOps documentation for YAML file format expectations"
                   % (config_file,e))
            raise RuntimeError(msg)

    def get_parsing_errors(self):
        """
        Return the list of errors encountered while parsing self.original_config

        Returns
        -------
        list
            List of error messages encountered during parsing
        """
        return self.parsing_errors

    def verify(self):
        """
        Verify the content of self.config. This is done by recursively checking content
        in self.config against self.yaml_requirements using subclasses defined below.
        Parsing errors are added as strings to the self.parsing_errors list
        """
        if not self.config:  # If entire config is empty
            msg =("ERROR: Config file %s appears to be empty. Make sure file exists and is"
                  " valid YAML syntax." % (self.config_file))
            self.parsing_errors.append(msg)
            raise RuntimeError(msg)
        if not isinstance(self.config, dict):  # If parser did not produce a dict
            msg =("ERROR: Config file %s doesn't match expected dictionary format. "
                  " See the TrickOps documentation for YAML file format expectations." %
                 (self.config_file))
            self.parsing_errors.append(msg)
            raise RuntimeError(msg)

        if 'globals' not in self.config:
          self.config['globals'] = {'env': ''}
        gv = self.GlobalVerifier(
          self.config['globals'], self.yaml_requirements['globals'])
        gv.verify()
        self.config['globals'] = gv.get_verified_globals_dict()
        self.parsing_errors += gv.get_parsing_errors()
        for sim in list(self.config.keys()):
          if not str(sim).startswith('SIM'):  # Ignore everything not starting with SIM
            continue
          else:
            sv = self.SimVerifier(sim_name=sim, sim_dict=self.config[sim],
              req_dict=self.yaml_requirements)
            try:
              # Replace the entire dict for this sim with it's verified equivalent
              sv.verify()
              self.config[sim] = sv.get_verified_sim_dict()
            # If not all required params given, remove this sim entry
            except RequiredParamException as e:
              del self.config[sim]
            finally:
              self.parsing_errors += sv.get_parsing_errors()
        # Return a copy of sanitized config
        return(dict(self.config))

    # These inner classes provide helper methods for merging default expected content
    # in dictionaries produced by YAML files with the actual content produced by
    # YAML files. The merged content is then verified for correctness according to
    # expected types, ranges, and other constraints.
    class DictVerifier():
        def __init__(self):
            self.errors = []  # collects all errors found in the YAML file parsing
        def get_parsing_errors(self):
            return(self.errors)
        def merge(self, default_dict, authoritative_dict):
            """
            Merges the content of authoritative_dict into default_dict, using values
            from authoritative_dict over values in default_dict
            """
            try:
              merged ={**default_dict, **authoritative_dict}
            except Exception as e:
              msg=( "Unable to merge given dictionary with default dictionary."
              "\n  Given: %s"
              "\n  Default: %s"
              "\nThis is usually caused by incorrect format in the YAML file. "
              "See the TrickOps documentation for YAML file format expectations."
              % (authoritative_dict, default_dict))
              raise IncorrectYAMLException(msg)
            return (merged)
        def defaults_only(self, thedict):
            """
            Given a dict of yaml_requirements format, produce a dict containing
            only {key: <key's default value>}
            """
            newdict = {}
            for k,v in thedict.items():
                newdict[k] = thedict[k]['default']
            return newdict
        # Utility methods for ensuring given content matches expected constraints.
        # These return True if check passes, False otherwise. Errors ar logged to
        # self.errors
        def expect_type(self, param, expected_type, where):
            if param is None and expected_type is None:
                return True
            if not isinstance(param, expected_type):
                self.errors.append(where +
                  "value \"%s\" expected to be type:%s, but got type:%s instead. Ignoring." %
                    ( param, expected_type, type(param)))
                return False
            return True
        def expect_less_than_or_eq(self, param, maximum, where):
            if (not param <= maximum):
                self.errors.append(where +
                  "value \"%s\" expected to be <= %s. Ignoring." %
                    ( param, maximum))
                return False
            return True
        def expect_more_than_or_eq(self, param, minimum, where):
            if (not param >= minimum):
                self.errors.append(where +
                  "value \"%s\" expected to be >= %s. Ignoring." %
                    ( param, minimum))
                return False
            return True
        def expect_contains(self, param, contained_in, where):
            if (param not in contained_in):
                self.errors.append(where +
                  "value \"%s\" expected to be in subset %s. Ignoring." %
                    ( param, contained_in))
                return False
            return True
        def verify(self, user_dict, req_dict, identifier):
            """
            Verifies the content of of user_dict matches the requirements of req_dict.
            The content of req_dict comes from the .yaml_requirements file
            The merge with defaults during __init__ guarantees all keys exist.
            """
            for k,v in req_dict.items():
              before_num_errors = len(self.errors)
              if v['required']:          # Must be given by user
                if not self.expect_type(user_dict[k], locate(v['type']),
                  where=("In %s, required param \"%s\" " % (identifier, k))):
                  raise RequiredParamException("%s must exist and be of type %s in %s"
                    % (k, locate(v['type']), identifier))
              elif v['default'] is None and user_dict[k] is None:  # Can be given but have None type
                # TODO: we may be able to set user_dict[k] = locate(v['type']) here
                # which would make runs: and labels: lists, but need to see if that
                # has downstream effects. If this works we can remove manual checking
                # for None in the derived verifier classes
                pass
              # Could be another type used by derived class, so don't check anything
              elif v['overridable'] == 1:
                pass
              else:  # Check optional params (majority)
                # Checking possible values only makes sense if we got the right type
                if self.expect_type(user_dict[k], locate(v['type']),
                  where=("In %s, param \"%s\" " % (identifier, k))):
                  if v['min'] is not None:
                    self.expect_more_than_or_eq(int(user_dict[k]), int(v['min']),
                      where=("In %s, param \"%s\" " % (identifier, k)))
                  if v['max'] is not None:
                    self.expect_less_than_or_eq(int(user_dict[k]), int(v['max']),
                      where=("In %s, param \"%s\" " % (identifier, k)))
                  if v['contains'] is not None:
                    self.expect_contains(user_dict[k], v['contains'],
                     where=("In %s, param \"%s\" " % (identifier, k)))
              # If we encountered any errors, set param to default value
              if len(self.errors) > before_num_errors:
                  user_dict[k] = v['default']

    class GlobalVerifier(DictVerifier):
        def __init__(self, globals_dict, globals_req_dict):
            super().__init__()
            self.globals_req_dict = globals_req_dict
            self.defaults = self.defaults_only(globals_req_dict)
            self.globals_dict = self.merge(self.defaults, globals_dict)
        def verify(self):
            """
            Verifies the content of of self.globals_dict. The merge with defaults
            during __init__ guarantees all keys exist.
            """
            TrickWorkflowYamlVerifier.DictVerifier.verify(self, user_dict=self.globals_dict,
              req_dict=self.globals_req_dict, identifier='globals')
        def get_verified_globals_dict(self):
            return (dict(self.globals_dict)) # Return a copy

    class SimVerifier(DictVerifier):
        def __init__(self, sim_name, sim_dict, req_dict):
            super().__init__()
            sim_dict = {} if not isinstance(sim_dict, dict) else sim_dict
            self.req_dict = req_dict # TODO every derived class needs this, move it to the base class
            self.defaults = self.defaults_only(self.req_dict['sim'])
            self.defaults['name'] = sim_name
            # TODO what if SIM: dict is empty?
            self.sim_dict = self.merge(self.defaults, sim_dict)
        def verify(self):
            TrickWorkflowYamlVerifier.DictVerifier.verify(self, user_dict=self.sim_dict,
              req_dict=self.req_dict['sim'], identifier=self.sim_dict['name'])
            if self.sim_dict['labels'] is None:
               self.sim_dict['labels'] = []   # If not given, make it an empty list
            # Handle the edge case where labels is a list, but a list of not just strings
            if any([ not isinstance(label, str) for label in self.sim_dict['labels']]):
              self.errors.append("In %s, labels \"%s\" expected to be list of strings. Ignoring." %
                  ( self.sim_dict['name'], self.sim_dict['labels']))
              self.sim_dict['labels'] = []
            if self.sim_dict['runs'] is None:
               self.sim_dict['runs'] = {}   # If not given, make it an empty dict
            for run in list(self.sim_dict['runs']):
              rv = (TrickWorkflowYamlVerifier.RunVerifier( run,
                  self.sim_dict['runs'][run], self.req_dict))
              try:
                rv.verify()
                self.sim_dict['runs'][run] = rv.get_verified_run_dict()
              # If not all required params given, remove this run entry. I believe this
              # to be unreachable code, but added anyhow for safety -Jordan 12/2022
              except RequiredParamException as e:
                del self.sim_dict['runs'][run]
              finally:
                # Pass errors up to sim level
                self.errors += rv.get_parsing_errors()

        def get_verified_sim_dict(self):
            return (dict(self.sim_dict)) # Return a copy

    class RunVerifier(DictVerifier):
        def __init__(self, run_name, run_dict, req_dict):
            super().__init__()
            self.req_dict = req_dict
            self.defaults = self.defaults_only(self.req_dict['run'])
            self.defaults['input'] = run_name
            # If run_dict is None, use defaults, otherwise merge content into defaults
            self.run_dict = (self.merge(self.defaults, run_dict) if run_dict else dict(self.defaults))
        def verify(self):
            """
            Verifies the content of of self.run_dict. The merge with defaults
            during __init__ guarantees all keys exist.
            """
            # Verify the run
            TrickWorkflowYamlVerifier.DictVerifier.verify(self, user_dict=self.run_dict,
              req_dict=self.req_dict['run'], identifier=self.run_dict['input'])
            # Verify the analyze: section of the run, setting to None if invalid
            if (self.run_dict['analyze'] is not None and
               not self.expect_type(self.run_dict['analyze'], str,
               where=("In %s, analyze section " % (self.run_dict['input'])))):
               self.run_dict['analyze'] = None
            if self.run_dict['compare'] is None:
               self.run_dict['compare'] = []   # If not given, make it an empty list
            # Verify the compare list of the run, removing any that aren't correct
            # The check for list allows all other non-list types in the yaml file,
            # allowing groups to define their own comparison methodology
            if isinstance(self.run_dict['compare'], list):
              for compare in list(self.run_dict['compare']):
                if (not self.expect_type(compare, str,
                     where=("In %s, compare section " % (self.run_dict['input']))) or
                    not self.expect_contains(" vs. ", compare,
                     where=("In %s, compare section  " % (self.run_dict['input'])))
                ):
                  self.run_dict['compare'].remove(compare)

        def get_verified_run_dict(self):
            return (dict(self.run_dict)) # Return a copy

