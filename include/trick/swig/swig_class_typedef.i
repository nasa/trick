
/*
  This is the 1D array support code for SWIG

  TYPE is the actual type of the variable (int, double, units::values::m)
  NAME is the type with ':' substituted with '_' (int, double, units__values__m)

 */

%define %trick_swig_class_typemap(TYPE, NAME)

%typemap(memberin) TYPE [ANY] {
    std::cout << "Cannot assign directly to $1_type\n" << std::endl ;
}

%typemap(out) TYPE [ANY] {

    std::string temp_name ;

    swig_ref * t = new swig_ref ;

    t->ref.address = (void *)$1;
    t->ref.units = NULL ;

    t->ref.attr = new ATTRIBUTES() ;

    t->ref.attr->size = sizeof($1_basetype) ;
    t->ref.attr->type = TRICK_STRUCTURED ;

    t->ref.attr->type_name  = strdup("$1_basetype") ;
    t->ref.attr->units = NULL ;
    t->ref.attr->attr  = NULL ;
    t->ref.attr->io  = TRICK_VAR_OUTPUT | TRICK_VAR_INPUT | TRICK_CHKPNT_OUTPUT | TRICK_CHKPNT_INPUT ;

    t->ref.attr->num_index  = 1 ;
    t->ref.attr->index[0].size  = $1_dim0 ;
    t->ref.attr->index[0].start  = 0 ;

    t->ref.create_add_path  = 0 ;
    t->ref.num_index  = 0 ;
    t->ref.num_index_left  = 1 ;
    t->ref.ref_type  = REF_ADDRESS ;

    t->ref.attr->units = strdup("1") ;

    //cout << "returning a structured swig_ref" << std::endl ;
    $result = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_ref"), SWIG_POINTER_OWN);
}

%typemap(in) TYPE * , TYPE ** {
    // TYPE *
    void * temp_ptr ;

    SwigPyObject *sobj = SWIG_Python_GetSwigThis($input) ;
    // isDisown is set to SWIG_POINTER_DISOWN by default
    int isDisown = SWIG_POINTER_DISOWN ;
    if (sobj) {
        // isDisown is re-set based on whether the Python processor owns the input object 
        isDisown = !sobj->own ;
    }
    
    if ( SWIG_IsOK(SWIG_ConvertPtr($input, &temp_ptr,$1_descriptor, isDisown)) ) {
        $1 = reinterpret_cast< $1_ltype >(temp_ptr) ;
    } else if ( SWIG_IsOK(SWIG_ConvertPtr($input, &temp_ptr,SWIG_TypeQuery("_p_swig_ref"), 0)) ) {
        // Array to pointer assignment
        swig_ref * temp_swig_ref = reinterpret_cast< swig_ref * >(temp_ptr);
        if ( temp_swig_ref != NULL ) {
            $1 = reinterpret_cast< $1_ltype >(temp_swig_ref->ref.address) ;
        }
    } else if ( SWIG_IsOK(SWIG_ConvertPtr($input, &temp_ptr,SWIG_TypeQuery("_p_REF2"), 0)) ) {
        // We have an address coming in, we don't have to do any translation
        REF2 * temp_ref = reinterpret_cast< REF2 * >(temp_ptr) ;
        if ( temp_ref != NULL ){
            $1 = reinterpret_cast< $1_ltype >(temp_ref->address) ;
        }
    } else if ( SWIG_IsOK(SWIG_ConvertPtr($input, &temp_ptr,SWIG_TypeQuery("_p_void"), 0)) ) {
        // We have an address coming in, we don't have to do any translation
        $1 = reinterpret_cast< $1_ltype >(temp_ptr) ;
    }
}

/*
   Note: There is no "%typemap(out) TYPE *" defined because having one conflicts with
   what SWIG wants to do when allocating new classes. The extend TYPE __getitem__ at
   the bottom of this file provides us indexing to single dimension class variables.
 */

%typemap(out) TYPE * [ANY] {

    //TYPE * [ANY] OUT

    $1_basetype dummy ;

    swig_ref * t = new swig_ref ;

    t->ref.address = (void *)$1;
    t->ref.units = NULL ;

    t->ref.attr = new ATTRIBUTES() ;

    t->ref.attr->size = sizeof($1_basetype) ;
    t->ref.attr->type = TRICK_STRUCTURED ;

    t->ref.attr->type_name  = strdup("$1_basetype") ;
    t->ref.attr->units = NULL ;
    t->ref.attr->attr  = NULL ;
    t->ref.attr->io  = TRICK_VAR_OUTPUT | TRICK_VAR_INPUT | TRICK_CHKPNT_OUTPUT | TRICK_CHKPNT_INPUT ;

    t->ref.attr->num_index  = 2 ;
    t->ref.attr->index[0].size  = $1_dim0 ;
    t->ref.attr->index[1].size  =  0 ;
    t->ref.attr->units = strdup("1") ;

    t->ref.create_add_path  = 0 ;
    t->ref.num_index  = 0 ;
    t->ref.num_index_left  = 2 ;
    t->ref.ref_type  = REF_ADDRESS ;

    $result = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_ref"), SWIG_POINTER_OWN);

}

