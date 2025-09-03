#!/usr/bin/env python3.11
"""
VIRGO: Versatile Imaging and Rendering for Galactic Operations
A Practical, Analytical, and Hardworking 3D Visualization tool
leveraging python-VTK
"""
import os, sys, inspect, bisect, math, time
thisFileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))

# For TrickPy, the python module that can load trick data record produced files
sys.path.append(os.path.abspath(os.path.join(thisFileDir, '../')))

import vtk
import numpy as np
MAX_CLIPPING_RANGE=1e9
# Example of custom interactor style to override the default 'e' key behavior
class VirgoInteractorStyle(vtk.vtkInteractorStyleTrackballCamera):
    """
    Virgo Interactor Style which adds some capabilities on top of the VTK
    provided vtkInteractorStyleTrackballCamera
    """
    def __init__(self):
        super().__init__()
        self.actor = None
        self.renderer = None
        self.AddObserver("CharEvent", self.onChar)
        self.AddObserver(vtk.vtkCommand.EndInteractionEvent, self.StoreRelativeCameraInfo)
        self.relative_offset = None  # Positional offset between camera and followed actor
        self.view_up = None    # Camera GetViewUp() direction when self.realtive_offset was stored

    def set_actor(self, actor):
        self.actor = actor
    def set_renderer(self, renderer):
        self.renderer = renderer

    # Override base class onChar to ignore the 'e' key
    def onChar(self, obj, event):
        key = self.GetInteractor().GetKeySym()
        if key != 'e':  # Ignore 'e' key to prevent exit
            super().OnChar()

    def StoreRelativeCameraInfo(self, caller, event):
        '''
        Store off the relative offset between the followed actor and the camera
        in self.relative_offset. This is called as a callback tied to
        vtk.vtkCommand.EndInteractionEvent which lets us store the camera information
        relative to self.actor when the interaction with the mouse completes

        TODO: I'm not convinced this callback needs to live in the interactor,
            it might be better placed in the bigger Controller class especially
            since we aren't really overriding any methods of
            vtkInteractorStyleTrackballCamera in this approach
        '''
        if not self.actor or not self.renderer:
            return
        # Update relative offset after user interaction
        camera = self.renderer.GetActiveCamera()
        matrix = self.actor.GetMatrix()
        actor_pos =[ matrix.GetElement(0, 3),  matrix.GetElement(1, 3), matrix.GetElement(2, 3)]
        # Store off the relative offset of camera
        self.relative_offset = np.array([camera.GetPosition()[i] - actor_pos[i] for i in range(3)])
        self.view_up = camera.GetViewUp()

    def GetRelativeCameraInfo(self, info):
        '''
        Get the relative offset between the followed actor and the camera
        '''
        #print(f"DEBUG: Getting relative_offset: {self.relative_offset}")
        if   info == 'position':
            return self.relative_offset
        elif info == 'view_up':
            return self.view_up
        else:
            return None


