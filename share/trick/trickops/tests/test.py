#!/usr/bin/env python3

"""Unit test script to test local modules."""

import os, sys, pdb
import unittest

import ut_send_hs
import ut_WorkflowCommon
import ut_TrickWorkflowYamlVerifier
import ut_TrickWorkflow
import ut_MonteCarloGenerationHelper

# Define load_tests function for dynamic loading using Nose2
def load_tests(*args):
    passed_args = locals()
    suite = unittest.TestSuite()
    suite.addTests(ut_send_hs.suite())
    suite.addTests(ut_TrickWorkflowYamlVerifier.suite())
    suite.addTests(ut_TrickWorkflow.suite())
    suite.addTests(ut_WorkflowCommon.suite())
    suite.addTests(ut_MonteCarloGenerationHelper.suite())
    return suite

# Local module level execution only
if __name__ == '__main__':
    suites = unittest.TestSuite()
    suites.addTests(ut_send_hs.suite())
    suites.addTests(ut_TrickWorkflowYamlVerifier.suite())
    suites.addTests(ut_TrickWorkflow.suite())
    suites.addTests(ut_WorkflowCommon.suite())
    suites.addTests(ut_MonteCarloGenerationHelper.suite())

    unittest.TextTestRunner(verbosity=2).run(suites)
