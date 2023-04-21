import os, sys, shutil
import unittest
import pdb
from testconfig import this_trick, tests_dir
from TrickWorkflow import *

def suite():
    """Create test suite from test cases here and return"""
    suites = []
    suites.append(unittest.TestLoader().loadTestsFromTestCase(TrickWorkflowTestCase))
    suites.append(unittest.TestLoader().loadTestsFromTestCase(TrickWorkflowSingleRunTestCase))
    return (suites)

class TrickWorkflowSingleRunTestCase(unittest.TestCase):
    def setUp(self):
        self.instance= SingleRun(name='testname', command='echo hi',
          log_file='/tmp/WorkflowCommonTestCase_hi.txt')

    def tearDown(self):
        del self.instance
        self.instance = None

    def test_SingleRun_Nominal(self):
        self.assertEqual(self.instance.name, 'testname')
        self.assertEqual(self.instance._command, 'echo hi')
        self.assertEqual(self.instance.log_file, '/tmp/WorkflowCommonTestCase_hi.txt')
        self.assertEqual(self.instance._log_file, None)
        self.assertEqual(self.instance._expected_exit_status, 0)
        self.assertEqual(self.instance.get_use_var_server(), True)
        # Test the setter for var server interaction
        self.instance.set_use_var_server(False)
        self.assertEqual(self.instance.get_use_var_server(), False)

