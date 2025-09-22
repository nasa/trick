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
    suites.append(unittest.TestLoader().loadTestsFromTestCase(VirgoDataPlaybackInitTestCase))
    suites.append(unittest.TestLoader().loadTestsFromTestCase(VirgoDataPlaybackFunctionsTestCase))
    return (suites)

    import time

class VirgoDataPlaybackInitTestCase(unittest.TestCase):
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

    def tearDown(self):
        self.instance.tear_down()
        self.instance = None

    def test_init_RUN_0(self):
        self.instance = VirgoDataPlayback(
            run_dir=os.path.join(tests_dir, 'recorded_data/RUN_0'),
            scene=self.scene)

        self.instance.initialize()
    
class VirgoDataPlaybackFunctionsTestCase(VisualizableTestCase):
    def setUp(self):
        VisualizableTestCase().setUp()
        self.scene = {}
        self.scene['actors']  = {}
        self.scene['actors']['test_actor'] = {'mesh': 'PREFAB:cube'}
        self.scene['vectors']  = {}
        self.scene['vectors']['test_vector'] = {'parent': 'test_actor'}
        self.scene['recorded_data']  = {}
        self.scene['recorded_data']['time']  = {}
        self.scene['recorded_data']['pos']  = {}
        self.scene['recorded_data']['time']['group']  = "one_body_static"
        self.scene['recorded_data']['pos']['group']  = "one_body_static"
        self.scene['recorded_data']['time']['var']  = "sys.exec.out.time"
        self.scene['recorded_data']['pos']['var']  = "position[0-2]"
        self.instance = VirgoDataPlayback(
            run_dir=os.path.join(tests_dir, 'recorded_data/RUN_0'),
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
        #self.visualize=True
        #self.vis(actor)

    def test_create_vector(self):
        vector_dict = self.scene['vectors']['test_vector']
        vector = self.instance.create_vector('test_vector', vector_dict)
        self.assertEqual(vector.name, 'test_vector')
        self.assertEqual(vector.mesh, 'PREFAB:arrow')
        #self.visualize=True
        #self.vis(vector)

    
