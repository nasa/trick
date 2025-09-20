import vtk
import math
import numpy as np

from VirgoActor import VirgoActor

class VirgoSceneNode():
    """
    This class provides a vtkActor and vtkAssembly Node/Graph system such that
    actors are automatically added to assemblies and children (other actors and
    assemblies) can be nested underneath a node as well. This provides the basis
    for building trees (Directed Acyclic Graphs) of actors in the Virgo
    framework and allowing those parts of an assembly to move independently as
    well as the entire tree as a unit.

    This class also provides an intermediate layer between the actors in the scene
    and the rendering provided by higher level Virgo classes
    """
    def __init__(self, name, actor=None):
        self.verbosity=1  # TODO make adjustable
        self.fs = 14      # font size
        self.name = name
        self.actor = actor
        self.assembly = vtk.vtkAssembly()     # The assembly associated with this node
        self.axes_scale = 0.5  # Default scale factor for axes
        self.axes = self.create_axes()
        self._last_opacity = 1.0  # For remembering opacity when turning visiblity off
        if self.actor:
            self.assembly.AddPart(self.actor)
        if self.axes:
            self.assembly.AddPart(self.axes)

        # self.local_transform represents the local transformation of this Node,
        # meaning the position/rotation/scale of this node relative to it's
        # parent, or world coordinates if parent == None. It is stored as a member
        # because the VTK rendering pipeline has no way to query the world state
        # information of an actor inside an assembly after it's moved, so we have
        # to store the transformation in each node to be able to get that information
        self.local_transform = vtk.vtkTransform()
        self.local_transform.Identity()
        self.assembly.SetUserTransform(self.local_transform)

        # If not None, self.parent is another VirgoSceneNode instance that this node
        # is parented to.
        self.parent = None
        self.children = []

        self.data_source = None
        self.static = False

        # Trail related members
        self._trail_points = None
        self._trail_lines = None
        self._trail_polydata = None
        self._trail_mapper = None
        self._trail_actor = None
        self._trail_polyline = None
        self.myscale = 1.0


    def set_static(self, value=True):
        self.static = value

    def is_static(self):
        return self.static

    def set_data_source(self, source):
        # Check that data_source has the functions required by by the
        # base class and if not raise an exception
        self.data_source = source

    def add_child(self, child):
        """
        Add child (another instance of VirgoSceneNode) to this Node
        scale adjustments to this Node's self.assembly
        """
        if self == child:
            msg = (f"ERROR: Cannot add child {child} to {self} becasue "
                  f"self-referential loops are not supported.")
            raise RuntimeError (msg)
        child.parent = self
        self.assembly.AddPart(child.assembly)
        self.children.append(child)

    def update(self, world_time):
        if not self.data_source:
            return

        # Query self.data_souce for the data we need to drive the node
        self.data_source.set_current_time(world_time)
        pos   = self.data_source.get_current_position()
        rot   = self.data_source.get_current_rotation()
        scale = self.data_source.get_current_scale()
        opacity = self.data_source.get_current_opacity()
        #import pdb; pdb.set_trace()
        if self.verbosity > 3:
            print(f"In VirgoSceneNode.update() with world_time: {world_time} and")
            print(f"  data_source pos:   {pos}")
            print(f"  data_source rot:   {rot}")
            print(f"  data_source scale: {scale}")
            if self.verbosity > 4:
                self.report()

        # Pose the node with this data
        self.set_pose(pos=pos, dcm=rot, scale=scale)

        # if specified, adjust the opacity of the actor
        if opacity != None:
            self.actor.GetProperty().SetOpacity(float(opacity))

        self.update_trail()

    def dump_assembly(self, asm, indent=0):
        parts = asm.GetParts()
        parts.InitTraversal()
        for i in range(parts.GetNumberOfItems()):
            prop = parts.GetNextProp()
            prefix = "  " * indent + "- "
            print(f"{prefix}{prop.GetClassName()}")
            # If this part is another assembly, recurse into it
            if isinstance(prop, vtk.vtkAssembly):
                self.dump_assembly(prop, indent + 1)
            elif isinstance(prop, vtk.vtkActor):
                # Optional: print some extra info about actors
                bounds = prop.GetBounds()
                print(f"{'  ' * (indent+1)}Bounds: {bounds}")
                print(f"{'  ' * (indent+1)}Visibility: {prop.GetVisibility()}")
                print(f"{'  ' * (indent+1)}Pickable: {prop.GetPickable()}")


    def report(self):
        print(f"=============VirgoSceneNode Report {self.name}================")
        matrix = self.assembly.GetMatrix()
        print("Assembly transform matrix:")
        for i in range(4):
            print([matrix.GetElement(i, j) for j in range(4)])
        print("Bounds:", self.assembly.GetBounds())
        print("Visibility:", self.assembly.GetVisibility())
        print("Pickable:", self.assembly.GetPickable())
        self.dump_assembly(self.assembly)
        print(f"======== End VirgoSceneNode Report {self.name}================")

        
    def set_pose(self, pos=None, rot=None, dcm=None, scale=None):
        """
        Manipulate self.local_transform to provide position, rotation, and
        scale adjustments to this Node's self.assembly
        """
        if rot is not None and dcm is not None:
            raise ValueError("Cannot specify both rot and dcm.")

        self.local_transform.Identity()
        self.local_transform.PostMultiply()

        # If dcm is given, provide it and pos in a single transform
        if dcm is not None:
            matrix = vtk.vtkMatrix4x4()
            matrix.Identity()
            for i in range(3):
                for j in range(3):
                    matrix.SetElement(i, j, dcm[i][j])
            if pos is not None:
                matrix.SetElement(0, 3, pos[0])
                matrix.SetElement(1, 3, pos[1])
                matrix.SetElement(2, 3, pos[2])
            self.local_transform.SetMatrix(matrix)
        # If only pos is given, use the Translate() approach
        elif pos is not None:
            self.local_transform.Translate(*pos)

        if rot is not None:
            rx, ry, rz = rot
            self.local_transform.RotateZ(rz)
            self.local_transform.RotateY(ry)
            self.local_transform.RotateX(rx)


        if scale is not None:
            self.local_transform.Scale(*scale)

    def get_world_position(self, local_point=(0.0, 0.0, 0.0)):
        """
        Transform local_point (in this node's local coords) into world coordinates
        by applying local transforms from this node upward to the root.
        """
        p = tuple(local_point)
        node = self
        while node is not None:
            p = node.local_transform.TransformPoint(p)
            node = node.parent
        return p
    
    def get_local_position(self):
        """
        Return this node's local position (translation relative to its parent).
        """
        return self.local_transform.GetPosition()

    def create_axes(self, position=[0,0,0]):
        """
        Create a vtkAxesActor positioned at the given position relative to this
        node's assembly and return it to the calling function.
        """
        # Create axes actor
        axes = vtk.vtkAxesActor()
        
        if self.actor:
            bb = self.actor.get_bounding_box()
        else:
            bb = [ 1, 1, 1]

        # Calculate arrow length as avg of largest and smallest bounding box dimension
        length = (max(bb) + min(bb))/2.0
        # TODO make this arrow scale a value changeable in the YAML file
        #scale = 0.5
        # Set axes properties
        axes.SetTotalLength(self.axes_scale*length, self.axes_scale*length,
                            self.axes_scale*length)
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
        #self.axes_label_render_threshold = length*20

        return axes

    def get_axes(self):
        return self.axes

    def are_axes_visible(self):
        if not self.axes:
            return False
        return(self.axes.GetVisibility())

    def hide_axes(self):
        if not self.axes:
            return
        # Make the entire axes not visible
        self.axes.SetVisibility(False)
        self.axes.Modified()
        self.actor.GetProperty().SetOpacity(self._last_opacity)

    def show_axes(self):
        if not self.axes:
            return
        # Make the entire axes visible
        #print(f"DEBUG: turning visibility of {self.name} axes on.")
        self.axes.SetVisibility(True)
        self.axes.Modified()
        print(f"DEBUG: in show_axes and self.name is {self.name}")
        self._last_opacity = self.actor.GetProperty().GetOpacity()
        self.actor.GetProperty().SetOpacity(0.7)

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
        self._trail_actor.GetProperty().SetColor(color[0], color[1], color[2])
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

        #import pdb; pdb.set_trace()
        pos = self.data_source.get_current_position()
        pid = self._trail_points.InsertNextPoint(pos[0], pos[1], pos[2])
        self._trail_polyline.GetPointIds().InsertNextId(pid)
    
        # Update line structure
        self._trail_lines.Reset()
        self._trail_lines.InsertNextCell(self._trail_polyline)
    
        self._trail_points.Modified()
        self._trail_lines.Modified()
        self._trail_polydata.Modified()

    def get_current_position(self):
        pos = None
        if self.data_source:
            pos = self.data_source.get_current_position()
        return pos

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


