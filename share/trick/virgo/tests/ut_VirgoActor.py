import os, sys, shutil, inspect
import unittest
import pdb

# Add path to virgo module
thisFileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))
virgo_dir=os.path.abspath(os.path.join(thisFileDir, '../'))
sys.path.append(virgo_dir)
from Virgo import *
meshes_dir=os.path.join(virgo_dir, 'meshes')

def suite():
    """Create test suite from test cases here and return"""
    suites = []
    suites.append(unittest.TestLoader().loadTestsFromTestCase(VirgoActorTestCase))
    return (suites)

class VirgoActorTestCase(unittest.TestCase):

    def setUp(self):
        # Nominal no-error when parsing the trick-sims config file scenario
        self.instance = VirgoActor(mesh=os.path.join(meshes_dir, 'teapot.obj'))

    def test_init_nominal(self):
        self.assertEqual(self.instance.name, 'No Name')
        self.assertEqual(self.instance.init_pyr, None)