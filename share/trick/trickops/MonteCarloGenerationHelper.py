"""
Module to be used in conjunction with the MonteCarloGenerate (MCG) sim module
provided by Trick and optionally TrickOps. This module allows MCG users to
easily generate monte-carlo runs and execute them locally or through an HPC job
scheduler like SLURM. Below is an example usage of the module assuming:
 1. The using script inherits from TrickWorkflow, giving access to execute_jobs()
 2. SIM_A/RUN_mc/input.py is configured with MonteCarloGenerate.sm sim module
    to generate runs when executed

# Instantiate an MCG helper instance, providing the sim and input file for generation
mgh = MonteCarloGenerationHelper(sim_path="path/to/SIM_A", input_path="RUN_mc/input.py")
# Get the generation SingleRun() instance
gj = mgh.get_generation_job()
# Execute the generation Job to generate RUNS
ret = self.execute_jobs([gj])

if ret == 0:  # Successful generation
  # Get a SLURM sbatch array job for all generated runs
  sbj = mgh.get_sbatch_job(monte_dir="path/to/MONTE_RUN_mc")
  # Execute the sbatch job, which queues all runs in SLURM for execution
  # Use hpc_passthrough_args ='--wait' to block until all runs complete
  ret = self.execute_jobs([sbj])

  # Instead of using SLURM, generated runs can be executed locally through
  # TrickOps calls. First get a list of run jobs
  run_jobs = mgh.get_generated_run_jobs(monte_dir="path/to/MONTE_RUN_mc")
  # Execute all generated SingleRun instances, up to 10 at once
  ret = self.execute_jobs(run_jobs, max_concurrent=10)
"""

import sys, os

import argparse, glob

import subprocess, errno
import pprint, logging
import WorkflowCommon, TrickWorkflow

# This global is the result of hours of frustration and debugging. See comment at the top of
# TrickWorkflow.py for details
this_trick = os.path.normpath(os.path.join(os.path.dirname(os.path.realpath(__file__)), '../../..'))

