#!/usr/bin/env python3
import pytest
import sys
import os



if __name__ == "__main__":
    ### CREATE VIRTUAL DISPLAY ###
    os.system('Xvfb :1 -screen 0 1600x1200x16  &')    # create virtual display with size 1600x1200 and 16 bit color. Color can be changed to 24 or 8
    os.environ['DISPLAY']=':1.0'    # tell X clients to use our virtual DISPLAY :1.0.
    
    args = list(sys.argv[1:]) + ["tests"]
    print("Runing tests with arguments:", args)
    sys.exit(pytest.main(args))