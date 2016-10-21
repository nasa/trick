package s_source ;

use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(s_source);

use strict ;
use gte ;
use trick_version ;

sub s_source($) {

    my ($sim_ref) = @_ ;
    my (@cpp_headers , @c_headers) ;

    #--------------------------------------------------------------
    # Generate S_source.c
    open S_SOURCE, ">build/S_source.cpp" or die "Couldn't open build/S_source.cpp!\n";
    open S_SOURCE_H, ">S_source.hh" or die "Couldn't open S_source.hh!\n";

    # Get Trick version
    my ($version, $thread) = get_trick_version() ;

    #---------------------------
    # Get date
    my $date  = localtime ;
    my ($sec,$min,$hour,$mday,$mon,$year) = localtime ;
    $year += 1900 ;
    $mon += 1 ;
    my ($login) = getpwuid($<) ;

    #---------------------------
    # Print version - date
    printf S_SOURCE "\n/* Created $year/%02d/%02d %02d\:%02d\:%02d $login \$ */\n",
     $mon , $mday , $hour , $min , $sec ;

    #---------------------------
    # Includes
    print S_SOURCE_H "
/**
PURPOSE:
    (Contains sim ojbects)
*/

#ifndef __S_SOURCE_HH__
#define __S_SOURCE_HH__

#include <new>
#include <iostream>
#include <stdlib.h>
#include <cstddef>

#include \"trick/attributes.h\"
#include \"trick/populate_attribute_maps.hh\"
#include \"trick/ClassSizeCheck.hh\"

#include \"trick/SimObject.hh\"
#include \"trick/JobData.hh\"
#include \"trick/UnitsMap.hh\"\n\n" ;

    # TODO get rid of this statement!  This is still here because of the collect
    print S_SOURCE_H "#define ip_alloc calloc\n" ;
    print S_SOURCE_H "$$sim_ref{system_headers}\n" ;

    foreach my $h ( @{$$sim_ref{sim_class_includes}} ) {
        if ( $h !~ /\.h$/ ) {
            push @cpp_headers , $h ;
        }
        else {
            my ($full_path) ;
            $full_path = $$sim_ref{headers_full_path}{$h} ;
            if ($$sim_ref{headers_lang}{$h} eq "cpp" ) {
                push @cpp_headers , $h ;
            }
            else {
                push @c_headers , $h ;
            }
        }
    }

    my %temp_hash ;
    @cpp_headers = grep ++$temp_hash{$_} < 2, @cpp_headers ;

    foreach my $h ( sort @cpp_headers ) {
        my $printed = 0 ;
        # list header files that are not under the trick_source directory
        # (e.g. trick_models/ directory)
        if ( $h !~ s/^TRICK_HOME\/trick_source\/// ) {
            foreach ( @{$$sim_ref{inc_paths}} ) {
                if ($h =~ s,^$_/,, ) {
                    print S_SOURCE_H "#include \"$h\"\n";
                    $printed = 1 ;
                    last ;
                }
            }
        }
        # list header files that are in trick_source
        if ( $printed eq 0 ) {
            print S_SOURCE_H "#include \"$h\"\n";
        }
    }

    if ( scalar @c_headers ) {
        print S_SOURCE_H "\n#ifdef __cplusplus\n" ,
         "extern \"C\" {\n" ,
         "#endif\n\n" ;

        foreach my $h ( sort @c_headers ) {
            my $printed = 0 ;
            if ( $h !~ s/^TRICK_HOME\/trick_source\/// ) {
                foreach ( @{$$sim_ref{inc_paths}} ) {
                    if ($h =~ s,^$_/,, ) {
                        print S_SOURCE_H "#include \"$h\"\n";
                        $printed = 1 ;
                        last ;
                    }
                }
            }
            if ( $printed eq 0 ) {
                print S_SOURCE_H "#include \"$h\"\n";
            }
        }

        print S_SOURCE_H "\n#ifdef __cplusplus\n" ,
         "}\n" ,
         "#endif\n" ;
    }
    print S_SOURCE_H "\n" ;

    # prints the user defined header code
    print S_SOURCE_H "$$sim_ref{user_header}\n" ;

    # prints classes defined in S_define file.  Set in parse_s_define.pm
    print S_SOURCE_H $$sim_ref{sim_class_code} ;

    # prints an instantiation for each templated sim object for ICG to process.
    print S_SOURCE_H "#ifdef TRICK_ICG\n" ;
    print S_SOURCE_H $$sim_ref{template_instance_declarations} ;
    print S_SOURCE_H "#endif\n\n" ;

    # prints an extern declaration for each sim object
    print S_SOURCE_H "#ifndef SWIG\n" ;
    print S_SOURCE_H $$sim_ref{extern_instance_declarations} ;
    print S_SOURCE_H "#endif\n" ;

    print S_SOURCE_H "\n\n#endif\n" ;

    close S_SOURCE_H ;

    print S_SOURCE "#include \"../S_source.hh\"\n" ;

    print S_SOURCE "$$sim_ref{user_code}\n" ;

    # prints associated call functions
    print S_SOURCE $$sim_ref{sim_class_call_functions} ;

    # prints instantiations from S_define file. Set in parse_s_define.pm
    print S_SOURCE "\n// Instantiate stuff\n" ;
    print S_SOURCE $$sim_ref{instance_declarations} ;

    # prints integration loop sim objects from S_define file
    print S_SOURCE "\n// Integration Loop Sim Object(s) JMP\n" ;
    foreach my $integ_loop ( @{$$sim_ref{integ_loop}} ) {
        print S_SOURCE "IntegLoopSimObject $$integ_loop{name}($$integ_loop{cycle}, $$integ_loop{child}" ;
        my @integ_loop_sim_objects = split(/\s*,\s*/, $$integ_loop{name_list} ) ;
        foreach my $integ_loop_sim_obj (@integ_loop_sim_objects) {
            print S_SOURCE ", &$integ_loop_sim_obj" ;
        }
        print S_SOURCE ", (void *)NULL);\n" ;
    }

    #---------------------------
    # Default Environment
    print S_SOURCE "\n/* Default Environment */\n" ,
     "SimEnvironment::SimEnvironment() {\n\n" ;
    print S_SOURCE gte("S_source");
    print S_SOURCE "}\n\n" ;


    #---------------------------
    # Memory Init
    print S_SOURCE "Trick::ClassSizeCheck * Trick::ClassSizeCheck::pInstance = NULL ;\n" ;

    print S_SOURCE "\n/* Memory Init */\n" ,
     "void memory_init( void ) {\n\n" ;

    print S_SOURCE " " x 4 , "ALLOC_INFO * ai ;\n" ;
    print S_SOURCE " " x 4 , "exec_set_version_date_tag\( \"@(#)CP Version $version, $date\" \) ;\n" ;
    print S_SOURCE " " x 4 , "exec_set_build_date\( \"$date\" \) ;\n" ;
    print S_SOURCE " " x 4 , "exec_set_current_version\( \"$version\" \) ;\n\n" ;

    print S_SOURCE " " x 4 , "populate_sim_services_class_map\(\) ;\n" ;
    print S_SOURCE " " x 4 , "populate_sim_services_enum_map\(\) ;\n" ;
    print S_SOURCE " " x 4 , "populate_class_map\(\) ;\n" ;
    print S_SOURCE " " x 4 , "populate_enum_map\(\) ;\n" ;
    print S_SOURCE "\n" ;

    # prints the job class order for the cyclic jobs
    foreach my $cl ( @{$$sim_ref{user_class_order}} ) {
        print S_SOURCE"    exec_add_scheduled_job_class(\"$cl\") ;\n" ;
    }

    print S_SOURCE "\n" ;

    foreach my $inst ( @{$$sim_ref{instances}} ) {
        print S_SOURCE " " x 4 , "exec_add_sim_object(&$inst, \"$inst\") ;\n" ;
        my $temp_type = $$sim_ref{instances_type}{$inst} ;
        $temp_type =~ s/\s//g ;
        $temp_type =~ s/[<>,:*]/_/g ;
        print S_SOURCE " " x 4 , "TMM_declare_ext_var(&$inst, TRICK_STRUCTURED,\"$temp_type\", 0, \"$inst\", 0, NULL) ;\n" ;
        print S_SOURCE " " x 4 , "if ( (ai = get_alloc_info_at(&$inst)) != NULL ) {\n" ;
        print S_SOURCE " " x 8 , "ai->alloced_in_memory_init = 1 ;\n" ;
        print S_SOURCE " " x 4 , "}\n" ;
    }

    print S_SOURCE " " x 4 , "// Add Integration Loop Sim Object(s) JMP\n" ;
    foreach my $integ_loop ( sort sim_integ_by_name @{$$sim_ref{integ_loop}} ) {
        print S_SOURCE " " x 4 , "exec_add_sim_object(&$$integ_loop{name} , \"$$integ_loop{name}\") ;\n" ;
        print S_SOURCE " " x 4 , "TMM_declare_ext_var(&$$integ_loop{name}, TRICK_STRUCTURED,\"IntegLoopSimObject\", 0, \"$$integ_loop{name}\", 0, NULL) ;\n" ;
    }
    print S_SOURCE "\n" ;

    print S_SOURCE $$sim_ref{create_connections} ;

    print S_SOURCE "\n" ,
     " " x 4 , "Trick::ClassSizeCheck::class_size_check()->print_nonzero_diffs() ;\n" ,
     " " x 4 , "Trick::ClassSizeCheck::reset_instance() ;\n" ,
     " " x 4 , "return ;\n" ,
     "}\n\n" ;

    #------------------------------------------
    # Write out the 'exec_collect_init()' function in S_source.c
    #
    # exec_collect_init() is responsible for initializing all the pointer
    # management associated with 'collect' statements in the S_define file.
    #
    # For every collect statement, perform the following:
    #    (I highly recommended viewing S_source.c to get a clearer picture
    #    of what is going on here.)
    # 1) Get a pointer to the collect info in the CP structures.
    # 2) Allocate memory for the collect pointer list based on the number
    #    of items listed in the collect statement, allocate an additional
    #    8 bytes to hold the number of items in the list.
    # 3) store the address of the new memory in the 'n' parameter.
    # 4) store the number of items in the list at the address pointed to
    #    by 'n'.
    # 5) Shift the address f the original allocation past the 8 byte integer
    #    stored at 'n', and save this address in the pointer defined by the
    #    collect statement. This address represents the first element of an
    #    array of addresses of each of the elements listed in the collect list.
    # 6) Save the address of each element in the collect list in the
    #    respective pointer list just created.
    #

    #-------------
    # Write the exec_collect_init() entry point.

    print S_SOURCE "\n/*---------------------------------------------------------------------*/\n\n" ;

    print S_SOURCE "void exec_collect_init( void ) {\n\n" ;

    if ( exists $$sim_ref{collect} ) {

#        print S_SOURCE   " " x 4 , "long * lp ;\n\n" ;
#
#
#        foreach my $collect ( sort sim_collect_by_name @{$$sim_ref{collect}} ) {
#            # Number of params in collect statement
#            my $np = $$collect{num_params} ;
#
#            print S_SOURCE " " x 4 , "$$collect{name} = (void**)ip_alloc( $np + 1 , sizeof(void*));\n" ,
#             " " x 4 , "lp = (long *)$$collect{name} ;\n" ,
#             " " x 4 , "*lp = $np ;\n" ,
#             " " x 4 , "$$collect{name}++ ;\n" ;
#
#            for ( my $i = 0; $i < $np ; $i++ ) {
#                my $name = @{$$collect{params}}[$i] ;
#                print S_SOURCE " " x 4 , "$$collect{name}\[$i\] = (void*)&($name);\n" ;
#            }
#        }
        #print S_SOURCE "\n#if 0\n" ;
        foreach my $collect ( @{$$sim_ref{collect}} ) {
            print S_SOURCE " " x 4 , "$$collect{name} = NULL ;\n" ;
            my $np = $$collect{num_params} ;
            foreach my $name ( @{$$collect{params}} ) {
                print S_SOURCE " " x 4 , "$$collect{name} = add_collect($$collect{name}, (void*)&($name));\n" ;
            }
        }
        #print S_SOURCE "\n#endif\n" ;
    }

    if ( exists $$sim_ref{vcollect} ) {

        foreach my $collect ( sort sim_vcollect_by_name @{$$sim_ref{vcollect}} ) {

            my $name = $$collect{name} ;
            my $func = $$collect{constructor} ;
            undef $func if ( defined $func ) && ( $func eq "" ) ;
            foreach my $item ( sort @{$$collect{items}} ) {
                my $ref = $item ;
                if ( $ref =~ s/&// ) {
                    $ref = "&($ref)" ;
                } else {
                    $ref = "$ref" ;
                }
                if (defined $func ) {
                    $ref = "$func( $ref )" ;
                }
                print S_SOURCE " " x 4, "$name.push_back(\n", " " x 24, "$ref );\n" ;
            }
            print S_SOURCE "\n" ;
        }
    }

    print S_SOURCE " " x 4, "return ;\n" ,
     "}\n\n" ;

    print S_SOURCE "int master( int nargs, char ** args ) ;\n\n" ;
    print S_SOURCE "int main( int nargs, char ** args ) {\n" ;
    print S_SOURCE " " x 4 , "return master(nargs, args) ;\n" ;
    print S_SOURCE "}\n" ;

    close S_SOURCE ;


    open S_INSTANCE, ">build/CP_instances" or die "Couldn't open build/CP_instances!\n";
    print S_INSTANCE $$sim_ref{extern_instance_declarations} ;
    foreach my $integ_loop ( @{$$sim_ref{integ_loop}} ) {
        print S_INSTANCE "extern IntegLoopSimObject $$integ_loop{name} ;\n" ;
    }
    close S_INSTANCE ;
}

#--------------------------------------------------------------
# Given an array of sim integrate statements, provide sort routines
#
sub sim_integ_by_name {

    return
        #Sort by sim integrate name in ASCII order
        ( ${$a}{name} cmp ${$b}{name} )  ||
        #If two names are identical, then sort based on cycle time
        ( ${$a}{cycle} <=> ${$b}{cycle} );
}

#--------------------------------------------------------------
# Given an array of sim collect statements, provide sort routines
#
sub sim_collect_by_name {

    return
        #Sort by sim collect statement name in ASCII order
        ( ${$a}{name} cmp ${$b}{name} )  ||
        #If two params are identical, then sort based on num_params
        ( ${$a}{num_params} <=> ${$b}{num_params} );
}

#--------------------------------------------------------------
# Given an array of sim Vcollect statements, provide sort routines
#
sub sim_vcollect_by_name {

    return
        #Sort by sim collect statement name in ASCII order
        ( ${$a}{name} cmp ${$b}{name} );
}

1;
