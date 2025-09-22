import re
import numpy as np

class VirgoDataFileLoader:
    """
    Class that manages loading data from the scene recorded_data: section
    of a dict in Virgo-expected format

    Uses trickpy to load the data, then puts it into a format best suited
    for consumption in the Virgo framework
    """
    def __init__(self, run_dir, scene_recorded_data, verbosity=1):
        self.run_dir=run_dir
        self.scene_recorded_data=dict(scene_recorded_data)
        self.verbosity=verbosity
        self.drg = None

    @staticmethod
    def expand_arrays(varname):
        """
        Given a string, check the string for array syntax ([ - ]). If the given variable
        specifies an array, return the string expanded into a list containing each array
        member.

        For consistency's sake, a list is always returned, even when the given string
        is not expanded.

        For example:
        input: varname="path.to.variable[0-2]"
        output: ["path.to.variable[0]", "path.to.variable[1]", "path.to.variable[2]"]

        input: "variable"
        output: ["variable"]

        Args:
            varname (string): A string containing the trick name corresponding to the
            given alias.
        """
        # Define a regex which finds any text that uses our format for
        # multiple elements in an array: ([ - ]).
        array_regex = re.compile(r"(?<=\[)\w+(?:-\w+)+(?=\])")
        new_varname = [varname]
        found_match = True
        while found_match:
            found_match = False
            for i, s in enumerate(new_varname):
                match = array_regex.search(s)
                if match:
                    if "-" in match.group():
                        found_match = True
                        start_index = int(match.group().split("-")[0])
                        stop_index = int(match.group().split("-")[1])
                        # Replace trick-name with list of each array element:
                        new_varname[i : i + 1] = [s[: match.start()] + str(ii) + s[match.end() :] for ii in range(start_index, stop_index + 1)]
                    else:
                        print("Regex expression error.")
        return new_varname

    def load_variables(self):
        """
        Use trickpy to load the variables defined in the 'recorded_data:' section
        of the scene

        Populates: self.drg
        Returns:   self.drg
        Raises: RuntimeError if a 'var' in 'recorded_data:' is not found when loading
        the self.run data

        TODO: Need uniqueness check, no var: can be repeated twice in the recorded_data: area
        """
        import trickpy.data_record as dr
        load_these_variables = []
        # Find every var: given in the scene dict and pass them to dr.load_run
        # as the subset of variables to load.
        for v in self.scene_recorded_data:
          expanded_vars = self.expand_arrays(self.scene_recorded_data[v]['var'])
          load_these_variables += expanded_vars
        if len(load_these_variables) == 0:
          msg = (f"ERROR: There are no variables in the recorded_data: section of scene. "
                f"Nothing to load! ")
          raise RuntimeError (msg)

        if self.verbosity > 0:
            print(f"Loading data from {self.run_dir}...")
            if self.verbosity > 1:
                for v in load_these_variables:
                    print(f"  {v}")
        self.drg = dr.load_run(self.run_dir, variables=load_these_variables)
        if self.verbosity > 0:
            print(f"Done.")

        # Make sure dr.load_run found all the vars we requested (trickpy doesn't error if
        # some were not found)
        expected_vars = list(load_these_variables)
        for group in self.drg:
          if len(expected_vars) == 0:
             break
          for var in self.drg[group]:
            if len(expected_vars) == 0:
               break
            if var in load_these_variables:
              expected_vars.remove(var)
              continue
        if len(expected_vars) > 0:
          msg = (f"ERROR: The following variables from recorded_data: section of scene "
                f"were not found in data loading of {self.run_dir}\n{expected_vars}")
          raise RuntimeError (msg)
        return (self.drg)

    def does_alias_exist(self, alias):
      """
      TODO: can this handle if [] are in the alias? I don't think so
      """
      (alias, indices) = self.split_alias_and_indices(alias_in=alias)
      if alias in self.scene_recorded_data:
        variable = self.scene_recorded_data[alias]['var']
        return(self.does_variable_exist(variable))
      else:
        return False

    @staticmethod
    def split_alias_and_indices(alias_in):
        """
        Search for the [] specified_indices in the alias given, and split it
        into alias=(alias with no specified_indices),
        specified_indices=(specified_indices) where specified_indices is None
        if not given
        """
        alias=str(alias_in)
        specified_indices = ''
        indices_pattern = r"\[.*\]"
        match = re.search(indices_pattern, alias)
        if match:
            specified_indices = match.group(0)
            alias = alias.split(specified_indices)[0]
        return (alias, specified_indices)

    def does_variable_exist(self, variable):
        """
        Query self.drg for the existence of variable
        """
        exists = False
        for grp in self.drg:
          expanded_variables = self.expand_arrays(variable)
          #import pdb; pdb.set_trace()
          for var in expanded_variables:
              # TODO this is a loose check, it doesn't check for all array indices
              if var in self.drg[grp]:
                exists = True
                break
        return exists

    def get_recorded_data(self, alias):
        """
        Get the recorded data associated with a single variable alias defined in
        recorded_data: dict. For example, for this YAML equivalent of a dict:

        recorded_data:
          time:                          # Variable Alias
            group: Satellite             # DR Group  
            var: sys.exec.out.time       # DR variable name
          sat_pos:                       # Variable Alias
            group: Satellite             # DR Group
            var: dyn.satellite.pos[0-2]  # DR variable name
          sat_vel:                       # Variable Alias
            group: Satellite             # DR Group
            var: dyn.satellite.vel[0-2]  # DR variable name

        For variables with index ranges ([0-2] or [0-2][0-2] for example) the
        indices must be specified such that a single variable is requested.

        For example get_recorded_data(alias='sat_pos[0]') will return a list of 
        dyn.satellite.pos[0] values associated with group Satellite.

        Returns: A list of data values associated with alias

        Raises: RuntimeError if unable to find data associated with alias or there's
          an issue with the indices specified.
        """
        import re
        if not self.drg:
          msg = (f"ERROR: Cannot get_recorded_data as self.drg is not populated.")
          raise RuntimeError (msg)

        alias, specified_indices = self.split_alias_and_indices(alias)
        if alias not in self.scene_recorded_data:
          msg = (f"ERROR: {alias} not found in recorded_data section of scene. Make sure to use"
                 " the alias name, not the var: name.")
          raise RuntimeError (msg)

        variable = self.scene_recorded_data[alias]['var']
        # Search for the [] var_indices in the var associated with the alias
        # given, and split it into variable=(variable with no var_indices),
        # var_indices=(var_indices) where var_indices is None if not given
        indices_pattern = r"\[.*\]"
        match = re.search(indices_pattern, variable)
        var_indices = ''
        if match:
            var_indices = match.group(0)
            variable = variable.split(var_indices)[0]
        # TODO: This check may not be robust to two-dimensional arrays, if alias=rot[0] for
        # example but the underlying var is defined as rotation[0-2][0-2]
        if specified_indices == '' and var_indices != '':
          msg = (f"ERROR: {alias} is multidimensional. Specify indices to request a single " 
                 "variable instead, ex: pos[0] instead of pos, rot[1][2] instead of rot")
          raise RuntimeError (msg)
        grp = self.scene_recorded_data[alias]['group']
        return self.drg[grp][variable+specified_indices].tolist()
        
    def get_recorded_datas(self, alias):
        """
        Get recorded datas (more than one value at a time) associated with an
        alias defined in recorded_data: dict. For example, for this YAML
        equivalent of a dict:

        recorded_data:
          time:                          # Variable Alias
            group: Satellite             # DR Group  
            var: sys.exec.out.time       # DR variable name
          sat_pos:                       # Variable Alias
            group: Satellite             # DR Group
            var: dyn.satellite.pos[0-2]  # DR variable name
          sat_vel:                       # Variable Alias
            group: Satellite             # DR Group
            var: dyn.satellite.vel[0-2]  # DR variable name

        This function returns entire sets of recorded data as lists of primitives
        arranged by array sizes of the data. For example:
           No array  (ex: time)      Returns a list of time values
           1-D array (ex sat_pos)    Returns a list of tuples where is tuple is of size
                                     associated with the 1-D array, e.g.
                                     [(x0, y0, z0), (x1, y1, z1) ... ]
           2-D array (ex sat_rot)    Returns a list of np.arrays of size associated with
                                     the 2-D array, e.g.
                                     [ 
                                       np.array((R0[0][0], R0[0][1], R0[0][2]),
                                                (R0[1][0], R0[1][1], R0[1][2]),
                                                (R0[2][0], R0[2][1], R0[2][2])),
                                       np.array((R1[0][0], R1[0][1], R1[0][2]),
                                                (R1[1][0], R1[1][1], R1[1][2]),
                                                (R1[2][0], R1[2][1], R1[2][2])),
                                       ...

                                       ]

        Returns: A list of data values associated with alias

        Raises: RuntimeError if unable to find data associated with alias or there's
          an issue with the indices specified.
        """
        datas = []
        # This function doesn't want indices, if we detect them, error out
        indices_pattern = r"\[.*\]"
        match = re.search(indices_pattern, alias)
        if match:
            msg = (f"ERROR: Indices found in {alias}. Do not include indices [] when calling "
                   "get_recorded_datas()")
            raise RuntimeError (msg)
        # Get the array indices defintion from the variable in the scene
        grp = self.scene_recorded_data[alias]['group']
        variable = self.scene_recorded_data[alias]['var']

        # Expand the variables to see how many array indices there are
        expanded_variables = self.expand_arrays(variable)
        # Search for the [] var_indices in the var associated with the alias
        # given, and split it into variable=(variable with no var_indices),
        # var_indices=(var_indices) where var_indices is None if not given
        match = re.search(indices_pattern, variable)
        var_indices = ''
        if match:
            var_indices = match.group(0)
            variable = variable.split(var_indices)[0]
        # We can assume the data type by looking at expansion of the variable notation
        # [0-2] means a size 3
        # [0-2][0-2] means a size 3x3 aka 9
        if len(expanded_variables) == 1:
            datas = self.drg[grp][variable].tolist()
        elif len(expanded_variables) == 3:
            datas = list(zip(
                self.drg[grp][variable+'[0]'].tolist(),
                self.drg[grp][variable+'[1]'].tolist(),
                self.drg[grp][variable+'[2]'].tolist()))
        elif len(expanded_variables) == 9:
            tidx = 0
            list_length = len(self.drg[grp][variable+'[0][0]'])
            for i in range(0, list_length):
                datas.append( np.array(
                [  [ self.drg[grp][variable+'[0][0]'][i], self.drg[grp][variable+'[0][1]'][i], self.drg[grp][variable+'[0][2]'][i] ],
                   [ self.drg[grp][variable+'[1][0]'][i], self.drg[grp][variable+'[1][1]'][i], self.drg[grp][variable+'[1][2]'][i] ],
                   [ self.drg[grp][variable+'[2][0]'][i], self.drg[grp][variable+'[2][1]'][i], self.drg[grp][variable+'[2][2]'][i] ] ] )
                ) 
            pass

        return datas
