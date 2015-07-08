package make_makefile ;

use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(make_makefile);

use lib $ENV{"TRICK_HOME"} . "/libexec/trick/pm" ;
use Cwd ;
use Cwd 'abs_path';
use File::Basename ;
use strict ;
use trick_version ;

sub make_makefile($$$) {

    my ($h_ref , $sim_ref , $make_cwd ) = @_ ;
    my ($n , $f , $k , $i , $m);
    my $num_inc_objs ;
    my %all_mis_depends ;
    my %temp_hash ;
    my @all_cfly_files ;
    my @all_read_only_libs ;
    my @all_compile_libs ;
    my %files_by_dir ;
    my ( $sp_dir , $src_dir , $sp_file , $base_name , $suffix) ;
    my @temp_array ;

    if ( exists $$sim_ref{all_mis_depends} ) {
        %all_mis_depends = %{$$sim_ref{all_mis_depends}} ;
    }

    my @exclude_dirs ;
    @exclude_dirs = split /:/ , $ENV{"TRICK_EXCLUDE"};
    # See if there are any elements in the exclude_dirs array
    if (scalar @exclude_dirs) {
        @exclude_dirs = sort(@exclude_dirs );
        # Error check - delete any element that is null
        # (note: sort forced all blank names to front of array
        @exclude_dirs = map { s/(^\s+|\s+$)//g ; $_ } @exclude_dirs ;
        while ( not length @exclude_dirs[0] ) {
            # Delete an element from the left side of an array (element zero)
            shift @exclude_dirs ;
        }
        @exclude_dirs = map { (-e $_) ? abs_path($_) : $_ } @exclude_dirs ;
    }

    # get a list of all source files required by this file
    foreach $n ( @{$$sim_ref{mis_entry_files}} ) {
        foreach $k ( grep !/last_look/ , (keys %{$all_mis_depends{$n}}) ) {
            push @all_cfly_files , @{$all_mis_depends{$n}{$k}} ;
        }
    }

    # remove duplicate elements
    undef %temp_hash ;
    @all_cfly_files = grep ++$temp_hash{$_} < 2, @all_cfly_files ;
    @all_read_only_libs = sort (grep /^-/ , @all_cfly_files) ;
    @all_compile_libs = grep /\.a$/ , @all_cfly_files ;
    @all_compile_libs = sort (grep !/trick_source/ , @all_compile_libs) ;
    @all_cfly_files = sort (grep !/^-|trick_source|a$/ , @all_cfly_files) ;

    # split off files by directory
    foreach ( @all_cfly_files ) {
        $sp_file = basename($_) ;
        $_ = abs_path(dirname($_)) ;

        ( $sp_dir , $src_dir ) = /(.*?)(?:\/(src))?$/ ;
        $src_dir .= "/" if ($src_dir ne "") ;
        ($base_name , $suffix) = $sp_file =~ /(.*?)([cfly]$|C$|cc$|cxx$|cpp$|c\+\+$)/ ;

        $files_by_dir{$sp_dir}{src_dir} = $src_dir ;
        push @{$files_by_dir{$sp_dir}{$suffix}} , $base_name ;
    }

    # get all of the files required by compiled libraries
    # compile all files as normal files,  we're not going to make a library anymore.
    foreach $n ( @all_compile_libs ) {
        my @local_files ;
        $sp_file = basename($n) ;
        $sp_dir = dirname($n) ;
        $sp_dir =~ s/\/object_\$\{TRICK_HOST_CPU\}?$// ;
        $sp_dir = abs_path($sp_dir) ;
        $src_dir = ( -e "$sp_dir/src" ) ? "src/" : "" ;
        $files_by_dir{$sp_dir}{src_dir} = $src_dir ;
        opendir THISDIR, "$sp_dir/$src_dir" or die "Could not open the directory $sp_dir/$src_dir";
        @local_files = grep !/^\.\.\./ , readdir THISDIR;
        @local_files = grep /\.[cfly]$|C$|cc$|cxx$|cpp$|c\+\+$/ , @local_files;
        foreach $k ( @local_files ) {
            ($base_name , $suffix) = $k =~ /(.*?)([cfly]$|C$|cc$|cxx$|cpp$|c\+\+$)/ ;
            push @{$files_by_dir{$sp_dir}{$suffix}} , $base_name ;
        }
        closedir THISDIR ;
    }

    # sort and weed out duplicate files
    foreach $k ( keys %files_by_dir ) {
        foreach $n ( qw{ c f l y h C cc cxx cpp c++} ) {
            undef %temp_hash ;
            @{$files_by_dir{$k}{$n}} = sort grep ++$temp_hash{$_} < 2, @{$files_by_dir{$k}{$n}} ;
        }
    }

    foreach $k ( sort keys %files_by_dir ) {
        foreach my $ie ( @exclude_dirs ) {
            # if file location begins with $ie (an ICG exclude dir)
            if ( $k =~ /^\Q$ie/ ) {
                delete $files_by_dir{$k} ;
                print "[33mexcluding $k from build[00m\n" ;
                last ;  # break out of loop
            }
        }
    }

    # set the "dir_num" of each directory.
    foreach $k ( sort keys %files_by_dir ) {
        $_ = $k ;
        ($files_by_dir{$k}{dir_num} = $_) =~ s#^/## ;
        $files_by_dir{$k}{dir_num} =~ s/[\/.]/_/g ;
        # if a particular directory had an override file, save that into memory
        if (open OV_FILE, "$k/makefile_overrides") {
            while ( <OV_FILE> ) {
                s/(#.*)// ;
                my ($comment) = $1 ;
                s/\$[{(]CURDIR[})]\/(\S+)/$k\/$1/g ;
                s/(?:\$[{(]CURDIR[})]\/)?(\S*)\$[{(]OBJ_DIR[})]/$k\/$1object_\${TRICK_HOST_CPU}/g ;
                s/\$[{(]CURDIR[})]/$k/g ;
                while ( s,/[^/]+/\.\.,, ) {}
                s//$comment/ ;
                if ( s/^objects\s*:\s*// ) {
                    foreach my $ext ( qw{c C cc cxx cpp CPLUSPLUS l y} ) {
                        $files_by_dir{$k}{overrides} .= "\$(MODEL_${ext}_OBJ_$files_by_dir{$k}{dir_num}): $_" ;
                    }
                }
                elsif ( s/^depend\s*:\s*// ) {
                    $files_by_dir{$k}{overrides} .= "depend_$files_by_dir{$k}{dir_num}: $_" ;
                }
                elsif ( s/([cfhy]|C|cc|cxx|cpp|CPLUSPLUS)_objects\s*:\s*// ) {
                    $files_by_dir{$k}{overrides} .= "\$(MODEL_$1_OBJ_$files_by_dir{$k}{dir_num}): $_" ;
                }
                else {
                    $files_by_dir{$k}{overrides} .= $_ ;
                }
            }
        }
    }

    chdir($make_cwd ) ;
    my $wd = abs_path(cwd()) ;
    my $dt = localtime();
    my ($trick_ver) = get_trick_version() ;
    chomp $trick_ver ;

    open MAKEFILE , ">build/Makefile_sim" or return ;

    print MAKEFILE "\
#############################################################################
# Makefile:
#    This is a makefile for maintaining the
#    '$wd'
#    simulation directory. This make file was automatically generated by trick-CP
#
#############################################################################
# Creation:
#    Author: Trick Configuration Processor - trick-CP Version $trick_ver
#    Date:   $dt
#
#############################################################################

include \${TRICK_HOME}/share/trick/makefiles/Makefile.common

S_MAIN  = \$(CURDIR)/S_main_\${TRICK_HOST_CPU}.exe
ifeq (\$(MAKECMDGOALS), test_all)
TRICK_HOST_CPU := \$(shell \$(TRICK_HOME)/bin/trick-gte TRICK_HOST_CPU)_test
S_MAIN  = \$(CURDIR)/T_main_\${TRICK_HOST_CPU}.exe
endif

LIB_DIR = \$(CURDIR)/build/lib

ifdef TRICK_VERBOSE_BUILD
PRINT_ICG =
PRINT_COMPILE =
PRINT_INC_LINK =
PRINT_EXE_LINK =
PRINT_S_DEF_DEPS =
ECHO_CMD =
else
PRINT_ICG = \@echo \"[34mRunning ICG[0m\"
PRINT_COMPILE = \@echo \"[34mCompiling[0m \$(subst \$(CURDIR)/build,build,\$<)\"
PRINT_INC_LINK = \@echo \"[34mPartial linking[0m \$(subst \$(CURDIR)/build,build,\${<D})\"
PRINT_EXE_LINK = \@echo \"[34mFinal linking[0m \$(subst \$(CURDIR)/,,\$(S_MAIN))\"
PRINT_S_DEF_DEPS = \@echo \"[34mGenerating dependencies[0m for S_define\"
ECHO_CMD = \@
ifeq (\$(MAKECMDGOALS), all)
\$(info [34mPerforming build with these compilation flags[0m)
\$(info [33mTRICK_CFLAGS = \$(TRICK_CFLAGS)[0m)
\$(info [33mTRICK_CXXFLAGS = \$(TRICK_CXXFLAGS)[0m)
endif
endif

S_OBJECT_FILES = \$(CURDIR)/build/S_source.o

\$(S_OBJECT_FILES) : | \$(LIB_DIR)\n\n" ;

    my %object_files_by_type ;
    $num_inc_objs = 0 ;
    # list out all of the source and object files
    foreach $k ( sort keys %files_by_dir ) {
        foreach my $ext ( qw{ c C cc cxx cpp c++ l y} ) {
            my $print_ext ;
            if ( $ext eq "c++" ) {
                $print_ext = "CPLUSPLUS" ;
            } else {
                $print_ext = $ext ;
            }
            if ( scalar @{$files_by_dir{$k}{$ext}} ne 0 ) {
                print MAKEFILE "MODEL_${print_ext}_OBJ_$files_by_dir{$k}{dir_num} =" ;
                foreach $f ( @{$files_by_dir{$k}{$ext}} ) {
                    print MAKEFILE " \\\n \$(CURDIR)/build$k/$files_by_dir{$k}{src_dir}$f" . "o" ;
                }
                push @{$object_files_by_type{$ext}}, "MODEL_${print_ext}_OBJ_$files_by_dir{$k}{dir_num}" ;

                print MAKEFILE "\n\n\$(MODEL_${print_ext}_OBJ_$files_by_dir{$k}{dir_num}) : | \$(CURDIR)/build$k/$files_by_dir{$k}{src_dir}$f\n" ;
                print MAKEFILE "\$(CURDIR)/build$k/$files_by_dir{$k}{src_dir}$f :\n" ;
                print MAKEFILE "\t@ mkdir -p \$\@\n\n" ;

                print MAKEFILE "\$(CURDIR)/build/lib/o${num_inc_objs}.o : \$(MODEL_${print_ext}_OBJ_$files_by_dir{$k}{dir_num})\n" ;
                print MAKEFILE "\t\$(PRINT_INC_LINK)\n" ;
                print MAKEFILE "\t\$(ECHO_CMD)cd \${<D} ; ld -Ur -o \$\@ \$(notdir \$^)\n\n" ;
                $num_inc_objs++ ;
            }
        }
        if ( scalar @{$files_by_dir{$k}{l}} ne 0 ) {
            print MAKEFILE "MODEL_clex_SRC_$files_by_dir{$k}{dir_num} =" ;
            foreach $f ( @{$files_by_dir{$k}{l}} ) {
                print MAKEFILE " \\\n \$(CURDIR)/build$k/$files_by_dir{$k}{src_dir}$f" . "clex" ;
            }
            print MAKEFILE "\n\n" ;
        }
        if ( scalar @{$files_by_dir{$k}{y}} ne 0 ) {
            print MAKEFILE "MODEL_y_c_SRC_$files_by_dir{$k}{dir_num} =" ;
            foreach $f ( @{$files_by_dir{$k}{y}} ) {
                print MAKEFILE " \\\n \$(CURDIR)/build$k/$files_by_dir{$k}{src_dir}$f" . "y.c" ;
            }
            print MAKEFILE "\n\n" ;
        }
    }

    foreach my $ext ( sort keys %object_files_by_type ) {
        my $print_ext ;
        if ( $ext eq "c++" ) {
            $print_ext = "CPLUSPLUS" ;
        } else {
            $print_ext = $ext ;
        }
        print MAKEFILE "MODEL_${print_ext}_OBJ =" ;
        foreach $f ( @{$object_files_by_type{$print_ext}} ) {
            print MAKEFILE " \\\n \$($f)" ;
        }
        print MAKEFILE "\n\n" ;
    }

    # Write out the compile rules for each type of file.
    print MAKEFILE "\${MODEL_c_OBJ} : \$(CURDIR)/build\%.o : \%.c\n" ;
    print MAKEFILE "\t\$(PRINT_COMPILE)\n\n" ;
    print MAKEFILE "\t\$(ECHO_CMD)\$(TRICK_CC) \$(TRICK_CFLAGS) -I\${<D} -I\${<D}/../include -MMD -MP -c \$< -o \$\@\n\n" ;

    print MAKEFILE "\${MODEL_C_OBJ} : \$(CURDIR)/build\%.o : \%.C\n" ;
    print MAKEFILE "\t\$(PRINT_COMPILE)\n\n" ;
    print MAKEFILE "\t\$(ECHO_CMD)\$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -I\${<D} -I\${<D}/../include -MMD -MP -c \$< -o \$\@\n\n" ;

    print MAKEFILE "\${MODEL_cc_OBJ} : \$(CURDIR)/build\%.o : \%.cc\n" ;
    print MAKEFILE "\t\$(PRINT_COMPILE)\n\n" ;
    print MAKEFILE "\t\$(ECHO_CMD)\$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -I\${<D} -I\${<D}/../include -MMD -MP -c \$< -o \$\@\n\n" ;

    print MAKEFILE "\${MODEL_cpp_OBJ} : \$(CURDIR)/build\%.o : \%.cpp\n" ;
    print MAKEFILE "\t\$(PRINT_COMPILE)\n\n" ;
    print MAKEFILE "\t\$(ECHO_CMD)\$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -I\${<D} -I\${<D}/../include -MMD -MP -c \$< -o \$\@\n\n" ;

    print MAKEFILE "\${MODEL_cxx_OBJ} : \$(CURDIR)/build\%.o : \%.cxx\n" ;
    print MAKEFILE "\t\$(PRINT_COMPILE)\n\n" ;
    print MAKEFILE "\t\$(ECHO_CMD)\$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -I\${<D} -I\${<D}/../include -MMD -MP -c \$< -o \$\@\n\n" ;

    print MAKEFILE "\${MODEL_CPLUSPLUS_OBJ} : \$(CURDIR)/build\%.o : \%.c++\n" ;
    print MAKEFILE "\t\$(PRINT_COMPILE)\n\n" ;
    print MAKEFILE "\t\$(ECHO_CMD)\$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -I\${<D} -I\${<D}/../include -MMD -MP -c \$< -o \$\@\n\n" ;

    print MAKEFILE "\${MODEL_clex_SRC} : \$(CURDIR)/build\%.clex : \%.l\n" ;
    print MAKEFILE "\t\$(LEX) -o\$\@ \$<\n\n" ;

    print MAKEFILE "\${MODEL_y_c_SRC} : \$(CURDIR)/build\%.y.c : \%.y\n" ;
    print MAKEFILE "\t\$(YACC) -o\$\@ \$<\n\n" ;

    print MAKEFILE "\${MODEL_y_OBJ} : \$(CURDIR)/build\%.o : \%.y.c\n" ;
    print MAKEFILE "\t\$(PRINT_COMPILE)\n\n" ;
    print MAKEFILE "\t\$(ECHO_CMD)cd \$(<D) ; \$(TRICK_CC) \$(TRICK_CXXFLAGS) -MMD -MP -c \${<F} -o \$\@\n\n" ;

    # Include all of the dependency files for each object code file
    print MAKEFILE "-include \$(MODEL_c_OBJ:.o=.d)\n" ;
    print MAKEFILE "-include \$(MODEL_C_OBJ:.o=.d)\n" ;
    print MAKEFILE "-include \$(MODEL_cc_OBJ:.o=.d)\n" ;
    print MAKEFILE "-include \$(MODEL_cpp_OBJ:.o=.d)\n" ;
    print MAKEFILE "-include \$(MODEL_cxx_OBJ:.o=.d)\n" ;
    print MAKEFILE "-include \$(MODEL_CPLUSPLUS_OBJ:.o=.d)\n\n" ;

    printf MAKEFILE "\n\nOBJECTS =" ;
    for( $i = 0 ; $i < $num_inc_objs ; $i++ ) {
        print MAKEFILE " \\\n\t\$(LIB_DIR)/o$i.o" ;
    }

    # print out the libraries we link
    print MAKEFILE "\n\nREAD_ONLY_LIBS = ";
    foreach ( @all_read_only_libs ) {
        print MAKEFILE " \\\n\t$_" ;
    }

    print MAKEFILE "\n\n
test_all: TRICK_CXXFLAGS += -DTRICK_UNIT_TEST
test_all: TRICK_CFLAGS += -DTRICK_UNIT_TEST

all: S_main

test_all: all

ICG:
\t\$(PRINT_ICG)
\t\$(ECHO_CMD)\${TRICK_HOME}/bin/trick-ICG -m \${TRICK_CXXFLAGS} S_source.hh

force_ICG:
\t\$(PRINT_ICG)
\t\$(ECHO_CMD)\${TRICK_HOME}/bin/trick-ICG -f -m \${TRICK_CXXFLAGS} S_source.hh

S_main : \$(S_MAIN) build/S_define.deps S_sie.resource
\t@ echo \"\"
\t@ echo \"[32m=== Simulation make complete ===[00m\"

\$(S_MAIN): \${TRICK_STATIC_LIB} \$(OBJECTS) \$(S_OBJECT_FILES)
\t\$(PRINT_EXE_LINK)
\t\$(ECHO_CMD)\$(TRICK_LD) \$(TRICK_LDFLAGS) -o \$@ \\
\t\t\$(S_OBJECT_FILES) build/lib/*.o \\
\t\t\${TRICK_USER_LINK_LIBS} \${READ_ONLY_LIBS} \\
\t\t\$(LD_WHOLE_ARCHIVE) \${TRICK_LIBS} \$(LD_NO_WHOLE_ARCHIVE)\\
\t\t\${TRICK_EXEC_LINK_LIBS}

\$(OBJECTS) : | \$(LIB_DIR)

\$(LIB_DIR) :
\t@ mkdir -p \$@

\$(CURDIR)/build/S_source.o: \$(CURDIR)/build/S_source.cpp
\t\$(PRINT_COMPILE)
\t\$(ECHO_CMD)\$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -MMD -MP -c \$\< -o \$\@

-include build/S_source.d

build/S_define.deps:
\t\$(PRINT_S_DEF_DEPS)
\t\$(ECHO_CMD)\$(TRICK_CPPC) \$(TRICK_SFLAGS) -M -MT Makefile_sim -MF build/S_define.deps -x c++ S_define

\$(CURDIR)/build/S_source.cpp S_default.dat: S_define
\t\$(PERL) \${TRICK_HOME}/bin/trick-CP -s -d

sie: S_sie.resource

S_sie.resource: \$(S_MAIN)
\t@ echo \"[34mGenerating S_sie.resource...[0m\"
\t\$(ECHO_CMD)\$(S_MAIN) sie

S_define_exp:
\t\$(TRICK_CC) -E -C -xc++ \${TRICK_SFLAGS} S_define > \$@\n\n" ;

    # write out the override files we have read in
    foreach $k ( sort keys %files_by_dir ) {
        if ( exists $files_by_dir{$k}{overrides} ) {
            print MAKEFILE "\n# Overrides from $k\n\n" ;
            print MAKEFILE "$files_by_dir{$k}{overrides}\n" ;
        }
    }

    print MAKEFILE "\n-include build/Makefile_io_src\n" ;
    print MAKEFILE "include build/Makefile_swig\n" ;
    print MAKEFILE "-include S_overrides.mk\n" ;

    close MAKEFILE ;

    # write out all of the files we used to S_library_list
    open LIB_LIST, ">build/S_library_list" or die "Could not open build/S_library_list" ;
    foreach $k ( sort keys %files_by_dir ) {
        foreach my $ext ( qw{ c C cc cxx cpp c++ y l } ) {
            if ( scalar @{$files_by_dir{$k}{$ext}} ne 0 ) {
                foreach $f ( @{$files_by_dir{$k}{$ext}} ) {
                    push @temp_array, "$k/$files_by_dir{$k}{src_dir}$f$ext" ;
                }
            }
        }
    }
    @temp_array = sort @temp_array;
    print LIB_LIST (sort join "\n" , @temp_array) , "\n" ;
    close LIB_LIST ;

    return ;
}

1;
