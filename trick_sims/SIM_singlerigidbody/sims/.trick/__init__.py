from pkgutil import extend_path
__path__ = extend_path(__path__, __name__)
import sys
import os
sys.path.append(os.getcwd() + "/trick.zip/trick")

import _sim_services
from sim_services import *

# create "all_cvars" to hold all global/static vars
all_cvars = new_cvar_list()
combine_cvars(all_cvars, cvar)
cvar = None

# /users/bnwalla1/SingleRigidBody/include/body.hh
import _me18dda184e7a3a5300b5efa2b558f756
combine_cvars(all_cvars, cvar)
cvar = None

# /users/bnwalla1/SingleRigidBody/sims/S_source.hh
import _m2ddd17f9284f93dc40b8a127bd142ae5
combine_cvars(all_cvars, cvar)
cvar = None

# /users/bnwalla1/SingleRigidBody/include/body.hh
from me18dda184e7a3a5300b5efa2b558f756 import *
# /users/bnwalla1/SingleRigidBody/sims/S_source.hh
from m2ddd17f9284f93dc40b8a127bd142ae5 import *

# S_source.hh
import _m2ddd17f9284f93dc40b8a127bd142ae5
from m2ddd17f9284f93dc40b8a127bd142ae5 import *

import _top
import top

import _swig_double
import swig_double

import _swig_int
import swig_int

import _swig_ref
import swig_ref

from shortcuts import *

from exception import *

cvar = all_cvars

