import os, sys, shutil, inspect
import unittest
import pdb

# Add path to virgo module
thisFileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))
virgo_dir=os.path.abspath(os.path.join(thisFileDir, '../'))
sys.path.append(virgo_dir)
from Virgo import *
meshes_dir=os.path.join(virgo_dir, 'meshes')
tests_dir=os.path.join(virgo_dir, 'tests')
from VisualizableTestCase import VisualizableTestCase

def suite():
    """Create test suite from test cases here and return"""
    suites = []
    suites.append(unittest.TestLoader().loadTestsFromTestCase(VirgoDataPlaybackActorTestCase))
    return (suites)

    import time



class VirgoDataPlaybackActorTestCase(VisualizableTestCase):

#    def setUp(self):
#        VisualizableTestCase().setUp()

    def tearDown(self):
        self.vis()
        super().tearDown()
        self.instance = None
        del self.instance


    def test_incomplete_init(self):
        """
        Test a incomplete construction with optional args left out
        """
        self.instance = VirgoDataPlaybackActor(mesh=os.path.join(meshes_dir, 'teapot.obj'))
        self.assertEqual(self.instance.name, 'No Name')
        self.assertEqual(self.instance.offset_pyr, None)

        #self.assertEqual(self.instance.are_axes_visible(), False)

    def test_init_with_pyr_offset(self):
        """
        Test a incomplete construction with a 45-degree pitch offset given
        """
        self.instance = VirgoDataPlaybackActor(mesh=os.path.join(meshes_dir, 'teapot.obj'),
                                               offset_pyr=[45, 0, 0])
        self.instance.initialize()
        #import pdb; pdb.set_trace()
        self.assertEqual(self.instance.name, 'No Name')
        self.assertEqual(self.instance.offset_pyr, [45, 0, 0])
        for i, (a, b) in enumerate(zip(self.instance.GetOrientation(), [45.0, 0.0, 0.0])):
            self.assertAlmostEqual(a, b)
        #self.visualize = True

    def test_init_with_pos_offset(self):
        """
        Test a incomplete construction with a [1, 0, 0] unit position offset given
        """
        self.instance = VirgoDataPlaybackActor(mesh=os.path.join(meshes_dir, 'teapot.obj'),
                                               offset_pos=[1, 0, 0])
        self.instance.initialize()
        #import pdb; pdb.set_trace()
        self.assertEqual(self.instance.name, 'No Name')
        #self.assertAlmostEqual(self.instance.GetPosition()[0], 1.0)
        self.assertEqual(self.instance.offset_pos, [1, 0, 0])
        for i, (a, b) in enumerate(zip(self.instance.GetPosition(), [1.0, 0.0, 0.0])):
            self.assertAlmostEqual(a, b)
        #self.visualize = True
        #self.show_grid = True
        #self.show_origin = True

    def test_init_with_pos_and_pyr_offset(self):
        """
        Test a incomplete construction with a [0, 1, 0] unit position offset
        and [-45, 0, 0] pyr offset given
        """
        self.instance = VirgoDataPlaybackActor(mesh=os.path.join(meshes_dir, 'teapot.obj'),
                                               offset_pos=[0, 1, 0],
                                               offset_pyr=[-45, 0, 0])
        self.instance.initialize()
        #import pdb; pdb.set_trace()
        self.assertEqual(self.instance.name, 'No Name')
        #self.assertAlmostEqual(self.instance.GetPosition()[0], 1.0)
        self.assertEqual(self.instance.offset_pos, [0, 1, 0])
        for i, (a, b) in enumerate(zip(self.instance.GetPosition(), [0.0, 1.0, 0.0])):
            self.assertAlmostEqual(a, b)
        for i, (a, b) in enumerate(zip(self.instance.GetOrientation(), [-45.0, 0.0, 0.0])):
            self.assertAlmostEqual(a, b)
        #self.visualize = True
        #self.show_grid = True
        #self.show_origin = True

    def test_init_prefab_moon(self):
        """
        Test the PREFAB:moon option
        """
        self.instance = VirgoDataPlaybackActor(mesh="PREFAB:moon8k")
        self.origin_axes.SetTotalLength(3e6, 3e6, 3e6)  # Size of axes (x, y, z lengths)
        # TODO assertions go here!
        self.visualize = True
        #self.show_origin = True
        #self.show_grid = True

    def test_init_prefab_earth(self):
        """
        Test the PREFAB:earth option
        """
        self.instance = VirgoDataPlaybackActor(mesh="PREFAB:earth")
        self.origin_axes.SetTotalLength(1e7, 1e7, 1e7)  # Size of axes (x, y, z lengths)
        # TODO assertions go here!
        #self.visualize = True
        #self.show_origin = True
        #self.show_grid = True