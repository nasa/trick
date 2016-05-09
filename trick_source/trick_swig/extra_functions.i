// These are utility routines that are used within the python scripts to do unusual conversions

%inline %{

// This takes a pointer and adds a level of indirection.  
void * wrap_ptr ( void * in_ptr ) {
    void ** temp_ptr = new void * ;
    temp_ptr[0] = in_ptr ;
    return(temp_ptr) ;
}

double unhex_double ( long long in_value ) {

    union LL2D {
        double d ;
        long long ll ;
    } ;

    union LL2D ll2d ;
    ll2d.ll = in_value ;

    return(ll2d.d) ;

}

float unhex_float ( long in_value ) {

    union L2F {
        float f ;
        long l ;
    } ;

    union L2F l2f ;
    l2f.l = in_value ;

    return(l2f.f) ;

}

%}

