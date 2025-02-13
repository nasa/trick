import os

path = os.path.dirname(os.path.abspath(__file__)) + "/build_trickify.py"
exec(open(path).read())

build_S_source()