class MonteCarloGenerationHelper():
    """
    Helper class for generating runs via Trick's built-in MonteCarloGenerate sim module.
    """
    def __init__(self, sim_path, input_path, S_main_name='S_main*.exe', env=None, sim_args_gen=None):
        """
        Initialize this instance.

        >>> mgh = MonteCarloGenerationHelper(sim_path=os.path.join(this_trick, "test/SIM_mc_generation"), input_path="RUN_nominal/input_a.py")

        Parameters
        ----------
        sim_path : str
            Path to the directory containing the built simulation
        input_path : str
            Path to input file used for monte-carlo generation, relative to sim_path
        S_main_name : str
            Literal string of S_main binary. Defaults to (lazy) S_main*.exe
        env : str
            Optional literal string to execute before everything, typically for sourcing
            a project environment file.
        sim_args_gen : str
            Optional literal string to pass to the simulation after the input_path during
            generation.
        """
        self.sim_path    = os.path.abspath(sim_path)
        self.input_path  = input_path
        self.S_main_name = S_main_name
        self.sim_args_gen    = '' if not sim_args_gen else sim_args_gen
        self.env         = '' if not env else env

        self.generated_input_files = []  # Will be list of successfully generated runs
        # Sanity check inputs
        if not isinstance(self.sim_args_gen, str):
            msg = (f"ERROR: Given sim_args_gen ({sim_args_gen}) has invalid type {type(self.sim_args_gen)}."
               f" It must be a string.")
            raise TypeError(msg)
        if not isinstance(self.env, str):
            msg = (f"ERROR: Given env ({env}) has invalid type {type(self.env)}."
               f" It must be a string.")
            raise TypeError(msg)
        if not os.path.isfile(os.path.join(self.sim_path, self.input_path)):
            msg = (f"ERROR: input_path {os.path.join(self.sim_path, self.input_path)} "
               f"doesn't exist.")
            raise RuntimeError(msg)
        # Construct job used to generate runs
        cmd = "%s cd %s && " % (self.env, self.sim_path)
        cmd += "./%s " % (self.S_main_name)
        cmd += "%s " % self.input_path
        if self.sim_args_gen:
            cmd += "%s" % self.sim_args_gen
        self.generation_job = TrickWorkflow.SingleRun(
          name=f'Monte Carlo Generation for: {self.sim_path}/{self.input_path}',
          command=cmd, log_file=os.path.join(self.sim_path, os.path.dirname(self.input_path),
            'MCG_generation_output.txt'),
          expected_exit_status=0, use_var_server=False)

    def get_generation_job(self):
        """
        Return the SingleRun (Job) instance corresponding to an MCG generation execution
        as configured inside this instance. This SingleRun can be executed via
        WorkflowCommon.execute_jobs() to generate runs.

        >>> mgh = MonteCarloGenerationHelper(sim_path=os.path.join(this_trick, "test/SIM_mc_generation"), input_path="RUN_nominal/input_a.py")
        >>> job = mgh.get_generation_job()
        """
        return self.generation_job

    def get_generated_input_files(self, monte_dir):
        """
        Return a sorted list of absolute paths to the generated input files.

        Parameters
        ----------
        monte_dir : str
            Path to monte_dir in which input_files were generated. This is a required input
            since this class has no way to know where the user configured the sim to locate
            the generated runs.

        Returns
        ----------
        list of strings (absolute paths)
            Sorted list of absolute paths to input files found under monte_dir

        Raises
        ------
        RuntimeError
            If an error in finding input files occurs.
        """
        if os.path.isdir(monte_dir) == False:
            msg = (f"Given monte_dir {monte_dir} doesn't exist! Cannot get run list.")
            raise RuntimeError(msg)
        just_input_file = ("monte_"+os.path.basename(self.input_path))
        monte_dir_path_full = os.path.abspath(monte_dir)
        run_list = [x for x in os.listdir(monte_dir_path_full) if \
                   (x.startswith("RUN_") and os.path.isdir(os.path.join(monte_dir_path_full,x)))]
        if len(run_list) == 0:
            msg = (f"Error: {monte_dir} doesn't have any runs!")
            raise RuntimeError(msg)
        self.generated_input_files = [os.path.join(monte_dir, x, just_input_file) for x in run_list
          if os.path.isfile(os.path.join( monte_dir_path_full, x, just_input_file ))]
        if len(self.generated_input_files) == 0:
            msg = (f"Error: {monte_dir}'s RUN directories don't have any input files of expected "
              f"name: {just_input_file}. Make sure input_path is correct and that MCG is configured "
              "appropriately.")
            raise RuntimeError(msg)
        self.generated_input_files.sort()
        # error checking regarding missing files or not enough runs
        if len(run_list) != len(self.generated_input_files):
            msg = ("WARNING in get_generated_input_files(): There's a mismatch between the number "
              f"of MONTE*/RUN* directories and the number of input files ({just_input_file}) "
              "in those directories. Returning only the found subset")
            print(msg)

        return(list(self.generated_input_files))

    def get_zero_padding(self, monte_dir=None):
        """
        Returns an integer representing the highest zero-padding contained in
        monte_dir, or in self.generated_input_files list if monte_dir is None

        Parameters
        ----------
        monte_dir : str
            Path to monte_dir in which input_files were generated

        Returns
        ----------
        int
            Integer representing zero-padding length. Ready for use in printf-style
            "%0<int>d" use-cases.

        Raises
        ------
        RuntimeError
            If zero-padding information cannot be determined
        """
        gif = []
        if monte_dir:
            gif = self.get_generated_input_files(monte_dir)
        else:
            gif = self.generated_input_files

        if not gif or len(gif) < 1:
            msg = ("Cannot find zero-padding information because self.generated_input_files "
                   "is empty! Have you run get_generated_input_files()?")
            raise RuntimeError(msg)
        # This exception may be unreachable, but for extra safety...
        try:
            padding = len(os.path.dirname(gif[-1]).split('_')[-1])
        except Exception as e:
            msg = ("Encountered unhandled exception attempting to determine zero-padding "
                   f"information. Error:\n{e}")
            raise RuntimeError(msg)
        return padding

    def get_sbatch_job(self, monte_dir, sim_args=None, hpc_passthrough_args=None):
        """
        Return a Job() whose command is an sbatch array job for running generated runs
        found in monte_dir, with the given hpc_passthrough_args if specified. This single
        Job() when executed will submit the entire set of monte-carlo runs in monte_dir
        to a SLURM job scheduling system as an array job. This function simply creates
        the Job(), it does not execute it.

        Parameters
        ----------
        monte_dir : str
            Path to monte_dir in which input_files were generated. This is a required input
            since this class has no way to know where the user configured the sim to locate
            the generated runs.

        hpc_passthrough_args : str
            Literal string of args to be passed through to the HPC scheduling system SLURM

        sim_args : str
            Literal string of args to be passed through to the simulation binary

        Returns
        ----------
        Job()
            Job instance with sbatch command configured for array submission.
        """
        generated_runs = self.get_generated_input_files(monte_dir)
        zero_padding = self.get_zero_padding()
        just_input_file = os.path.basename(generated_runs[0])
        sbatch_cmd = self.env + " sbatch "

        # Build the default --array option, this is overridden later if the user gave it
        # Homogeneous array assumption, last zero-padded num in last run in array
        array = ("--array 0-%s " % os.path.dirname(generated_runs[-1]).split('_')[-1])
        # A couple sbatch options are special: if the user has specified --array , use
        # theirs, if not then generate the --array field for the mc_num_runs given. If --wait
        # is given, we won't be able to post-process, so store that information off. Easiest
        # way to get this passthrough information is to use arparse to parse the
        # hpc_passthrough_args list, leaving what's left in rest list to be passed through
        sbatch_parser = argparse.ArgumentParser()
        sbatch_parser.add_argument('-a', '--array')
        sbatch_parser.add_argument('-W', '--wait', action="store_true")
        if hpc_passthrough_args:
          subargs, rest = sbatch_parser.parse_known_args(hpc_passthrough_args.split())
          if subargs.wait:  # Since --wait not in rest, re-add it to passthrough args
              rest.append('--wait')
          if subargs.array:  # If custom array given by user
              # Custom array definition
              array = "--array %s " % subargs.array
          sbatch_cmd += ' '.join(rest) + ' '
        sbatch_cmd += array
        sbatch_cmd += "--wrap 'RUN_NUM=`printf %0" + str(zero_padding) + 'd $SLURM_ARRAY_TASK_ID`; '
        sbatch_cmd += "%s cd %s && " % (self.env, self.sim_path)
        sbatch_cmd += "./%s %s/RUN_${RUN_NUM}/%s" % (self.S_main_name, monte_dir, just_input_file)
        if sim_args:
            sbatch_cmd += " " + sim_args + " "

        sbatch_cmd += (" 2> %s/RUN_${RUN_NUM}/stderr 1> %s/RUN_${RUN_NUM}/stdout '" %
            (monte_dir, monte_dir))
        job = WorkflowCommon.Job(name=(f'Running sbatch array job for '
          f'{os.path.basename(self.sim_path)}'
          f' {os.path.basename(monte_dir)}'),
          command=sbatch_cmd, log_file=f'{os.path.join(monte_dir)}/sbatch_out.txt')
        return (job)

    #TODO Implement a Portable Batch System (PBS) job getter, and support for other HPC
    # schedulers go here

    def get_generated_run_jobs(self, monte_dir, sim_args=None):
        """
        Return a list of SingleRun() instances, configured for each of the RUNs in
        monte_dir. Each run's output goes to the generated RUN location in a file
        called stdouterr, containing both stdout and stderr

        Parameters
        ----------
        monte_dir : str
            Path to monte_dir in which input_files were generated. This must either be a
            path relative to the sim dir or an absolute path.

        sim_args : str
            Literal string of args to be passed through to the sim binary
        """
        jobs = []
        generated_runs = self.get_generated_input_files(monte_dir)
        for run in generated_runs:
          cmd = "%s cd %s && " % (self.env, self.sim_path)
          cmd += "./%s %s" % (self.S_main_name, run)
          if sim_args:
              cmd += " " + sim_args + " "
          jobs.append( TrickWorkflow.SingleRun(
            name=f'Executing generated run {os.path.basename(os.path.dirname(run))}',
            command=cmd, log_file=os.path.join(os.path.dirname(run),
              'stdouterr'), expected_exit_status=0)
          )
        return jobs

    def is_sim_built(self):
        """
        Return True if self.S_main_name exists in self.sim_path. False otherwise
        """
        if glob.glob(os.path.join(self.sim_path, self.S_main_name)):
          return True
        else:
          return False

