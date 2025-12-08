import trick
from trick.unit_test import *

# This was just here for convenience to dump the checkpoints.

def main():
    exec(open("Modified_data/foo2.dr").read())

    drg[DR_GROUP_ID].set_cycle(0.1)
    drg[DR_GROUP_ID].add_cycle(0.25)
    
    trick.checkpoint(7.0)

    trick.stop(10.0)

if __name__ == "__main__":
    main()
