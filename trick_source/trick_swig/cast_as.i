
/*
  TYPE is the actual type of the variable (int, double, units::values::m)
  NAME is the type with ':' substituted with '_' (int, double, units__values__m)
 */

%define %trick_cast_as(TYPE, NAME)

%inline %{
/* caseAs##NAME casts incoming ptrs as the target type.  Takes incoming object and tests if it
   is a compatible type, a void *, or an integer (raw pointer value).  If it is any of these
   types it returns the incoming value as a pointer to the outgoing type.  The integer
   conversion is used by the input processor to associate named allocations in the
   memory manager to names in pyton space.
 */
TYPE * castAs##NAME( PyObject * obj1 ) {
    void *argp = NULL ;
    TYPE * ret = (TYPE *)NULL ;

    //std::cout << "calling __cast_as" << std::endl ;
    if ( SWIG_IsOK(SWIG_ConvertPtr(obj1,&argp,SWIG_TypeQuery("TYPE *"),SWIG_POINTER_DISOWN)) ) {
        //std::cout << "we can cast to TYPE! " << argp << std::endl ;
        ret = reinterpret_cast< TYPE *>(argp) ;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(obj1, &argp,SWIG_TypeQuery("void *"), 0 ))) {
        //std::cout << "we can cast (wrapped void *) to TYPE! " << argp << std::endl ;
        ret = reinterpret_cast< TYPE *>(argp) ;
    } else if ( PyInt_Check(obj1) ) {
        ret = reinterpret_cast< TYPE *>( PyInt_AsLong(obj1) ) ;
        //std::cout << "we are using incoming value as address " << PyInt_AsLong(obj1) << std::endl ;
    } else {
        std::cout << "Cast to TYPE failed." << std::endl ;
    }
    return ret ;
}
%}

%enddef

