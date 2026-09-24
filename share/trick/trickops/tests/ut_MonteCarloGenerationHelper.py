import os, sys, glob
import unittest, shutil
from pathlib import Path
from tempfile import TemporaryDirectory
import pdb
from testconfig import this_trick, tests_dir
import TrickWorkflow
from MonteCarloGenerationHelper import *

def suite():
    """Create test suite from test cases here and return"""
    suites = []
    suites.append(unittest.TestLoader().loadTestsFromTestCase(MCGNominalTestCase))
    suites.append(unittest.TestLoader().loadTestsFromTestCase(MCGAllArgsTestCase))
    suites.append(unittest.TestLoader().loadTestsFromTestCase(MCGInvalidGenerationTestCase))
    suites.append(unittest.TestLoader().loadTestsFromTestCase(MCGInvalidInputsTestCase))
    suites.append(unittest.TestLoader().loadTestsFromTestCase(MCGRelativePathsTestCase))
    return (suites)

class MCGNominalTestCase(unittest.TestCase):

    def setUp(self):
        # Nominal no-error when parsing the trick-sims config file scenario
        self.instance = MonteCarloGenerationHelper(sim_path=os.path.join(this_trick, 'test/SIM_mc_generation'),
          input_path='RUN_nominal/input_a.py')
        # Create expected generated output directories and files this tests needs. Note this
        # matches what SIM_mc_generation produces when run and should work whether output
        # from that sim's tests are lingering in the workspace or not
        self.monte_dir = os.path.join(this_trick, 'test/SIM_mc_generation', 'MONTE_RUN_nominal')
        os.makedirs(os.path.join(self.monte_dir, 'RUN_000'), exist_ok=True)
        os.makedirs(os.path.join(self.monte_dir, 'RUN_001'), exist_ok=True)
        for dir in ['RUN_000', 'RUN_001']:
          if not os.path.isfile(os.path.join(self.monte_dir, dir, 'monte_input_a.py')):
            with open(os.path.join(self.monte_dir, dir, 'monte_input_a.py'), 'w') as fp:
              pass

    def tearDown(self):
        if self.instance:
            del self.instance
        self.instance = None

    def test_init(self):
        self.assertEqual(self.instance.S_main_name, "S_main*.exe")
        self.assertEqual(self.instance.sim_path,  os.path.join(this_trick,"test/SIM_mc_generation"))
        self.assertEqual(self.instance.input_path, "RUN_nominal/input_a.py")
        self.assertEqual(self.instance.sim_args_gen, '')
        self.assertEqual(self.instance.env, '')
        self.assertEqual(self.instance.generated_input_files, [])
        self.assertTrue(('trick/test/SIM_mc_generation && ./S_main*.exe RUN_nominal/input_a.py')
          in self.instance.generation_job._command)
        self.assertTrue(os.path.join(self.instance.sim_path, os.path.dirname(self.instance.input_path),
          'MCG_generation_output.txt') in self.instance.generation_job.log_file)

    def test_get_generation_job(self):
        j = self.instance.get_generation_job()
        self.assertTrue(isinstance(j, TrickWorkflow.SingleRun))

    def test_get_generated_input_files(self):
        gif = self.instance.get_generated_input_files(os.path.join(self.instance.sim_path, 'MONTE_RUN_nominal'))
        self.assertTrue('trick/test/SIM_mc_generation/MONTE_RUN_nominal/RUN_000/monte_input_a.py' in gif[0])
        self.assertTrue('trick/test/SIM_mc_generation/MONTE_RUN_nominal/RUN_001/monte_input_a.py' in gif[1])
        # Given the relative path from sim
        os.chdir(self.instance.sim_path)
        gif = self.instance.get_generated_input_files('MONTE_RUN_nominal')

        # Given invalid path (relative to cwd())
        with self.assertRaises(RuntimeError):
            gif = self.instance.get_generated_input_files('fake/path/MONTE_RUN_nominal')

    def test_get_zero_padding(self):
        zp = self.instance.get_zero_padding(monte_dir=self.monte_dir)
        self.assertEqual(zp, 3)
        # Without any args it should use interal self.generated_input_files
        zp = self.instance.get_zero_padding()
        self.assertEqual(zp, 3)

    def test_get_sbatch_job(self):
        sbj = self.instance.get_sbatch_job(self.monte_dir)
        self.assertTrue('--array 0-001 ' in sbj._command)
        self.assertTrue(self.instance.S_main_name in sbj._command)
        self.assertTrue(self.monte_dir   in sbj._command)
        self.assertTrue('RUN_${RUN_NUM}' in sbj._command)

    def test_get_sbatch_job_with_passthrough_args(self):
        sbj = self.instance.get_sbatch_job(self.monte_dir, hpc_passthrough_args="--array 50-80 --wait")
        self.assertTrue('--array 50-80 ' in sbj._command)
        self.assertTrue('--wait ' in sbj._command)
        sbj = self.instance.get_sbatch_job(self.monte_dir, hpc_passthrough_args="--array 1-100:10")
        self.assertTrue('--array 1-100:10 ' in sbj._command)
        self.assertTrue('--wait ' not in sbj._command)
        sbj = self.instance.get_sbatch_job(self.monte_dir, hpc_passthrough_args="--begin 16:00")
        self.assertTrue('--begin 16:00 ' in sbj._command)
        self.assertTrue('--array 0-001 ' in sbj._command)
        sbj = self.instance.get_sbatch_job(self.monte_dir, sim_args="--logging=verbose")
        self.assertTrue('--logging=verbose' in sbj._command)

    def test_get_generated_run_jobs(self):
        jobs = self.instance.get_generated_run_jobs(self.monte_dir)
        self.assertEqual(len(jobs), 2)
        self.assertTrue('MONTE_RUN_nominal/RUN_000/monte_input_a.py' in jobs[0]._command)
        self.assertTrue('MONTE_RUN_nominal/RUN_001/monte_input_a.py' in jobs[1]._command)
        jobs = self.instance.get_generated_run_jobs(self.monte_dir, sim_args='--logging=off')
        self.assertTrue('--logging=off' in jobs[0]._command)
        self.assertTrue('--logging=off' in jobs[1]._command)

