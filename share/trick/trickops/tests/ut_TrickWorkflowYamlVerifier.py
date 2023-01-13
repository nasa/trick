import os, sys
import unittest
import pdb
from testconfig import this_trick, tests_dir
from TrickWorkflowYamlVerifier import *

def suite():
    """Create test suite from TrickWorkflowTestCase unit test class and return"""
    return unittest.TestLoader().loadTestsFromTestCase(TrickWorkflowYamlVerifierTestCase)

class TrickWorkflowYamlVerifierTestCase(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def _verify(self, config_file):
        """
        Given a config_file, create a TrickWorkflowYamlVerifier, assert expectations,
        and return the instance for further examination
        """
        twyv = TrickWorkflowYamlVerifier(config_file=config_file)
        twyv.verify()
        sim_keys     = [sim for sim     in twyv.config.keys() if sim.startswith('SIM')]
        self.assertTrue('globals' in twyv.config)
        self.assertTrue('env'     in twyv.config['globals'])
        for sk in sim_keys:
            self.assertTrue( isinstance( twyv.config[sk]['name'           ], str))
            self.assertTrue( isinstance( twyv.config[sk]['binary'         ], str))
            self.assertTrue( isinstance( twyv.config[sk]['build_args'], str) or
                              twyv.config[sk]['build_args'] == None )
            self.assertTrue( isinstance( twyv.config[sk]['name'           ], str))
            self.assertTrue( isinstance( twyv.config[sk]['parallel_safety'], str))
            self.assertTrue( twyv.config[sk]['description'    ] is None)
            self.assertTrue( isinstance( twyv.config[sk]['runs'], dict))
            for run in twyv.config[sk]['runs']:
                self.assertTrue( isinstance( twyv.config[sk]['runs'][run]['input'],    str))
                self.assertTrue( isinstance( twyv.config[sk]['runs'][run]['returns'],  int))
                self.assertTrue( isinstance( twyv.config[sk]['runs'][run]['valgrind'], str) or
                                            (twyv.config[sk]['runs'][run]['valgrind'] == None) )
                self.assertTrue( isinstance( twyv.config[sk]['runs'][run]['analyze'], str) or
                                            (twyv.config[sk]['runs'][run]['analyze'] == None) )
                self.assertTrue( isinstance( twyv.config[sk]['runs'][run]['phase'],    int))
                self.assertTrue( isinstance( twyv.config[sk]['runs'][run]['compare'], list))
            self.assertTrue( isinstance( twyv.config[sk]['phase'          ], int))
            self.assertTrue( isinstance( twyv.config[sk]['path'           ], str))
        return twyv

    def test_type_errors_in_config(self):
        twyv = self._verify(config_file=os.path.join(tests_dir,"type_errors.yml"))
        self.assertEqual(len(twyv.get_parsing_errors()), 21)

    def test_no_SIM_dict_keys_in_config(self):
        twyv = TrickWorkflowYamlVerifier(config_file=os.path.join(tests_dir,"errors_fatal2.yml"))
        with self.assertRaises(RuntimeError):
            twyv.verify()
        self.assertEqual(len(twyv.get_parsing_errors()), 1)

    def test_empty_config(self):
        twyv = TrickWorkflowYamlVerifier(config_file=os.path.join(tests_dir,"empty.yml"))
        with self.assertRaises(RuntimeError):
            twyv.verify()
        self.assertEqual(len(twyv.get_parsing_errors()), 1)

    def test_nominal_config(self):
        twyv = self._verify(config_file=os.path.join(tests_dir,"trick_sims.yml"))
        sim_keys     = [sim for sim     in twyv.config.keys() if sim.startswith('SIM')]
        non_sim_keys = [entry for entry in twyv.config.keys() if not entry.startswith('SIM')]
        self.assertEqual(len(non_sim_keys), 2)  # Expect 2 non-SIM.*: dict key
        self.assertEqual(len(sim_keys), 56)     # Expect 56 SIM.*: dict keys
        self.assertEqual(len(twyv.get_parsing_errors()), 0)
