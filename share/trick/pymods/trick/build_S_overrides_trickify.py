import os
import sys

path = os.path.dirname(os.path.abspath(__file__)) + "/build_trickify.py"
exec(open(path).read())

build_S_overrides_trickify(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
