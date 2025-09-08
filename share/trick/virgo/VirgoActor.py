#!/usr/bin/env python3.11
import os, sys, inspect, bisect, math
thisFileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))

import vtk

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
        self.axes_default_scale = 0.5  # Default scaling of axes to be applied
        # Trail related members
        self._trail_points = None
        self._trail_lines = None
        self._trail_polydata = None
        self._trail_mapper = None
        self._trail_actor = None
        self._trail_polyline = None

    def set_static(self, value=True):
        self.static = value

    def initialize(self):
        """
        Perform pre-rendering setup/initialization
        """
        if self.offset_pos:
          # Apply the initial position/rotation to this object
          self.AddPosition(self.offset_pos)
        # TODO: PYR may not be sufficient for end users, we may need to consider
        # different rotation schemes
        if self.offset_pyr:
          # Apply the initial position/rotation to this object
          self.RotateX(self.offset_pyr[0])
          self.RotateY(self.offset_pyr[1])
          self.RotateZ(self.offset_pyr[2])
        self.verify()

    def set_times(self, times):
        self._times = times

    def set_positions(self, positions):
        self._positions = positions

    def set_rotations(self, rotations):
        self._rotations = rotations

    def get_bounding_box(self):
        '''
        Return a list of actor bounding box [x_size, y_size, z_size]
        '''

        # Get size of actor to automatically scale size of axes
        bounds = self.GetBounds()
        # Bounds are returned as (xmin, xmax, ymin, ymax, zmin, zmax)
        center = [(bounds[0] + bounds[1]) / 2.0,  # x-center
                  (bounds[2] + bounds[3]) / 2.0,  # y-center
                  (bounds[4] + bounds[5]) / 2.0]  # z-center
        # Calculate the size of the bounding box
        size = [bounds[1] - bounds[0],  # x-size
                bounds[3] - bounds[2],  # y-size
                bounds[5] - bounds[4]]  # z-size

        return size

    def create_trail(self, color=[0.0, 0.0, 0.0], thickness=2):
        self._trail_points = vtk.vtkPoints()
        self._trail_lines = vtk.vtkCellArray()
        self._trail_polydata = vtk.vtkPolyData()
        self._trail_polydata.SetPoints(self._trail_points)
        self._trail_polydata.SetLines(self._trail_lines)
        
        self._trail_mapper = vtk.vtkPolyDataMapper()
        self._trail_mapper.SetInputData(self._trail_polydata)
        
        self._trail_actor = vtk.vtkActor()
        self._trail_actor.SetMapper(self._trail_mapper)
        self._trail_actor.GetProperty().SetColor(color[0], color[1], color[2])  # yellow trail
        self._trail_actor.GetProperty().SetLineWidth(thickness)
        
        # Maintain a polyline of visited positions
        self._trail_polyline = vtk.vtkPolyLine()
        self._trail_polyline.GetPointIds().SetNumberOfIds(0)  # start empty

        return self._trail_actor
        
    def update_trail(self):
        """
        Add point and connecting line to the trail using the current actor
        position
        """
        if not self._trail_points:
            return
        # TODO: it might be better to get the position of the actor here
        # instead of the truth data from the data record file even though
        # they should always be identical
        pos = self.get_current_position()
        pid = self._trail_points.InsertNextPoint(pos[0], pos[1], pos[2])
        self._trail_polyline.GetPointIds().InsertNextId(pid)
    
        # Update line structure
        self._trail_lines.Reset()
        self._trail_lines.InsertNextCell(self._trail_polyline)
    
        self._trail_points.Modified()
        self._trail_lines.Modified()
        self._trail_polydata.Modified()

    def reset_trail(self):
        """
        Remove all lines/points from this actor's trail actor
        """
        if not self._trail_points:
            return
        self._trail_polyline.GetPointIds().Reset()
        self._trail_points.Reset()
        self._trail_lines.Reset()
        self._trail_points.Modified()
        self._trail_lines.Modified()
        self._trail_polydata.Modified()

    def add_axes(self, position=[0,0,0]):
        """
        Create a vtkAxesActor positioned at the given position relative to this
        actor's origin, store it internally in self._axes, and also return it
        to the calling function.
        """
        # Skip non-pickable actors. TODO: we need more axes options, this is a quick fix
        #if not actor.GetPickable():
        #    continue
        # Create axes actor
        axes = vtk.vtkAxesActor()
        
        bb = self.get_bounding_box()
        # Calculate arrow length as avg of largest and smallest bounding box dimension
        length = (max(bb) + min(bb))/2.0
        # TODO make this arrow scale a value changeable in the YAML file
        #scale = 0.5
        # Set axes properties
        axes.SetTotalLength(self.axes_default_scale*length, self.axes_default_scale*length,
                            self.axes_default_scale*length)
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
        
        # Set the position of the axes to the actor's origin
        axes.SetPosition(position[0], position[1], position[2])
        axes.SetVisibility(False) # Default to not visible
        # TODO: This should probably be set inside the actor based on stored actor bounds
        # when the actor is init'd. Not sure if I even like this feature so maybe remove it
        self.axes_label_render_threshold = length*20

        self._axes = axes

    def get_axes(self):
        return self._axes

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
        transform_actor = vtk.vtkTransform()
        matrix = vtk.vtkMatrix4x4()
        if rot is not None:
            for i in range(3):
                for j in range(3):
                    matrix.SetElement(i, j, rot[i][j])
        if pos is not None:
            matrix.SetElement(0, 3, pos[0])
            matrix.SetElement(1, 3, pos[1])
            matrix.SetElement(2, 3, pos[2])
        transform_actor.SetMatrix(matrix)
        self.SetUserTransform(transform_actor)

        # Move the axes to where the parent actor is using a different transform
        # since transforms should not be shared across actors
        transform_axes = vtk.vtkTransform()
        transform_axes.SetMatrix(matrix)
        self._axes.SetUserTransform(transform_axes)
        self._axes.Modified()
        ##################################################################################
        # DEBUG prints used trying to figure out why self._axes will not appear until the
        # camera is rotated. The best evidence is self._axes.GetBounds() which shows the
        # bounding box for the axes does not snap to the new position described by axes_pos
        # I've asked every AI and searched every VTK issue and I cannot resolve this even
        # after trying every suggestion I can find. At this point I'm thinking this might
        # just be a bug in our version of vtk - why doesn't the bounding box update? -Jordan
        #matrix = self._axes.GetMatrix()
        #axes_pos =[ matrix.GetElement(0, 3),  matrix.GetElement(1, 3), matrix.GetElement(2, 3)]
        #print(f'DEBUG: self._axes.GetVisibility is {self._axes.GetVisibility()} with axes_pos {axes_pos}')
        #print(f"BOTTOM OF UPDATE Axes bounds: {self._axes.GetBounds()} with GetUseBounds() {self._axes.GetUseBounds()}")
        ##################################################################################
        # If there's a trail, update it
        self.update_trail()

    def are_axes_visible(self):
        if not self._axes:
            return False
        return(self._axes.GetVisibility())

    def hide_axes(self):
        if not self._axes:
            return
        # Make the entire axes not visible
        self._axes.SetVisibility(False)
        self._axes.Modified()
        self.GetProperty().SetOpacity(1.0)

    def show_axes(self):
        if not self._axes:
            return
        # Make the entire axes visible
        #print(f"DEBUG: turning visibility of {self.name} axes on.")
        self._axes.SetVisibility(True)
        self._axes.Modified()
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
