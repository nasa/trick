import trick
from trick.unit_test import *

def main():

    trick.load_checkpoint("RUN_test/chkpnt_in")
    trick.load_checkpoint_job()

    trick.exec_set_job_onoff("the_object.stlc.addData", 1, False)
    trick.exec_set_job_onoff("the_object.stlc.test", 1, True)
    trick.exec_set_job_onoff("the_object.stlc.print", 1, False)

    trick_utest.unit_tests.enable()
    trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_stl.xml" ) 
    trick_utest.unit_tests.set_test_name( "STLCheckpointTest" ) 

    trick.stop(1.0)

if __name__ == "__main__":
    main()

