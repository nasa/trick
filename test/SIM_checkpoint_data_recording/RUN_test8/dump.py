import trick
from trick.unit_test import *

# This was just here for convenience to dump the checkpoints.

def main():
    exec(open("Modified_data/fooChange2.dr").read())
    
    trick.checkpoint(5.0)

    trick.stop(20.0)

if __name__ == "__main__":
    main()