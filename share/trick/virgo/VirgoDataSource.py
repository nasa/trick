class VirgoDataSource():
    """
    Base class for Virgo Data Sources. Derive from this class and
    define the functions listed below in order to provide a custom
    data source for your application

    TODO: Should some of these methods be abstract?
    """
    def __init__(self, name="Unnamed Source"):
        self.name=name

    def get_first_time(self) -> float:
        """
        Return the last time associated with this data
        """
        return 0.0

    def get_current_time(self) -> float:
        """
        Return current time associated with this data
        """
        return 0.0

    def get_last_time(self) -> float:
        """
        Return the last time associated with this data
        """
        return None

    def get_current_position(self) -> tuple[float]:
        """
        Return current (x,y,z) position associated with this data
        """
        return None

    def get_current_rotation(self) -> tuple[tuple[float]]:
        """
        Return current rotation transformation 3x3 matrix associated with this data
        """
        return None

    def get_current_scale(self) -> tuple[float]:
        """
        Return current (x,y,z) scale associated with this data
        """
        return None

    def get_current_opacity(self) -> float:
        """
        Return current visiblity associated with this data
        """
        return None

    def get_additional_data(self, variable) -> float:
        return None

    def is_there_additional_data(self) -> bool:
        return False


import bisect, math
class VirgoDataFileSource(VirgoDataSource):
    """
    Derived class for storing data extracted specifically from a
    VirgoDataLoader to be used by the VirgoSceneNode class as an
    interface. 
    """
    def __init__(self, name="Unnamed Source", times=None, rotations=None,
                 positions=None, scales=None, opacities=None, **kwargs):
        """
        Args:
          times (list): sorted list of ascending doubles
              List representing all time steps for all data associated with this
              actor
          positions (list): List of 3-size tuples of doubles
              List represents (x, y, z) positions of this actor one per entry in
              times list. Length of positions must == length of times
          rotations (list): List of 3X3 numpy array of doubles
              List represents 3X3 (rotation matrix) of this actor, one per entry
              in times list.  Length of rotations must == length of times
          scales (list): List of 3-size tuples of doubles
              List represents (x, y, z) scales of this actor one per entry in
              times list. Length of positions must == length of times
          opacities (list): List of scalar values between 0.0 and 1.0 of this actor
              one per entry in the times list. Length of opacities  must ==
              length of times.
          name (str): Name given to this actor
          **kwargs : Additional keyword arguments
              Specify other lists of datas to be served by this instance. Array
              lengths must == length of times
        # Lists of state data
        """
        self._times = times
        self._positions = positions
        self._rotations = rotations
        self._scales = scales
        self._opacities = opacities 
        self._current_time = 0.0
        self._current_time_idx = 0
        self.initialized = False
        self.additional_data = dict(kwargs)

    def is_initialized(self):
        return(self.initialized)

    def set_times(self, times):
        self._times = times

    def set_positions(self, positions):
        self._positions = positions

    def set_rotations(self, rotations):
        self._rotations = rotations

    def set_opacities(self, opacities):
        self._opacities = opacities

    def get_current_time_idx(self):
        """
        Return current time index associated with data source. This is the index
        of the self._times array associated with the last set_current_time()
        call
        """
        return(self._current_time_idx)

    def get_current_time(self):
        """
        Return current time associated with data source. This is the time
        associated with the last set_current_time() call and is a value in the
        self._times array
        """
        return(self._current_time)

    def get_last_time(self):
        """
        Return the last time associated with data source. This is the final
        value in the self._times array
        """
        return(self._times[-1])

    def get_first_time(self):
        """
        Return the last time associated with data source. This is the first
        value in the self._times array
        """
        return(self._times[0])


    def get_current_position(self):
        if self._positions:
            return(self._positions[self.get_current_time_idx()])
        else:
            return None

    def get_current_rotation(self):
        if self._rotations:
            return(self._rotations[self.get_current_time_idx()])
        else:
            return None

    def get_current_scale(self):
        #import pdb; pdb.set_trace()
        if self._scales:
            return(self._scales[self.get_current_time_idx()])
        else:
            return None

    def get_current_opacity(self):
        if self._opacities:
            return(self._opacities[self.get_current_time_idx()])
        else:
            return None

    def get_additional_data(self, variable) -> float:
        #import pdb; pdb.set_trace()
        if variable not in self.additional_data:
            msg = (f"ERROR: Additional data {variable} not found in {self}!")
            raise RuntimeError (msg)
        return self.additional_data[variable][self.get_current_time_idx()]

    def is_there_additional_data(self) -> bool:
        if self.additional_data:
            return True
        return False

    def initialize(self):
        self.verify()

    def verify(self):
        """
        Checks for required assumptions of the model and raises if any issues found
        Assigns self.initialized = True when all conditions are met
        TODO: Add other checks if we add other paramters
        """
        # Ensure self._times is a list
        if not isinstance(self._times, list):
          raise ValueError(f"Expected {self.name}'s self._times to be a list of doubles "
                           f"but got {type(self._times)} instead")
        # Ensure self._times list contains only doubles
        for i, t in enumerate(self._times):
          if not isinstance(t, float):
            raise ValueError(f"Expected {self.name}'s self._times[{i}] to be a double "
                             f"but got {type(self._times[i])} instead")
        # Ensure self._times list of doubles is sorted, lowest to highest
        self.assert_sorted(self._times)

        # Ensure lists are the same length
        if self._positions and len(self._times) != len(self._positions):
          raise ValueError(f"{self.name}'s self._times and self._positions are not the same length")
        if self._rotations and (len(self._times) != len(self._rotations)):
          raise ValueError(f"{self.name}'s self._times and self._rotations lists are not the same length")
        if self._scales and (len(self._times) != len(self._scales)):
          raise ValueError(f"{self.name}'s self._times and self._scales lists are not the same length")
        if self._opacities and (len(self._times) != len(self._opacities)):
          raise ValueError(f"{self.name}'s self._times and self._opacities lists are not the same length")
        # TODO need a check that self._opacities values are all between 0-1 when
        if self.additional_data:
          for ad in self.additional_data:
            if (len(self.additional_data[ad]) != len(self._times)):
              raise ValueError(f"{self.name}'s self._times and self.additional_data[{ad}] "
                               "lists are not the same length")

        # If we made it this far, the actor is fully initialized
        self.initialized = True

    def set_current_time_idx(self, idx):
        """
        Set the current time index associated with data for this actor. This is
        not global scene time, this is an index from a TrickPy generated array
        of the first column of tricked logged data.
        """
        if not self.initialized:
            return None
        if idx < len(self._current_time):
            self._current_time_idx = idx
        else:
            raise ValueError(f"Cannot set time index {idx} as it's longer than len(self._current_time)")

    def increment_current_time(self,):
        """
        Increase the self._current_time_idx and update self.current_time accordingly
        """
        if not self.initialized:
            return None
        self._current_time_idx += 1
        self._current_time = self._times[self._current_time_idx]

    def decrement_current_time(self,):
        """
        Decrease the self._current_time_idx and update self.current_time accordingly
        """
        if not self.initialized:
            return None
        self._current_time_idx -= 1
        self._current_time = self._times[self._current_time_idx]

    def get_next_time(self, world_time):
        """
        Return the time from self._times after given world_time
        """
        if not self.initialized:
            return None
        if  self._current_time_idx + 1 >= len(self._times):
            return self._times[0]  # loop back to beginning
        elif self._times[self._current_time_idx] > world_time:  
            return self._times[0]
        else:
            return(self._times[self._current_time_idx+1])

    def get_previous_time(self, world_time):
        """
        Return the time from self._times before given world_time
        """
        if not self.initialized:
            return None
        if self._times[self._current_time_idx] < world_time: 
            return self._times[self._current_time_idx]
        else:
            return self._times[self._current_time_idx-1]

    def set_current_time(self, time, strategy='down'):
        """
        Attempt to set self._current_time by snapping to the closest data point
        in self._times associated with given time, using strategy provided.
        
        strategies:
          down:    Move to the closest self._current_time_idx without going above time
          up:      Move to the closest self._current_time_idx without going below time
          closest: Move to the closest self._current_time_idx regardless of time's

        Returns the actual time index this actor snaps to.
        TODO: This will probably break if there's more than one data point at the same time
          step - i.e. Trick DR_Changes_Step, see:
          https://nasa.github.io/trick/documentation/simulation_capabilities/Data-Record

        TODO: This function is less computationally efficient that a simple increase/decrease
          of the actor's current_time_idx, but I'm doing it this way to support multiple
          data recording groups at different rates. Maybe there's a better way that doesn't
          end up bisecting a list every time (see _find_index).
        """
        if not self.initialized:
            return None

        self._current_time_idx = self._find_index(mylist=self._times, input_value=time, mode=strategy)
        self._current_time = self._times[self._current_time_idx]
        return (self._current_time_idx)

    @staticmethod
    def assert_sorted(lst):
        if not all(lst[i] <= lst[i + 1] for i in range(len(lst) - 1)):
            raise ValueError("List is not sorted in ascending order.")

    @staticmethod
    def _find_index(mylist, input_value, mode='down'):
        """
        Find index of sorted mylist closest to input_value according to mode:
        - 'closest': nearest value (either side)
        - 'down':    snap down to closest entry in list
        - 'up':      snap up to closest entry in list
    
        Note that math.isclose() is used to do floating point
        equality checks when input_value is extremely close to a value
        in mylist
    
        Returns found index or None if out of range in strict modes
        """
        # get the index where input_value would be inserted before
        # (to the left of) any existing occurrences of input_value.
        pos = bisect.bisect_left(mylist, input_value)
    
        if mode == 'down':
            if pos == 0:
                return 0  # First entry in the list
            if pos == len(mylist):
                return -1 # Last entry in the list
            if math.isclose(input_value, mylist[pos]):
                return pos
            return pos - 1
    
        elif mode == 'up':
            if pos == len(mylist):
                return -1 # Last entry in the list
            return pos
    
        # closest
        if pos == 0:
            return 0
        if pos == len(mylist):
            return - 1  # Last entry in the list
    
        before = mylist[pos - 1]
        after = mylist[pos]
        if abs(after - input_value) < abs(input_value - before):
            return pos
        else:
            return pos - 1