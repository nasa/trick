import os
import time
import trick
from trick.unit_test import *

# Checks TMM_get_checkpoint_write_state() for checkpoints written by the sim process (no fork)
# and by a forked process (checkpoint_cpu), with and without the SIGCHLD trap.
# A forked end checkpoint is also written at shutdown; check_end_checkpoint.sh verifies that
# the simulation waited for it, so the file is complete once the simulation has exited.

test_suite = "checkpoint_write_state"


def check_no_fork():
    trick.checkpoint_cpu(0)
    trick.checkpoint("chkpnt_nofork_ok")
    TRICK_EXPECT_TRUE(trick.TMM_get_checkpoint_write_state(), test_suite, "no_fork_write_ok")
    trick.checkpoint("no_such_dir/chkpnt_nofork_bad")
    TRICK_EXPECT_FALSE(trick.TMM_get_checkpoint_write_state(), test_suite, "no_fork_open_failure")


def check_fork():
    trick.checkpoint_cpu(1)
    trick.checkpoint("chkpnt_fork_ok")
    TRICK_EXPECT_TRUE(trick.TMM_get_checkpoint_write_state(), test_suite, "fork_write_ok")
    trick.checkpoint("no_such_dir/chkpnt_fork_bad")
    TRICK_EXPECT_FALSE(trick.TMM_get_checkpoint_write_state(), test_suite, "fork_open_failure")


def check_fork_with_sigchld_trap():
    # With the trap on, Trick's SIGCHLD handler reaps the forked writer before the result is
    # requested, so the result has to come from what the child reported through shared memory.
    trick.exec_set_trap_sigchld(1)
    trick.checkpoint("chkpnt_fork_trap_ok")
    time.sleep(0.5)
    TRICK_EXPECT_TRUE(trick.TMM_get_checkpoint_write_state(), test_suite, "fork_sigchld_trap_write_ok")
    trick.checkpoint("no_such_dir/chkpnt_fork_trap_bad")
    time.sleep(0.5)
    TRICK_EXPECT_FALSE(trick.TMM_get_checkpoint_write_state(), test_suite, "fork_sigchld_trap_open_failure")
    trick.exec_set_trap_sigchld(0)


def main():
    trick_utest.unit_tests.enable()
    trick_utest.unit_tests.set_file_name(os.getenv("TRICK_HOME") + "/trick_test/SIM_checkpoint_write_state.xml")
    trick_utest.unit_tests.set_test_name("CheckpointWriteState")

    trick.add_read(1.0, "check_no_fork()")
    trick.add_read(1.5, "check_fork()")
    trick.add_read(2.0, "check_fork_with_sigchld_trap()")

    # checkpoint_cpu(1) is still set at shutdown, so the end checkpoint is forked.
    trick.checkpoint_end(True)

    trick.stop(3.0)


if __name__ == "__main__":
    main()
