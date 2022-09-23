# This file contains any globals all tests use, so that it can be changed
# in a single place if they ever need to be updated
import os, sys, pdb
# Global location of this trick instance, for all tests to use
module_rel_path = 'share/trick/trickops'
this_trick = os.path.abspath(os.path.join(os.getcwd(), '../../../..'))
sys.path.append(os.path.join(this_trick, module_rel_path ))
tests_dir = os.path.join(this_trick,  module_rel_path, 'tests')
