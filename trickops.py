import argparse
import os
import signal
import sys
from pathlib import Path

sys.path.append(str(Path(__file__).resolve().parent / "share" / "trick" / "trickops"))

from TrickWorkflow import TrickWorkflow
from WorkflowCommon import Job

THIS_DIR = Path(__file__).resolve().parent

# Runs ordered by phase: some test sims depend on artifacts from an earlier
# phase (SIM_stls / SIM_checkpoint_data_recording read checkpoints written by a
# prior run; SIM_test_varserver has runs that cannot be concurrent;
# SIM_mc_generation generates runs and then executes them).
PHASES = (-1, 0, 1, 2, 3)

# SIM_test_varserv's err1/err2 runs are expected to exit with SIGUSR1. The exit
# code differs Mac vs Linux, so it can't be hardcoded in the yaml; tag them here.
VARSERV_SIGUSR1_RUNS = frozenset({
    "Run test/SIM_test_varserv RUN_test/err1_test.py",
    "Run test/SIM_test_varserv RUN_test/err2_test.py",
})

# Jobs skipped on macOS (SIM_trickified_shared does not build/run there).
DARWIN_SKIP_BUILDS = frozenset({"Build test/SIM_trickified_shared"})
DARWIN_SKIP_RUNS = frozenset({"Run test/SIM_trickified_shared RUN_test/unit_test.py"})


class SimTestWorkflow(TrickWorkflow):
    def __init__(
        self,
        quiet: bool,
        trick_top_level: str | Path,
        cpus: int,
        config_file: str,
        trick_dir: str | Path | None = None,
    ) -> None:
        self.cpus = cpus
        project_root = Path(trick_top_level)

        # Create the trick_test directory if it doesn't already exist
        (project_root / "trick_test").mkdir(exist_ok=True)

        # trick_dir defaults to trick_top_level (historical behavior: sims and
        # the installed Trick used by bin/trick-CP live in the same in-source
        # tree). A CMake-driven test run passes them separately: sim
        # directories (test/SIM_*) only exist in the source tree, but the
        # installed product (bin/trick-CP, libexec/, share/) may be a staged
        # out-of-source CMake install instead.
        if trick_dir is None:
            trick_dir = project_root

        # Base Class initialize, this creates internal management structures
        super().__init__(
            project_top_level=str(project_root),
            log_dir=str(project_root / "trickops_logs"),
            trick_dir=str(trick_dir),
            config_file=str(project_root / config_file),
            cpus=self.cpus,
            quiet=quiet,
        )

    def _skip_darwin(self, jobs: list, skip_names: frozenset) -> list:
        """Return jobs with the macOS-excluded ones filtered out (announcing each)."""
        if self.platform != "darwin":
            return jobs
        kept = []
        for job in jobs:
            if job.name in skip_names:
                print(f"REMOVING JOB: {job.name}")
            else:
                kept.append(job)
        return kept

    def run(self) -> int:
        build_jobs = self._skip_darwin(self.get_jobs(kind="build"), DARWIN_SKIP_BUILDS)
        analysis_jobs = self.get_jobs(kind="analyze")

        for job in self.get_jobs(kind="run"):
            if job.name in VARSERV_SIGUSR1_RUNS:
                job._expected_exit_status = signal.SIGUSR1.value

        builds_status = self.execute_jobs(
            build_jobs, max_concurrent=self.cpus, header="Executing all sim builds."
        )

        jobs = list(build_jobs)
        run_status = 0
        for phase in PHASES:
            run_jobs = self._skip_darwin(
                self.get_jobs(kind="run", phase=phase), DARWIN_SKIP_RUNS
            )
            this_status = self.execute_jobs(
                run_jobs,
                max_concurrent=self.cpus,
                header=f"Executing phase {phase} runs.",
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
            status = job.get_status()
            if status in (Job.Status.FAILED, Job.Status.TIMEOUT):
                header = (
                    f"Failing job: {job.name}"
                    if status == Job.Status.FAILED
                    else f"Timed out job: {job.name}"
                )
                print("*" * 120)
                print(f" {header} ".center(120, "*"))
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


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Build, run, and compare all test sims for Trick",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "--trick_top_level",
        type=str,
        help="Path to the project tree containing the sim "
        "directories referenced by --config_file",
        default=str(THIS_DIR),
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
        default=os.cpu_count() or 8,
        help="Number of cpus to use for testing. For builds this number is used for MAKEFLAGS *and* number of "
        "concurrent builds (cpus^2). For sim runs this controls the maximum number of simultaneous runs.",
    )
    parser.add_argument(
        "--config_file",
        type=str,
        help="Run configuration file to use, relative to trick_top_level",
        default="test_sims.yml",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    quiet = args.quiet or os.getenv("CI") is not None
    return SimTestWorkflow(
        quiet=quiet,
        trick_top_level=args.trick_top_level,
        trick_dir=args.trick_dir,
        cpus=args.cpus,
        config_file=args.config_file,
    ).run()


if __name__ == "__main__":
    raise SystemExit(main())
