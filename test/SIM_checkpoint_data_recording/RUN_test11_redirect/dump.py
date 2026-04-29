import trick
from trick.unit_test import *

# This was just here for convenience to dump the checkpoints.

def main():
    exec(open("Modified_data/foo.dr").read())

    drg[DR_GROUP_ID].set_cycle(0.1)

    trick.add_read(0.0, 'trick.load_checkpoint("RUN_test11/chkpnt_5.500000", "RUN_test11_redirect")')
    trick.stop(10.0)

if __name__ == "__main__":
    main()
