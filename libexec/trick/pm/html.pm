package html ;

use Exporter ();
use File::Basename ;
@ISA = qw(Exporter);
@EXPORT = qw(extract_trick_header );

use strict ;

sub extract_trick_header($$$$) {

    my ($file_name , $contents, $verbose, $indent) = @_ ;
    my $trick_header = "" ;
    my %header ;

    if ($contents =~ /\/\*((.*?)PURPOSE:(.*?))\*+\//is ) {
        $trick_header = $1 ;
        $trick_header =~ s/^.*?([A-Za-z])/$1/s ;
        $trick_header =~ s/(\))[^)]*$/$1/s ;
        $trick_header =~ s///g ;
    }

    $header{doc_title} = $1 if $trick_header =~                   /DOC TITLE:[^(]*(.*?)\)([A-Z _\t\n\r]+:|[ \t\n\r]*$)/si ;
    $header{purpose}   = $1 if $trick_header =~                     /PURPOSE:[^(]*(.*?)\)([A-Z _\t\n\r]+:|[ \t\n\r]*$)/si ;
    $header{reference} = $1 if $trick_header =~                   /REFERENCE:[^(]*(.*?)\)([A-Z _\t\n\r]+:|[ \t\n\r]*$)/si ;
    $header{requirements} = $1 if $trick_header =~             /REQUIREMENTS:[^(]*(.*?)\)([A-Z _\t\n\r]+:|[ \t\n\r]*$)/si ;
    $header{"assumptions and limitations"} = $1 if $trick_header =~ /ASSUMPTIONS AND LIMITATIONS:[^(]*(.*?)\)([A-Z _\t\n\r]+:|[ \t\n\r]*$)/si ;
    $header{class}     = $1 if $trick_header =~                       /CLASS:[^(]*(.*?)\)([A-Z _\t\n\r]+:|[ \t\n\r]*$)/si ;
    $header{libdep}    = $2 if $trick_header =~ /LIBRARY[ _]DEPENDENC(Y|IES):[^(]*(.*?)\)([A-Z _\t\n\r]+:|[ \t\n\r]*$)/si ;
    $header{iodep}     = $2 if $trick_header =~         /IO DEPENDENC(Y|IES):[^(]*(.*?)\)([A-Z _\t\n\r]+:|[ \t\n\r]*$)/si ;
    $header{icg_ignore} = $2 if $trick_header =~  /ICG[ _]IGNORE[ _]TYPE(S)?:[^(]*(.*?)\)([A-Z _\t\n\r]+:|[ \t\n\r]*$)/si ;
    $header{swig}       = $1 if $trick_header =~                       /SWIG:[^(]*\((.*?)\)([A-Z _\t\n\r]+:|[ \t\n\r]*$)/si ;
    $header{default_data} = $1 if $trick_header =~          /DEFAULT[ _]DATA:[^(]*(.*?)\)([A-Z _\t\n\r]+:|[ \t\n\r]*$)/si ;
    $header{python_module} = $1 if $trick_header =~        /PYTHON[ _]MODULE:[^(]*\((.*?)\)([A-Z _\t\n\r]+:|[ \t\n\r]*$)/si ;
    $header{programmers} = $1 if $trick_header =~               /PROGRAMMERS:[^(]*(.*?)\)([A-Z _\t\n\r]+:|[ \t\n\r]*$)/si ;
    $header{language}  = $1 if $trick_header =~                    /LANGUAGE:[^(]*(.*?)\)([A-Z _\t\n\r]+:|[ \t\n\r]*$)/si ;

    if ( $verbose ) {
            print " "x$indent , "/*\n" ;
            print " "x($indent+3) , "DOC TITLE:\n\t$header{doc_title})\n" if ( exists $header{doc_title} )  ;
            print " "x($indent+3) , "PURPOSE:\n\t$header{purpose})\n" if ( exists $header{purpose} )  ;
            print " "x($indent+3) , "REFERENCE:\n\t$header{reference})\n" if ( exists $header{reference} )  ;
            print " "x($indent+3) , "ASSUMPTIONS AND LIMITATIONS:\n\t$header{assump})\n" if ( exists $header{assump} )  ;
            print " "x($indent+3) , "CLASS:\n\t$header{class})\n" if ( exists $header{class} )  ;
            print " "x($indent+3) , "LIBRARY DEPENDENCIES:\n\t$header{libdep})\n" if ( exists $header{libdep} )  ;
            print " "x($indent+3) , "SWIG:\n\t($header{swig})\n" if ( exists $header{swig} )  ;
            print " "x($indent+3) , "PROGRAMMERS:\n\t$header{program})\n" if ( exists $header{program} )  ;
            print " "x($indent+3) , "LANGUAGE:\n\t$header{language})\n" if ( exists $header{language} )  ;
            print " "x$indent , "*/\n\n" ;
    }

    if ( $contents =~ /(?:@|\\)trick_li(?:nk|b)_dependency\s*{\s*(.*?)\s*}/) {
        my @lib_list ;
        @lib_list = ($contents =~ m/(?:@|\\)trick_li(?:nk|b)_dependency\s*{\s*(.*?)\s*}/gs) ;
        # save doxygen style trick_lib_dependency fields in field = liblist.
        $header{liblist} = [@lib_list] ;
    }
    if ( $contents =~ /(?:@|\\)python_module\s*{\s*(.*?)\s*}/) {
        $header{python_module} = $1;
    }

    $header{language} = "CPP" if ( $header{language} =~ /c[p\+]+/i ) ;

    return %header ;

}

1;
