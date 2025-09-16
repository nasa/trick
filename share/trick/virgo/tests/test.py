#!/usr/bin/env python3

"""Unit test script to test VIRGO module"""

import os, sys, pdb
import unittest, argparse

import ut_VirgoDataFileSource
import ut_VirgoDataPlaybackActor
import ut_VirgoDataPlayback
import ut_VirgoSceneNode

# Define load_tests function for dynamic loading using Nose2
def load_tests(*args):
    passed_args = locals()
    suite = unittest.TestSuite()
    suite.addTests(ut_VirgoDataPlaybackActor.suite())
    suite.addTests(ut_VirgoDataPlayback.suite())
    return suite

# Local module level execution only
if __name__ == '__main__':
    parser = argparse.ArgumentParser(
      description='Run all VIRGO unit tests.'
    )
    parser.add_argument('-v', '--visualize', action="store_true",
      help='Visualize all tests configured with this capability by setting'
      ' VIRGO_VISUALIZE_TESTS=1 before execution'
    )
    parser.add_argument('-s', '--save-images', action="store_true",
      help='Save images for all tests configured with this capability by'
      ' setting VIRGO_WRITE_TEST_IMAGES=1 before execution'
    )
    parser.add_argument('--ci-mode', action="store_true",
      help='Forcibly suppress all rendered windows by setting'
      ' VIRGO_BATCH_TESTS_OVERRIDE=1 before execution. Overrides -v.'
    )
    args = parser.parse_args()
    if args.visualize:
      os.environ["VIRGO_VISUALIZE_TESTS"] = "1"
    if args.save_images:
      os.environ["VIRGO_WRITE_TEST_IMAGES"] = "1"
    if args.ci_mode: # Never bring up a rendering window
      os.environ["VIRGO_BATCH_TESTS_OVERRIDE"] = "1"


    # Create the suite
    suites = unittest.TestSuite()

    suites.addTests(ut_VirgoDataFileSource.suite())
    suites.addTests(ut_VirgoDataPlaybackActor.suite())
    suites.addTests(ut_VirgoSceneNode.suite())
    # NOTE: this suite is last purposefully as putting it earlier appears to
    # result in random "bus error" and/or "segfault" messages when
    # self.visualize = True which results in rendered windows of unit tests.
    # Before VirgotDataPlayback.tear_down() existed as a function this suite
    # would somehow find an interactor/controller/renderer from previous tests
    # even though VirgoDataPlaybackTestCase never calls a function to start a
    # rendered window. Strange and needs more investigation - Jordan 9/2025
    suites.addTests(ut_VirgoDataPlayback.suite())
    # Execute all tests
    unittest.TextTestRunner(verbosity=2).run(suites)
