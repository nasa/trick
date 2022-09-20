import trick
# from trick.unit_test import *

def main():
    trick.exec_set_job_onoff("the_object.stlc.addData", 1, True)
    trick.exec_set_job_onoff("the_object.stlc.test", 1, False)
    trick.exec_set_job_onoff("the_object.stlc.print", 1, False)


    # trick.checkpoint_post_init(True)
    # trick.checkpoint_end(True)
    trick.add_read( 0.5, 'trick.checkpoint("chkpnt_in1234")')

    # Data recording HDF5 test
    trick.exec_set_freeze_frame(0.10)
    trick.stop(1.0)

if __name__ == "__main__":
    main()