%typemap(out) TYPE ** {
    // TYPE ** out
    std::string temp_name ;

    swig_ref * t = new swig_ref ;

    t->ref.address = (void *)$1;
    t->ref.units = NULL ;

    t->ref.attr = new ATTRIBUTES() ;

    t->ref.attr->size = sizeof($1_basetype) ;
    t->ref.attr->type = TRICK_STRUCTURED ;

    t->ref.attr->type_name  = strdup("$1_basetype") ;
    t->ref.attr->units = NULL ;
    t->ref.attr->attr  = NULL ;
    t->ref.attr->io  = TRICK_VAR_OUTPUT | TRICK_VAR_INPUT | TRICK_CHKPNT_OUTPUT | TRICK_CHKPNT_INPUT ;

    t->ref.attr->num_index  = 2 ;
    t->ref.attr->index[0].size  = get_truncated_size((char *)t->ref.address) ;
    t->ref.attr->index[1].size  = 0 ;

    t->ref.create_add_path  = 0 ;
    t->ref.num_index  = 0 ;
    t->ref.num_index_left  = 2 ;
    t->ref.ref_type  = REF_ADDRESS ;

    t->ref.attr->units = strdup("1") ;

    $result = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_ref"), SWIG_POINTER_OWN);
}

%typemap(memberin) TYPE [ANY][ANY] {
    std::cout << "Cannot assign directly to $1_type\n" << std::endl ;
}

%typemap(out) TYPE [ANY][ANY] {

    std::string temp_name ;

    swig_ref * t = new swig_ref ;

    t->ref.address = (void *)$1;
    t->ref.units = NULL ;

    t->ref.attr = new ATTRIBUTES() ;

    t->ref.attr->size = sizeof($1_basetype) ;
    t->ref.attr->type = TRICK_STRUCTURED ;

    t->ref.attr->type_name  = strdup("$1_basetype") ;
    t->ref.attr->units = NULL ;
    t->ref.attr->attr  = NULL ;
    t->ref.attr->io  = TRICK_VAR_OUTPUT | TRICK_VAR_INPUT | TRICK_CHKPNT_OUTPUT | TRICK_CHKPNT_INPUT ;

    t->ref.attr->num_index  = 2 ;
    t->ref.attr->index[0].size  = $1_dim0 ;
    t->ref.attr->index[0].start  = 0 ;
    t->ref.attr->index[1].size  = $1_dim1 ;
    t->ref.attr->index[1].start  = 0 ;

    t->ref.create_add_path  = 0 ;
    t->ref.num_index  = 0 ;
    t->ref.num_index_left  = 2 ;
    t->ref.ref_type  = REF_ADDRESS ;

    t->ref.attr->units = strdup("1") ;

    //cout << "returning a structured swig_ref" << std::endl ;
    $result = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_ref"), SWIG_POINTER_OWN);
}

%typemap(memberin) TYPE [ANY][ANY][ANY] {
    std::cout << "Cannot assign directly to $1_type\n" << std::endl ;
}

%newobject TYPE::__dynamic_cast ;

