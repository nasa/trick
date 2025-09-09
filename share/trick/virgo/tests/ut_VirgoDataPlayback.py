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
    suites.append(unittest.TestLoader().loadTestsFromTestCase(VirgoDataLoaderTestCase))
    suites.append(unittest.TestLoader().loadTestsFromTestCase(VirgoDataPlaybackTestCase))
    return (suites)

    import time

class VirgoDataLoaderTestCase(unittest.TestCase):
    def setUp(self):
        self.recorded_data  = {}
        self.recorded_data['time']  = {}
        self.recorded_data['pos']  = {}
        self.recorded_data['rot']  = {}
        self.recorded_data['time']['group'] = "one_body_static"
        self.recorded_data['pos']['group']  = "one_body_static"
        self.recorded_data['rot']['group']  = "one_body_static"
        self.recorded_data['time']['var']   = "sys.exec.out.time"
        self.recorded_data['pos']['var']    = "position[0-2]"
        self.recorded_data['rot']['var']    = "R[0-2][0-2]"

    def test_expand_arrays(self):
        """
        Test the expand_arrays static method
        """
        t = VirgoDataLoader.expand_arrays(varname='foo.bar[0-2]')
        for i in range(3):
          self.assertEqual(t[i], f'foo.bar[{i}]')
        t = VirgoDataLoader.expand_arrays(varname='foo.bar[0-2][0-2]')
        self.assertEqual(t[0], f'foo.bar[0][0]')
        self.assertEqual(t[1], f'foo.bar[0][1]')
        self.assertEqual(t[2], f'foo.bar[0][2]')
        self.assertEqual(t[3], f'foo.bar[1][0]')
        self.assertEqual(t[4], f'foo.bar[1][1]')
        self.assertEqual(t[5], f'foo.bar[1][2]')
        self.assertEqual(t[6], f'foo.bar[2][0]')
        self.assertEqual(t[7], f'foo.bar[2][1]')
        self.assertEqual(t[8], f'foo.bar[2][2]')

        t = VirgoDataLoader.expand_arrays(varname='foo[1-2].bar[0-2]')

    def test_get_recorded_data(self):
        self.instance = VirgoDataLoader(
            run_dir=os.path.join(tests_dir, 'recorded_data/RUN_0'),
            scene_recorded_data=self.recorded_data,
            verbosity=1)
        self.instance.load_variables()
        times = self.instance.get_recorded_data(alias='time')
        self.assertEqual(times, [0.0, 1.0, 2.0, 3.0, 4.0, 5.0])
        posxs = self.instance.get_recorded_data(alias='pos[0]')
        posys = self.instance.get_recorded_data(alias='pos[1]')
        poszs = self.instance.get_recorded_data(alias='pos[2]')
        # TODO need more assertions here
        self.assertEqual(posxs[0], 0.0)
        self.assertEqual(posys[0], 0.0)
        self.assertEqual(poszs[0], 0.0)

        Rxxs = self.instance.get_recorded_data(alias='rot[0][0]')
        # TODO need more assertions here
        self.assertEqual(Rxxs[0], 1.0)

        with self.assertRaises(RuntimeError):
            # Indices not specified when they need to be
            posxs = self.instance.get_recorded_data(alias='pos')

    def test_init_missing_recorded_data_variables(self):
        """
        Add variables to the scene that don't exist in the logged data file
        """
        # Add variables to scene that won't be found
        self.recorded_data['noexist']  = {}
        self.recorded_data['noexist']['var']  = "noexist[0-2]"
        self.instance = VirgoDataLoader(
            run_dir=os.path.join(tests_dir, 'recorded_data/RUN_0'),
            scene_recorded_data=self.recorded_data)
        # Try to load the scene, knowing it will fail
        with self.assertRaises(RuntimeError):
            self.instance.load_variables()

    def test_get_recorded_datas(self):
        self.instance = VirgoDataLoader(
            run_dir=os.path.join(tests_dir, 'recorded_data/RUN_0'),
            scene_recorded_data=self.recorded_data,
            verbosity=1)
        self.instance.load_variables()
        times = self.instance.get_recorded_datas(alias='time')
        self.assertEqual(times, [0.0, 1.0, 2.0, 3.0, 4.0, 5.0])
        positions = self.instance.get_recorded_datas(alias='pos')
        self.assertEqual(positions[0], (0.0, 0.0, 0.0))
        rotations = self.instance.get_recorded_datas(alias='rot')
        expected_rot_0 = np.array( [ [1.0, 0.0, 0.0 ],
                                     [0.0, 1.0, 0.0 ],
                                     [0.0, 0.0, 1.0 ],
                                   ] )
        np.testing.assert_array_equal(rotations[0], expected_rot_0)
        pass


class VirgoDataPlaybackTestCase(unittest.TestCase):
    def setUp(self):
        self.scene = {}
        self.scene['actors']  = {}
        self.scene['actors']['test_actor'] = {'mesh': 'PREFAB:cube'}
        self.scene['recorded_data']  = {}
        self.scene['recorded_data']['time']  = {}
        self.scene['recorded_data']['pos']  = {}
        self.scene['recorded_data']['time']['group']  = "one_body_static"
        self.scene['recorded_data']['pos']['group']  = "one_body_static"
        self.scene['recorded_data']['time']['var']  = "sys.exec.out.time"
        self.scene['recorded_data']['pos']['var']  = "position[0-2]"

    def test_init_RUN_0(self):
        self.instance = VirgoDataPlayback(
            run_dir=os.path.join(tests_dir, 'recorded_data/RUN_0'),
            scene=self.scene)

        self.instance.initialize()
        # TODO NEXT: this isn't working yet, it's passed into trickpy as
        # position[0-2] because I haven't used the expand_arrays method
        # during the __init__ yet. Still need to understand the details
        # on variables=[list of vars] given to trickpy wrt how [] arrays
        # are expected or not
