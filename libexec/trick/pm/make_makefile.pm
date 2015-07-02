package make_makefile ;

use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(make_makefile);

use lib $ENV{"TRICK_HOME"} . "/bin/pm" ;
use Cwd ;
use Cwd 'abs_path';
use File::Basename ;
use strict ;
use gte ;
use trick_print ;
use trick_version ;

# bm_build , bm_and and bm_or taken from www.perl.com
sub _bm_build {
    my $condition = shift;
    my $sub_pat = shift;
    my @regexp = @_;  # this MUST not be local(); need my()
    my $expr = join $condition => map { "s/\$regexp[$_]/$sub_pat/o" } (0..$#regexp);
    my $match_func = eval "sub { $expr }";
    die if $@;  # propagate $@; this shouldn't happen!
    return $match_func;
}
sub bm_and { _bm_build('&&', @_) }
sub bm_or  { _bm_build('||', @_) }

sub make_makefile($$$) {

    my ($h_ref , $sim_ref , $make_cwd ) = @_ ;
    my ($n , $f , $k , $i , $j, $m);
    my %all_mis_depends ;
    my %temp_hash ;
    my @all_h_files ;
    my @all_cfly_files ;
    my @all_read_only_libs ;
    my @all_compile_libs ;
    my %compile_libs ;
    my %files_by_dir ;
    my ( $sp_dir , $src_dir , $sp_file , $base_name , $suffix) ;
    my $num_src_files ;
    my (@temp_array , @temp_array2) ;
    my $sub_multiple ;
    my %real_extension_h ;
    my %real_extension_cpp ;

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
    foreach $n ( @all_compile_libs ) {
        my @local_files ;
        $sp_file = basename($n) ;
        $sp_dir = dirname($n) ;
        $sp_dir =~ s/\/object_\$\{TRICK_HOST_CPU\}?$// ;
        $sp_dir = abs_path($sp_dir) ;
        $src_dir = ( -e "$sp_dir/src" ) ? "src/" : "" ;
        ($base_name) = $sp_file =~ /lib(.*?)\.a$/ ;
        $files_by_dir{$sp_dir}{base_name} = $base_name ;
        $files_by_dir{$sp_dir}{dir_num} = $base_name ;
        $files_by_dir{$sp_dir}{full_name} = "$sp_dir/object_\${TRICK_HOST_CPU}/$sp_file" ;
        $files_by_dir{$sp_dir}{src_dir} = $src_dir ;
        opendir THISDIR, "$sp_dir/$src_dir" or die "Could not open the directory $sp_dir/$src_dir";
        @local_files = grep !/^\.\.\./ , readdir THISDIR;
        @local_files = grep /\.[cfly]$|C$|cc$|cxx$|cpp$|c\+\+$/ , @local_files;
        foreach $k ( @local_files ) {
            #if ( $k =~ /(C$|cc$|cxx$|cpp$|c\+\+$)/ ) {
            #    ($base_name) = $k =~ /(.*?)(C$|cc$|cxx$|cpp$|c\+\+$)/ ;
            #    $real_extension_cpp{"$sp_dir/$src_dir$base_name"} = $2 ;
            #    $suffix = "cpp" ;
            #}
            #else {
            #    ($base_name , $suffix) = $k =~ /(.*?)([cfly])$/ ;
            #}
            ($base_name , $suffix) = $sp_file =~ /(.*?)([cfly]$|C$|cc$|cxx$|cpp$|c\+\+$)/ ;
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

    $i = 0 ;
    $num_src_files = 0;

    foreach $k ( sort keys %files_by_dir ) {
        foreach my $ie ( @exclude_dirs ) {
            # if file location begins with $ie (an IGC exclude dir)
            if ( $k =~ /^\Q$ie/ ) {
                delete $files_by_dir{$k} ;
                print "[33mexcluding $k from build[00m\n" ;
                last ;  # break out of loop
            }
        }
    }

    # check to see if each directory is writable and count the number of source files
    foreach $k ( sort keys %files_by_dir ) {
        $files_by_dir{$k}{writable} = ( -w $k ) ? 1 : 0 ;
        $_ = $k ;
        ($files_by_dir{$k}{dir_num} = $_) =~ s#^/## ;
        $files_by_dir{$k}{dir_num} =~ s/[\/.]/_/g ;
        if ( ! exists $files_by_dir{$k}{full_name} ) {
            $num_src_files += $#{$files_by_dir{$k}{c}} + $#{$files_by_dir{$k}{f}} + $#{$files_by_dir{$k}{y}} + 3 ;
            $num_src_files += $#{$files_by_dir{$k}{cpp}} + 1 ;
        }
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
                    $files_by_dir{$k}{overrides} .= "model_obj_$files_by_dir{$k}{dir_num}: $_" ;
                }
                elsif ( s/^depend\s*:\s*// ) {
                    $files_by_dir{$k}{overrides} .= "depend_$files_by_dir{$k}{dir_num}: $_" ;
                }
                elsif ( s/([cfhy]|cpp)_objects\s*:\s*// ) {
                    $files_by_dir{$k}{overrides} .= "model_$1_obj_$files_by_dir{$k}{dir_num}: $_" ;
                }
                else {
                    $files_by_dir{$k}{overrides} .= $_ ;
                }
            }
        }
    }

    chdir($make_cwd ) ;
    my $wd = abs_path(cwd()) ;
    my $sim_dir_name = basename($wd) ;
    $sim_dir_name =~ s/SIM_// ;
    my $dt = localtime();
    my ($trick_ver) = get_trick_version() ;
    chomp $trick_ver ;

    open MAKEFILE , ">Makefile_sim" or return ;

    print MAKEFILE "#############################################################################
# Makefile:
#    This is a makefile for maintaining the
#    '$wd'
#    simulation directory. This make file was automatically generated by CP
#
#############################################################################
# Creation:
#    Author: Trick Configuration Processor - trick-CP Version $trick_ver
#    Date:   $dt
#
#############################################################################
#
#  To get a list of make options, type 'make help'
#
#############################################################################

include \${TRICK_HOME}/share/trick/makefiles/Makefile.common" ;

    print MAKEFILE "
S_MAIN  = \$(CURDIR)/S_main_\${TRICK_HOST_CPU}.exe
ifeq (\$(MAKECMDGOALS), test_all)
TRICK_HOST_CPU := \$(shell \$(TRICK_HOME)/bin/trick-gte TRICK_HOST_CPU)_test
S_MAIN  = \$(CURDIR)/T_main_\${TRICK_HOST_CPU}.exe
endif" ;

    print MAKEFILE "
LIB_DIR = \$(CURDIR)/lib_\${TRICK_HOST_CPU}
SIM_LIB = \$(LIB_DIR)/lib_${sim_dir_name}.a

S_OBJECT_FILES = build/S_source.o

\$(S_OBJECT_FILES) : | \$(LIB_DIR)\n\n" ;

    my %object_files_by_type ;
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
                    print MAKEFILE " \\\n build$k/$files_by_dir{$k}{src_dir}$f" . "o" ;
                }
                push @{$object_files_by_type{$ext}}, "MODEL_${print_ext}_OBJ_$files_by_dir{$k}{dir_num}" ;

                print MAKEFILE "\n\n\$(MODEL_${print_ext}_OBJ_$files_by_dir{$k}{dir_num}) : | build$k/$files_by_dir{$k}{src_dir}$f\n" ;
                print MAKEFILE "build$k/$files_by_dir{$k}{src_dir}$f :\n" ;
                print MAKEFILE "\tmkdir -p \$\@\n" ;
            }
        }
        if ( scalar @{$files_by_dir{$k}{l}} ne 0 ) {
            print MAKEFILE "MODEL_clex_SRC_$files_by_dir{$k}{dir_num} =" ;
            foreach $f ( @{$files_by_dir{$k}{l}} ) {
                print MAKEFILE " \\\n build$k/$files_by_dir{$k}{src_dir}$f" . "clex" ;
            }
        }
        if ( scalar @{$files_by_dir{$k}{y}} ne 0 ) {
            print MAKEFILE "MODEL_y_c_SRC_$files_by_dir{$k}{dir_num} =" ;
            foreach $f ( @{$files_by_dir{$k}{y}} ) {
                print MAKEFILE " \\\n build$k/$files_by_dir{$k}{src_dir}$f" . "y.c" ;
            }
        }
    }
    print MAKEFILE "\n\n" ;

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
    }
    print MAKEFILE "\n\n" ;

    # Write out the compile rules for each type of file.
    print MAKEFILE "\${MODEL_c_OBJ} : build\%.o : \%.c\n" ;
    print MAKEFILE "\tcd \$(<D) ; \$(TRICK_CC) \$(TRICK_CXXFLAGS) -MMD -MP -c \${<F} -o \$(CURDIR)/\$\@\n\n" ;

    print MAKEFILE "\${MODEL_C_OBJ} : build\%.o : \%.C\n" ;
    print MAKEFILE "\tcd \$(<D) ; \$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -MMD -MP -c \${<F} -o \$(CURDIR)/\$\@\n\n" ;

    print MAKEFILE "\${MODEL_cc_OBJ} : build\%.o : \%.cc\n" ;
    print MAKEFILE "\tcd \$(<D) ; \$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -MMD -MP -c \${<F} -o \$(CURDIR)/\$\@\n\n" ;

    print MAKEFILE "\${MODEL_cpp_OBJ} : build\%.o : \%.cpp\n" ;
    print MAKEFILE "\tcd \$(<D) ; \$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -MMD -MP -c \${<F} -o \$(CURDIR)/\$\@\n\n" ;

    print MAKEFILE "\${MODEL_cxx_OBJ} : build\%.o : \%.cxx\n" ;
    print MAKEFILE "\tcd \$(<D) ; \$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -MMD -MP -c \${<F} -o \$(CURDIR)/\$\@\n\n" ;

    print MAKEFILE "\${MODEL_CPLUSPLUS_OBJ} : build\%.o : \%.c++\n" ;
    print MAKEFILE "\tcd \$(<D) ; \$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -MMD -MP -c \${<F} -o \$(CURDIR)/\$\@\n\n" ;

    print MAKEFILE "\${MODEL_clex_SRC} : build\%.clex : \%.l\n" ;
    print MAKEFILE "\t\$(LEX) -o\$\@ \$\<\n\n" ;

    print MAKEFILE "\${MODEL_y_c_SRC} : build\%.y.c : \%.y\n" ;
    print MAKEFILE "\t\$(YACC) -o\$\@ \$\?\n\n" ;

    print MAKEFILE "\${MODEL_y_OBJ} : build\%.o : \%.y.c\n" ;
    print MAKEFILE "\tcd \$(<D) ; \$(TRICK_CC) \$(TRICK_CXXFLAGS) -MMD -MP -c \${<F} -o \$(CURDIR)/\$\@\n\n" ;

    print MAKEFILE "\nDEFAULT_DATA_C_OBJ =" ;
    if ( exists $$sim_ref{def_data_c} ) {
        foreach my $d ( sort keys %{$$sim_ref{def_data_c}} ) {
            print MAKEFILE " \\\n\t$wd/lib_\${TRICK_HOST_CPU}/$$sim_ref{def_data_c}{$d}{file_name}" . ".o" ;
        }
    }

    print MAKEFILE "\n\nDEFAULT_DATA_CPP_OBJ =" ;
    if ( exists $$sim_ref{def_data_cpp} ) {
        foreach my $d ( sort keys %{$$sim_ref{def_data_cpp}} ) {
            print MAKEFILE " \\\n\t$wd/lib_\${TRICK_HOST_CPU}/$$sim_ref{def_data_cpp}{$d}{file_name}" . ".o" ;
        }
    }
    print MAKEFILE "\n\nDEFAULT_DATA_OBJECTS = \$(DEFAULT_DATA_C_OBJ) \$(DEFAULT_DATA_CPP_OBJ)\n\n" ;

    printf MAKEFILE "\n\nOBJECTS =" ;
    for( $i = 0 ; $i < $num_src_files ; $i++ ) {
        print MAKEFILE " \\\n\t\$(LIB_DIR)/o$i.o" ;
    }

    # print out the libraries we build
    print MAKEFILE "\n\nBUILD_USER_LIBS = ";
    foreach ( sort keys %files_by_dir ) {
        if ( exists $files_by_dir{$_}{full_name} ) {
            my $temp_str = $files_by_dir{$_}->{full_name} ;
            $temp_str =~ s/object_\${TRICK_HOST_CPU}/lib_\${TRICK_HOST_CPU}/ ;
            print MAKEFILE " \\\n\t$temp_str" ;
        }
    }

    # print out the libraries we link
    print MAKEFILE "\n\nREAD_ONLY_LIBS = ";
    foreach ( @all_read_only_libs ) {
        print MAKEFILE " \\\n\t$_" ;
    }

    print MAKEFILE "\n\n
