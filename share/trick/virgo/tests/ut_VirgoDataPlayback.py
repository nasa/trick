import os, sys, shutil, inspect
import unittest
import pdb

# Add path to virgo module
thisFileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))
virgo_dir=os.path.abspath(os.path.join(thisFileDir, '../'))
sys.path.append(virgo_dir)
from VirgoDataPlayback import *
meshes_dir=os.path.join(virgo_dir, 'meshes')
tests_dir=os.path.join(virgo_dir, 'tests')
from VisualizableTestCase import VisualizableTestCase

def suite():
    """Create test suite from test cases here and return"""
    suites = []
    suites.append(unittest.TestLoader().loadTestsFromTestCase(VirgoDataPlaybackInitTestCase))
    suites.append(unittest.TestLoader().loadTestsFromTestCase(VirgoDataPlaybackFunctionsTestCase))
    return (suites)

    import time

class VirgoDataPlaybackInitTestCase(unittest.TestCase):
    def setUp(self):
        self.scene = {}
        self.scene['actors']  = {}
        self.scene['actors']['test_actor'] = {'mesh': 'VIRGO_PREFAB:cube'}
        self.scene['data_source']  = {}
        self.scene['data_source']['trickpy']  = {}
        self.scene['data_source']['trickpy']['time']  = {}
        self.scene['data_source']['trickpy']['pos']  = {}
        self.scene['data_source']['trickpy']['time']['group']  = "one_body_static"
        self.scene['data_source']['trickpy']['pos']['group']  = "one_body_static"
        self.scene['data_source']['trickpy']['time']['var']  = "sys.exec.out.time"
        self.scene['data_source']['trickpy']['pos']['var']  = "position[0-2]"

    def tearDown(self):
        self.instance.tear_down()
        self.instance = None

    def test_init_RUN_0(self):
        self.instance = VirgoDataPlayback(
            run_dir=os.path.join(tests_dir, 'trickpy_data_source/RUN_0'),
            scene=self.scene)

        self.instance.initialize()
    
class VirgoDataPlaybackFunctionsTestCase(VisualizableTestCase):
    def setUp(self):
        VisualizableTestCase().setUp()
        self.scene = {}
        self.scene['actors']  = {}
        self.scene['actors']['test_actor'] = {'mesh': 'VIRGO_PREFAB:cube'}
        self.scene['vectors']  = {}
        self.scene['vectors']['test_vector'] = {'parent': 'test_actor'}
        self.scene['data_source']  = {}
        self.scene['data_source']['trickpy']  = {}
        self.scene['data_source']['trickpy']  = {}
        self.scene['data_source']['trickpy']['time']  = {}
        self.scene['data_source']['trickpy']['pos']  = {}
        self.scene['data_source']['trickpy']['time']['group']  = "one_body_static"
        self.scene['data_source']['trickpy']['pos']['group']  = "one_body_static"
        self.scene['data_source']['trickpy']['time']['var']  = "sys.exec.out.time"
        self.scene['data_source']['trickpy']['pos']['var']  = "position[0-2]"
        

        self.instance = VirgoDataPlayback(
            run_dir=os.path.join(tests_dir, 'trickpy_data_source/RUN_0'),
            scene=self.scene)

    def tearDown(self):
        # DO NOT super().tearDown() here, in this file we are calling
        # self.vis(actor) manually. If you super().tearDown() it gets
        # called twice and segfaults sometimes. This manual calling
        # approach might be better than doing it during tearDown anyhow
        # and maybe we should move everything to this approach
        self.instance = None
        del self.instance

    def test_create_actor(self):
        actor_dict = self.scene['actors']['test_actor']
        actor = self.instance.create_actor('test_actor', actor_dict)
        self.assertEqual(actor.name, 'test_actor')
        # TODO: There is still an issue with "Bus error" and "segmentation fault"
        # When multiple tests call self.vis() in a single TestCase
        #self.visualize=True
        #self.vis(actor)

    def test_create_vector(self):
        vector_dict = self.scene['vectors']['test_vector']
        vector = self.instance.create_vector('test_vector', vector_dict)
        self.assertEqual(vector.name, 'test_vector')
        self.assertEqual(vector.mesh, 'VIRGO_PREFAB:arrow')
        # TODO: There is still an issue with "Bus error" and "segmentation fault"
        # When multiple tests call self.vis() in a single TestCase
        #self.visualize=True
        #self.vis(vector)

    def test_create_custom_vector(self):
        """
        Test customization of a vector by changing it's configurable parameters
        """
        self.scene['vectors']['test_vector']['scale'] = 10.0
        self.scene['vectors']['test_vector']['tip_length'] = 0.7
        self.scene['vectors']['test_vector']['tip_radius'] = 0.1
        self.scene['vectors']['test_vector']['tip_resolution'] = 5
        self.scene['vectors']['test_vector']['shaft_radius'] = 0.03
        self.scene['vectors']['test_vector']['shaft_resolution'] = 5
        vector_dict = self.scene['vectors']['test_vector']
        custom_vector = self.instance.create_vector('test_vector', vector_dict)
        self.assertEqual(custom_vector.name, 'test_vector')
        self.assertEqual(custom_vector.mesh, 'VIRGO_PREFAB:arrow')
        # TODO: There is still an issue with "Bus error" and "segmentation fault"
        # When multiple tests call self.vis() in a single TestCase
        #self.visualize=True
        #self.vis(custom_vector)
