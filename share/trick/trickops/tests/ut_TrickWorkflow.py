import os, sys, shutil
import unittest
import pdb
from testconfig import this_trick, tests_dir
from TrickWorkflow import *

def suite():
    """Create test suite from TrickWorkflowTestCase unit test class and return"""
    return unittest.TestLoader().loadTestsFromTestCase(TrickWorkflowTestCase)

class TrickWorkflowTestCase(unittest.TestCase):

    def setUp(self):
        # Nominal no-error when parsing the trick-sims config file scenario
        self.instance = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/',
          trick_dir=this_trick, config_file=os.path.join(tests_dir,"trick_sims.yml"),
          quiet=True)

    def tearDown(self):
        if self.instance:
            self.instance._cleanup()  # Remove the log file this instance creates
            del self.instance
        self.instance = None

    def setUpWithEmptyConfig(self):
        self.instance = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/',
          trick_dir=this_trick, config_file=os.path.join(tests_dir,"empty.yml"),
          quiet=True)

    def setUpWithErrorConfig(self, file):
        self.tearDown()  # Cleanup the instance we get by default
        self.instance = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/',
          trick_dir=this_trick, config_file=os.path.join(tests_dir,file),
          quiet=True)

    def test_init_nominal(self):
        self.assertEqual(self.instance.cpus, 3)
        self.assertEqual(self.instance.parallel_safety, 'loose')
        self.assertEqual(self.instance.config_errors, False)
        self.instance.report()
        build_jobs = self.instance.get_jobs('build')
        self.assertEqual(len(build_jobs), 56)
        self.assertEqual(len(self.instance.sims), 56)
        run_jobs = self.instance.get_jobs('run')
        self.assertEqual(len(run_jobs), 38)

    def test_init_empty_so_raises(self):
        with self.assertRaises(RuntimeError):
            self.setUpWithEmptyConfig()

    def test_init_bad_yaml_so_raises(self):
        with self.assertRaises(RuntimeError):
            self.setUpWithErrorConfig("errors_fatal1.yml")
        with self.assertRaises(RuntimeError):
            self.setUpWithErrorConfig("errors_fatal2.yml")

    def test_init_errors_but_no_raise(self):
        self.setUpWithErrorConfig("errors_nonfatal.yml")
        self.assertTrue(self.instance.config_errors)
        self.assertEqual(self.instance.parallel_safety , 'loose')
        self.assertEqual(len(self.instance.sims), 4)
        self.assertEqual(len(self.instance.get_sim('SIM_ball_L1').get_runs()), 1)
        self.assertEqual(len(self.instance.get_sim('SIM_ball_L1').get_valgrind_runs()), 0)
        self.assertEqual(len(self.instance.get_sim('SIM_alloc_test').get_runs()), 1)
        self.assertEqual(len(self.instance.get_sim('SIM_alloc_test').get_valgrind_runs()), 0)
        self.assertEqual(self.instance.config['SIM_alloc_test']['runs']['RUN_test/input.py']['extension'], 'foobar')
        self.assertEqual(len(self.instance.get_sim('SIM_events').get_runs()), 0)
        self.assertTrue('fine1' in self.instance.get_sim('SIM_events').labels)
        self.assertTrue('fine2' in self.instance.get_sim('SIM_events').labels)
        self.assertEqual(self.instance.get_sim('SIM_threads').get_run('RUN_test/sched.py').returns, 0)
        self.assertEqual(self.instance.get_sim('SIM_threads').get_run('RUN_test/amf.py').returns, 0)
        self.assertEqual(self.instance.get_sim('SIM_threads').get_run('RUN_test/async.py').returns, 0)
        self.assertEqual(self.instance.get_sim('SIM_threads').get_run('RUN_test/unit_test.py').returns, 7)
        self.assertTrue(self.instance.get_sim('SIM_demo_inputfile') is None)
        self.assertTrue(self.instance.get_sim('SIM_L1_ball') is None)
        self.assertTrue(self.instance.get_sim('SIM_foobar') is None)
        self.assertTrue(self.instance.get_sim('SIM_parachute') is None)

        self.assertTrue(self.instance.config['extension_example'])
        self.instance.report()

    def test_get_sim_nominal(self):
        sim = self.instance.get_sim(identifier='SIM_ball_L1')
        self.assertTrue(type(sim) == TrickWorkflow.Sim)
        sim = self.instance.get_sim(identifier='trick_sims/Ball/SIM_ball_L1')
        self.assertTrue(type(sim) == TrickWorkflow.Sim)

    def test_get_sim_not_found(self):
        sim = self.instance.get_sim(identifier='SIM_doesnt_exist')
        self.assertTrue(sim is None)

    def test_get_sim_raises(self):
        with self.assertRaises(TypeError):
            sim = self.instance.get_sim(identifier={})

    def test_get_sims_nominal(self):
        sims = self.instance.get_sims(labels='unit_test')
        self.assertEqual(len(sims), 14)
        sims = self.instance.get_sims(labels=['unit_test'])
        self.assertEqual(len(sims), 14)
        sims = self.instance.get_sims(labels=['unit_test', 'demo'])
        self.assertEqual(len(sims), 1)
        sims = self.instance.get_sims(labels=['unit_test', 'events'])
        self.assertEqual(len(sims), 1)
        sims = self.instance.get_sims(labels=['events'])
        self.assertEqual(len(sims), 1)

    def test_get_sims_not_found(self):
        sims = self.instance.get_sims(labels=['unit_test', 'noexitlabel'])
        self.assertTrue(len(sims) == 0)

    def test_get_sims_raises(self):
        with self.assertRaises(TypeError):
            sims = self.instance.get_sims(identifier={})

    def test_get_run_nominal(self):
        sim = self.instance.get_sim(identifier='SIM_ball_L1')
        run = sim.get_run('RUN_test/input.py')

    def test_get_unique_comparison_dirs(self):
        ucd = self.instance.get_unique_comparison_dirs()
        self.assertTrue(ucd[0] is not None)
        self.assertTrue(ucd[0][0] is not None)
        self.assertTrue(ucd[0][1] is not None)

    def test_get_koviz_report_jobs_nominal(self):
        krj = self.instance.get_koviz_report_jobs()
        self.assertTrue(isinstance(krj[0][0], Job))
        self.assertTrue(not krj[1])

    def test_get_koviz_report_job_missing_dir(self):
        krj = self.instance.get_koviz_report_job('share/trick/trickops/tests/testdata_noexist',
          'share/trick/trickops/tests/baselinedata')
        self.assertTrue(krj[0] is None)
        self.assertTrue('ERROR' in krj[1])

    def test_status_summary_nominal(self):
        summary = self.instance.status_summary()
        # Nothing was run, so nothing could fail
        self.assertEqual(summary, 'SUCCESS')

    def test_get_and_pop_run(self):
        sim = self.instance.get_sim('SIM_ball_L1')
        run = sim.get_run('RUN_test/input.py')
        self.assertEqual(run.input, 'RUN_test/input.py')
        run = sim.pop_run('RUN_test/input.py')
        self.assertEqual(run.input, 'RUN_test/input.py')
        self.assertEqual(len(sim.get_runs()), 0)

    def test_check_run_jobs(self):
        sim = self.instance.get_sim('SIM_ball_L1')
        normal_run_jobs = sim.get_run_jobs()
        self.assertTrue('valgrind' not in normal_run_jobs[0]._command)
        valgrind_run_jobs = sim.get_run_jobs(kind='valgrind')
        self.assertTrue('valgrind' in valgrind_run_jobs[0]._command)

    def test_compare(self):
        sim = self.instance.get_sim('SIM_ball_L1')
        # Sim level comparison (test_data.csv vs. baseline_data.csv) will fail
        self.assertEqual(sim.compare(), 1)
        # Run level comparison (test_data.csv vs. baseline_data.csv) will fail
        run = sim.get_run('RUN_test/input.py')
        self.assertEqual(run.compare(), 1)
        # Top level call to all comparisons will fail
        self.assertEqual(self.instance.compare(), 1)
        self.assertEqual(run.comparisons[0]._translate_status(), '\x1b[31mFAIL\x1b[0m')

    def test_get_jobs_nominal(self):
        # Test all the permissive permutations
        builds = self.instance.get_jobs('build')
        self.assertEqual(len(builds), 56)
        builds = self.instance.get_jobs('builds')
        self.assertEqual(len(builds), 56)
        runs = self.instance.get_jobs('run')
        self.assertEqual(len(runs), 38)
        runs = self.instance.get_jobs('runs')
        self.assertEqual(len(runs), 38)
        vg = self.instance.get_jobs('valgrind')
        self.assertEqual(len(vg), 1)
        vg = self.instance.get_jobs('valgrinds')
        self.assertEqual(len(vg), 1)
        a = self.instance.get_jobs('analysis')
        self.assertEqual(len(a), 1)
        a = self.instance.get_jobs('analyses')
        self.assertEqual(len(a), 1)
        a = self.instance.get_jobs('analyze')
        self.assertEqual(len(a), 1)

    def test_get_jobs_raises(self):
        with self.assertRaises(TypeError):
            jobs = self.instance.get_jobs(kind='bucees')

    def test_get_comparisons_nominal(self):
        c = self.instance.get_comparisons()
        self.assertEqual(len(c), 1)
        self.assertEqual(c[0]._translate_status(), '\x1b[33mNOT RUN\x1b[0m')

    def test_add_comparison(self):
        sim = self.instance.get_sim('SIM_alloc_test')
        run = sim.get_run('RUN_test/input.py')
        run.add_comparison('share/trick/trickops/tests/baselinedata/log_a.csv',
          'share/trick/trickops/tests/testdata/log_a.csv')
        self.assertTrue(len(run.comparisons) == 1)

    def test_add_analysis_nominal(self):
        sim = self.instance.get_sim('SIM_alloc_test')
        run = sim.get_run('RUN_test/input.py')
        run.add_analysis('echo analysis goes here')
        self.assertTrue( 'echo analysis goes here' in run.analysis._command)

    def test_add_analysis_warning(self):
        sim = self.instance.get_sim('SIM_ball_L1')
        run = sim.get_run('RUN_test/input.py')
        run.add_analysis('echo overwriting analysis')
        self.assertTrue( 'echo overwriting analysis' in run.analysis._command)

    def test_run_init(self):
        r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input='RUN_test/input.py --someflag',
          binary='S_main_Linux_x86_64.exe')
        self.assertEqual(r.sim_dir, 'test/SIM_alloc_test')
        self.assertEqual(r.prerun_cmd, '')
        self.assertTrue(r.input == 'RUN_test/input.py --someflag')
        self.assertEqual(r.returns, 0)
        self.assertTrue(r.valgrind_flags is None)
        self.assertEqual(r.log_dir, '/tmp/')
        self.assertEqual(r.just_input,'RUN_test/input.py')
        self.assertEqual(r.just_run_dir, 'RUN_test')
        self.assertEqual(r.run_dir_path, 'test/SIM_alloc_test/RUN_test')
        self.assertEqual(r.binary, 'S_main_Linux_x86_64.exe')
        self.assertTrue(r.run_job is None)
        self.assertEqual(len(r.comparisons), 0)
        self.assertTrue(r.analysis is None)

    def test_comparison_init(self):
        test_data = 'share/trick/trickops/tests/testdata/log_a.csv'
        baseline_data ='share/trick/trickops/tests/baselinedata/log_a.csv'
        c = TrickWorkflow.Comparison(test_data,baseline_data)
        self.assertTrue(c.baseline_data == baseline_data)
        self.assertTrue(c.test_data == test_data)
        self.assertTrue(c.error is None)

    def test_run_compare_pass(self):
        r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input='RUN_test/input.py --someflag',
          binary='S_main_Linux_x86_64.exe')
        # Use same data to get a pass
        test_data     = 'share/trick/trickops/tests/baselinedata/log_a.csv'
        baseline_data = 'share/trick/trickops/tests/baselinedata/log_a.csv'
        r.add_comparison(test_data, baseline_data)
        self.assertEqual(r.compare(), 0)

    def test_run_compare_fail(self):
        r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input='RUN_test/input.py --someflag',
          binary='S_main_Linux_x86_64.exe')
        # Use same data to get a pass
        test_data     = 'share/trick/trickops/tests/testdata/log_a.csv'
        baseline_data = 'share/trick/trickops/tests/baselinedata/log_a.csv'
        r.add_comparison(test_data, baseline_data)
        self.assertEqual(r.compare(), 1)

    def setup_deep_directory_structure(self, runs, parallel_safety='loose'):
        '''
        Build a temporary dir structure to test deep pathing
        runs is a list of runs relative to SIM_fake directory
        returns: str/path of config file for this deep setup
        '''
        deep_root = os.path.join(tests_dir, 'deep/')
        SIM_root = os.path.join(deep_root, 'SIM_fake/')
        for run in runs:
          just_RUN_rel = os.path.dirname(run)
          just_RUN_root = os.path.join(SIM_root, just_RUN_rel)
          SIM_root_rel =  os.path.relpath(SIM_root, this_trick)
          os.makedirs(just_RUN_root, exist_ok=True)
          Path(os.path.join(SIM_root,run)).touch()
        yml_content=textwrap.dedent("""
        globals:
            parallel_safety: """ + parallel_safety + """
        SIM_fake:
            path: """ +  SIM_root_rel + """
            runs:
        """)
        for run in runs:
            yml_content += "        " + run + ":\n"

        config_file = os.path.join(deep_root, "config.yml")
        f = open(config_file, "w")
        f.write(yml_content)
        f.close()
        return(config_file)
    def teardown_deep_directory_structure(self):
        # Clean up the dirs we created
        deep_root = os.path.join(tests_dir, 'deep/')
        shutil.rmtree(deep_root)

    def test_deep_directory_structure_nominal(self):
        '''
        Set up a deep directory structure for sims and runs which will exercise
        some of the path logic in _validate_config(). Nominal with no errors
        '''
        self.tearDown()  # Cleanup the instance we get by default, don't need it
        config_file = self.setup_deep_directory_structure(runs=['SET_fake/RUN_fake/input.py'])
        self.instance = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/',
          trick_dir=this_trick, config_file=config_file, quiet=True)
        self.assertEqual(len(self.instance.get_sims()), 1)
        self.assertEqual(len(self.instance.get_sim('SIM_fake').get_runs()), 1)
        self.teardown_deep_directory_structure()

    def test_deep_directory_structure_loose(self):
        '''
        Set up a deep directory structure for sims and runs which will exercise
        some of the path logic in _validate_config(). Duplicate runs with
        parallel safety = loose, should not error.
        '''
        self.tearDown()  # Cleanup the instance we get by default, don't need it
        config_file = self.setup_deep_directory_structure(runs=['SET_fake/RUN_fake/input.py',
          'SET_fake/RUN_fake/test.py'])
        self.instance = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/',
          trick_dir=this_trick, config_file=config_file, quiet=True)
        self.assertEqual(len(self.instance.get_sims()), 1)
        self.assertEqual(len(self.instance.get_sim('SIM_fake').get_runs()), 2)
        self.teardown_deep_directory_structure()

    def test_deep_directory_structure_strict_error(self):
        '''
        Set up a deep directory structure for sims and runs which will exercise
        some of the path logic in _validate_config(). Duplicate runs with
        parallel safety = strict, should non-fatal error.
        '''
        self.tearDown()  # Cleanup the instance we get by default, don't need it
        config_file = self.setup_deep_directory_structure(runs=['SET_fake/RUN_fake/input.py',
          'SET_fake/RUN_fake/test.py'], parallel_safety='strict')
        self.instance = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/',
          trick_dir=this_trick, config_file=config_file, quiet=True)
        self.assertEqual(len(self.instance.get_sims()), 1)
        self.assertEqual(len(self.instance.get_sim('SIM_fake').get_runs()), 1)
        self.teardown_deep_directory_structure()