class TrickWorkflowTestCase(unittest.TestCase):

    # TODO: not all jobs even use this setUp, probably should split the ones
    # out that need to load a different file or no file at all (static tests)
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

    def setUpWithEmptyConfig(self, file="empty.yml"):
        self.instance = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/',
          trick_dir=this_trick, config_file=os.path.join(tests_dir,file),
          quiet=True)

    def setUpWithErrorConfig(self, file):
        self.tearDown()  # Cleanup the instance we get by default
        self.instance = TrickWorkflow(project_top_level=this_trick, log_dir='/tmp/',
          trick_dir=this_trick, config_file=os.path.join(tests_dir,file),
          quiet=True)

    def test_static_members(self):
        self.assertEqual(TrickWorkflow.all_possible_phases,
          range(TrickWorkflow.allowed_phase_range['min'], TrickWorkflow.allowed_phase_range['max']+1) )
        self.assertEqual(TrickWorkflow.listify_phase(3), [3])     # Int becomes list of int
        self.assertEqual(TrickWorkflow.listify_phase([-3, 3, 9]), [-3, 3, 9]) # Valid list passes through
        self.assertEqual(TrickWorkflow.listify_phase(), list(TrickWorkflow.all_possible_phases))
        self.assertEqual(TrickWorkflow.listify_phase(None), list(TrickWorkflow.all_possible_phases))
        self.assertEqual(TrickWorkflow.listify_phase(range(0,5)), [0, 1, 2, 3, 4])
        with self.assertRaises(RuntimeError):
            p = TrickWorkflow.listify_phase('hey')                # Bad type
        with self.assertRaises(RuntimeError):
            p = TrickWorkflow.listify_phase([-3, 3, 'hi'])        # Bad type
        with self.assertRaises(RuntimeError):
            p = TrickWorkflow.listify_phase([-30000, 3, '30000']) # Out of range
        with self.assertRaises(RuntimeError):
            p = TrickWorkflow.listify_phase([-30000, 3, 'hello']) # Out of range and bad type

    def test_init_nominal(self):
        self.assertEqual(self.instance.cpus, 3)
        self.assertEqual(self.instance.config_errors, [])
        self.instance.report()
        build_jobs = self.instance.get_jobs('build')
        self.assertEqual(len(build_jobs), 56)
        self.assertEqual(len(self.instance.sims), 56)
        run_jobs = self.instance.get_jobs('run')
        self.assertEqual(len(run_jobs), 36)
        val_run_jobs = self.instance.get_jobs('valgrind')
        self.assertEqual(len(val_run_jobs), 1)

    def test_init_empty_so_raises(self):
        with self.assertRaises(RuntimeError):
            self.setUpWithEmptyConfig()

    def test_init_empty_after_parsing_so_raises(self):
        with self.assertRaises(RuntimeError):
            self.setUpWithEmptyConfig("empty1.yml")

    def test_init_bad_yaml_so_raises(self):
        with self.assertRaises(RuntimeError):
            self.setUpWithErrorConfig("errors_fatal1.yml")
        with self.assertRaises(RuntimeError):
            self.setUpWithErrorConfig("errors_fatal2.yml")

    def test_init_errors_but_no_raise(self):
        self.setUpWithErrorConfig("errors_nonfatal.yml")
        self.assertEqual(len(self.instance.parsing_errors), 11)
        self.assertEqual(len(self.instance.config_errors), 2)
        self.assertEqual(len(self.instance.sims), 4)
        self.assertEqual(len(self.instance.get_sim('SIM_ball_L1').get_runs()), 1)
        #import pprint; pprint.pprint(self.instance.parsing_errors)
        self.assertEqual(len(self.instance.get_sim('SIM_ball_L1').get_valgrind_runs()), 0)
        self.assertEqual(self.instance.get_sim('SIM_ball_L1').get_phase(), 0)
        self.assertEqual(len(self.instance.get_sim('SIM_alloc_test').get_runs()), 2)
        self.assertEqual(len(self.instance.get_sim('SIM_alloc_test').get_valgrind_runs()), 0)
        self.assertEqual(self.instance.config['SIM_alloc_test']['runs']['RUN_test/input.py']['extension'], 'foobar')
        self.assertEqual(len(self.instance.get_sim('SIM_events').get_runs()), 0)
        self.assertEqual(self.instance.get_sim('SIM_events').labels, [])
        self.assertEqual(self.instance.get_sim('SIM_threads').get_run('RUN_test/sched.py').returns, 0)
        self.assertEqual(self.instance.get_sim('SIM_threads').get_run('RUN_test/amf.py').returns, 0)
        self.assertEqual(self.instance.get_sim('SIM_threads').get_run('RUN_test/async.py').returns, 0)
        self.assertEqual(self.instance.get_sim('SIM_threads').get_run('RUN_test/unit_test.py').returns, 7)
        self.assertTrue(self.instance.get_sim('SIM_demo_inputfile') is None)
        self.assertTrue(self.instance.get_sim('SIM_L1_ball') is None)
        self.assertTrue(self.instance.get_sim('SIM_foobar') is None)
        self.assertTrue(self.instance.get_sim('SIM_parachute') is None)
        self.assertEqual(self.instance.config['extension_example'], {'should': 'be ignored by this framework'})
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

    def test_set_binary_with_cpu_formatter (self):
        sim = self.instance.get_sim(identifier="SIM_stls")
        cpu = self.instance.get_trick_host_cpu()
        run = sim.get_run('RUN_test/unit_test.py')
        print("Binary: " , run.binary)
        self.assertEqual(run.binary, "T_main_" + cpu + "_test.exe")

    def test_set_binary_without_cpu_formatter (self):
        sim = self.instance.get_sim(identifier="SIM_segments")
        run = sim.get_run('RUN_test/input.py')
        print("Binary: " , run.binary)
        self.assertEqual(run.binary, "T_main_*.exe")

    def test_absolute_path_to_custom_build_cmd (self):
        sim = self.instance.get_sim(identifier="SIM_stls")
        absolute_path = os.path.join(self.instance.trick_dir, "bin", "trick-CP -t")
        self.assertEqual(sim.build_cmd, absolute_path)

    def test_absolute_path_to_default_build_cmd (self):
        sim = self.instance.get_sim(identifier="SIM_segments")
        absolute_path = os.path.join(self.instance.trick_dir, "bin", "trick-CP")
        self.assertEqual(sim.build_cmd, absolute_path)

    def test_get_unique_comparison_dirs(self):
        ucd = self.instance.get_unique_comparison_dirs()
        self.assertTrue(ucd[0] is not None)
        self.assertTrue(ucd[0][0] is not None)
        self.assertTrue(ucd[0][1] is not None)

    def test_get_koviz_report_jobs(self):
        # Since instrumenting "is koviz on your PATH?" is difficult, this test is
        # designed to check both error (not on PATH) and non-error (on PATH) cases
        krj = self.instance.get_koviz_report_jobs()
        if krj[1] == []: # No errors implies we have koviz on PATH
          self.assertTrue(isinstance(krj[0], list))
          self.assertTrue(isinstance(krj[0][0], Job))
          self.assertTrue(not krj[1])
        else:            # Otherwise koviz not on PATH
          self.assertTrue('koviz is not found' in krj[1][0])
          self.assertTrue(isinstance(krj[0], list))
          self.assertEqual(len(krj[0]), 0)

    def test_get_koviz_report_job_missing_dir(self):
        krj = self.instance.get_koviz_report_job('share/trick/trickops/tests/testdata_noexist',
          'share/trick/trickops/tests/baselinedata')
        self.assertTrue(krj[0] is None)
        # Loose check but works even if koviz not on PATH
        self.assertTrue('ERROR' in krj[1])

    def test_status_summary_nominal(self):
        summary = self.instance.status_summary()
        # Nothing was run, so nothing could fail
        self.assertEqual(summary, 'SUCCESS')

    def test_get_and_pop_run(self):
        sim = self.instance.get_sim('SIM_ball_L1')
        run = sim.get_run('RUN_test/input.py')
        self.assertEqual(run.input_file, 'RUN_test/input.py')
        run = sim.pop_run('RUN_test/input.py')
        self.assertEqual(run.input_file, 'RUN_test/input.py')
        self.assertEqual(len(sim.get_runs()), 0)

    def test_get_run_jobs_kind(self):
        sim = self.instance.get_sim('SIM_ball_L1')
        normal_run_jobs = sim.get_run_jobs()
        self.assertTrue('valgrind' not in normal_run_jobs[0]._command)
        sim = self.instance.get_sim('SIM_alloc_test')
        valgrind_run_jobs = sim.get_run_jobs(kind='valgrind')
        self.assertTrue('valgrind' in valgrind_run_jobs[0]._command)

    def test_get_run_jobs_phase(self):
        sim = self.instance.get_sim('SIM_demo_sdefine')
        run_jobs = sim.get_run_jobs()                     # Ask for everything
        self.assertEqual(len(run_jobs), 2)
        run_jobs_phase0_int = sim.get_run_jobs(phase=0)   # Ask for only phase 0 as int
        self.assertEqual(len(run_jobs_phase0_int), 1)
        run_jobs_phase0_list = sim.get_run_jobs(phase=[0]) # Ask for only phase 0 as list
        self.assertEqual(run_jobs_phase0_int, run_jobs_phase0_list)  # Should be the same
        self.assertEqual(len(run_jobs_phase0_list), 1)
        run_jobs_phase1_int = sim.get_run_jobs(phase=1)   # Ask for only phase 1 as int
        self.assertEqual(len(run_jobs_phase1_int), 1)
        run_jobs_phase1_list = sim.get_run_jobs(phase=[1]) # Ask for only phase 1 as list
        self.assertEqual(run_jobs_phase1_int, run_jobs_phase1_list)  # Should be the same
        self.assertEqual(len(run_jobs_phase1_list), 1)
        run_jobs_both_phases_list = sim.get_run_jobs(phase=[0,1]) # Ask for phase 0-1 as list
        self.assertEqual(len(run_jobs_both_phases_list), 2)
        self.assertEqual(run_jobs_both_phases_list, run_jobs)  # These should be equivalent
        run_jobs_one_phase_not_exist = sim.get_run_jobs(phase=[0,1,2]) # Ask for phase 0-2 as list, 2 doesn't exist
        self.assertEqual(len(run_jobs_one_phase_not_exist), 2)         # We only get what exists
        # Ask for everything explicitly, this is far slower than phase=None but should produce equivalent list
        run_jobs_all_possible_explicit = sim.get_run_jobs(phase=TrickWorkflow.all_possible_phases)
        self.assertEqual(run_jobs_all_possible_explicit , run_jobs)  # These should be equivalent
        # Error cases
        with self.assertRaises(RuntimeError):
          run_jobs_invalid_input = sim.get_run_jobs(phase=['abc',1]) # strings aren't permitted
        with self.assertRaises(RuntimeError):
          run_jobs_invalid_input = sim.get_run_jobs(phase=-10000) # out of range as int
        with self.assertRaises(RuntimeError):
          run_jobs_invalid_input = sim.get_run_jobs(phase=[-10000]) # out of range as list

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
        # Test all the kinds permutations
        builds = self.instance.get_jobs('build')
        self.assertEqual(len(builds), 56)
        builds = self.instance.get_jobs('builds')
        self.assertEqual(len(builds), 56)
        runs = self.instance.get_jobs('run')
        self.assertEqual(len(runs), 36)
        runs = self.instance.get_jobs('runs')
        self.assertEqual(len(runs), 36)
        vg = self.instance.get_jobs('valgrind')
        self.assertEqual(len(vg), 1)
        vg = self.instance.get_jobs('valgrinds')
        self.assertEqual(len(vg), 1)
        a = self.instance.get_jobs('analysis')
        self.assertEqual(len(a), 8)
        a = self.instance.get_jobs('analyses')
        self.assertEqual(len(a), 8)
        a = self.instance.get_jobs('analyze')
        self.assertEqual(len(a), 8)

    def test_get_jobs_builds_with_phases(self):
        # All builds all phases
        builds = self.instance.get_jobs('build', phase=None)
        self.assertEqual(len(builds), 56)
        builds = self.instance.get_jobs('build', phase=TrickWorkflow.all_possible_phases)
        self.assertEqual(len(builds), 56)
        # Builds only specific phases
        builds = self.instance.get_jobs('build', phase=970)
        self.assertEqual(len(builds), 0)
        builds = self.instance.get_jobs('build', phase=0)
        self.assertEqual(len(builds), 53)
        builds = self.instance.get_jobs('build', phase=[0])
        self.assertEqual(len(builds), 53)
        builds = self.instance.get_jobs('build', phase=-1)
        self.assertEqual(len(builds), 1)
        builds = self.instance.get_jobs('build', phase=72)
        self.assertEqual(len(builds), 1)
        builds = self.instance.get_jobs('build', phase=-88)
        self.assertEqual(len(builds), 1)
        builds = self.instance.get_jobs('build', phase=[-88, 72])
        self.assertEqual(len(builds), 2)

    def test_get_jobs_runs_with_phases(self):
        # All runs all phases
        vruns = self.instance.get_jobs('valgrind', phase=None)
        self.assertEqual(len(vruns), 1)
        vruns = self.instance.get_jobs('valgrind', phase=7)
        self.assertEqual(len(vruns), 0)
        runs = self.instance.get_jobs('run', phase=None)
        self.assertEqual(len(runs), 36)
        runs = self.instance.get_jobs('run', phase=TrickWorkflow.all_possible_phases)
        self.assertEqual(len(runs), 36)
        # Runs specific phases
        runs = self.instance.get_jobs('run', phase=[8, 19])
        self.assertEqual(len(runs), 0)
        runs = self.instance.get_jobs('run', phase=1)
        self.assertEqual(len(runs), 3)
        runs = self.instance.get_jobs('run', phase=2)
        self.assertEqual(len(runs), 2)
        runs = self.instance.get_jobs('run', phase=3)
        self.assertEqual(len(runs), 2)
        runs = self.instance.get_jobs('run', phase=4)
        self.assertEqual(len(runs), 1)

    def test_get_jobs_analysis_with_phases(self):
        # All analysis all phases
        an = self.instance.get_jobs('analysis', phase=None)
        self.assertEqual(len(an), 8)
        an = self.instance.get_jobs('analysis', phase=TrickWorkflow.all_possible_phases)
        self.assertEqual(len(an), 8)
        # Analysis specific phases
        an = self.instance.get_jobs('analysis', phase=[8, 19])
        self.assertEqual(len(an), 0)
        an = self.instance.get_jobs('analysis', phase=1)
        self.assertEqual(len(an), 2)
        an = self.instance.get_jobs('analysis', phase=2)
        self.assertEqual(len(an), 2)
        an = self.instance.get_jobs('analysis', phase=3)
        self.assertEqual(len(an), 1)

    def test_get_jobs_raises(self):
        with self.assertRaises(TypeError):
            jobs = self.instance.get_jobs(kind='bucees')
        with self.assertRaises(RuntimeError):
            jobs = self.instance.get_jobs(kind='build', phase='abx')
        with self.assertRaises(RuntimeError):
            jobs = self.instance.get_jobs(kind='run', phase=[-10000, 60000])

    def test_get_comparisons_nominal(self):
        c = self.instance.get_comparisons()
        self.assertEqual(len(c), 1)
        self.assertEqual(c[0]._translate_status(), '\x1b[33mNOT RUN\x1b[0m')

    def test_add_comparison(self):
        sim = self.instance.get_sim('SIM_demo_inputfile')
        run = sim.get_run('RUN_test/input.py')
        run.add_comparison('share/trick/trickops/tests/testdata/log_a.csv',
          'share/trick/trickops/tests/baselinedata/log_a.csv')
        self.assertTrue(len(run.comparisons) == 1)

    def test_add_analysis_nominal(self):
        sim = self.instance.get_sim('SIM_demo_inputfile')
        run = sim.get_run('RUN_test/input.py')
        run.add_analysis('echo analysis goes here')
        self.assertTrue( 'echo analysis goes here' in run.analysis._command)

    def test_add_analysis_warning(self):
        sim = self.instance.get_sim('SIM_ball_L1')
        run = sim.get_run('RUN_test/input.py')
        run.add_analysis('echo overwriting analysis')
        self.assertTrue( 'echo overwriting analysis' in run.analysis._command)

    def test_run_init(self):
        r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_test/input.py --someflag',
          binary='S_main_Linux_x86_64.exe')
        self.assertEqual(r.sim_dir, 'test/SIM_alloc_test')
        self.assertEqual(r.prerun_cmd, '')
        self.assertTrue(r.input_file == 'RUN_test/input.py --someflag')
        self.assertEqual(r.returns, 0)
        self.assertEqual(r.phase, 0)
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
        r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_test/input.py --someflag',
          binary='S_main_Linux_x86_64.exe')
        # Use same data to get a pass
        test_data     = 'share/trick/trickops/tests/baselinedata/log_a.csv'
        baseline_data = 'share/trick/trickops/tests/baselinedata/log_a.csv'
        r.add_comparison(test_data, baseline_data)
        self.assertEqual(r.compare(), 0)

    def test_run_compare_fail(self):
        r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_test/input.py --someflag',
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
        SIM_fake:
            parallel_safety: """ + parallel_safety + """
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

    def test_sim_init_default_args(self):
        s = TrickWorkflow.Sim(name='mySim', sim_dir='sims/SIM_fake')
        self.assertEqual(s.name, 'mySim')
        self.assertEqual(s.sim_dir, 'sims/SIM_fake')
        self.assertEqual(s.description, None)
        self.assertEqual(s.build_cmd, 'trick-CP')
        self.assertEqual(s.cpus, 3)
        self.assertEqual(s.size, 2200000)
        self.assertEqual(s.labels, [])
        self.assertEqual(s.phase, 0)
        self.assertEqual(s.log_dir, '/tmp')
        self.assertEqual(s.build_job, None)
        self.assertEqual(s.runs, [])
        self.assertEqual(s.valgrind_runs, [])
        self.assertTrue(isinstance(s.printer, ColorStr))
        job = s.get_build_job()
        self.assertEqual(s.build_job, job ) # First get stores it locally
        self.assertTrue('cd sims/SIM_fake && export MAKEFLAGS=-j3 && trick-CP' in job._command )
        runs = s.get_run_jobs()
        self.assertEqual(runs, [])  # No runs have been added

    def test_sim_init_all_args(self):
        s = TrickWorkflow.Sim(name='yourSim', sim_dir='sims/SIM_foo', description='desc',
          labels=['label1', 'label2'], prebuild_cmd='source env/env.sh; ',
          build_cmd='trick-CP --flag', cpus=2, size=10000, phase=2, log_dir='~/logs')
        self.assertEqual(s.name, 'yourSim')
        self.assertEqual(s.sim_dir, 'sims/SIM_foo')
        self.assertEqual(s.description, 'desc')
        self.assertEqual(s.build_cmd, 'trick-CP --flag')
        self.assertEqual(s.cpus, 2)
        self.assertEqual(s.size, 10000)
        self.assertEqual(s.labels, ['label1', 'label2'])
        self.assertEqual(s.phase, 2)
        self.assertEqual(s.log_dir, '~/logs')
        job = s.get_build_job()
        self.assertTrue('cd sims/SIM_foo && export MAKEFLAGS=-j2 && trick-CP --flag' in job._command )
        self.assertTrue('source env/env.sh;' in job._command )
        #import pdb; pdb.set_trace()

    def test_phase_getters_setters(self):
        s = TrickWorkflow.Sim(name='mySim', sim_dir='sims/SIM_fake')
        s.set_phase(99)    # OK
        self.assertEqual(s.get_phase(), 99)
        s.set_phase(999)    # OK
        self.assertEqual(s.get_phase(), 999)
        s.set_phase(TrickWorkflow.allowed_phase_range['max'])
        self.assertEqual(s.get_phase(), TrickWorkflow.allowed_phase_range['max']) # OK boundary
        with self.assertRaises(RuntimeError):
            s.set_phase(TrickWorkflow.allowed_phase_range['max']+1)  # Over boundary
        with self.assertRaises(RuntimeError):
            s.set_phase(TrickWorkflow.allowed_phase_range['min']-1)  # Under boundary
        r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_test/input.py',
          binary='S_main_Linux_x86_64.exe')
        r.set_phase(99)    # OK
        self.assertEqual(r.get_phase(), 99)
        r.set_phase(999)    # OK
        self.assertEqual(r.get_phase(), 999)
        with self.assertRaises(RuntimeError):
            r.set_phase(TrickWorkflow.allowed_phase_range['max']+1)  # Over boundary
        with self.assertRaises(RuntimeError):
            r.set_phase(TrickWorkflow.allowed_phase_range['min']-1)  # Under boundary

    def test_run__find_range_string(self):
        r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_test/input.py',
          binary='S_main_Linux_x86_64.exe')
        self.assertEqual(TrickWorkflow._find_range_string("[01-09]"), "[01-09]")
        self.assertEqual(TrickWorkflow._find_range_string("SET_foo/RUN_[0-9]/input.py"), "[0-9]")
        self.assertEqual(TrickWorkflow._find_range_string("SET_foo/RUN_[01-09]/input.py"), "[01-09]")
        self.assertEqual(TrickWorkflow._find_range_string("SET_foo/RUN_[001-999]/input.py"), "[001-999]")
        self.assertEqual(TrickWorkflow._find_range_string("SET_foo/RUN_[0000-9999]/input.py"), "[0000-9999]")
        self.assertEqual(TrickWorkflow._find_range_string("SET_[01-09]/RUN_hi/input.py"),  "[01-09]" )
        self.assertEqual(TrickWorkflow._find_range_string("[01-09]/RUN_hello/input.py"),   "[01-09]")
        self.assertEqual(TrickWorkflow._find_range_string("SET_foo/RUN_bar/input.py"),     None)
        self.assertEqual(TrickWorkflow._find_range_string("SET_foo/RUN_[001-009/input.py"), None)
        self.assertEqual(TrickWorkflow._find_range_string("SET_foo/RUN_(01-09)/input.py"),  None)
        with self.assertRaises(RuntimeError):
          TrickWorkflow._find_range_string("SET_[00-03]/RUN_[01-09]/input.py")

    def test_run__get_range_list(self):
        r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_test/input.py',
          binary='S_main_Linux_x86_64.exe')
        myRange = r._get_range_list("[01-09]")
        self.assertEqual(myRange, ["01", "02", "03", "04", "05", "06", "07", "08", "09"])
        myRange = r._get_range_list("[001-009]")
        self.assertEqual(myRange, ["001", "002", "003", "004", "005", "006", "007", "008", "009"])
        with self.assertRaises(RuntimeError):
          myRange = r._get_range_list("[009-001]")  # Min not less than max
        with self.assertRaises(RuntimeError):
          myRange = r._get_range_list("[09-001]")   # Inconsistent leading zeros
        with self.assertRaises(RuntimeError):
          myRange = r._get_range_list("[abc-009]")  # Min Can't be converted to int
        with self.assertRaises(RuntimeError):
          myRange = r._get_range_list("[01-zy]")    # Max Can't be converted to int
        with self.assertRaises(RuntimeError):
          myRange = r._get_range_list("01-04")        # Wrong syntax
        with self.assertRaises(RuntimeError):
          myRange = r._get_range_list("[01-04")       # Wrong syntax
        with self.assertRaises(RuntimeError):
          myRange = r._get_range_list("[01/04")       # Wrong syntax
        myRange = r._get_range_list("[[01-04]]")      # Extra brackets are ignored
        self.assertEqual(myRange, ["01", "02", "03", "04"])

    def test_run__multiply(self):
        r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_[00-05]/input.py',
          binary='S_main_Linux_x86_64.exe')
        runs = r.multiply()
        self.assertEqual(len(runs), 6)  # Expect 6 copies

        r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_[0000-9999]/input.py',
          binary='S_main_Linux_x86_64.exe')
        runs = r.multiply()
        self.assertEqual(len(runs), 10000)  # Expect 10000 copies

        r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_[05-01]/input.py',
          binary='S_main_Linux_x86_64.exe')
        with self.assertRaises(RuntimeError):
          runs = r.multiply()           # Reverse order in syntax in input_file

        r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='SET_[01-02]/RUN_[05-01]/input.py',
          binary='S_main_Linux_x86_64.exe')
        with self.assertRaises(RuntimeError):
          runs = r.multiply()           # Invalid syntax, more than one pattern found

    def test_run__multiply_with_double_pattern_comparisons(self):
        r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_[00-05]/input.py',
          binary='S_main_Linux_x86_64.exe')
        # Add a fake comparison with correct [min-max] notation
        r.add_comparison('testdata/RUN_[00-05]/log_a.csv', 'baselinedata/RUN_[00-05]/log_a.csv')
        runs = r.multiply()
        # Ensure the run's comparisons patterns are replaced with the expected value
        self.assertEqual(len(runs), 6)  # Expect 6 copies
        self.assertEqual(runs[0].comparisons[0].test_data,     'testdata/RUN_00/log_a.csv')
        self.assertEqual(runs[0].comparisons[0].baseline_data, 'baselinedata/RUN_00/log_a.csv')
        self.assertEqual(runs[5].comparisons[0].test_data,     'testdata/RUN_05/log_a.csv')
        self.assertEqual(runs[5].comparisons[0].baseline_data, 'baselinedata/RUN_05/log_a.csv')

    def test_run__multiply_with_single_pattern_comparisons(self):
        r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_[00-05]/input.py',
          binary='S_main_Linux_x86_64.exe')
        # Compare many to one
        r.add_comparison('testdata/RUN_[00-05]/log_common.csv', 'baselinedata/RUN_common/log_common.csv')
        r.add_comparison('testdata/RUN_[00-05]/log_a.csv',      'baselinedata/RUN_[00-05]/log_a.csv')
        runs = r.multiply()
        # Ensure the run's comparisons patterns are replaced with the expected value
        self.assertEqual(len(runs), 6)  # Expect 6 copies
        self.assertEqual(runs[0].comparisons[0].test_data,     'testdata/RUN_00/log_common.csv')
        self.assertEqual(runs[0].comparisons[0].baseline_data, 'baselinedata/RUN_common/log_common.csv')
        self.assertEqual(runs[0].comparisons[1].test_data,     'testdata/RUN_00/log_a.csv')
        self.assertEqual(runs[0].comparisons[1].baseline_data, 'baselinedata/RUN_00/log_a.csv')
        self.assertEqual(runs[5].comparisons[0].test_data,     'testdata/RUN_05/log_common.csv')
        self.assertEqual(runs[5].comparisons[0].baseline_data, 'baselinedata/RUN_common/log_common.csv')
        self.assertEqual(runs[5].comparisons[1].test_data,     'testdata/RUN_05/log_a.csv')
        self.assertEqual(runs[5].comparisons[1].baseline_data, 'baselinedata/RUN_05/log_a.csv')

    def test_run__multiply_with_mismatched_patterns(self):
        r = TrickWorkflow.Run(sim_dir='test/SIM_alloc_test', input_file='RUN_[00-05]/input.py',
          binary='S_main_Linux_x86_64.exe')
        # Compare many to one
        r.add_comparison('testdata/RUN_[00-06]/log_common.csv', 'baselinedata/RUN_common/log_common.csv')
        with self.assertRaises(RuntimeError):
          runs = r.multiply()
