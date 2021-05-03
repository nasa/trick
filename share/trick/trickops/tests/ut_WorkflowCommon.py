import os, sys
import unittest, time
import pdb
from testconfig import this_trick, tests_dir
from WorkflowCommon import *

def suite():
    """Create test suite from WorkflowCommonTestCase unit test class and return"""
    return unittest.TestLoader().loadTestsFromTestCase(WorkflowCommonTestCase)

class WorkflowCommonTestCase(unittest.TestCase):

    def setUp(self):
        # Nominal instance, using this Trick as top level project
        self.instance = WorkflowCommon(project_top_level=this_trick, log_dir='/tmp/', quiet=True)
        # Nominal job that just echo's 'hi', this creates it, it doesn't execute it
        self.job_nominal = Job(name='testname', command='echo hi',
          log_file='/tmp/WorkflowCommonTestCase_hi.txt')
        self.job_that_fails = Job(name='testname', command='echo hi',
          log_file='/tmp/WorkflowCommonTestCase_hi.txt')
        pass

    def tearDown(self):
        os.remove(self.instance.log)
        del self.instance
        self.instance = None

    def test_run_subprocess(self):
        result = run_subprocess(command='echo hi', m_shell=subprocess.PIPE, m_stdout=subprocess.PIPE)
        self.assertEqual(result.code, 0)
        self.assertTrue('hi' in result.stdout )

    def test_validate_output_file(self):
        dir = '/tmp/aoeifmeganrsdfalk/'
        rest = 'b/c/d/foo.txt'
        file = os.path.join(dir, rest)
        filename = validate_output_file(file)
        self.assertEqual(filename, file)
        self.assertTrue(os.path.exists(file))
        # Clean up everything we just created
        import shutil
        shutil.rmtree(dir)

    def test_create_progress_bar(self):
        bar = create_progress_bar(fraction=0.25, text='hello')
        self.assertTrue('hello' in bar)
        self.assertTrue('====================' in bar)
        self.assertTrue('=====================' not in bar)
        self.assertTrue(len(bar) == 80)

    def test_sanitize_cpus(self):
        cpus = sanitize_cpus(num_cpus=3, num_tasks=4, fallback_cpus=2)
        # This check is weird but it's b/c the function acts differently per machine
        self.assertTrue(cpus[0] == 3 or cpus[0] == 2)

    def test_unixify_string(self):
        string = unixify_string("hi/ there ()! bud?")
        self.assertEqual(string, "hi__there__bud")

    def test_init_nominal(self):
        self.assertEqual(self.instance.project_top_level, this_trick)
        self.assertEqual(self.instance.log_dir, '/tmp/')
        self.assertEqual(self.instance.env, '')
        self.assertTrue(self.instance.creation_time < time.time())
        self.assertTrue(self.instance.host_name != '' )
        self.assertTrue(self.instance.this_os == None )
        self.assertTrue(self.instance.quiet == True )
        self.assertTrue(os.getcwd() == this_trick )
        self.assertTrue(self.job_nominal.name == 'testname')

    def test_get_operating_system_nominal(self):
        '''
        This test will respond differently per-platform. Nominal checks
        here should pass for all "supported platforms"
        '''
        self.assertTrue(self.instance.get_operating_system() != 'unknown')

    def test_get_installed_packages_nominal(self):
        '''
        This test will respond differently per-platform. Nominal checks
        here should pass for all "supported platforms"
        '''
        pkgs = self.instance.get_installed_packages()
        self.assertTrue(len(pkgs) > 0)

    def test_job_nominal(self):
        self.instance.execute_jobs([self.job_nominal], max_concurrent=1)
        self.assertTrue(self.job_nominal.get_status() == Job.Status.SUCCESS)
        f = open('/tmp/WorkflowCommonTestCase_hi.txt', 'r')
        self.assertTrue(f.readlines()[0].strip() == 'hi')
        f.close()

    def test_job_nominal_quiet(self):
        self.instance.quiet = True
        self.instance.execute_jobs([self.job_nominal], max_concurrent=1)
        self.assertTrue(self.job_nominal.get_status() == Job.Status.SUCCESS)
        f = open('/tmp/WorkflowCommonTestCase_hi.txt', 'r')
        self.assertTrue(f.readlines()[0].strip() == 'hi')
        f.close()
