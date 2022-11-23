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
Run started with data recording group 1
Checkpoint loaded at t=5
Expected: ?

Overall: expectation is that what loads in from the checkpoint should take precedence and overwrite the file of the same name.