class VirgoDataPlaybackActor(vtk.vtkActor):
    """
    Wrapper around VTK actor to facilitate the Trick Logged data
    and other capabilities
    """
    @staticmethod
    def assert_sorted(lst):
        if not all(lst[i] <= lst[i + 1] for i in range(len(lst) - 1)):
            raise ValueError("List is not sorted in ascending order.")
    def __init__(self, mesh, offset_pos=None, offset_pyr=None, times=None,
                 positions=None, rotations=None, name='No Name', fontsize=12):
        """
        Initialize this instance.

        >>> va = VirgoDataPlaybackActor(mesh=os.path.join(thisFileDir,"meshes/teapot.obj"), name='tea')
        >>> va.name
        'tea'

        Parameters
        ----------
        mesh : str
            3D representation of the actor in the scene, which is either 
             a path to an object file to load like .obj, .stl, or a supported
             PREFAB (TODO more info needed here)
        offset_pos : list of 3 doubles
             X-Y-Z position to apply to the model on
             init. Used to adjust position of a model relative to the origin of
             of the model as provided by the CAD or PREFAB itself
        offset_pyr : list of 3 doubles
             X-Y-Z (Pitch-Yaw-Roll) rotation in degrees to apply to the model on
             init. Used to rotate a model into another frame immediately upon
             model creation. This rotation is performed after offset_pos is applied.
        times : sorted list of ascending doubles
            List representing all time steps for all data associated with this
            actor
        positions : List of 3-size tuples of doubles
            List represents (x, y, z) positions of this actor in the world frame,
            one per entry in times list. Length of positions must == length of times
        rotations : List of 3X3 numpy array of doubles
            List represents 3X3 (rotation matrix) of this actor, one per entry
            in times list.  Length of rotations must == length of times
        name : str
            Name given to this actor
        fontsize : int
            Default fontsize for text associated with this actor

        TODO: Probably need init_pos offset, and to consider how this class would
          operate if the times, positions, rotations list is too big to fit into
          memory
        """
        # Call the parent class constructor to ensure proper initialization
        super().__init__()

        self.mesh  = mesh
        self._map_mesh(mesh)
        self.offset_pos  = offset_pos     # Initialization position offset to apply to to actor
        self.offset_pyr  = offset_pyr     # Initialization pitch/yaw/roll to apply to to actor
        self.axes_label_render_threshold = 50    # min distance axes labels can be seen
        
        self.name = name            # Name of this actor
        self.fs = fontsize          # Font size for text associated with this actor
        self._axes = None           # Cartesian axes for this actor
        # Lists of state data
        self._times = times
        self._positions = positions 
        self._rotations = rotations 
        #import pdb; pdb.set_trace()
        self._last_time    = 0.0      # Last time associated with this actor
        self._last_time_idx = 0       # Index in self._times associated with self._last_time
        self._current_time = 0.0      # Time associated with this actor's data right now, must equal a value in self._times
        self._current_time_idx = 0    # Index in self._times associated with self._current_time
        self.static = False       # If true, this actor never moves
        self.initialized = False  # True only if this actor has all needed data to be used
        if self.offset_pos:
          # Apply the initial position/rotation to this object
          self.AddPosition(offset_pos)
        # TODO: PYR may not be sufficient for end users, we may need to consider
        # different rotation schemes
        if self.offset_pyr:
          # Apply the initial position/rotation to this object
          self.RotateX(self.offset_pyr[0])
          self.RotateY(self.offset_pyr[1])
          self.RotateZ(self.offset_pyr[2])

    def set_static(self, value=True):
        self.static = value

    def initialize(self):
        self.verify()

    def set_times(self, times):
        self._times = times

    def set_positions(self, positions):
        self._positions = positions

    def set_rotations(self, rotations):
        self._rotations = rotations

    def _map_mesh(self, mesh):
        """
        Given a string mesh (a path to a model file or a PREFAB), set up the vtk
        mapper with the configuration of geometry given
        """
        # TODO: this is quick and dirty, need a PREFAB management class that VirgoDataPlaybackActor uses
        mapper = vtk.vtkPolyDataMapper()
        texture = None
        if 'PREFAB:sphere' in str(mesh):
            # Create a sphere source
            sphere_source = vtk.vtkSphereSource()
            sphere_source.SetRadius(1.0)  # Set radius of the sphere
            sphere_source.SetThetaResolution(300)  # Number of divisions in theta (longitude)
            sphere_source.SetPhiResolution(300)  # Number of divisions in phi (latitude)
            
            # Create a mapper to map the sphere's geometry to graphics primitives
            mapper.SetInputConnection(sphere_source.GetOutputPort())
        elif 'PREFAB:earth' in str(mesh):
            # Create a sphere source
            earth_source = vtk.vtkTexturedSphereSource()
            earth_source.SetRadius(6371000.0)     # Set radius of earth
            earth_source.SetThetaResolution(300)  # Number of divisions in theta (longitude)
            earth_source.SetPhiResolution(300)  # Number of divisions in phi (latitude)
            # Read Earth texture image
            reader = vtk.vtkJPEGReader()
            reader.SetFileName(os.path.join(thisFileDir, 'images/earth/2k_earth_daymap.jpg'))
            texture = vtk.vtkTexture()
            texture.SetInputConnection(reader.GetOutputPort())
            texture.InterpolateOn()
            # Create a mapper to map the sphere's geometry to graphics primitives
            mapper.SetInputConnection(earth_source.GetOutputPort())
        elif 'PREFAB:moon' in str(mesh):
            # Create a sphere source
            earth_source = vtk.vtkTexturedSphereSource()
            earth_source.SetRadius(1740000.0)     # Set radius of moon
            earth_source.SetThetaResolution(300)  # Number of divisions in theta (longitude)
            earth_source.SetPhiResolution(300)  # Number of divisions in phi (latitude)
            # Read Moon texture image
            reader = vtk.vtkJPEGReader()
            reader.SetFileName(os.path.join(thisFileDir, 'images/moon/lroc_color_poles_4k.jpg'))
            texture = vtk.vtkTexture()
            texture.SetInputConnection(reader.GetOutputPort())
            texture.InterpolateOn()
            # Create a mapper to map the sphere's geometry to graphics primitives
            mapper.SetInputConnection(earth_source.GetOutputPort())
        elif 'PREFAB:cube' in str(mesh):
            cube_source = vtk.vtkCubeSource()
            cube_source.SetXLength(1.0)
            cube_source.SetYLength(1.0)
            cube_source.SetZLength(1.0)
            # Create a mapper to map the cube's geometry to graphics primitives
            mapper.SetInputConnection(cube_source.GetOutputPort())
        elif '.obj' in str(mesh):
            # Read in the geometry
            reader = vtk.vtkOBJReader()
            reader.SetFileName(mesh)
            reader.Update()
            # Create a mapper and set the reader's output
            mapper.SetInputConnection(reader.GetOutputPort())
        elif '.stl' in str(mesh):
            # Read in the geometry
            reader = vtk.vtkSTLReader()
            reader.SetFileName(mesh)
            reader.Update()
            # Create a mapper and set the reader's output
            mapper.SetInputConnection(reader.GetOutputPort())

        # Set the mapper to the actor
        self.SetMapper(mapper)
        if texture:
            self.SetTexture(texture)

    def set_axes(self, axes):
        self._axes = axes

    def report(self):
        """
        Debug prints
        """
        print(f"{self.name}:")
        print(f"  position: {self.get_current_position()}")
        print(f"  rotation: {self.get_current_rotation()}")

    def verify(self):
        """
        Checks for required assumptions of the model and raises if any issues found
        Assigns self.initialized = True when all conditions are met
        TODO: Add other checks if we add other paramters
        """
        if not self.static:
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

        # If we made it this far, the actor is fully initialized
        self.initialized = True

    def get_current_time(self):
        """
        Return current time associated with data for this actor. This is not
        global scene time, this is a value from a TrickPy generated array of the
        first column of tricked logged data.
        """
        return(self._current_time)

    def get_current_time_idx(self):
        """
        Return current time index associated with data for this actor. This is
        not global scene time, this is a value from a TrickPy generated array of
        the first column of tricked logged data.
        """
        return(self._current_time_idx)

    def set_current_time_idx(self, idx):
        """
        Set the current time index associated with data for this actor. This is
        not global scene time, this is an index from a TrickPy generated array
        of the first column of tricked logged data.
        """
        if self._times:
            if idx < len(self._current_time):
                self._current_time_idx = idx
            else:
                raise ValueError(f"Cannot set time index {idx} as it's longer than len(self._current_time)")

    def increment_current_time(self,):
        """
        Increase the self._current_time_idx and update self.current_time accordingly
        """
        if not self.initialized or self.static:
            return None
        self._current_time_idx += 1
        self._current_time = self._times[self._current_time_idx]

    def decrement_current_time(self,):
        """
        Decrease the self._current_time_idx and update self.current_time accordingly
        """
        if not self.initialized or self.static:
            return None
        self._current_time_idx -= 1
        self._current_time = self._times[self._current_time_idx]

    def get_next_time(self, world_time):
        """
        Return the time from self._times after given world_time
        """
        if not self.initialized or self.static:
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
        if not self.initialized or self.static:
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
        if not self.initialized or self.static:
            return None

        self._current_time_idx = self._find_index(mylist=self._times, input_value=time, mode=strategy)
        self._current_time = self._times[self._current_time_idx]
        return (self._current_time_idx)

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

    def update(self, world_time):
        """
        Update this actor's position and rotation to the values associated
        with data closest to world_time (see set_current_time) by applying
        a matrix transform to self.
        """
        if not self.initialized or self.static:
            return # Nothing to do
        self.set_current_time(world_time)
        pos = self.get_current_position()
        rot = self.get_current_rotation()
        if pos == None and rot == None:
            return  # Nothing to update

        # Convert rotation matrix to VTK transform
        transform = vtk.vtkTransform()
        matrix = vtk.vtkMatrix4x4()
        if rot is not None:
            for i in range(3):
                for j in range(3):
                    matrix.SetElement(i, j, rot[i][j])
        if pos is not None:
            matrix.SetElement(0, 3, pos[0])
            matrix.SetElement(1, 3, pos[1])
            matrix.SetElement(2, 3, pos[2])
        transform.SetMatrix(matrix)
        self.SetUserTransform(transform)

        # Move the axes to where the parent actor is
        self._axes.SetUserTransform(transform)

    def are_axes_visible(self):
        if not self._axes:
            return False
        return(self._axes.GetVisibility())

    def hide_axes(self):
        if not self._axes:
            return
        # Make the entire axes not visible
        self._axes.SetVisibility(False)
        self.GetProperty().SetOpacity(1.0)

    def show_axes(self):
        if not self._axes:
            return
        # Make the entire axes visible
        #print(f"DEBUG: turning visibility of {self.name} axes on.")
        self._axes.SetVisibility(True)
        self.GetProperty().SetOpacity(0.7)


    def hide_or_unhide_axes_labels(self, camera):
        """
        Turn the axes labels off if actor is far enough from the camera

        TODO: this looks funky if you're close to the actor but it's behind the
            camera, we also need a check that it's visible, or maybe we don't need
            this feature at all
        """
        if not self._axes:
            return
        camera_pos = camera.GetPosition()
        matrix = self._axes.GetMatrix()
        axes_pos =[ matrix.GetElement(0, 3),  matrix.GetElement(1, 3), matrix.GetElement(2, 3)]
        distance = math.sqrt(
            (camera_pos[0] - axes_pos[0])**2 +
            (camera_pos[1] - axes_pos[1])**2 +
            (camera_pos[2] - axes_pos[2])**2
        )
        self._axes.SetAxisLabels(distance < self.axes_label_render_threshold)

