#ifndef _STL_S_DEFINE_MACRO_HH_
#define _STL_S_DEFINE_MACRO_HH_

#define CHECKPOINT_STL(varname) \
    ("checkpoint")      checkpoint_stl(varname, name + std::string("_") + std::string(#varname), std::string("")) ; \
    ("post_checkpoint") delete_stl    (varname, name + std::string("_") + std::string(#varname), std::string("")) ; \
    ("restart")         restore_stl   (varname, name + std::string("_") + std::string(#varname), std::string(""))

#endif
