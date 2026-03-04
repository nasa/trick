This test suite runs different data recording and checkpoint combinations in an effort to document and test expected behaviors.

RUN_test1
Checkpoint dumped at t=5 with data recording
Run started without data recording
Checkpoint loaded at t=5
Expected: log_foo.csv contains data recorded from t=5+

RUN_test2
Checkpoint dumped at t=5 without data recording
Run started with data recording
Checkpoint loaded at t=5
Expected: log_foo.csv does not exist

RUN_test3
Checkpoint dumped at t=5 with data recording
Run started with data recording
Checkpoint loaded at t=5
Expected: log_foo.csv contains data recorded from t=5+

RUN_test4
Checkpoint dumped at t=2 with data recording
Run started with data recording
Checkpoint loaded at t=5
Expected: log_foo.csv contains data recorded from t=2+

RUN_test5
Checkpoint dumped at t=7 with data recording
Run started with data recording
Checkpoint loaded at t=5
Expected: log_foo.csv contains data recorded from t=7+

RUN_test6
Checkpoint dumped at t=7 with data recording group foo2
Run started with data recording group foo
Checkpoint loaded at t=5
Expected: log_foo2.csv contains data record from t=7+

RUN_test7
Checkpoint dumped at t=5 with change-only data recording group fooChange
Run started with data recording group fooChange
Checkpoint loaded at t=5
Expected: log_fooChange.csv contains chang-only data record from t=5+

RUN_test8
Checkpoint dumped at t=5 with change-only data recording group fooChange2
Run started with data recording group fooChange2
Checkpoint loaded at t=5
Expected: log_fooChange2.csv contains chang-only data record from t=5+

RUN_test9
Checkpoint dumped with at t=7.01 with multiple loggings rates
RUN started with different logging setup
Checkpoint loaded at t=5
Expected: logging with multiple rates to start from t=7.01+ with no offset

RUN_test11 and RUN_test11_redirect
RUN_test11 runs with data recording and drops a checkpoint at 5.5 seconds
RUN_test11_redirect loads the checkpoint from RUN_test11 at time = 0.0 and redirects the output to its own directory
Expected: ref_log_foo and log_foo in RUN_test11 should match, showing that the checkpoint did not overwrite previous data;
ref_log_foo and log_foo in RUN_test11_redirect should match, showing that the checkpoint redirected output to the new dir.



Overall: expectation is that what loads in from the checkpoint should take precedence and overwrite the file of the same name - expect for RUN_test11*.
