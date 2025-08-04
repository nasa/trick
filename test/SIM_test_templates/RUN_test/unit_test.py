
import math
from trick.unit_test import *

def main():
    tso.tobj.TTT_var_scalar_builtins.aa = 1000
    tso.tobj.TTT_var_scalar_builtins.bb = 2000
    print ("tso.tobj.TTT_var_scalar_builtins.aa =" , tso.tobj.TTT_var_scalar_builtins.aa)
    print ("tso.tobj.TTT_var_scalar_builtins.bb =" , tso.tobj.TTT_var_scalar_builtins.bb)

    tso.tobj.TTT_var_array_builtins.aa = [1, 2]
    tso.tobj.TTT_var_array_builtins.bb = [1.1234, 2.5678, -3.9]
    print ("tso.tobj.TTT_var_array_builtins.aa =" , tso.tobj.TTT_var_array_builtins.aa)
    print ("tso.tobj.TTT_var_array_builtins.bb =" , tso.tobj.TTT_var_array_builtins.bb)

    tso.tobj.TTT_var_enum.aa = trick.Bar_1
    tso.tobj.TTT_var_enum.bb = [trick.Bar_1, trick.Bar_2]
    print ("tso.tobj.TTT_var_enum.aa =" , tso.tobj.TTT_var_enum.aa)
    print ("tso.tobj.TTT_var_enum.bb =" , tso.tobj.TTT_var_enum.bb)

    trick.checkpoint_end(True)

if __name__ == "__main__":

    main()