#    def hide_or_unhide_axes_labels(self, camera):
#        """
#        Turn the axes labels off if actor is far enough from the camera
#
#        TODO: this looks funky if you're close to the actor but it's behind the
#            camera, we also need a check that it's visible, or maybe we don't need
#            this feature at all
#        """
#        if not self.axes:
#            return
#        camera_pos = camera.GetPosition()
#        matrix = self.axes.GetMatrix()
#        axes_pos =[ matrix.GetElement(0, 3),  matrix.GetElement(1, 3), matrix.GetElement(2, 3)]
#        distance = math.sqrt(
#            (camera_pos[0] - axes_pos[0])**2 +
#            (camera_pos[1] - axes_pos[1])**2 +
#            (camera_pos[2] - axes_pos[2])**2
#        )
#        self.axes.SetAxisLabels(distance < self.axes_label_render_threshold)


class VirgoSceneNodeVector(VirgoSceneNode):
    """
    Derived class which defined specific methods for a vector application
    of a VirgoSceneNode
    """
    def __init__(self, name, actor=None):
        super().__init__(name=name, actor=actor)

    def update(self, world_time):
        if not self.data_source:
            return

        # Query self.data_souce for the data we need to drive the node
        self.data_source.set_current_time(world_time)
        pos  = self.data_source.get_current_position()
        self.set_vector_pose(vec=pos)
        if self.verbosity > 3:
            print(f"In VirgoSceneNodeVector.update() with world_time: {world_time} and")
            print(f"  data_source pos:   {pos}")
            if self.verbosity > 4:
                self.report()

    def set_vector_pose(self, vec):
        """
        Pose (orient) self.local_transform according to the x,y,z values
        in vector vec. This is the only way in VTK to take an arrow mesh
        and orient it along a vector's direction
        """
        # Normalize vector to get direction
        v = np.array(vec, dtype=float)
        length = np.linalg.norm(v)
        if length == 0:
            raise ValueError(f"Zero-length vector for {self.name} cannot be displayed.")
        direction = v / length
    
        # Compute rotation between X-axis and target direction
        x_axis = np.array([1,0,0])
        axis = np.cross(x_axis, direction)
        angle = np.degrees(np.arccos(np.clip(np.dot(x_axis, direction), -1, 1)))
    
        self.local_transform.Identity()
        self.local_transform.PostMultiply()
        #transform.Translate(0,0,0)   # (optional, could add vector origin offset here)
        if np.linalg.norm(axis) > 1e-6:
            self.local_transform.RotateWXYZ(angle, *axis)
        scale = self.data_source.get_current_scale()
        if scale is not None:
            self.local_transform.Scale(*scale)
    
