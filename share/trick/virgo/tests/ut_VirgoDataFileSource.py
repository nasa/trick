import os, sys, shutil, inspect, time
import unittest
import pdb

# Add path to virgo module
thisFileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))
virgo_dir=os.path.abspath(os.path.join(thisFileDir, '../'))
sys.path.append(virgo_dir)
from VirgoDataSource import *
meshes_dir=os.path.join(virgo_dir, 'meshes')
tests_dir=os.path.join(virgo_dir, 'tests')

def suite():
    """Create test suite from test cases here and return"""
    suites = []
    suites.append(unittest.TestLoader().loadTestsFromTestCase(VirgoDataFileSourceStaticMethodsTestCase))
    suites.append(unittest.TestLoader().loadTestsFromTestCase(VirgoDataFileSourceTestCase))
    return (suites)


class VirgoDataFileSourceStaticMethodsTestCase(unittest.TestCase):

    def test__find_index(self):
        """
        Test the _find_index static method
        """
        # A sorted list of doubles starting at 0.0 ending at 100.0 in steps of 0.5
        times = sorted([x * 0.5 for x in range(201)])
        # Test the different strategies
        #import pdb; pdb.set_trace()
        idx = VirgoDataFileSource._find_index(times, 0.7, mode='down')
        self.assertEqual(idx, 1)
        idx = VirgoDataFileSource._find_index(times, 0.7, mode='up')
        self.assertEqual(idx, 2)
        idx = VirgoDataFileSource._find_index(times, 0.7, mode='closest')
        self.assertEqual(idx, 1)
        # At a precise value, all modes should return the same index
        idx1 = VirgoDataFileSource._find_index(times, 0.0, mode='down')
        idx2 = VirgoDataFileSource._find_index(times, 0.0, mode='up')
        idx3 = VirgoDataFileSource._find_index(times, 0.0, mode='closest')
        self.assertEqual(idx1, 0)
        self.assertEqual(idx2, 0)
        self.assertEqual(idx3, 0)
        # Off the end of the list should return -1 (last index)
        idx = VirgoDataFileSource._find_index(times, 101.0)
        self.assertEqual(idx, -1)
        # Before the beginning of the list should return 0 (first index)
        idx = VirgoDataFileSource._find_index(times, -10.0)
        self.assertEqual(idx, 0)

    def test__find_index_timing(self):
        """
        Test the _find_index static method with respect to wall clock time.

        This function is called every update frame so it's efficiency is critical
        TODO: This might not be a good idea as wall-clock execution time changes
              machine to machine, we may wish to disable this test and just leave it
              around for manual use. -Jordan 9/2025
        """
        def get_time_taken(times_list, chosen_time):
            start_time = time.perf_counter()
            idx = VirgoDataFileSource._find_index(times_list, chosen_time, mode='down')
            end_time = time.perf_counter()
            time_taken = end_time - start_time
            print(f"DEBUG: _find_index() took {time_taken} sec operating on list of size {len(times_list)}")
            return time_taken

        # 10 entries
        time_taken = get_time_taken(sorted([x for x in range(10)]), 3.3)
        self.assertLess(time_taken, 1e-5)
        # 100 entries
        time_taken = get_time_taken(sorted([x for x in range(100)]), 33.3)
        self.assertLess(time_taken, 1e-5)
        # 1000 entries
        time_taken = get_time_taken(sorted([x for x in range(1000)]), 333.3)
        self.assertLess(time_taken, 1e-5)
        # 10000 entries
        time_taken = get_time_taken(sorted([x for x in range(10000)]), 3333.3)
        self.assertLess(time_taken, 1e-4)
        # 100000 entries
        time_taken = get_time_taken(sorted([x for x in range(100000)]), 33333.3)
        self.assertLess(time_taken, 1e-4)
        # 1000000 entries
        time_taken = get_time_taken(sorted([x for x in range(1000000)]), 333333.3)
        self.assertLess(time_taken, 1e-4)

class VirgoDataFileSourceTestCase(unittest.TestCase):
    def setUp(self):
        self.instance = VirgoDataFileSource()

    def test_nominal(self):
        """
        Test the expand_arrays static method
        """
        pass
