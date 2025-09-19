#!/usr/bin/env python3.11
import os, sys, inspect, math
thisFileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))

import vtk
class VirgoActor(vtk.vtkActor):
    """
    Wrapper around VTK actor to facilitate the Trick Logged data
    and other capabilities
    """
    def __init__(self, mesh, offset_pos=None, offset_pyr=None, name='No Name', fontsize=12):
        """
        Initialize this instance.

        >>> va = VirgoActor(mesh=os.path.join(thisFileDir,"meshes/teapot.obj"), name='tea')
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
        #import pdb; pdb.set_trace()
        self.static = False       # If true, this actor never moves
        self.initialized = False  # True only if this actor has all needed data to be used
        self.axes_default_scale = 0.5  # Default scaling of axes to be applied


    def set_static(self, value=True):
        self.static = value

    def is_static(self):
        return self.static

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

    def verify(self):
        # TO BE FILLED OUT
        pass

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


    def _map_mesh(self, mesh):
        """
        Given a string mesh (a path to a model file or a PREFAB), set up the vtk
        mapper with the configuration of geometry given
        """
        # TODO: this is quick and dirty, need a PREFAB management class that VirgoActor uses
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
        elif 'PREFAB:moon8k' in str(mesh) :
            # Create a sphere source
            earth_source = vtk.vtkTexturedSphereSource()
            earth_source.SetRadius(1740000.0)     # Set radius of moon
            earth_source.SetThetaResolution(300)  # Number of divisions in theta (longitude)
            earth_source.SetPhiResolution(300)  # Number of divisions in phi (latitude)
            # Read Moon texture image
            reader = vtk.vtkJPEGReader()
            reader.SetFileName(os.path.join(thisFileDir, 'images/moon/lroc_color_poles_8k.jpg'))
            texture = vtk.vtkTexture()
            texture.SetInputConnection(reader.GetOutputPort())
            texture.InterpolateOn()
            # Create a mapper to map the sphere's geometry to graphics primitives
            mapper.SetInputConnection(earth_source.GetOutputPort())
        elif 'PREFAB:moon' in str(mesh) or 'PREFAB:moon4k' in str(mesh) :
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
        elif 'PREFAB:arrow' in str(mesh):
            arrow_source = vtk.vtkArrowSource()
            arrow_source.SetTipLength(0.35)   # Fraction of total length
            arrow_source.SetTipRadius(0.2)    # Radius of the cone
            arrow_source.SetShaftRadius(0.03) # Radius of the cylinder
            arrow_source.Update()
            mapper.SetInputConnection(arrow_source.GetOutputPort())
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

#    def report(self):
#        """
#        Debug prints
#        """
#        print(f"{self.name}:")
#        print(f"  position: {self.get_current_position()}")
#        print(f"  rotation: {self.get_current_rotation()}")
#        print(f"     scale: {self.get_current_scale()}")


#    def update(self, world_time):
#        """
#        Update this actor's position and rotation to the values associated
#        with data closest to world_time (see set_current_time) by applying
#        a matrix transform to self.
#        """
#        if not self.initialized or self.static:
#            return # Nothing to do
#
#        self.set_current_time(world_time)
#        pos = self.get_current_position()
#        rot = self.get_current_rotation()
#        scale = self.get_current_scale()
#        if pos == None and rot == None:
#            return  # Nothing to update
#
#        # Convert rotation matrix to VTK transform
#        transform_actor = vtk.vtkTransform()
#        matrix = vtk.vtkMatrix4x4()
#        if rot is not None:
#            for i in range(3):
#                for j in range(3):
#                    matrix.SetElement(i, j, rot[i][j])
#        if pos is not None:
#            matrix.SetElement(0, 3, pos[0])
#            matrix.SetElement(1, 3, pos[1])
#            matrix.SetElement(2, 3, pos[2])
#        transform_actor.SetMatrix(matrix)
#        if scale is not None:
#            transform_actor.Scale(scale, scale, scale)
#        self.SetUserTransform(transform_actor)
#
#        # Move the axes to where the parent actor is using a different transform
#        # since transforms should not be shared across actors
#        transform_axes = vtk.vtkTransform()
#        transform_axes.SetMatrix(matrix)
#        self._axes.SetUserTransform(transform_axes)
#        self._axes.Modified()
#        ##################################################################################
#        # DEBUG prints used trying to figure out why self._axes will not appear until the
#        # camera is rotated. The best evidence is self._axes.GetBounds() which shows the
#        # bounding box for the axes does not snap to the new position described by axes_pos
#        # I've asked every AI and searched every VTK issue and I cannot resolve this even
#        # after trying every suggestion I can find. At this point I'm thinking this might
#        # just be a bug in our version of vtk - why doesn't the bounding box update? -Jordan
#        # See: https://discourse.vtk.org/t/vtkaxesactor-fails-to-render-at-some-angles/4864/2
#        #matrix = self._axes.GetMatrix()
#        #axes_pos =[ matrix.GetElement(0, 3),  matrix.GetElement(1, 3), matrix.GetElement(2, 3)]
#        #print(f'DEBUG: self._axes.GetVisibility is {self._axes.GetVisibility()} with axes_pos {axes_pos}')
#        #print(f"BOTTOM OF UPDATE Axes bounds: {self._axes.GetBounds()} with GetUseBounds() {self._axes.GetUseBounds()}")
#        ##################################################################################
#        # If there's a trail, update it
#        self.update_trail()