class MCGAllArgsTestCase(unittest.TestCase):

    def setUp(self):
        # Nominal no-error when parsing the trick-sims config file scenario
        self.instance = MonteCarloGenerationHelper(sim_path=os.path.join(this_trick, 'test/SIM_mc_generation'),
          input_path='RUN_nominal/input_a.py', env='source bashrc;', sim_args_gen='--monte-carlo --runs=32')

    def tearDown(self):
        if self.instance:
            del self.instance
        self.instance = None

    def test_init(self):
        self.assertTrue( self.instance.generation_job._command.startswith('source bashrc; '))
        self.assertTrue( self.instance.generation_job._command.endswith('--monte-carlo --runs=32'))

class MCGInvalidGenerationTestCase(unittest.TestCase):

    def setUp(self):
        # Nominal no-error when parsing the trick-sims config file scenario
        self.instance = MonteCarloGenerationHelper(sim_path=os.path.join(this_trick, 'test/SIM_mc_generation'),
          input_path='RUN_nominal/input_a.py')
        # Create incorrect generated output directories and files this tests needs.
        # This simulates a generation error and tests that the MonteCarloGenerationHelper class can
        # detect it
        monte_dir = os.path.join(this_trick, 'test/SIM_mc_generation', 'MONTE_RUN_missing_input_files')
        os.makedirs(os.path.join(monte_dir, 'RUN_000'), exist_ok=True)
        os.makedirs(os.path.join(monte_dir, 'RUN_001'), exist_ok=True)
        monte_dir = os.path.join(this_trick, 'test/SIM_mc_generation', 'MONTE_RUN_mismatch_input_files')
        os.makedirs(os.path.join(monte_dir, 'RUN_000'), exist_ok=True)
        os.makedirs(os.path.join(monte_dir, 'RUN_001'), exist_ok=True)
        with open(os.path.join(monte_dir, 'RUN_000', 'monte_input_a.py'), 'w') as fp:
          pass
        monte_dir = os.path.join(this_trick, 'test/SIM_mc_generation', 'MONTE_RUN_incorrect_input_files')
        os.makedirs(os.path.join(monte_dir, 'RUN_000'), exist_ok=True)
        os.makedirs(os.path.join(monte_dir, 'RUN_001'), exist_ok=True)
        for dir in ['RUN_000', 'RUN_001']:
          if not os.path.isfile(os.path.join(monte_dir, dir, 'input_a.py')):
            with open(os.path.join(monte_dir, dir, 'input_a.py'), 'w') as fp:
              pass
        monte_dir = os.path.join(this_trick, 'test/SIM_mc_generation', 'MONTE_RUN_completely_bonkers')
        os.makedirs(os.path.join(monte_dir, 'RUN_makes_no'), exist_ok=True)
        os.makedirs(os.path.join(monte_dir, 'RUN_sense'), exist_ok=True)
        for dir in ['RUN_makes_no', 'RUN_sense']:
          if not os.path.isfile(os.path.join(monte_dir, dir, 'input_a.py')):
            with open(os.path.join(monte_dir, dir, 'input_a.py'), 'w') as fp:
              pass

    def tearDown(self):
        if self.instance:
            del self.instance
        self.instance = None
        # Remove the fake directory tree created in this test
        shutil.rmtree(os.path.join(this_trick, 'test/SIM_mc_generation', 'MONTE_RUN_missing_input_files'))
        shutil.rmtree(os.path.join(this_trick, 'test/SIM_mc_generation', 'MONTE_RUN_mismatch_input_files'))
        shutil.rmtree(os.path.join(this_trick, 'test/SIM_mc_generation', 'MONTE_RUN_incorrect_input_files'))
        shutil.rmtree(os.path.join(this_trick, 'test/SIM_mc_generation', 'MONTE_RUN_completely_bonkers'))

    def test_get_generated_input_files(self):
        # Just a warning
        gif = self.instance.get_generated_input_files(
          os.path.join(self.instance.sim_path, 'MONTE_RUN_mismatch_input_files'))
        with self.assertRaises(RuntimeError):
          gif = self.instance.get_generated_input_files(
            os.path.join(self.instance.sim_path, 'MONTE_RUN_missing_input_files'))
        with self.assertRaises(RuntimeError):
          gif = self.instance.get_generated_input_files(
            os.path.join(self.instance.sim_path, 'MONTE_RUN_incorrect_input_files'))
        with self.assertRaises(RuntimeError):
          gif = self.instance.get_generated_input_files(
            os.path.join(self.instance.sim_path, 'MONTE_RUN_completely_bonkers'))

    def test_get_zero_padding(self):
        # Without monte_dir and get_generated_input files never having been run, this will fail
        with self.assertRaises(RuntimeError):
          zp = self.instance.get_zero_padding()
        # Nothing in monte_dir, will fail
        with self.assertRaises(RuntimeError):
          zp = self.instance.get_zero_padding(monte_dir=os.path.join(this_trick, 'test/SIM_mc_generation', 'MONTE_RUN_missing_input_files'))