test_all: TRICK_CXXFLAGS += -DTRICK_UNIT_TEST
test_all: TRICK_CFLAGS += -DTRICK_UNIT_TEST\n\n" ;

    print MAKEFILE "all: S_main\n\n" ;

    print MAKEFILE "test_all: all\n\n" ;

    print MAKEFILE "ICG:\n" ;
    print MAKEFILE "\t\${TRICK_HOME}/bin/trick-ICG -m \${TRICK_CXXFLAGS} S_source.hh\n" ;
    print MAKEFILE "force_ICG:\n" ;
    print MAKEFILE "\t\${TRICK_HOME}/bin/trick-ICG -f -m \${TRICK_CXXFLAGS} S_source.hh\n" ;

    print MAKEFILE "
S_main : objects \$(LIB_DIR) \$(S_MAIN) build/S_define.deps S_sie.resource
\t@ echo \"\"
\t@ echo \"[32m=== Simulation make complete ===[00m\"

objects : \$(OBJECTS) build_user_lib

build_user_lib : \$(BUILD_USER_OBJ_DIRS) \$(BUILD_USER_LIBS)

" ;

    if ( $num_src_files > 1 ) {
        print MAKEFILE "\
\$(S_MAIN): \$(BUILD_USER_LIBS) \${TRICK_STATIC_LIB} \$(SIM_LIB) \$(S_OBJECT_FILES) \$(SIM_LIBS)
\t\$(TRICK_LD) \$(TRICK_LDFLAGS) -o \$@ \\
\t\t\$(S_OBJECT_FILES)\\
\t\t\$(LD_WHOLE_ARCHIVE) \$(SIM_LIB) \$(LD_NO_WHOLE_ARCHIVE)\\
\t\t\${BUILD_USER_LIBS} \${TRICK_USER_LINK_LIBS} \${READ_ONLY_LIBS} \\
\t\t\$(LD_WHOLE_ARCHIVE) \$(SIM_LIBS) \${TRICK_LIBS} \$(LD_NO_WHOLE_ARCHIVE)\\
\t\t\$(HDF5_LIB) \${TRICK_EXEC_LINK_LIBS}" ;
    } else {
        print MAKEFILE "\
\$(S_MAIN): \$(BUILD_USER_LIBS) \${TRICK_STATIC_LIB} \$(S_OBJECT_FILES) \$(SIM_LIBS)
\t\$(TRICK_LD) \$(TRICK_LDFLAGS) -o \$@ \\
\t\tbuild/*.o\\
\t\t\${BUILD_USER_LIBS} \${TRICK_USER_LINK_LIBS} \${READ_ONLY_LIBS} \\
\t\t\$(LD_WHOLE_ARCHIVE) \$(SIM_LIBS) \${TRICK_LIBS} \$(LD_NO_WHOLE_ARCHIVE)\\
\t\t\$(HDF5_LIB) \${TRICK_EXEC_LINK_LIBS}" ;
    }

    print MAKEFILE "\n
\$(OBJECTS) : | \$(LIB_DIR)

\$(LIB_DIR) :
\t@ mkdir -p \$@
" ;

    print MAKEFILE "
build/S_source.o: S_source.cpp
\t\$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -MMD -MP -c \$\< -o \$\@
#\t\@\${TRICK_HOME}/\$(LIBEXEC)/trick/depend_objs S_source.cpp

-include build/S_source.dep

build/S_define.deps:
\t\$(TRICK_CPPC) \$(TRICK_SFLAGS) -M -MT Makefile_sim -MF build/S_define.deps -x c++ S_define
" ;

    print MAKEFILE "
S_source.cpp S_default.dat: S_define
\t\$(PERL) \${TRICK_HOME}/bin/trick-CP -s -d

sie: S_sie.resource

S_sie.resource: \$(S_MAIN)
\t@ echo \"Generating S_sie.resource...\"
\t\$(S_MAIN) sie

S_define_exp:
\t\$(TRICK_CC) -E -C -xc++ \${TRICK_SFLAGS} S_define > \$@\n\n" ;

    # write out the the rules to make the libraries
    print MAKEFILE "\n#LIBS\n\n"  ;
    print MAKEFILE "\$(SIM_LIB) : \$(OBJECTS) \$(DEFAULT_DATA_OBJECTS)\n" ;
    print MAKEFILE "\t@ echo \"[36mCreating libraries...[00m\"\n" ;
    print MAKEFILE "\t\@ rm -rf \$\@\n" ;
    print MAKEFILE "\tar crs \$\@ \$(LIB_DIR)/*.o || touch \$\@\n\n" ;

    $m = 0 ;
    foreach $k ( sort keys %files_by_dir ) {
        foreach my $ext ( qw{ c C cc cxx cpp c++ y l } ) {
            if ( scalar @{$files_by_dir{$k}{$ext}} ne 0 ) {
                foreach $f ( @{$files_by_dir{$k}{$ext}} ) {
                    print MAKEFILE "\$(LIB_DIR)/o$m.o : build$k/$files_by_dir{$k}{src_dir}${f}o\n" ;
                    print MAKEFILE "\tln -f -s ../\$< \$@\n" ;
                    $m++;
                }
            }
        }
    }

    print MAKEFILE "\n\n#DEFAULT_DATA_C_OBJ\n\n" ;
    foreach my $d ( sort keys %{$$sim_ref{def_data_c}} ) {
        print MAKEFILE "$wd/lib_\${TRICK_HOST_CPU}/$$sim_ref{def_data_c}{$d}{file_name}" , ".o" ,
         " : $wd/Default_data/$$sim_ref{def_data_c}{$d}{file_name}" , ".c\n";
        print MAKEFILE "\tcd $wd/Default_data ; \$(TRICK_CC) \$(TRICK_CFLAGS) -c \${\@F:.o=.c} -o \$\@\n" ;
    }

    print MAKEFILE "\n\n#DEFAULT_DATA_CPP_OBJ\n\n" ;
    foreach my $d ( sort keys %{$$sim_ref{def_data_cpp}} ) {
        print MAKEFILE "$wd/lib_\${TRICK_HOST_CPU}/$$sim_ref{def_data_cpp}{$d}{file_name}" , ".o" ,
         " : $wd/Default_data/$$sim_ref{def_data_cpp}{$d}{file_name}" , ".cpp\n";
        print MAKEFILE "\tcd $wd/Default_data ; \$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -c \${\@F:.o=.cpp} -o \$\@\n" ;
    }

    # write out the override files we have read in
    foreach $k ( sort keys %files_by_dir ) {
        if ( exists $files_by_dir{$k}{overrides} ) {
            print MAKEFILE "\n# Overrides from $k\n\n" ;
            print MAKEFILE "$files_by_dir{$k}{overrides}\n" ;
        }
    }

    print MAKEFILE "\n-include Makefile_io_src\n" ;
    print MAKEFILE "\ninclude Makefile_swig\n" ;
    print MAKEFILE "\n-include S_overrides.mk\n" ;

    close MAKEFILE ;

    if ( ! -e "build" ) {
        mkdir "build", 0775 ;
    }

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

    # Add the default data files to S_library_list
    my %temp_hash ;
    foreach my $obj ( @{$$sim_ref{objs}} ) {
        foreach my $str ( @{$$obj{structs}} ) {
            foreach my $l ( @{$$str{def_data}} ) {
                $temp_hash{$l} = 1 if ( $l !~ /trick_source/ ) ;
            }
        }
    }
    push @temp_array , (sort keys %temp_hash) ;

    print LIB_LIST (sort join "\n" , @temp_array) , "\n" ;
    close LIB_LIST ;

    return ;
}

1;
