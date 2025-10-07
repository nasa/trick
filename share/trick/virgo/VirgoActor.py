#!/usr/bin/env python3.11
import os, sys, inspect, math
thisFileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))

import vtk
class VirgoActor(vtk.vtkActor):
    """
    Wrapper around VTK actor to facilitate the Trick Logged data
    and other capabilities
    """
    def __init__(self, mesh, offset_pos=None, offset_ypr=None, name='No Name', fontsize=12):
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
             VIRGO_PREFAB (TODO more info needed here)
        offset_pos : list of 3 doubles
             X-Y-Z position to apply to the model on
             init. Used to adjust position of a model relative to the origin of
             of the model as provided by the CAD or VIRGO_PREFAB itself
        offset_ypr : list of 3 doubles
             Z-Y-X (Yaw-Pitch-Roll) Tait-Bryan rotation in degrees to apply to
             the model on init. Used to rotate a model into another frame
             immediately upon model creation. This rotation is performed after
             offset_pos is applied.
        name : str
            Name given to this actor
        fontsize : int
            Default fontsize for text associated with this actor
        """
        # Call the parent class constructor to ensure proper initialization
        super().__init__()

        self.source = None  # If using a vtk*Source, it's stored here for reference
        self.mesh  = mesh
        self._map_mesh(mesh)
        self.offset_pos  = offset_pos     # Initialization position offset to apply to to actor
        self.offset_ypr  = offset_ypr     # Initialization pitch/yaw/roll to apply to to actor
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
        if self.offset_ypr:
          # Apply the initial position/rotation to this object in Z-Y-X order
          self.RotateZ(self.offset_ypr[2])
          self.RotateY(self.offset_ypr[1])
          self.RotateX(self.offset_ypr[0])
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
        Given a string mesh (a path to a model file or a VIRGO_PREFAB), set up the vtk
        mapper with the configuration of geometry given
        """
        # TODO: this is quick and dirty, need a VIRGO_PREFAB management class that VirgoActor uses
        mapper = vtk.vtkPolyDataMapper()
        texture = None
        if 'VIRGO_PREFAB:sphere' in str(mesh):
            # Create a sphere source
            self.source = vtk.vtkSphereSource()
            self.source.SetRadius(1.0)  # Set radius of the sphere
            self.source.SetThetaResolution(300)  # Number of divisions in theta (longitude)
            self.source.SetPhiResolution(300)  # Number of divisions in phi (latitude)
            
            # Create a mapper to map the sphere's geometry to graphics primitives
            mapper.SetInputConnection(self.source.GetOutputPort())
        elif 'VIRGO_PREFAB:earth' in str(mesh):
            # Create a sphere source
            self.source = vtk.vtkTexturedSphereSource()
            self.source.SetRadius(6371000.0)     # Set radius of earth
            self.source.SetThetaResolution(300)  # Number of divisions in theta (longitude)
            self.source.SetPhiResolution(300)  # Number of divisions in phi (latitude)
            # Read Earth texture image
            reader = vtk.vtkJPEGReader()
            reader.SetFileName(os.path.join(thisFileDir, 'images/earth/2k_earth_daymap.jpg'))
            texture = vtk.vtkTexture()
            texture.SetInputConnection(reader.GetOutputPort())
            texture.InterpolateOn()
            # Create a mapper to map the sphere's geometry to graphics primitives
            mapper.SetInputConnection(self.source.GetOutputPort())
            # Scale the sphere to make it an oblate spheroid
            # Earth radii: equatorial ~6378 km, polar ~6357 km
            equatorial = 6378137
            polar = 6356752
            scale_z = polar / equatorial
            self.SetScale(1.0, 1.0, scale_z)  # squash along Z
        elif 'VIRGO_PREFAB:moon8k' in str(mesh) :
            # Create a sphere source
            self.source = vtk.vtkTexturedSphereSource()
            self.source.SetRadius(1738100.0)     # Set radius of moon
            self.source.SetThetaResolution(300)  # Number of divisions in theta (longitude)
            self.source.SetPhiResolution(300)  # Number of divisions in phi (latitude)
            # Read Moon texture image
            reader = vtk.vtkJPEGReader()
            reader.SetFileName(os.path.join(thisFileDir, 'images/moon/lroc_color_poles_8k.jpg'))
            texture = vtk.vtkTexture()
            texture.SetInputConnection(reader.GetOutputPort())
            texture.InterpolateOn()
            # Create a mapper to map the sphere's geometry to graphics primitives
            mapper.SetInputConnection(self.source.GetOutputPort())
            # Scale the sphere to make it an oblate spheroid
            equatorial = 1738100.0
            polar = 1736000.0
            scale_z = polar / equatorial   # ~0.99879
            self.SetScale(1.0, 1.0, scale_z)
        elif 'VIRGO_PREFAB:moon' in str(mesh) or 'VIRGO_PREFAB:moon4k' in str(mesh) :
            # Create a sphere source
            self.source = vtk.vtkTexturedSphereSource()
            self.source.SetRadius(1738100.0)     # Set radius of moon
            self.source.SetThetaResolution(300)  # Number of divisions in theta (longitude)
            self.source.SetPhiResolution(300)  # Number of divisions in phi (latitude)
            # Read Moon texture image
            reader = vtk.vtkJPEGReader()
            reader.SetFileName(os.path.join(thisFileDir, 'images/moon/lroc_color_poles_4k.jpg'))
            texture = vtk.vtkTexture()
            texture.SetInputConnection(reader.GetOutputPort())
            texture.InterpolateOn()
            # Create a mapper to map the sphere's geometry to graphics primitives
            mapper.SetInputConnection(self.source.GetOutputPort())
            # Scale the sphere to make it an oblate spheroid
            equatorial = 1738100.0
            polar = 1736000.0
            scale_z = polar / equatorial   # ~0.99879
            self.SetScale(1.0, 1.0, scale_z)
        elif 'VIRGO_PREFAB:cube' in str(mesh):
            self.source = vtk.vtkCubeSource()
            self.source.SetXLength(1.0)
            self.source.SetYLength(1.0)
            self.source.SetZLength(1.0)
            # Create a mapper to map the cube's geometry to graphics primitives
            mapper.SetInputConnection(self.source.GetOutputPort())
        elif 'VIRGO_PREFAB:cylinder' in str(mesh):
            self.source = vtk.vtkCylinderSource()
            self.source.SetHeight(1.0)         # Set height to 3 units
            self.source.SetRadius(0.5)         # Set radius to 1 unit
            self.source.SetCenter(0.0, 0.0, 0.0) # 
            self.source.SetResolution(50)      # Use 50 facets for a smooth cylinder
            self.source.SetCapping(True)       # Ensure the bases are capped
            # Create a mapper to map the cube's geometry to graphics primitives
            mapper.SetInputConnection(self.source.GetOutputPort())
        elif'VIRGO_PREFAB:cone-32' in str(mesh):
            self.source = vtk.vtkConeSource()
            height = 1.0
            self.source.SetHeight(height)
            self.source.SetRadius(0.5)
            self.source.SetResolution(32)
            self.source.SetDirection(-1, 0, 0)  # tip pointed along -X
            self.source.SetCenter(height/2, 0.0, 0.0)  # tip at (0,0,0)
            mapper.SetInputConnection(self.source.GetOutputPort())
        elif 'VIRGO_PREFAB:cone' in str(mesh) or  'VIRGO_PREFAB:cone-16' in str(mesh):
            self.source = vtk.vtkConeSource()
            height = 1.0
            self.source.SetHeight(height)
            self.source.SetRadius(0.5)
            self.source.SetResolution(16)
            self.source.SetDirection(-1, 0, 0)  # tip pointed along -X
            self.source.SetCenter(height/2, 0.0, 0.0)  # tip at (0,0,0)
            mapper.SetInputConnection(self.source.GetOutputPort())
        elif 'VIRGO_PREFAB:arrow' in str(mesh):
            self.source = vtk.vtkArrowSource()
            self.source.SetTipLength(0.35)   # Fraction of total length
            self.source.SetTipRadius(0.2)    # Radius of the cone
            self.source.SetShaftRadius(0.03) # Radius of the cylinder
            self.source.Update()
            mapper.SetInputConnection(self.source.GetOutputPort())
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

