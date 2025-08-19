#!/usr/bin/env python3

"""Unit test script to test VIRGO module"""

import os, sys, pdb
import unittest

import ut_VirgoActor

# Define load_tests function for dynamic loading using Nose2
def load_tests(*args):
    passed_args = locals()
    suite = unittest.TestSuite()
    suite.addTests(ut_VirgoActor.suite())
    return suite

# Local module level execution only
if __name__ == '__main__':
    suites = unittest.TestSuite()
    suites.addTests(ut_VirgoActor.suite())

    unittest.TextTestRunner(verbosity=2).run(suites)