class VirgoDataPlaybackControlCenter:
    """
    The control center class for Virgo Data playback
    """
    def __init__(self, renderer, interactor, scene, data_record_groups, world_time=0.0):
        """
        Constructor
        """
        # Current scene world time (not necessarily aligned with sim data time)
        self.world_time = self.world_time_start = world_time 
        self.wallclock_time = time.time()  # Actual wall clock time in real life
        self.max_sim_time = 0.0            # highest sim time across all dr groups
        self.mode = 'PLAYING'              # 'PAUSED' or 'PLAYING'
        self.fs = 14      # font size
        self.renderer = renderer
        self.camera = renderer.GetActiveCamera()
        self.camera_follows = None  # if the camera should follow an actor, this is the one
        # TODO this hardcoded value assummes the actor is HUGE
        self.camera_follow_offset = None
        self.interactor = interactor
        self.scene = scene
        self.data_record_groups = data_record_groups
        self.skybox = self.create_skybox()
        self.actors = {}
        self.text_actors={}

        self.help = False  # Display help message when true
        self.picker = vtk.vtkPropPicker()
        self.picked_actor      = None # Currently picked actor or None if not
        self.last_picked_actor = None
        self.original_colors = {}  # Store original colors for actors

        # TODO: do we need to check this is smaller than minimum delta t in logged data?
        self.frame_rate = 50 # Frame rate for timer callback (millisec)
        # self.dt: Increment time by this amount every timer update (sec)
        self.default_dt = self.dt = self.frame_rate / 1000.0
        self.playback_speed = 1.0  # Speed of playback
        # TODO make this configurable in the YAML file
        self.available_speeds = [1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0]
        self.huge = 1.0e30   # A huge floating point number used for finding smallest values
        self._initialized = False
        self.verbosity = 1

    def initialize(self):
        """
        Set up the scene with actors
        """
        self.init_actors()
        self.text_actors['mode'] = self.create_text_actor()
        self.text_actors['picked'] = self.create_text_actor(pos=[10,10])
        self.text_actors['time'] = self.create_text_actor()
        self.text_actors['help'] = self.create_text_actor()
        self.text_actors['camera'] = self.create_text_actor()
        self.text_actors['warning'] = self.create_text_actor()

        # Add actors in scene
        for a in self.actors:
            self.renderer.AddActor(self.actors[a])
        for t in self.text_actors:
            self.renderer.AddActor(self.text_actors[t])

        self.init_camera()
        self._initialized = True

    def is_initialized(self):
        return self._initialized

    def get_verbosity(self):
        return self.verbosity

    def set_verbosity(self, value):
        if not isinstance(value, int):
            print("ERROR: verbosity must be an integer")
        else:
            self.verbosity = value

    def add_axes_to_actors(self):
        """
        Add a vtkAxesActor to each actor in self.actors, positioned at the actor's origin.

        TODO: this should probably be moved to inside the VirgoDataPlaybackActor class
        """
        for name, actor in self.actors.items():
            # Skip non-pickable actors. TODO: we need more axes options, this is a quick fix
            if not actor.GetPickable():
                continue
            # Create axes actor
            axes = vtk.vtkAxesActor()
            
            # Get size of actor to automatically scale size of axes
            bounds = actor.GetBounds()
            # Bounds are returned as (xmin, xmax, ymin, ymax, zmin, zmax)
            center = [(bounds[0] + bounds[1]) / 2.0,  # x-center
                      (bounds[2] + bounds[3]) / 2.0,  # y-center
                      (bounds[4] + bounds[5]) / 2.0]  # z-center
            # Calculate the size of the bounding box
            size = [bounds[1] - bounds[0],  # x-size
                    bounds[3] - bounds[2],  # y-size
                    bounds[5] - bounds[4]]  # z-size
            # Calculate arrow length as avg of largest and smallest bounding box dimension
            length = (max(size) + min(size))/2.0
            # TODO make this arrow scale a value changeable in the YAML file
            scale = 0.5
            # Set axes properties
            axes.SetTotalLength(scale*length, scale*length, scale*length)  # Size of axes (x, y, z lengths)
            axes.SetShaftTypeToCylinder()  # Cylindrical shafts for visibility
            axes.SetAxisLabels(True)  # Show x, y, z labels
            axes.SetConeRadius(0.5)  # Size of arrowheads
            
            # Customize colors for clarity
            axes.GetXAxisCaptionActor2D().GetProperty().SetColor(1, 0, 0)  # Red X
            axes.GetYAxisCaptionActor2D().GetProperty().SetColor(0, 1, 0)  # Green Y
            axes.GetZAxisCaptionActor2D().GetProperty().SetColor(0, 0, 1)  # Blue Z

            # Set font size for axis labels
            axes.GetXAxisCaptionActor2D().GetTextActor().GetTextProperty().SetFontSize(int(self.fs*2))
            axes.GetXAxisCaptionActor2D().GetTextActor().SetTextScaleModeToNone()
            axes.GetYAxisCaptionActor2D().GetTextActor().GetTextProperty().SetFontSize(int(self.fs*2))
            axes.GetYAxisCaptionActor2D().GetTextActor().SetTextScaleModeToNone()
            axes.GetZAxisCaptionActor2D().GetTextActor().GetTextProperty().SetFontSize(int(self.fs*2))
            axes.GetZAxisCaptionActor2D().GetTextActor().SetTextScaleModeToNone()
            
            # Optionally set font family (e.g., to match your text actor's Courier)
            axes.GetXAxisCaptionActor2D().GetTextActor().GetTextProperty().SetFontFamilyToCourier()
            axes.GetYAxisCaptionActor2D().GetTextActor().GetTextProperty().SetFontFamilyToCourier()
            axes.GetZAxisCaptionActor2D().GetTextActor().GetTextProperty().SetFontFamilyToCourier()
            
            # Fallback to center if no origin
            origin = actor.GetOrigin() or actor.GetCenter()
            if origin is None:  # Fallback if GetCenter() fails
                origin = [0, 0, 0]
            
            # Set the position of the axes to the actor's origin
            axes.SetPosition(origin[0], origin[1], origin[2])
            axes.SetVisibility(False) # Default to not visible
            
            # Add axes to renderer
            self.renderer.AddActor(axes)
            
            # Store axes actor for potential updates
            actor.set_axes(axes)
            # TODO: This should probably be set inside the actor based on stored actor bounds
            # when the actor is init'd
            actor.axes_label_render_threshold = length*20

    def init_actors(self):
        '''
        Create the VTK actors from the scene dictionary and initialize
        their configurable parameters

        Populates self.actors dict from self.scene dict
        self.actors takes the form:
          key: Name of actor
          value: VirgoDataPlaybackActor object as loaded from self.scene data

        TODO all this checking for dict/YAML validity needs to be safer, i.e.
        checked in a YAML verifier before getting here similar to what
        TrickOps does with TrickWorkflowYamlVerifier. See VirgoDataPlayback.
        _verify_scene()

        ASSUMPTION: self.scene['actors'][a]['mesh'] is path defined
        relative to current os.getcwd()
        '''
        def convert_tmat_to_numpy_arrays(dr, tmat_var, list_of_times):
            '''
            dr: data record group from TrickPy
            tmat_var:  variable name which has 3x3 structure
            Returns: rotations_list which contains a list of 3x3 numpy arrays,
              one for each index in list_of_times

            TODO: certainly there's a better way to do this, right?
            '''
            rotations_list = []
            tidx = 0
            for t in list_of_times:
                rotations_list.append( np.array(
                [  [ dr[var+'[0][0]'][tidx], dr[var+'[0][1]'][tidx], dr[var+'[0][2]'][tidx] ],
                   [ dr[var+'[1][0]'][tidx], dr[var+'[1][1]'][tidx], dr[var+'[1][2]'][tidx] ],
                   [ dr[var+'[2][0]'][tidx], dr[var+'[2][1]'][tidx], dr[var+'[2][2]'][tidx] ] ] )
                ) 
                tidx += 1
            return rotations_list
        for a in self.scene['actors']:
            if self.verbosity > 0:
                print(f"Initializing {a} ...")
            # Figure out the details of whether the actor is driven or not to prepare
            # inputs for initializing the actor
            positions = None
            rotations = None
            times = None
            driven_by= self.scene['actors'][a]['driven_by']
            if driven_by != 0:
                time = driven_by['time']
                group = driven_by['pos'].split(' ')[0]
                var   = driven_by['pos'].split(' ')[1]
                times = self.data_record_groups[group][time].tolist()
                # Store off the highest simulation time across all actors
                if times[-1] > self.max_sim_time:
                    self.max_sim_time = times[-1]
                # ASSUMPTION: pos as specified in the YAML file is an array of length 3
                positions=list(zip(
                    self.data_record_groups[group][var+'[0]'].tolist(),
                    self.data_record_groups[group][var+'[1]'].tolist(),
                    self.data_record_groups[group][var+'[2]'].tolist()))
                group = driven_by['rot'].split(' ')[0]
                var   = driven_by['rot'].split(' ')[1]
                # ASSUMPTION: rot as specified in the YAML file is a 3X3 sized array
                rotations = convert_tmat_to_numpy_arrays(self.data_record_groups[group], var, times)
            # Create the Actor
            self.actors[a] = VirgoDataPlaybackActor(
                mesh=self.scene['actors'][a]['mesh'],
                offset_pos=self.scene['actors'][a]['pos'],
                offset_pyr=self.scene['actors'][a]['pyr'],
                times=times,
                positions=positions,
                rotations=rotations,
                name=a,
                fontsize=self.fs
                )
            if driven_by == 0: # If nothing drives this, set static flag to True
                self.actors[a].set_static(True)
            self.actors[a].initialize()
            self.actors[a].SetScale(self.scene['actors'][a]['scale'])
            if 'color' in self.scene['actors'][a]:
                self.actors[a].GetProperty().SetColor(self.scene['actors'][a]['color'])
            if 'pickable' in self.scene['actors'][a] and self.scene['actors'][a]['pickable'] == 0:
                self.actors[a].PickableOff()

        self.add_axes_to_actors()
        self.update_actors()

    def create_text_actor(self, pos=[0, 0]):
        text = vtk.vtkTextActor()
        text.GetTextProperty().SetFontFamilyToCourier()
        text.GetTextProperty().SetFontSize(self.fs)
        text.GetTextProperty().SetColor(1, 1, 1)
        text.SetDisplayPosition(pos[0], pos[1])
        return text

    def update_actors(self):
        '''
        Move actors into position based on world time compared to data
        '''
        for actor in self.actors:
            self.actors[actor].update(self.world_time)

    def update_scene(self):

        if self.mode == 'PLAYING':
          if self.world_time <= self.max_sim_time:
            self.world_time += self.dt
            self.update_actors()
            if self.camera_follows:
                self.camera_follow(self.camera_follows)
          else:
             self.world_time = self.world_time_start

        self.configure_hud()
        # Toggle axes labels based on distance
        for actor in self.actors:
            self.actors[actor].hide_or_unhide_axes_labels(self.camera)
        # I DONT KNOW WHY BUT THIS ONE LINE FIXES THE STARS IN THE BACKGROUND
        self.renderer.ResetCameraClippingRange() # This will auto-adjust clipping based on visible actors

        # THIS VALUE IS CRITICAL, SMALLER NUMBERS (0.00001) MAKE THE BACKGROUND
        # SKYBOX STARS WIGGLE BUT LARGER NUMBERS (0.0001) MAKE SMALLER ACTORS NOT
        # VISIBLE DUE TO CLIPPING. TODO this probably shouldn't be set every frame 
        self.renderer.SetNearClippingPlaneTolerance(0.00005)
        self.renderer.GetRenderWindow().Render()


    def on_timer(self, caller, event):
        self.wallclock_time = time.time()  # Actual wall clock time in real life
        self.update_scene()

        time_one_frame_took = time.time() - self.wallclock_time
        if time_one_frame_took > self.dt:
            print(f"WARNING: Frame took {time_one_frame_took} sec to complete which is "
                  f"larger than self.dt ({self.dt}). Playback rate may not be accurate")

    def configure_hud(self):
        '''
        Configure the heads-up-display in preparation for rendering.
        Does not render.
        '''
        hud_padding = 20 # pixels
        window_width, window_height  = self.interactor.GetRenderWindow().GetSize()
        
        ############################################################################
        # Picked Actor (or last picked if nothing picked) information in bottom left
        ############################################################################
        actor = self.picked_actor if self.picked_actor else self.last_picked_actor
        if actor:
            matrix = actor.GetMatrix()
            # Extract translation (elements 3, 7, 11 in 4x4 matrix)
            pos =[ matrix.GetElement(0, 3),  matrix.GetElement(1, 3), matrix.GetElement(2, 3)]
            name = self.get_actor_name(actor)
            self.text_actors['picked'].SetInput(
                f"{name}\n World position: {pos[0]:<10.5f}, {pos[1]:<10.5f}, {pos[2]:<10.5f} meters"
                )
        else:
            self.text_actors['picked'].SetInput("")

        ############################################################################
        # Mode displayed in top right
        ############################################################################
        self.text_actors['mode'].SetInput(f"{self.mode} {self.playback_speed}X")
        bounds = [0] * 4 # Get the text bounding box in display coordinates  [xmin, xmax, ymin, ymax]
        self.text_actors['mode'].GetBoundingBox(self.renderer, bounds)
        text_width = bounds[1] - bounds[0] + 1  # Width in pixels
        text_height = bounds[3] - bounds[2] + 1  # Height in pixels
        # Calculate position for bottom-right corner with padding
        x_pos = window_width - text_width - hud_padding  # Right edge minus width
        y_pos = window_height - text_height - hud_padding  # Bottom edge (20 pixels from bottom)
        self.text_actors['mode'].SetPosition(x_pos, y_pos)
        
        ############################################################################
        # Time in top left
        ############################################################################
        self.text_actors['time'].SetPosition(hud_padding, y_pos)
        percent_complete = self.world_time / self.max_sim_time * 100
        self.text_actors['time'].SetInput(
            f"\nWorld time: {self.world_time:<10.5f} / {self.max_sim_time} sec [{percent_complete:<4.2f} %]"
            )
        ############################################################################
        # Help message in bottom right (if active)
        ############################################################################
        if self.help == True:
            #self.text_actors['help'].SetDisplayPosition(window_width - 600, 40)
            self.text_actors['help'].SetInput(
                f"\nMOUSE"
                f"\n L-click drag: Rotate"
                f"\n Shift+L-click: Pan"
                f"\n Ctrl+L-click: Roll"
                f"\n Scroll wheel: Zoom"
                f"\n L-click: Pick Actor"
                f"\n"
                f"\nKEYBOARD"
                f"\n SPACE: Pause/Play"
                f"\n S: Cycle playback speeds"
                f"\n R: Fit camera to scene"
                f"\n <- -> : Step back/forward in time"
                f"\n  -  + : Embiggen/Ensmallen text"
                f"\n A: Toggle Axes Visibility"
                f"\n H: Toggle this help message"
                f"\n BackSpace: Toggle stars"
                f"\n Q: Quit"
                )
            self.text_actors['help'].GetBoundingBox(self.renderer, bounds)
            text_width = bounds[1] - bounds[0] + 1  # Width in pixels
            text_height = bounds[3] - bounds[2] + 1  # Height in pixels
            # Calculate position for bottom-right corner with 20-pixel padding
            x_pos = window_width - text_width - hud_padding    # Right edge minus width
            y_pos = hud_padding  # Bottom edge (20 pixels from bottom)
            self.text_actors['help'].SetPosition(x_pos, y_pos)

        else:
            self.text_actors['help'].SetInput("")

        ############################################################################
        # Camera info displayed in top center-right-ish
        ############################################################################
        camera_info = f"Camera: Follow {self.camera_follows.name}" if self.camera_follows else "Camera: Free" 
        self.text_actors['camera'].SetInput(f"{camera_info}")
        bounds = [0] * 4 # Get the text bounding box in display coordinates  [xmin, xmax, ymin, ymax]
        self.text_actors['camera'].GetBoundingBox(self.renderer, bounds)
        text_width = bounds[1] - bounds[0] + 1  # Width in pixels
        text_height = bounds[3] - bounds[2] + 1  # Height in pixels
        # Calculate position for bottom-right corner with padding
        x_pos = (window_width - text_width - hud_padding)/1.3  # 2/3ish the way over on right side
        y_pos = window_height - text_height - hud_padding  # Top edge (20 pixels from bottom)
        self.text_actors['camera'].SetPosition(x_pos, y_pos)

        ############################################################################
        # Prototype Warning
        ############################################################################
        bounds = [0] * 4 # Get the text bounding box in display coordinates  [xmin, xmax, ymin, ymax]
        self.text_actors['picked'].GetBoundingBox(self.renderer, bounds)
        text_height = bounds[3] - bounds[2] + 1  # Height in pixels
        self.text_actors['warning'].GetTextProperty().SetColor(1.0, 0.0, 0.0)
        self.text_actors['warning'].SetPosition(hud_padding, text_height + hud_padding)
        self.text_actors['warning'].SetInput(
            f"YOU ARE USING A PROTOTYPE NOT"
            f"\nSUITABLE FOR ENGINEERING ANALYSIS"
            )

    def get_actor_name(self, vtk_obj):
        for a in self.actors:
            if vtk_obj is self.actors[a]:
                return a
        return "UNKNOWN ACTOR"

    def focus_camera_on(self, actor):
        """
        Point the camera at the given actor
        Assumes actor is positioned with matrix data, not position data
        """
        matrix = actor.GetMatrix()
        pos =[ matrix.GetElement(0, 3),  matrix.GetElement(1, 3), matrix.GetElement(2, 3)]
        self.camera.SetFocalPoint(pos[0], pos[1], pos[2])

    def camera_follow(self, actor):
        """
        Move the camera to follow the given actor
        Assumes actor is positioned with matrix data, not position data
        """
        # Ask the interactor for the latest camera position
        self.camera_follow_offset = self.interactor.GetInteractorStyle().GetRelativeCameraInfo('position')
        # If it's not set, compute the initial offset from the bounding box of the actor
        if self.camera_follow_offset is None:
            # Get the bounding box of the actor
            bounds = actor.GetBounds()
            # Bounds are returned as (xmin, xmax, ymin, ymax, zmin, zmax)
            center = [(bounds[0] + bounds[1]) / 2.0,  # x-center
                      (bounds[2] + bounds[3]) / 2.0,  # y-center
                      (bounds[4] + bounds[5]) / 2.0]  # z-center
            # Calculate the size of the bounding box
            size = [bounds[1] - bounds[0],  # x-size
                    bounds[3] - bounds[2],  # y-size
                    bounds[5] - bounds[4]]  # z-size
            # Determine an offset distance based on the bounding box size
            # For example, place the camera at a distance of 5 times the maximum dimension
            max_dimension = max(size)
            offset_distance = max_dimension * 20.0
            self.camera_follow_offset = np.array([offset_distance, offset_distance/2.0, 0.0])

        matrix = actor.GetMatrix()
        pos =np.array([ matrix.GetElement(0, 3),  matrix.GetElement(1, 3), matrix.GetElement(2, 3)])
        camera_pos = pos + self.camera_follow_offset
        # Set camera position
        self.camera.SetPosition(camera_pos[0], camera_pos[1], camera_pos[2])
        # Set focal point to spacecraft's center
        self.camera.SetFocalPoint(pos[0], pos[1], pos[2])
        # Ask the interactor for the stored off view_up direction from end of interaction
        view_up = self.interactor.GetInteractorStyle().GetRelativeCameraInfo('view_up')
        # If view_up isn't defined, default it to "positive Z is up"
        if not view_up:
            view_up = (0.0, 0.0, 1.0)
        self.camera.SetViewUp(view_up)

    def on_left_click(self, obj, event):
        """
        Callback for left click actions. This currently provides "picking", assigning
        self.picked_actor to the actor the mouse clicked on, and changing it's color
        to self.scene['highlight_color']
        """
        click_pos = self.interactor.GetEventPosition()
        self.picker.Pick(click_pos[0], click_pos[1], 0, self.renderer)

        self.picked_actor = self.picker.GetActor()
        if self.picked_actor and self.picked_actor in self.actors.values():
            self.configure_hud()
            self.focus_camera_on(self.picked_actor)
        if self.last_picked_actor:
            self.last_picked_actor.GetProperty().SetColor(self.original_colors[self.last_picked_actor])

        if self.picked_actor:
            # Store original color if not already stored
            if self.picked_actor not in self.original_colors:
                original_color = self.picked_actor.GetProperty().GetColor()
                self.original_colors[self.picked_actor] = original_color
            
            # Restore color of the previously picked actor
            if self.last_picked_actor and self.last_picked_actor != self.picked_actor:
                self.last_picked_actor.GetProperty().SetColor(self.original_colors[self.last_picked_actor])
            
            # Highlight the current picked actor
            self.picked_actor.GetProperty().SetColor(self.scene['highlight_color'])
            # Get the X,Y,Z coordinates of the actor's orign
            matrix = self.picked_actor.GetMatrix()
            actor_pos =[ matrix.GetElement(0, 3),  matrix.GetElement(1, 3), matrix.GetElement(2, 3)]
            
            # Update last picked actor
            self.last_picked_actor = self.picked_actor
            
        self.renderer.Render()

    def toggle_axes(self):
        for a in self.actors:
            if self.actors[a].are_axes_visible():
                self.actors[a].hide_axes()
                #print(f'DEBUG: Hiding axes for {a}')
            else:
                self.actors[a].show_axes()
                #print(f'DEBUG: Showing axes for {a}')

    def fontsize(self, direction='up'):
        """
        Increment/decrement fontsize of all text actors given the direction
        """
        increment = 2 if direction == 'up' else -2
        for t in self.text_actors:
            self.fs = self.text_actors[t].GetTextProperty().GetFontSize() + increment
            self.text_actors[t].GetTextProperty().SetFontSize(self.fs)
        # Adjust the fontsize inside the actor instances
        # TODO this should probably be done with a setter rather than straight assignment
        # Also this was intended for axes label size but I never quite got that to work
        for a in self.actors:
            self.actors[a].fs = self.fs

    def on_key_press(self, caller, event):
        key = self.interactor.GetKeySym()
        #print(f'DEBUG: Key pressed: {key}')
        if key == 'equal':
            self.fontsize('up')
        if key == 'minus':
            pass
            self.fontsize('down')
        if key == 'a':
            # Turn actor axes on/off
            self.toggle_axes()
        if key == "s":
            # Cycle through the available playback speeds
            self.cycle_playback_speed()
        if key == "h":
            self.help = True if self.help == False else False
        if key == "BackSpace":
            # Toggle skybox visibility
            self.skybox.SetVisibility(False) if self.skybox.GetVisibility() else self.skybox.SetVisibility(True)
        if key == "v":
            # Verbosely print state of all non-text actors
            for actor in self.actors:
                self.actors[actor].report()
        if key == "c":
            self.camera_report()
        if key == "space":
            self.mode = 'PLAYING' if self.mode == 'PAUSED' else 'PAUSED'
            self.text_actors['mode'].SetInput(self.mode)
        if key == "Left" or key =='comma':
            self.mode = 'PAUSED'
            self.decrement_time()
            self.update_actors()
        if key == "Right" or key == 'period':
            self.mode = 'PAUSED'
            self.increment_time()
            self.update_actors()
        self.update_scene()

    def cycle_playback_speed(self):
        """
        Cycle the playback speed by iterating forward in the list of 
        self.available_speeds
        """
        # Find our current speed index in the list of available speeds
        idx = self.available_speeds.index(self.playback_speed)
        if idx < len(self.available_speeds) - 1:
            self.playback_speed = self.available_speeds[idx+1]
        else:
            self.playback_speed = self.available_speeds[0]
        # Adjust self.dt so the scene runs based on new speed
        self.dt = self.default_dt * self.playback_speed

    def decrement_time(self):
        """
        Decrement world time by the minimum resolution across all actors logged data

        TODO: This has not been tested with multiple data record groups logging at
        specific rates.
        """
        largest_time = - self.huge # Negative Huge
        for a in self.actors:
            pt = self.actors[a].get_previous_time(self.world_time)
            if pt is None:
                continue
            if pt > largest_time:
                largest_time = pt
        # Decrease the world time
        if largest_time > - self.huge:
            #print(f"Setting (decrement) world_time to {largest_time}")
            self.world_time = largest_time
            for a in self.actors:
                self.actors[a].set_current_time(self.world_time, strategy='closest')
                
    def increment_time(self):
        """
        Increment world time by the minimum resolution across all actors logged data

        TODO: This has not been tested with multiple data record groups logging at
        specific rates.
        """
        smallest_time = self.huge # Huge
        for a in self.actors:
            nt = self.actors[a].get_next_time(self.world_time)
            if nt is None:
                continue
            if nt < smallest_time:
                smallest_time = nt
        # Decrease the world time
        if smallest_time < self.huge:
            #print(f"Setting (increment) world_time to {smallest_time}")
            self.world_time = smallest_time
            for a in self.actors:
                self.actors[a].set_current_time(self.world_time, strategy='closest')

    # Put the camera in a nice position to see docking
    def init_camera(self):
        """
        Set the initial camera position

        TODO: The hardcoded values should probably be either removed entirely
        and replaced with whatever 'r' keypress is doing via the default
        behavior of vtk.vtkInteractorStyleTrackballCamera, or a better approach
        which places the camera automatically based on what's in the scene, or
        based on camera init information read from the YAML file

        TODO: The camera follow capability is locked to a single actor here, we
        should consider the ability to change which actor is followed during
        runtime. We could consider using self.picked_actor to drive the camera
        follow mechanism.
        """
        self.camera.SetPosition(487.9, 52.2, -11.35)
        self.camera.SetFocalPoint(0, 0, 0)
        self.camera.SetViewUp(-0.10, -0.09, -0.98)

        # If the camera is setup to follow an actor, save that actor in
        # self.camera_follows and pass that actor into the Interactor so
        # it can track camera information relative to an actor
        if 'camera' in self.scene and 'follow' in self.scene['camera']:
            for actor in self.actors:
                if self.actors[actor].name == self.scene['camera']['follow']:
                    self.camera_follows = self.actors[actor]
                    self.interactor.GetInteractorStyle().set_actor(self.camera_follows)
                    self.interactor.GetInteractorStyle().set_renderer(self.renderer)

    def camera_report(self):
        """
        Debug function for seeing where the camera is
        """
        print("Camera Position:", self.camera.GetPosition())
        print("  Focal Point:", self.camera.GetFocalPoint())
        print("  View Up:", self.camera.GetViewUp())
        print(f"  Clipping range: {self.camera.GetClippingRange()} ")
        print("Renderer info:")
        print(f"  NearClippingPlanTolerance: {self.renderer.GetNearClippingPlaneTolerance()} ")

    def register_callbacks(self):
        """
        Register all callbacks with the interactor
        """
        self.interactor.AddObserver("LeftButtonPressEvent", self.on_left_click)
        self.interactor.AddObserver("TimerEvent", self.on_timer)
        self.interactor.AddObserver("KeyPressEvent", self.on_key_press)
        self.timer_id = self.interactor.CreateRepeatingTimer(self.frame_rate)

    def create_skybox(self):
        """
        Create the skybox of the starfield using images from
        https://svs.gsfc.nasa.gov/4851
        """
        # Load cube map texture
        texture = vtk.vtkTexture()
        texture.CubeMapOn()
        texture.InterpolateOn()
        
        # File names for each direction
        faces = {
            'posx': os.path.join(thisFileDir, 'images/space_cubemap/posx.jpg'),
            'negx': os.path.join(thisFileDir, 'images/space_cubemap/negx.jpg'),
            'posy': os.path.join(thisFileDir, 'images/space_cubemap/posy.jpg'),
            'negy': os.path.join(thisFileDir, 'images/space_cubemap/negy.jpg'),
            'posz': os.path.join(thisFileDir, 'images/space_cubemap/posz.jpg'),
            'negz': os.path.join(thisFileDir, 'images/space_cubemap/negz.jpg')
        }
        
        # Map each direction to the correct image
        imageReaders = {}
        face_order = ['posx', 'negx', 'posy', 'negy', 'posz', 'negz']
        for i, face in enumerate(face_order):
            reader = vtk.vtkJPEGReader()
            reader.SetFileName(faces[face])
            reader.Update()
            texture.SetInputDataObject(i, reader.GetOutput())
        
        # Create the skybox
        skybox = vtk.vtkSkybox()
        skybox.SetTexture(texture)
        skybox.SetVisibility(True) # On by default

        self.renderer.AddActor(skybox)
        return skybox

