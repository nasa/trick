#! /home/cherpin/gsetup2/venv/bin/python3.8
#used for test cases
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--hello", help=f"print hello world.", action="store_true")
args = parser.parse_args()
if args.hello:
    print("Hello World!")
else:
    import os
    print(os.environ.get("CC"))
    print(os.environ.get("CFLAGS"))
    print(os.environ.get("CPP"))
    print(os.environ.get("CPPFLAGS"))