class MCGInvalidInputsTestCase(unittest.TestCase):

    def setUp(self):
        # Invalid sim_path
        with self.assertRaises(RuntimeError):
          self.instance = MonteCarloGenerationHelper(sim_path=os.path.join(this_trick, 'test/SIM_notexist'),
            input_path='RUN_nominal/input_a.py')
        # Invalid input_path
        with self.assertRaises(RuntimeError):
          self.instance = MonteCarloGenerationHelper(sim_path=os.path.join(this_trick, 'test/SIM_mc_generation'),
            input_path='RUN_nominal/input_x.py',)
        # Invalid sim_args_gen
        with self.assertRaises(TypeError):
          self.instance = MonteCarloGenerationHelper(sim_path=os.path.join(this_trick, 'test/SIM_mc_generation'),
            input_path='RUN_nominal/input_a.py', sim_args_gen=3)
        # Invalid env
        with self.assertRaises(TypeError):
          self.instance = MonteCarloGenerationHelper(sim_path=os.path.join(this_trick, 'test/SIM_mc_generation'),
            input_path='RUN_nominal/input_a.py', env=3)

    def test_init(self):
        pass


class MCGRelativePathsTestCase(unittest.TestCase):
    """Generated paths must survive the job's change to its simulation directory."""

    def setUp(self):
        self.original_cwd = os.getcwd()
        self.directory = TemporaryDirectory()
        self.root = Path(self.directory.name).resolve()
        self.sim = self.root / "sim"
        (self.sim / "RUN_source").mkdir(parents=True)
        (self.sim / "RUN_source/input.py").write_text("", encoding="utf-8")
        self.monte = self.sim / "MONTE_source"
        for number in (2, 0):
            run = self.monte / ("RUN_%03d" % number)
            run.mkdir(parents=True)
            (run / "monte_input.py").write_text("run %d\n" % number, encoding="utf-8")
        # This local executable tests argument delivery, not the simulation engine.
        executable = self.sim / "S_main_fixture.exe"
        executable.write_text('#!/bin/sh\ncat "$1"\n', encoding="utf-8")
        executable.chmod(0o700)
        os.chdir(self.root)
        self.helper = MonteCarloGenerationHelper(
            str(self.sim), "RUN_source/input.py", S_main_name="S_main_fixture.exe"
        )

    def tearDown(self):
        os.chdir(self.original_cwd)
        self.directory.cleanup()

    def test_relative_input_paths_are_absolute_and_sorted(self):
        for directory in ("sim/MONTE_source", "./sim/MONTE_source"):
            with self.subTest(directory=directory):
                paths = self.helper.get_generated_input_files(directory)
                expected = [str(self.monte / name / "monte_input.py")
                            for name in ("RUN_000", "RUN_002")]
                self.assertEqual(paths, expected)
                os.chdir(self.sim)
                try:
                    self.assertTrue(all(os.path.isfile(path) for path in paths))
                finally:
                    os.chdir(self.root)
                paths.clear()
                self.assertEqual(self.helper.generated_input_files, expected)

    def test_absolute_input_paths_are_unchanged(self):
        paths = self.helper.get_generated_input_files(str(self.monte))
        self.assertEqual(paths, [str(self.monte / name / "monte_input.py")
                                for name in ("RUN_000", "RUN_002")])

    def test_generated_jobs_read_inputs_after_changing_directory(self):
        jobs = self.helper.get_generated_run_jobs("sim/MONTE_source")
        self.assertEqual(len(jobs), 2)
        for job, expected in zip(jobs, ("run 0\n", "run 2\n")):
            with self.subTest(job=job.name):
                job.set_use_var_server(False)
                try:
                    job.start()
                    job._process.wait(timeout=10)
                    self.assertEqual(job.get_status(), job.Status.SUCCESS)
                    self.assertTrue(os.path.isabs(job.log_file))
                    self.assertEqual(Path(job.log_file).read_text(), expected)
                finally:
                    if job._process is not None and job._process.poll() is None:
                        job.die()
                    else:
                        job._close_files()

    def test_jobs_created_in_simulation_directory_run_from_elsewhere(self):
        os.chdir(self.sim)
        jobs = self.helper.get_generated_run_jobs("MONTE_source")
        os.chdir(self.root)
        for job, expected in zip(jobs, ("run 0\n", "run 2\n")):
            with self.subTest(job=job.name):
                job.set_use_var_server(False)
                try:
                    job.start()
                    job._process.wait(timeout=10)
                    self.assertEqual(job.get_status(), job.Status.SUCCESS)
                    self.assertEqual(Path(job.log_file).read_text(), expected)
                finally:
                    if job._process is not None and job._process.poll() is None:
                        job.die()
                    else:
                        job._close_files()

    def test_relative_paths_from_simulation_directory(self):
        os.chdir(self.sim)
        paths = self.helper.get_generated_input_files("MONTE_source")
        self.assertEqual(paths, [str(self.monte / name / "monte_input.py")
                                for name in ("RUN_000", "RUN_002")])

    def test_missing_input_filter_is_unchanged(self):
        (self.monte / "RUN_001").mkdir()
        paths = self.helper.get_generated_input_files("sim/MONTE_source")
        self.assertEqual([Path(path).parent.name for path in paths], ["RUN_000", "RUN_002"])
        with self.assertRaises(RuntimeError):
            self.helper.get_generated_input_files("does-not-exist")
