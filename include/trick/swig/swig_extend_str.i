
/* 
  This extend directive allows users to print entire structs/classes with one command.
 */

%define %struct_str(TYPE) 

%extend TYPE {
    char * __str__() {

        extern Trick::MemoryManager* trick_MM;
        extern ATTRIBUTES attr##TYPE[] ;

        std::stringstream os ;
        std::string temp_str ;
        Trick::PythonPrint pp ;

        static char * temp = NULL ;

        os.str("") ;
        os << "[\n" ;

        trick_MM->set_CheckPointAgent(&pp) ;
        trick_MM->write_composite_var( os , (char *)self , attr##TYPE ) ;
        trick_MM->reset_CheckPointAgent() ;

        os << "]" ;

        temp = (char *)realloc( (void *)temp , os.str().length() + 1 ) ;
        temp_str = os.str() ;
        strncpy(temp , temp_str.c_str(), os.str().length()) ;
        temp[os.str().length()] = '\0' ;

        return temp ;
    }
}

%enddef
