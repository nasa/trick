import os, sys, glob
import unittest, shutil
import pdb
from testconfig import this_trick, tests_dir
import send_hs
def suite():
    """Create test suite from test cases here and return"""
    suites = []
    suites.append(unittest.TestLoader().loadTestsFromTestCase(SendHsTestCase))
    suites.append(unittest.TestLoader().loadTestsFromTestCase(SendHsNoFileTestCase))
    suites.append(unittest.TestLoader().loadTestsFromTestCase(SendHsMissingAllTestCase))
    suites.append(unittest.TestLoader().loadTestsFromTestCase(SendHsMissingSomeTestCase))
    suites.append(unittest.TestLoader().loadTestsFromTestCase(SendHsVerifyDiagNames))
    return (suites)

class SendHsTestCase(unittest.TestCase):

    def setUp(self):
        # Nominal - reading a valid send_hs output file
        self.instance = send_hs.send_hs(hs_file=os.path.join(tests_dir, 'send_hs.nominal'))
        self.assertEqual(self.instance.missing_diagnostics(), [])

    def tearDown(self):
        if self.instance:
            del self.instance
        self.instance = None

    def assert_values(self, instance):
        self.assertEqual(instance.hs_file, os.path.join(tests_dir, 'send_hs.nominal'))
        self.assertEqual(instance.num_lines, 29)
        # Test the values of each individual diagnostic name
        self.assertEqual(instance.get('ACTUAL INIT TIME'),                    42.606)
        self.assertEqual(instance.get('ACTUAL INIT'),                         42.606) # Partial name test
        self.assertEqual(instance.get('ACTUAL ELAPSED TIME'),                 55.551)
        self.assertEqual(instance.get('SIMULATION START TIME'),               0.0)
        self.assertEqual(instance.get('SIMULATION STOP TIME'),                68.955)
        self.assertEqual(instance.get('SIMULATION ELAPSED TIME'),             68.955)
        self.assertEqual(instance.get('USER CPU TIME USED'),                  55.690 )
        self.assertEqual(instance.get('SYSTEM CPU TIME USED'),                0.935)
        self.assertEqual(instance.get('SIMULATION / CPU TIME'),               1.218)
        self.assertEqual(instance.get('INITIALIZATION USER CPU TIME'),        42.783 )
        self.assertEqual(instance.get('INITIALIZATION SYSTEM CPU TIME'),      0.901)
        self.assertEqual(instance.get('SIMULATION RAM USAGE'),                1198.867)
        self.assertEqual(instance.get('VOLUNTARY CONTEXT SWITCHES (INIT)'),   792)
        self.assertEqual(instance.get('VOLUNTARY CONTEXT SWITCHES'),          792)  # Partial name test
        self.assertEqual(instance.get('INVOLUNTARY CONTEXT SWITCHES (INIT)'), 187)
        self.assertEqual(instance.get('VOLUNTARY CONTEXT SWITCHES (RUN)'),    97)
        self.assertEqual(instance.get('INVOLUNTARY CONTEXT SWITCHES (RUN)'),  14 )
        self.assertEqual(instance.is_valid(), True)
        self.assertEqual(instance.missing_diagnostics(), [])

        # Test the values of the full dict
        diags = self.instance.get_diagnostics()
        self.assertEqual(diags['ACTUAL INIT TIME'],                    42.606)
        self.assertEqual(diags['ACTUAL ELAPSED TIME'],                 55.551)
        self.assertEqual(diags['SIMULATION START TIME'],               0.0)
        self.assertEqual(diags['SIMULATION STOP TIME'],                68.955)
        self.assertEqual(diags['SIMULATION ELAPSED TIME'],             68.955)
        self.assertEqual(diags['USER CPU TIME USED'],                  55.690 )
        self.assertEqual(diags['SYSTEM CPU TIME USED'],                0.935)
        self.assertEqual(diags['SIMULATION / CPU TIME'],               1.218)
        self.assertEqual(diags['INITIALIZATION USER CPU TIME'],        42.783 )
        self.assertEqual(diags['INITIALIZATION SYSTEM CPU TIME'],      0.901)
        self.assertEqual(diags['SIMULATION RAM USAGE'],                1198.867)
        self.assertEqual(diags['VOLUNTARY CONTEXT SWITCHES (INIT)'],   792)
        self.assertEqual(diags['INVOLUNTARY CONTEXT SWITCHES (INIT)'], 187)
        self.assertEqual(diags['VOLUNTARY CONTEXT SWITCHES (RUN)'],    97)
        self.assertEqual(diags['INVOLUNTARY CONTEXT SWITCHES (RUN)'],  14 )

        # Test the other way to get diagnostics and ensure we get the same result
        diags2 = self.instance.diagnostics
        self.assertEqual(diags,  diags2 )


    def test_get_unknown_diag(self):
        with self.assertRaises(LookupError):
            self.instance.get('fake_diag_name')

    def test_init(self):
        # Init happened in setUp, make sure values are right
        self.assert_values(self.instance)

    def test_twostep_init(self):
        # Construct without giving the instance a file to parse
        self.tsi = send_hs.send_hs()
        # Everything is None before the file has been parsed
        for d in self.tsi._diagnostics:
            self.assertEqual(self.tsi._diagnostics[d], None)
        # Parse the file after construction
        self.tsi = send_hs.send_hs('send_hs.nominal')
        # Make sure values are right
        self.assert_values(self.instance)

