import os, sys, shutil, inspect
import unittest
import numpy as np
import vtk
import pdb

# Add path to virgo module
thisFileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))
virgo_dir=os.path.abspath(os.path.join(thisFileDir, '../'))
sys.path.append(virgo_dir)
from VirgoActor import VirgoDataPlaybackActor
from VirgoNode import VirgoSceneNode
meshes_dir=os.path.join(virgo_dir, 'meshes')
tests_dir=os.path.join(virgo_dir, 'tests')
from VisualizableTestCase import VisualizableTestCase

def suite():
    """Create test suite from test cases here and return"""
    suites = []
    suites.append(unittest.TestLoader().loadTestsFromTestCase(VirgoSceneNodeTestCase))
    return (suites)

class VirgoSceneNodeTestCase(VisualizableTestCase):

    def setUp(self):
        VisualizableTestCase().setUp()
        # Actors to be used in node construction
        self.teapot1 = VirgoDataPlaybackActor(mesh=os.path.join(meshes_dir, 'teapot.obj'))
        self.teapot2 = VirgoDataPlaybackActor(mesh=os.path.join(meshes_dir, 'teapot.obj'))
        self.teapot3 = VirgoDataPlaybackActor(mesh=os.path.join(meshes_dir, 'teapot.obj'))
        # Nodes to test with
        self.node1 = VirgoSceneNode(name='parent_teapot', actor=self.teapot1)
        self.node2 = VirgoSceneNode(name='child_teapot', actor=self.teapot2)
        self.node3 = VirgoSceneNode(name='grandchild_teapot', actor=self.teapot3)

    def tearDown(self):
        self.vis()
        super().tearDown()
        self.instance = None
        self.teapot1 = None
        self.teapot2 = None
        self.node1 = None
        self.node2 = None

    def test_construction(self):
        """
        Simple construction assertions
        """
        # Two nodes not at all linked together
        nodes = [self.node1, self.node2, self.node3]
        # The matrix we expect inside local_transform after construction
        default_matrix = np.array([
          [1.0, 0.0, 0.0,  0.0],
          [0.0, 1.0, 0.0,  0.0],
          [0.0, 0.0, 1.0,  0.0],
          [0.0, 0.0, 0.0,  1.0],
        ])
        for node in nodes:
            self.assertIn('teapot', node.name)
            self.assertEqual(node.children, [])
            self.assertIsNone(node.parent)
            self.assertIsNot(node.actor, None)
            for i in range(4):
              for j in range(4):
                self.assertEqual(default_matrix[i, j], node.local_transform.GetMatrix().GetElement(i, j))

    def test_add_child_ERROR(self):
        """
        Attempt to add a node to itself as a child, which isn't supported
        """
        with self.assertRaises(RuntimeError):
            # Make node2 a child of node1
            self.node1.add_child(self.node1)

    def test_add_child(self):
        """
        Add a child teapot to the parent, handle to tip, then rotate the entire
        assembly by 45 degrees in Y
        """
        # Make node2 a child of node1
        self.node1.add_child(self.node2)
        self.node2.set_pose(pos=(6.0, 1.7, 0.0)) # Positioned handle-to-tip

        node2_local_pos_before = self.node2.local_transform.GetPosition()
        node2_world_pos_before = self.node2.get_world_position()

        # Apply a rotation about y in parent node1
        self.node1.set_pose(rot=(0.0, 45.0, 0.0))

        node2_local_pos_after = self.node2.local_transform.GetPosition()
        node2_world_pos_after = self.node2.get_world_position()

        # Visually confirmed success criteria using self.show_grid = True
        node2_world_pos_after_expected = (4.242640687119285, 1.7, -4.242640687119286)
        # Observe the child node2 moves along with parent node1 rotation
        np.testing.assert_array_almost_equal(node2_world_pos_after, node2_world_pos_after_expected, decimal=10)
        np.testing.assert_array_equal(node2_local_pos_before, node2_local_pos_after)

        #print(f"{node2_world_pos_after}")
        self.instance = self.node1.assembly
        self.show_grid = True
        #self.visualize = True


    def test_add_grandchild(self):
        """
        Add a child teapot to the parent, handle to tip, and another
        grandchild teapot to the child, then rotate the entire
        assembly by 45 degrees in Y
        """
        # Make node2 a child of node1
        self.node1.add_child(self.node2)
        self.node2.set_pose(pos=(6.0, 1.7, 0.0)) # Positioned handle-to-tip
        # Make node3 a child of node2
        self.node2.add_child(self.node3)
        self.node3.set_pose(pos=(6.0, 1.7, 0.0)) # Positioned handle-to-tip

        node3_local_pos_before = self.node2.local_transform.GetPosition()
        node3_world_pos_before = self.node2.get_world_position()

        # Apply a rotation about y in parent node1
        self.node1.set_pose(rot=(0.0, 45.0, 0.0))

        node3_local_pos_after = self.node3.local_transform.GetPosition()
        node3_world_pos_after = self.node3.get_world_position()

        # Visually confirmed success criteria using self.show_grid = True
        node3_world_pos_after_expected = (8.48528137423857, 3.4, -8.485281374238571)
        # Observe the child node2 moves along with parent node1 rotation
        np.testing.assert_array_almost_equal(node3_world_pos_after, node3_world_pos_after_expected, decimal=10)
        np.testing.assert_array_equal(node3_local_pos_before, node3_local_pos_after)

        self.instance = self.node1.assembly
        self.show_grid = True
        #self.visualize = True