%extend TYPE {
    PyObject * __getitem__(int ii) {

        PyObject *resultobj ;
        int array_size = (int) get_size((char *)self) ;

        if ( ii < 0 ) {
            ii += array_size ;
        }

        if ( (array_size > 0) and (ii > array_size) ) {
            return NULL ;
        }

        //std::cout << "HERE in class __getitem__!!!" << std::endl ;
        // First try the normal SWIG type query which searches the human-readable str field (e.g.
        // "TrickHLA::Interaction *") of each type descriptor. This succeeds only when the "master"
        // descriptor (i.e. the one registered by the first module loaded that references this type)
        // has str set to a non-NULL value. Meaning that the first module loaded that references this
        // type must be the one that also wraps the class (and thus sets str to a non-NULL value).
        // If another module that references the type but does not wrap it is loaded first, then its
        // descriptor becomes the master with str=NULL and the normal SWIG_TypeQuery fails.
        //
        // How to tell whether a module "wraps" a type vs merely "references" it:
        // Look at the static swig_type_info declaration in the generated _py.cpp file.
        // The second field of the struct initializer is the str field:
        //   - Non-NULL (str set)   -> this module directly wraps the class:
        //       static swig_type_info _swigt__p_TrickHLA__Object =
        //           {"_p_TrickHLA__Object", "TrickHLA::Object *", 0, 0, (void*)0, 0};
        //   - NULL (str = 0)       -> this module only references the type as a dependency:
        //       static swig_type_info _swigt__p_TrickHLA__Object =
        //           {"_p_TrickHLA__Object", 0, 0, 0, 0, 0};
        //
        // TrickHLA examples (modules are loaded in alphabetical order by filename):
        //   - TrickHLA::Object: ExecutionConfiguration_py.cpp ('E') loads first and has
        //     str="TrickHLA::Object *" (non-NULL) because ExecutionConfigurationBase inherits from
        //     Object. SWIG sets str for any type that appears in an inheritance/upcast relationship
        //     with a class being wrapped in the module, so that SWIG_TypeQuery can resolve the base
        //     type when performing implicit upcasts at runtime. So ExecutionConfiguration_py.cpp's
        //     declaration:
        //       static swig_type_info _swigt__p_TrickHLA__Object =
        //           {"_p_TrickHLA__Object", "TrickHLA::Object *", 0, 0, (void*)0, 0};
        //     becomes the master. SWIG_TypeQuery succeeds.
        //   - TrickHLA::ExecutionConfiguration: ExecutionConfiguration_py.cpp ('E'+'C'+'o'+'n'+'f'...)
        //     has str="TrickHLA::ExecutionConfiguration *" and sorts before ExecutionControl_py.cpp
        //     ('E'+'C'+'o'+'n'+'t'...) which has str=0. So the home module wins the master slot.
        //     SWIG_TypeQuery succeeds.
        //   - TrickHLA::Interaction: Interaction is only referenced as a plain member field
        //     (Interaction *interactions) in InteractionServices.hh, which Federate.hh includes.
        //     Since there is no inheritance relationship, SWIG sets str=0 in Federate_py.cpp:
        //       static swig_type_info _swigt__p_TrickHLA__Interaction =
        //           {"_p_TrickHLA__Interaction", 0, 0, 0, 0, 0};
        //     Federate_py.cpp ('F') loads before Interaction_py.cpp ('I'), so this str=0 descriptor
        //     becomes the master. SWIG_TypeQuery fails.
        //
        // In the failing case, fall back to SWIG_MangledTypeQuery which searches the name field
        // (e.g. "_p_TrickHLA__Interaction", always non-NULL) and reliably finds the master descriptor
        // regardless of load order.

        swig_type_info *type_info = SWIG_TypeQuery("TYPE *");
        if (!type_info)
        {
#ifdef TRICK_SWIG_TYPEQUERY_DEBUG
            fprintf(stdout, "\nswig_class_typedef: SWIG_TypeQuery(\"TYPE *\") returned NULL, dumping all known types:\n\n");
            // Portable way to access SWIG_globals(). Using stdout for easy shell redirecting to a file if needed for debugging.
            swig_module_info *smod = SWIG_GetModule(0);
            if (smod)
            {
                // Iterate through the circular linked list of modules and print out all types in each module.
                // Note: smod is the entry point to the circular linked list.
                swig_module_info *iter = smod;
                do
                {
                    for (size_t i = 0; i < iter->size; ++i)
                    {
                        fprintf(stdout, "  name=%-60s  str=%s\n", iter->types[i]->name ? iter->types[i]->name : "(null)",
                                iter->types[i]->str ? iter->types[i]->str : "(null)");
                    }
                    iter = iter->next;
                } while (iter != smod);
            }
#endif
            type_info = SWIG_MangledTypeQuery("_p_" "NAME");
        }

        resultobj = SWIG_NewPointerObj(SWIG_as_voidptr(&self[ii]), type_info, 0);
        return (resultobj);
    }

    PyObject * __len__() {
        // get_size only works if "self" was an allocated pointer
        return PyInt_FromLong(get_size((char *)self)) ;
    }
}

/* The following 2 feature("shadow") blocks override the __init__ constructors in the python
   proxy class.  These constructors notify the memory manager that new memory has been
   allocated if a named argument named TMMName is included.  The value of TMMName
   is used as the memory manager name handle to the memory.

   The difference in the 2 blocks is that the first one handles constructor(s) with
   arguments, the second one handles classes that have only a single default constructor.
 */
%feature("shadow") TYPE::TYPE %{
    def __init__(self, *args, **kwargs):
        import _sim_services
        this = $action(*args)
        try: self.this.append(this)
        except: self.this = this
        if 'TMMName' in kwargs:
            this.own(0)
            self.this.own(0)
            isThisInMM = _sim_services.get_alloc_info_at(this)
            if isThisInMM:
                _sim_services.set_alloc_name_at(this, kwargs['TMMName'])
            else:
                _sim_services.TMM_declare_ext_var(this, _sim_services.TRICK_STRUCTURED, "TYPE", 0, kwargs['TMMName'], 0, None)
            alloc_info = _sim_services.get_alloc_info_at(this)
            alloc_info.stcl = _sim_services.TRICK_LOCAL
            alloc_info.alloc_type = _sim_services.TRICK_ALLOC_NEW
%}

%feature("shadow") TYPE::TYPE() %{
    def __init__(self, **kwargs):
        import _sim_services
        this = $action()
        try: self.this.append(this)
        except: self.this = this
        if 'TMMName' in kwargs:
            self.this.own(0)
            isThisInMM = _sim_services.get_alloc_info_at(this)
            if isThisInMM:
                _sim_services.set_alloc_name_at(this, kwargs['TMMName'])
            else:
                _sim_services.TMM_declare_ext_var(this, _sim_services.TRICK_STRUCTURED, "TYPE", 0, kwargs['TMMName'], 0, None)
            alloc_info = _sim_services.get_alloc_info_at(this)
            alloc_info.stcl = _sim_services.TRICK_LOCAL
            alloc_info.alloc_type = _sim_services.TRICK_ALLOC_NEW
%}

%enddef

