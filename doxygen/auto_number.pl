#!/usr/bin/perl

use strict ;

sub read_file($$$) {
    my ($file_name, $levels, $prefix) = @_ ;
    my ($out_file_name) ;
    my ($ii) ;

    local *FILE ;
    local *OUTFILE ;

    if ( $file_name =~ /dox_in$/ ) {

        if ( ! -e $file_name ) {
            if ( ! -e "$ENV{TRICK_HOME}/trick_source/$file_name" ) {
                die "could not open $file_name" ;
            } else {
                $file_name = "$ENV{TRICK_HOME}/trick_source/$file_name" ;
            }
        }
        open FILE, "$file_name" or die "could not open $file_name" ;

        $out_file_name = $file_name ;
        $out_file_name =~ s/_in$// ;
        open OUTFILE, "> $out_file_name" or die "could not open $file_name" ;

        while ( <FILE> ) {

            if ( /^#include\s+"(.*?)"/ ) {

                my ($include_file) = $1 ;
                my ($out_include_file) ;

                $out_include_file = $include_file ;
                if ( $out_include_file =~ s/_in$// ) {
                    print OUTFILE "#include \"$out_include_file\"\n" ;
                    read_file($include_file, $levels, $prefix) ;
                } else {
                    print OUTFILE ;
                }

            } elsif ( /^(.*?)LEVEL(\d+)(cont)?(.*)/ ) {

                my ($tag) = $1 ;
                my ($level) = $2 ;
                my ($cont) = $3 ;
                my ($title) = $4 ;
                my ($section, $level_dots)  ;

                if ( $cont eq "" ) {
                    $$levels[$level]++ ;
                    for( $ii = $level + 1 ; $ii < 8 ; $ii++ ) {
                        $$levels[$ii] = 0 ;
                    }
                }
                $section = sprintf "$prefix%02d" , $$levels[1] ;
                $level_dots = "$$levels[1]" ;
                for( $ii = 2 ; $ii <= $level ; $ii++ ) {
                    #$section .= $$levels[$ii] ;
                    $section .= sprintf "%02d" , $$levels[$ii] ;
                    $level_dots .= ".$$levels[$ii]" ;
                }
                if ( $tag =~ /\@\w+\s+$/ ) {
                    print OUTFILE "$tag$section $level_dots $title\n" ;
                } else {
                    print OUTFILE "$tag$level_dots $title\n" ;
                }

            } else {
                print OUTFILE ;
            }
        }
    }
}

my @levels = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ;

die "Usage: auto_number.pl <prefix>" if ( scalar @ARGV != 1 ) ;
read_file("main_page.dox_in", \@levels, @ARGV[0]) ;
