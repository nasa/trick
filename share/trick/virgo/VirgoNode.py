import vtk

class VirgoSceneNode():
    """
    This class provides a vtkActor and vtkAssembly Node/Graph system such that
    actors are automatically added to assemblies and children (other actors and
    assemblies) can be nested underneath a node as well. This provides the basis
    for building trees (Directed Acyclic Graphs) of actors in the Virgo
    framework and allowing those parts of an assembly to move independently as
    well as the entire tree as a unit.
    """
    def __init__(self, name, actor=None):
        self.name = name
        self.actor = actor or vtk.vtkActor()  # The (optional) actor associated with this node
        self.assembly = vtk.vtkAssembly()     # The assembly associated with this node
        self.assembly.AddPart(self.actor)

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

    def set_pose(self, pos=None, rot=None, dcm=None, scale=None):
        """
        Manipulate self.local_transform to provide position, rotation, and
        scale adjustments to this Node's self.assembly
        """
        if rot is not None and dcm is not None:
            raise ValueError("Cannot specify both rot and dcm.")

        self.local_transform.Identity()
        self.local_transform.PostMultiply()

        if pos is not None:
            self.local_transform.Translate(*pos)

        if rot is not None:
            rx, ry, rz = rot
            self.local_transform.RotateZ(rz)
            self.local_transform.RotateY(ry)
            self.local_transform.RotateX(rx)

        if dcm is not None:
            matrix = vtk.vtkMatrix4x4()
            matrix.Identity()
            for i in range(3):
                for j in range(3):
                    matrix.SetElement(i, j, dcm[i][j])
            self.local_transform.Concatenate(matrix)

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

class VirgoSceneGraph:
    def __init__(self):
        self.nodes = {}

    def add_node(self, node, parent_name=None):
        self.nodes[node.name] = node
        if parent_name is not None:
            parent = self.nodes[parent_name]
            parent.add_child(node)

    def get_node(self, name):
        return self.nodes.get(name)