class SendHsNoFileTestCase(unittest.TestCase):
    def setUp(self):
        # No-file constructions
        self.instance = send_hs.send_hs()

    def test_init(self):
        with self.assertRaises(RuntimeError):
            self.instance.parse()  # Nothing to parse, no file given

class SendHsMissingAllTestCase(unittest.TestCase):

    def setUp(self):
        # Invalid - reading a send_hs file missing all expected output
        self.instance = send_hs.send_hs(hs_file=os.path.join(tests_dir, 'send_hs.missing_all'))

    def tearDown(self):
        if self.instance:
            del self.instance
        self.instance = None

    def test_init(self):
        self.assertEqual(self.instance.num_lines, 5)
        self.assertEqual(self.instance.is_valid(), False)
        self.assertEqual(self.instance.missing_diagnostics(), [
            'ACTUAL INIT TIME',
            'ACTUAL ELAPSED TIME',
            'SIMULATION START TIME',
            'SIMULATION STOP TIME',
            'SIMULATION ELAPSED TIME',
            'USER CPU TIME USED',
            'SYSTEM CPU TIME USED',
            'SIMULATION / CPU TIME',
            'INITIALIZATION USER CPU TIME',
            'INITIALIZATION SYSTEM CPU TIME',
            'SIMULATION RAM USAGE',
            'VOLUNTARY CONTEXT SWITCHES (INIT)',
            'INVOLUNTARY CONTEXT SWITCHES (INIT)',
            'VOLUNTARY CONTEXT SWITCHES (RUN)',
            'INVOLUNTARY CONTEXT SWITCHES (RUN)'
            ])

class SendHsMissingSomeTestCase(unittest.TestCase):

    def setUp(self):
        # Invalid - reading a send_hs file missing all expected output
        self.instance = send_hs.send_hs(hs_file=os.path.join(tests_dir, 'send_hs.missing_some'))

    def tearDown(self):
        if self.instance:
            del self.instance
        self.instance = None

    def test_init(self):
        self.assertEqual(self.instance.num_lines, 26)
        self.assertEqual(self.instance.is_valid(), False)
        self.assertEqual(self.instance.missing_diagnostics(), [
            'USER CPU TIME USED',
            'INITIALIZATION SYSTEM CPU TIME',
            'INVOLUNTARY CONTEXT SWITCHES (INIT)',
            ])
        self.assertEqual(self.instance.get('ACTUAL INIT TIME'),                    32.306)
        self.assertEqual(self.instance.get('ACTUAL INIT'),                         32.306) # Partial name test
        self.assertEqual(self.instance.get('ACTUAL ELAPSED TIME'),                 35.551)
        self.assertEqual(self.instance.get('SIMULATION START TIME'),               -10.00)
        self.assertEqual(self.instance.get('SIMULATION STOP TIME'),                78.955)
        self.assertEqual(self.instance.get('SIMULATION ELAPSED TIME'),             88.955)
        self.assertEqual(self.instance.get('SYSTEM CPU TIME USED'),                3.235)
        self.assertEqual(self.instance.get('SIMULATION / CPU TIME'),               9.218)
        self.assertEqual(self.instance.get('INITIALIZATION USER CPU TIME'),        72.763 )
        self.assertEqual(self.instance.get('SIMULATION RAM USAGE'),                193.867)
        self.assertEqual(self.instance.get('VOLUNTARY CONTEXT SWITCHES (INIT)'),   292)
        self.assertEqual(self.instance.get('VOLUNTARY CONTEXT SWITCHES'),          292)  # Partial name test
        self.assertEqual(self.instance.get('VOLUNTARY CONTEXT SWITCHES (RUN)'),    37)
        self.assertEqual(self.instance.get('INVOLUNTARY CONTEXT SWITCHES (RUN)'),  17 )


class SendHsVerifyDiagNames(unittest.TestCase):
    '''
    This test is bonkers. Here we search for the keys in the self._diagnostics (which
    contain the text patterns we expect in the send_hs message) in the tracked source
    code of Trick. This test should fail if the diagnostics are ever changed in name.

    The other way to do this would be to ingest a real send_hs generated from a
    sim built and run before these tests run, but that adds significant complexity
    and a new dependency to this unit testing -Jordan 12/2024
    '''

    def setUp(self):
        # An empty instance is all we need since the diag keys are baked into the class
        self.instance = send_hs.send_hs()
        self.files_to_search = [
          os.path.join(this_trick, "trick_source/sim_services/Executive/Executive_shutdown.cpp"),
          os.path.join(this_trick, "trick_source/sim_services/RealtimeSync/RealtimeSync.cpp"),
          ]
        self.the_code = []
        for source_file in self.files_to_search:
          with open(source_file, 'r') as f:
            self.the_code += f.readlines()

    def tearDown(self):
        if self.instance:
            del self.instance
        self.instance = None

    def test_verify_diags(self):
        # Look for the diagnostic text in the files to search, and note
        # that they are FOUND if they are
        for d in self.instance.keys():
            if self.instance._diagnostics[d] == 'FOUND':
                continue
            for line in self.the_code:
                if d in line:
                    self.instance._diagnostics[d] = 'FOUND'
                    break
        diags = self.instance.diagnostics
        # Assert that all expected text was found in the source code
        for d in diags:
            self.assertEqual(diags[d], 'FOUND')





