#!/usr/bin/env python3
import argparse
import unittest
import os
import sys
import logging

from io import StringIO

import os, sys, pdb

this_dir = os.path.abspath(os.path.dirname(__file__))
sys.path.append(os.path.join(this_dir, '../'))
from WorkflowCommon import *

def run_tests(args):

    loader = unittest.TestLoader()

    suites = loader.discover(start_dir=this_dir, pattern='test.py', top_level_dir=this_dir)
    overall_suite = unittest.TestSuite()
    for test_suite in suites:
        if test_suite._tests:
            overall_suite.addTests(test_suite)

    logger = logging.getLogger()
    logger.handlers = []
    logger.disabled = True

    # Run all unit tests
    print("Executing all python unit test suites...")
    runner = unittest.TextTestRunner(verbosity=args.verbosity, buffer=True)
    ut_results = runner.run(overall_suite)

    # Run all doc tests by eating our own dogfood
    doctest_files = ['TrickWorkflow.py', 'WorkflowCommon.py']
    wc = WorkflowCommon(this_dir, quiet=True)
    jobs = []
    log_prepend = '_doctest_log.txt'
    for file in doctest_files:
        job = ( Job(name='Running doctest on ' + file, command='python -m doctest -v ../%s' % file,
           log_file=file+log_prepend))
        jobs.append(job)
        wc.execute_jobs([job])

    print("doctest verbose output can be found in *%s" % (log_prepend))
    return (len(ut_results.failures) > 0  or len(ut_results.errors) > 0 or
            any(job.get_status() is not job.Status.SUCCESS for job in jobs))


# Main execution block
if __name__ == '__main__':
    # Create our Argument Parser
    parser = argparse.ArgumentParser(
        description="Execute all trickops python unit and doc tests. Must be run from this directory"
        " inside a python3 environment with dependencies described by trickops/requirements.txt. "
        " If koviz is not on your path, koviz-related unit tests will fail.")
    parser.add_argument("-v", "--verbosity", type=int,
         help="Reporting level when running from the console.", choices=[1, 2, 3, 4, 5], default=1)

    # Parse the arguments
    args = parser.parse_args()

    # Execute our tests
    sys.exit(run_tests(args))
