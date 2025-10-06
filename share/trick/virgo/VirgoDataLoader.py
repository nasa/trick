import re

class VirgoDataLoader:
    """
    Base class that provides functions for loading data that must be populated
    by derived classes such that VirgoScene-based classes can use it.
    Intended to standardize the functions a data loader must fulfill in order
    to work within the VIRGO framework. Consider this dict information that
    may appear in a VIRGO YAML file:

    data_source: # Top level data_source dict, all data sources live under here
      trickpy:   # A "trickpy" based data source
        time:                         # The required time alias
          group: Satellite            # Trickpy Data recording group for this data
          var: sys.exec.out.time      # Time variable in "trickpy terminology"
        sat_pos:                      # User defined alias for satellite position
          group: Satellite            # Trickpy Data recording group for this data
          var: dyn.satellite.pos[0-2] # Satellite position variable in "trickpy terminology"

    The above dict represents an example of a VIRGO data_source: definition. In
    this example the data source is a trickpy-based file loader which comes
    with VIRGO as a default data loading option. This dict is consumed by
    a class VirgoTrickpyFileLoader.py which derives from this base class. That
    derived class fully implements the base class functions to acquire the
    data in the manner specific to the data_source.

    If you want to add a new data_source:, create a new sub-dict under data_source:
    and populate it with whatever information you might need to fulfill the
    functions in this base class.
    """

    def __init__(self):
        pass

    def load_variables(self):
        """
        Empty base class to be implemented by derived classes. Load the variables
        needed for this instance's get_* functions to be operational.
        """
        pass

    def get_alias_data(self, alias):
        """
        Get a list of data values associated with a single variable alias
        defined in the data_source: dict

        Empty base class to be implemented by derived classes.
        """
        return None

    def get_alias_datas(self, alias):
        """
        Get a list of data values associated with an entire variable alias
        (multidimensional array) defined in the data_source: dict

        Empty base class to be implemented by derived classes.
        """
        return None