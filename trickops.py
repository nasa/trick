import argparse
import os
import sys
from pathlib import Path

sys.path.append(str(Path(__file__).resolve().parent / "share" / "trick" / "trickops"))

from TrickWorkflow import TrickWorkflow
from WorkflowCommon import Job

thisdir = Path(__file__).resolve().parent

max_retries = 5


class SimTestWorkflow(TrickWorkflow):
    def __init__(self, quiet, trick_top_level, cpus, config_file, trick_dir=None):
        self.cpus = cpus
        trick_top_level = Path(trick_top_level)

        # Create the trick_test directory if it doesn't already exist
        (trick_top_level / "trick_test").mkdir(exist_ok=True)

        # trick_dir defaults to trick_top_level (historical behavior: sims and
        # the installed Trick used by bin/trick-CP live in the same in-source
        # tree). A CMake-driven test run passes them separately: sim
        # directories (test/SIM_*) only exist in the source tree, but the
        # installed product (bin/trick-CP, libexec/, share/) may be a staged
        # out-of-source CMake install instead.
        if trick_dir is None:
            trick_dir = trick_top_level
        else:
            trick_dir = Path(trick_dir)

        # Base Class initialize, this creates internal management structures
        TrickWorkflow.__init__(
            self,
            project_top_level=str(trick_top_level),
            log_dir=str(trick_top_level / "trickops_logs"),
            trick_dir=str(trick_dir),
            config_file=str(trick_top_level / config_file),
            cpus=self.cpus,
            quiet=quiet,
        )

    def run(self):
        build_jobs = self.get_jobs(kind="build")

        # This is awful but I can't think of another way around it
        # SIM_test_varserver has 2 tests that should return the code for SIGUSR1, the number is different on Mac vs Linux
        # so it can't be hardcoded in the input yml file. Maybe this is a case having a label on a run would be cleaner?
        import signal

        run_names = [
            "Run test/SIM_test_varserv RUN_test/err1_test.py",
            "Run test/SIM_test_varserv RUN_test/err2_test.py",
        ]
        for job in [job for job in self.get_jobs(kind="run") if job.name in run_names]:
            job._expected_exit_status = signal.SIGUSR1.value

        # Several test sims have runs that require ordering via phases:
        #  - SIM_stls dumps a checkpoint that is then read in and checked by a subsequent run
        #  - SIM_checkpoint_data_recording dumps checkpoints that are read by subsequent runs
        #  - SIM_test_varserver has 3 runs that cannot be concurrent
        #  - SIM_mc_generation generates runs and then runs them
        phases = [-1, 0, 1, 2, 3]

        analysis_jobs = self.get_jobs(kind="analyze")
        if self.platform == "darwin":
            for job in build_jobs:
                if job.name == "Build test/SIM_trickified_shared":
                    print("REMOVING JOB: " + job.name)
                    build_jobs.remove(job)
        builds_status = self.execute_jobs(
            build_jobs, max_concurrent=self.cpus, header="Executing all sim builds."
        )

        jobs = build_jobs

        run_status = 0
        for phase in phases:
            run_jobs = self.get_jobs(kind="run", phase=phase)
            if self.platform == "darwin":
                for job in run_jobs:
                    if (
                        job.name
                        == "Run test/SIM_trickified_shared RUN_test/unit_test.py"
                    ):
                        print("REMOVING JOB: " + job.name)
                        run_jobs.remove(job)
            this_status = self.execute_jobs(
                run_jobs,
                max_concurrent=self.cpus,
                header="Executing phase " + str(phase) + " runs.",
                job_timeout=1000,
            )
            run_status = run_status or this_status
            jobs += run_jobs

        comparison_result = self.compare()
        analysis_status = self.execute_jobs(
            analysis_jobs, max_concurrent=self.cpus, header="Executing all analysis."
        )

        self.report()  # Print Verbose report
        self.status_summary()  # Print a Succinct summary

        # Dump failing logs
        for job in jobs:
            if (
                job.get_status() == Job.Status.FAILED
                or job.get_status() == Job.Status.TIMEOUT
            ):
                print("*" * 120)
                if job.get_status() == Job.Status.FAILED:
                    header = "Failing job: " + job.name
                else:
                    header = "Timed out job: " + job.name

                numspaces = int((120 - 20 - len(header)) / 2 - 2)
                print(
                    "*" * 10,
                    " " * numspaces,
                    header,
                    " " * numspaces,
                    "*" * 10,
                )
                print("*" * 120)
                print(Path(job.log_file).read_text())
                print("*" * 120, "\n\n\n")

        return (
            builds_status
            or run_status
            or len(self.config_errors) > 0
            or comparison_result
            or analysis_status
        )


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Build, run, and compare all test sims for Trick",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "--trick_top_level",
        type=str,
        help="Path to the project tree containing the sim "
        "directories referenced by --config_file",
        default=str(thisdir),
    )
    parser.add_argument(
        "--trick_dir",
        type=str,
        help="Path to the installed/staged Trick (bin/trick-CP, "
        "libexec/, share/) used to build the sims. Defaults to --trick_top_level (historical in-source "
        "behavior, where sims and the Trick install share one tree).",
        default=None,
    )
    parser.add_argument(
        "--quiet",
        action="store_true",
        help="Suppress progress bars (automatically set to True if environment variable CI is present).",
    )
    parser.add_argument(
        "--cpus",
        type=int,
        default=(os.cpu_count() if os.cpu_count() is not None else 8),
        help="Number of cpus to use for testing. For builds this number is used for MAKEFLAGS *and* number of "
        "concurrent builds (cpus^2). For sim runs this controls the maximum number of simultaneous runs.",
    )
    parser.add_argument(
        "--config_file",
        type=str,
        help="Run configuration file to use, relative to trick_top_level",
        default="test_sims.yml",
    )

    myargs = parser.parse_args()
    should_be_quiet = myargs.quiet or os.getenv("CI") is not None
    sys.exit(
        SimTestWorkflow(
            quiet=should_be_quiet,
            trick_top_level=myargs.trick_top_level,
            trick_dir=myargs.trick_dir,
            cpus=myargs.cpus,
            config_file=myargs.config_file,
        ).run()
    )
