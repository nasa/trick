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
    suites.append(unittest.TestLoader().loadTestsFromTestCase(VirgoDataPlaybackActorStaticMethodsTestCase))
    suites.append(unittest.TestLoader().loadTestsFromTestCase(VirgoDataPlaybackActorTestCase))
    return (suites)

class VirgoDataPlaybackActorStaticMethodsTestCase(unittest.TestCase):
    def test__find_index(self):
        """
        Test the _find_index static method
        """
        # A sorted list of doubles starting at 0.0 ending at 100.0 in steps of 0.5
        times = sorted([x * 0.5 for x in range(201)])
        # Test the different strategies
        idx = VirgoDataPlaybackActor._find_index(times, 0.7, mode='down')
        self.assertEqual(idx, 1)
        idx = VirgoDataPlaybackActor._find_index(times, 0.7, mode='up')
        self.assertEqual(idx, 2)
        idx = VirgoDataPlaybackActor._find_index(times, 0.7, mode='closest')
        self.assertEqual(idx, 1)
        # At a precise value, all modes should return the same index
        idx1 = VirgoDataPlaybackActor._find_index(times, 0.0, mode='down')
        idx2 = VirgoDataPlaybackActor._find_index(times, 0.0, mode='up')
        idx3 = VirgoDataPlaybackActor._find_index(times, 0.0, mode='closest')
        self.assertEqual(idx1, 0)
        self.assertEqual(idx2, 0)
        self.assertEqual(idx3, 0)
        # Off the end of the list should return -1 (last index)
        idx = VirgoDataPlaybackActor._find_index(times, 101.0)
        self.assertEqual(idx, -1)
        # Before the beginning of the list should return 0 (first index)
        idx = VirgoDataPlaybackActor._find_index(times, -10.0)
        self.assertEqual(idx, 0)

class VirgoDataPlaybackActorTestCase(VisualizableTestCase):

    def setUp(self):
        VisualizableTestCase().setUp()
        # Nominal no-error when parsing the trick-sims config file scenario
        self.instance = VirgoDataPlaybackActor(mesh=os.path.join(meshes_dir, 'teapot.obj'))

    def tearDown(self):
        self.vis()

    def test_incomplete_init(self):
        """
        Test a incomplete construction with optional args left out
        """
        self.assertEqual(self.instance.name, 'No Name')
        self.assertEqual(self.instance.offset_pyr, None)

        self.assertIsNone(self.instance.get_current_position())
        self.assertEqual(self.instance.are_axes_visible(), False)
        # Actor doesn't have everything it needs, so we expect a raise
        # when attempting to initialize()
        with self.assertRaises(ValueError):
            self.instance.initialize()
        #self.assertAlmostEqual(self.instance.get_current_position(), [0.0, 0.0, 0.0])

    def test_incomplete_init_with_pyr_offset(self):
        """
        Test a incomplete construction with a 45-degree pitch offset given
        """
        self.instance = VirgoDataPlaybackActor(mesh=os.path.join(meshes_dir, 'teapot.obj'),
                                               offset_pyr=[45, 0, 0])
        #import pdb; pdb.set_trace()
        self.assertEqual(self.instance.name, 'No Name')
        self.assertEqual(self.instance.offset_pyr, [45, 0, 0])
        for i, (a, b) in enumerate(zip(self.instance.GetOrientation(), [45.0, 0.0, 0.0])):
            self.assertAlmostEqual(a, b)
        #self.visualize = True

    def test_incomplete_init_with_pos_offset(self):
        """
        Test a incomplete construction with a [1, 0, 0] unit position offset given
        """
        self.instance = VirgoDataPlaybackActor(mesh=os.path.join(meshes_dir, 'teapot.obj'),
                                               offset_pos=[1, 0, 0])
        #import pdb; pdb.set_trace()
        self.assertEqual(self.instance.name, 'No Name')
        #self.assertAlmostEqual(self.instance.GetPosition()[0], 1.0)
        self.assertEqual(self.instance.offset_pos, [1, 0, 0])
        for i, (a, b) in enumerate(zip(self.instance.GetPosition(), [1.0, 0.0, 0.0])):
            self.assertAlmostEqual(a, b)
        #self.visualize = True
        #self.show_grid = False
        #self.show_origin = True

    def test_incomplete_init_with_pos_and_pyr_offset(self):
        """
        Test a incomplete construction with a [0, 1, 0] unit position offset
        and [-45, 0, 0] pyr offset given
        """
        self.instance = VirgoDataPlaybackActor(mesh=os.path.join(meshes_dir, 'teapot.obj'),
                                               offset_pos=[0, 1, 0],
                                               offset_pyr=[-45, 0, 0])
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