class VirgoDataPlayback:
    """
    Class used as entrypoint for those wanting VIRGO data playback capability.

    Expects the consume a YAML file describing the scene and a Trick RUN
    directory containing logged data this scene will be driven by. The linkage
    between the data recording groups/variables and the actors in the scene
    is controlled by the "driven_by:" clause of the YAML file
    """
    def __init__(self, run_dir, scene, verbosity=1):
        self.verbosity = verbosity
        self.scene = scene # Dict of scene info from YAML file
        self._verify_scene()
        self.run_dir = run_dir
        import trickpy.data_record as dr
        # All logged data from run_dir in the dict TrickPy gives us
        # TODO: we can save on RAM by loading on the variables we care
        # about which we can get from self.scene, see the variables arg to dr.load_run() here:
        # https://web-fsl.jsc.nasa.gov/trickpy/latest/docs/trickpy.data_record.load_run.html#trickpy.data_record.load_run

        if self.verbosity > 0:
            print(f"Loading data from {self.run_dir}...")
        self.data_record_groups = dr.load_run(self.run_dir)

    def _load_variables_for_scene(self):
        pass

    def _verify_scene(self):
        """
        TODO: Call a dict verifier here, similar to TrickWorkflowYamlVerifier
        TODO: make sure we verify that the variables read from TrickPy actually exist
        """
        pass

    def run(self):
        """
        The entrypoint for starting up a rendered window
        """
        renderer = vtk.vtkRenderer()
        renderer.SetBackground(self.scene['background_color'])
    
        render_window = vtk.vtkRenderWindow()
        render_window.AddRenderer(renderer)
        render_window.SetSize(1920, 1080)
        render_window.SetWindowName(self.scene['description'])
    
        interactor = vtk.vtkRenderWindowInteractor()
        interactor.SetRenderWindow(render_window)
    
        # Set better camera interaction
        interactor_style = VirgoInteractorStyle()
        interactor.SetInteractorStyle(interactor_style)
    
        # Create controller
        controller = VirgoDataPlaybackControlCenter(renderer, interactor, self.scene, self.data_record_groups)
        controller.set_verbosity(self.verbosity)
        controller.register_callbacks()
        controller.initialize()

        if not controller.is_initialized():
            print("ERROR: Scene is not properly initialized. Exiting.")
            return(1)
    
        if self.verbosity > 0:
            print("Entering render window and interactor loop...")
        render_window.Render()
        interactor.Start()
        return 0