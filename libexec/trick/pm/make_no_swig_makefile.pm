package make_no_swig_makefile ;

use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(make_no_swig_makefile);

use strict ;

sub make_no_swig_makefile() {
    open MAKEFILE , ">Makefile_swig" or return ;
    print MAKEFILE "# Override TRICK_LIBS variables removing libtrick_pyip.a and remove python libs.\n" ;
    print MAKEFILE "TRICK_LIBS := \${TRICK_LIB_DIR}/libtrick.a\n" ;
    print MAKEFILE "PYTHON_LIB =\n\n" ;
    print MAKEFILE "convert_swig:\n\n" ;
    close MAKEFILE ;
    return ;
}

1;

