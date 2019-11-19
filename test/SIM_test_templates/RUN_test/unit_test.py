
import math
from trick.unit_test import *

def main():
    tso.tobj.TTT_var.aa = 1000
    tso.tobj.TTT_var.bb = 2000

    print ("tso.tobj.TTT_var.aa =") , tso.tobj.TTT_var.aa
    print ("tso.tobj.TTT_var.bb =") , tso.tobj.TTT_var.bb

    trick.checkpoint_end(True)

if __name__ == "__main__":

    main()


