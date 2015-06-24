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

my %icg_files_hash ;

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
    my @top_h_files ;
    my @all_depends ;
    my ($n , $f , $k , $i , $j, $m);
    my (%all_icg_depends) = %{$$sim_ref{all_icg_depends}} ;
    my %all_mis_depends ;
    my %temp_hash ;
    my @all_h_files ;
    my @all_cfly_files ;
    my @all_read_only_libs ;
    my @all_compile_libs ;
    my %compile_libs ;
    my %files_by_dir ;
    my ($writable_dirs , $readonly_dirs) ;
    my ( $sp_dir , $src_dir , $sp_file , $base_name , $suffix) ;
    my ( $print_me_l, $print_me_c, $print_me_clex) ;
    my ($lib_size , $num_src_files , $num_icg_files , $num_lib , $num_io_lib );
    my (@temp_array , @temp_array2) ;
    my $print_me ;
    my %module_files_hash ;
    my @inc_paths ;
    my @s_inc_paths ;
    my @valid_inc_paths ;
    my $subme ;
    my $sub_multiple ;
    my %real_extension_h ;
    my %real_extension_cpp ;

    if ( exists $$sim_ref{all_mis_depends} ) {
        %all_mis_depends = %{$$sim_ref{all_mis_depends}} ;
    }

    @inc_paths = $ENV{"TRICK_CFLAGS"} =~ /-I\s*(\S+)/g ;     # get include paths from TRICK_CFLAGS
    push @inc_paths , ("$ENV{\"TRICK_HOME\"}/trick_source" , "../include") ;
    @valid_inc_paths = () ;
    foreach (@inc_paths) {
        $_ = q($_) ;
        push @valid_inc_paths , $_ if ( -e $_ ) ;
    }
    @inc_paths = @valid_inc_paths ;

    $sub_multiple = bm_or "" , @inc_paths ;

    @s_inc_paths = $ENV{"TRICK_SFLAGS"} =~ /-I\s*(\S+)/g ;     # get include paths from TRICK_CFLAGS

    if ( exists $$sim_ref{sim_libraries} ) {
        for ($i = 0 ; $i < scalar @{$$sim_ref{sim_libraries}} ; $i++ ) {
            my ( $found ) = 0 ;
            foreach my $inc ( @s_inc_paths ) {
                if ( -e "$inc/@{$$sim_ref{sim_libraries}}[$i]" ) {
                    $found = 1 ;
                    @{$$sim_ref{sim_libraries}}[$i] = "$inc/@{$$sim_ref{sim_libraries}}[$i]" ;
                    open SLIB , "@{$$sim_ref{sim_libraries}}[$i]/S_library_list" ;
                    while ( <SLIB> ) {
                        chomp ;
                        $$sim_ref{sim_lib_files}{$_} = 1 ;
                    }
                    last ;
                }
            }
            if ( $found == 0 ) {
                trick_print($$sim_ref{fh}, "Could not find sim_lib @{$$sim_ref{sim_libraries}}[$i]\n" , "title_red" , $$sim_ref{args}{v}) ;
                exit ;
            }
        }
    }

    open SLIB_EXT, ">.S_library_list_ext" ;
    foreach my $f ( sort keys %{$$sim_ref{sim_lib_files}} ) {
        print SLIB_EXT "$f\n" ;
    }
    close SLIB_EXT ;

    # make a list of all the header files required by this sim
    foreach $n ( @$h_ref ) {
        push @all_h_files , $n ;
        foreach $k ( keys %{$all_icg_depends{$n}} ) {
            push @all_h_files , $k ;
            push @all_h_files , @{$all_icg_depends{$n}{$k}} ;
        }
    }

    # remove duplicate elements
    undef %temp_hash ;
    @all_h_files = grep ++$temp_hash{$_} < 2, @all_h_files ;
    # remove headers found in trick_source and ${TRICK_HOME}/include/trick
    @all_h_files = sort (grep !/trick_source/ , @all_h_files) ;
    @all_h_files = sort (grep !/$ENV{"TRICK_HOME"}\/include\/trick\// , @all_h_files) ;

    my @exclude_dirs ;
    my @icg_exclude_dirs ;

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

    @icg_exclude_dirs = split /:/ , $ENV{"TRICK_ICG_EXCLUDE"};
    # See if there are any elements in the icg_exclude_dirs array
    if (scalar @icg_exclude_dirs) {
        @icg_exclude_dirs = sort(@icg_exclude_dirs );
        # Error check - delete any element that is null
        # (note: sort forced all blank names to front of array
        @icg_exclude_dirs = map { s/(^\s+|\s+$)//g ; $_ } @icg_exclude_dirs ;
        while ( not length @icg_exclude_dirs[0] ) {
            # Delete an element from the left side of an array (element zero)
            shift @icg_exclude_dirs ;
        }
        @icg_exclude_dirs = map { (-e $_) ? abs_path($_) : $_ } @icg_exclude_dirs ;
    }

    # split off the header files by directory
    foreach ( @all_h_files ) {

        if (exists $$sim_ref{sim_lib_files}{$_}) {
            trick_print($$sim_ref{fh}, "CP skipping $_ (sim_lib)\n" , "normal_yellow" , $$sim_ref{args}{v}) ;
            next ;
        }
        if (exists $$sim_ref{icg_no}{$_}) {
            trick_print($$sim_ref{fh}, "CP skipping $_ (ICG No found)\n" , "normal_yellow" , $$sim_ref{args}{v}) ;
            next ;
        }

        my ( $continue ) = 1 ;

        foreach my $ie ( @icg_exclude_dirs ) {
            my $dum = $_ ;
            # if file location begins with $ie (an IGC exclude dir)
            if ( $dum =~ s/^\Q$ie// ) {
                if ( $dum =~ /^\// ) {
                    trick_print($$sim_ref{fh}, "CP skipping $f (ICG exclude dir $ie)\n" , "normal_yellow" , $$sim_ref{args}{v}) ;
                    $continue = 0 ;
                    last ;  # break out of loop
                }
            }
        }
        next if ( $continue == 0 ) ;

        ( $sp_dir , $src_dir , $sp_file ) = /(.*?)(?:\/(include))?\/([^\/]+)$/ ;
        $src_dir .= "/" if ($src_dir ne "") ;
        ($base_name = $sp_file) =~ s/hC?$|H$|hh$|h\+\+$|hxx$// ;
        if ( $sp_file =~ /(H$|hh$|h\+\+$|hxx$)/ ) {
            $real_extension_h{"$sp_dir/$src_dir$base_name"} = $1 ;
        }
        else {
            $real_extension_h{"$sp_dir/$src_dir$base_name"} = "h" ;
        }
        $files_by_dir{$sp_dir}{include_dir} = $src_dir ;
        push @{$files_by_dir{$sp_dir}{h}} , $base_name ;
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

        if (exists $$sim_ref{sim_lib_files}{"$_/$sp_file"}) {
            trick_print($$sim_ref{fh}, "CP skipping $_ (sim_lib)\n" , "normal_yellow" , $$sim_ref{args}{v}) ;
            next ;
        }

        ( $sp_dir , $src_dir ) = /(.*?)(?:\/(src))?$/ ;
        $src_dir .= "/" if ($src_dir ne "") ;
        ($base_name = $sp_file) =~ s/[cfly]$|C$|cc$|cxx$|cpp$|c\+\+$// ;

        # save off the extension used by a c++ file
        if ( $sp_file =~ /(C$|cc$|cxx$|cpp$|c\+\+$)/ ) {
            $real_extension_cpp{"$sp_dir/$src_dir$base_name"} = $1 ;
            $suffix = "cpp" ;
        }
        else {
            ($suffix) = $sp_file =~ /([cfly])$/ ;
        }
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
            if ( $k =~ /(C$|cc$|cxx$|cpp$|c\+\+$)/ ) {
                ($base_name) = $k =~ /(.*?)(C$|cc$|cxx$|cpp$|c\+\+$)/ ;
                $real_extension_cpp{"$sp_dir/$src_dir$base_name"} = $2 ;
                $suffix = "cpp" ;
            }
            else {
                ($base_name , $suffix) = $k =~ /(.*?)([cfly])$/ ;
            }
            push @{$files_by_dir{$sp_dir}{$suffix}} , $base_name ;
        }
        closedir THISDIR ;
    }

    # sort and weed out duplicate files
    foreach $k ( keys %files_by_dir ) {
        foreach $n ( qw{ c f l y h cpp} ) {
            undef %temp_hash ;
            @{$files_by_dir{$k}{$n}} = sort grep ++$temp_hash{$_} < 2, @{$files_by_dir{$k}{$n}} ;
        }
    }

    $i = 0 ;
    $num_src_files = 0;
    $num_icg_files = 0;

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
        &$sub_multiple ;
        ($files_by_dir{$k}{dir_num} = $_) =~ s#^/## ;
        $files_by_dir{$k}{dir_num} =~ s/[\/.]/_/g ;
        if ( ! exists $files_by_dir{$k}{full_name} ) {
            $num_src_files += $#{$files_by_dir{$k}{c}} + $#{$files_by_dir{$k}{f}} + $#{$files_by_dir{$k}{y}} + 3 ;
            $num_src_files += $#{$files_by_dir{$k}{cpp}} + 1 ;
            $num_icg_files += $#{$files_by_dir{$k}{h}} + 1;
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
    my ($trick_ver, $trick_thread) = get_trick_version() ;
    my ($trick_major , $trick_minor ) ;
    ($trick_major , $trick_minor ) = $trick_ver =~ /(\d+)\.([^\.]+)/ ;
    $trick_thread =~ s/\d+\.// ;
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
#    Author: Trick Configuration Processor - CP Version $trick_ver-$trick_thread
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
OBJECT_DIR = \$(CURDIR)/object_\${TRICK_HOST_CPU}

S_OBJECT_FILES = \$(OBJECT_DIR)/S_source.o \\
 \$(OBJECT_DIR)/io_S_source.o \\
 \$(OBJECT_DIR)/class_map.o \\
 \$(OBJECT_DIR)/enum_map.o

\$(S_OBJECT_FILES) : | \$(LIB_DIR) \$(OBJECT_DIR)
\n" ;

    # print some comments for all of the directories used
    foreach ( sort keys %files_by_dir ) {
        print MAKEFILE "#MODEL_DIR_$files_by_dir{$_}{dir_num} = $_\n"
    }

    my $all_model_files ;
    my $all_io_cpp_files ;
    # list out all of the source and object files
    foreach $k ( sort keys %files_by_dir ) {
        if ( $#{$files_by_dir{$k}{c}} ne -1 ) {
            print MAKEFILE "\n\nMODEL_C_SRC_$files_by_dir{$k}{dir_num} =" ;
            foreach $f ( @{$files_by_dir{$k}{c}} ) {
                print MAKEFILE " \\\n\t$k/$files_by_dir{$k}{src_dir}$f" . "c" ;
            }
            print MAKEFILE "\n\nMODEL_C_OBJ_$files_by_dir{$k}{dir_num} =" ;
            foreach $f ( @{$files_by_dir{$k}{c}} ) {
                print MAKEFILE " \\\n\t$k/object_\${TRICK_HOST_CPU}/$f" . "o" ;
            }
            if ( ! exists $$sim_ref{dir_src_only}{$k} ) {
                $all_model_files .= " \\\n\t\$(MODEL_C_SRC_$files_by_dir{$k}{dir_num})" ;
            }
        }
        if ( $#{$files_by_dir{$k}{cpp}} ne -1 ) {
            print MAKEFILE "\n\nMODEL_CPP_SRC_$files_by_dir{$k}{dir_num} =" ;
            foreach $f ( @{$files_by_dir{$k}{cpp}} ) {
                print MAKEFILE " \\\n\t$k/$files_by_dir{$k}{src_dir}$f" . $real_extension_cpp{"$k/$files_by_dir{$k}{src_dir}$f"} ;
            }
            print MAKEFILE "\n\nMODEL_CPP_OBJ_$files_by_dir{$k}{dir_num} =" ;
            foreach $f ( @{$files_by_dir{$k}{cpp}} ) {
                print MAKEFILE " \\\n\t$k/object_\${TRICK_HOST_CPU}/$f" . "o" ;
            }
            if ( ! exists $$sim_ref{dir_src_only}{$k} ) {
                $all_model_files .= " \\\n\t\$(MODEL_CPP_SRC_$files_by_dir{$k}{dir_num})" ;
            }
        }
        if ( $#{$files_by_dir{$k}{h}} ne -1 ) {
            print MAKEFILE "\n\nMODEL_IO_SRC_$files_by_dir{$k}{dir_num} =" ;
            foreach $f ( @{$files_by_dir{$k}{h}} ) {
                my $path = $k ;
                $path .= "/include" if ( -e "$k/include" ) ;
                my $final_ext = "cpp" ;
                print MAKEFILE " \\\n\t$k/io_src/io_$f" . $final_ext ;
            }

            print MAKEFILE "\n\nMODEL_IO_OBJ_$files_by_dir{$k}{dir_num} =" ;
            foreach $f ( @{$files_by_dir{$k}{h}} ) {
                print MAKEFILE " \\\n\t$k/object_\${TRICK_HOST_CPU}/io_$f" . "o" ;
            }

            print MAKEFILE "\n\nMODEL_IO_CPP_OBJ_$files_by_dir{$k}{dir_num} =" ;
            foreach $f ( @{$files_by_dir{$k}{h}} ) {
                my $path = $k ;
                $path .= "/include" if ( -e "$k/include" ) ;
                print MAKEFILE " \\\n\t$k/object_\${TRICK_HOST_CPU}/io_$f" . "o" ;
            }
            $all_io_cpp_files .= " \\\n\t\$(MODEL_IO_CPP_OBJ_$files_by_dir{$k}{dir_num})" ;

            print MAKEFILE "\n\nMODEL_HEADERS_$files_by_dir{$k}{dir_num} =" ;
            foreach $f ( @{$files_by_dir{$k}{h}} ) {
                my $path = $k ;
                $path .= "/include" if ( -e "$k/include" ) ;
                my $ext = $real_extension_h{"$path/$f"} ;
                print MAKEFILE " \\\n\t$path/$f" . $ext ;
            }
            if ( ! exists $$sim_ref{dir_src_only}{$k} ) {
                $all_model_files .= " \\\n\t\$(MODEL_HEADERS_$files_by_dir{$k}{dir_num})" ;
            }
        }

        if ( $#{$files_by_dir{$k}{y}} ne -1 ) {
            print MAKEFILE "\n\nMODEL_YACC_SRC_$files_by_dir{$k}{dir_num} =" ;
            foreach $f ( @{$files_by_dir{$k}{y}} ) {
                print MAKEFILE " \\\n\t$k/$files_by_dir{$k}{src_dir}$f" . "c" ;
            }
            print MAKEFILE "\n\nMODEL_YACC_OBJ_$files_by_dir{$k}{dir_num} =" ;
            foreach $f ( @{$files_by_dir{$k}{y}} ) {
                print MAKEFILE " \\\n\t$k/object_\${TRICK_HOST_CPU}/$f" . "o" ;
            }
            if ( ! exists $$sim_ref{dir_src_only}{$k} ) {
                $all_model_files .= " \\\n\t\$(MODEL_YACC_SRC_$files_by_dir{$k}{dir_num})" ;
            }
        }

        if ( $#{$files_by_dir{$k}{l}} ne -1 ) {
            print MAKEFILE "\n\nMODEL_LEX_SRC_$files_by_dir{$k}{dir_num} =" ;
            foreach $f ( @{$files_by_dir{$k}{l}} ) {
                print MAKEFILE " \\\n\t$k/$files_by_dir{$k}{src_dir}$f" . "clex" ;
            }
            if ( ! exists $$sim_ref{dir_src_only}{$k} ) {
                $all_model_files .= " \\\n\t\$(MODEL_LEX_SRC_$files_by_dir{$k}{dir_num})" ;
            }
        }
    }

    print MAKEFILE "\n\nDEFAULT_DATA_C_OBJ =" ;
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

    print MAKEFILE "\n\nMODEL_ALL_SRC =$all_model_files\n" ;

    print MAKEFILE "\n\nMODEL_ALL_IO_CPP_OBJ =$all_io_cpp_files\n" ;

    printf MAKEFILE "\n\nOBJECTS =" ;
    for( $i = 0 ; $i < $num_src_files ; $i++ ) {
        print MAKEFILE " \\\n\t\$(LIB_DIR)/o$i.o" ;
    }

    # only count to $num_icg_files - 1 because we will skip io_S_source.cpp
    print MAKEFILE "\n\nIO_OBJECTS =" ;
    for ( $i = 0 ; $i < $num_icg_files - 1 ; $i++ ) {
        print MAKEFILE " \\\n\t\$(LIB_DIR)/i$i.o" ;
    }

    # print out the object dirs if we can write to them.
    print MAKEFILE "\n\nMODEL_OBJ_DIRS =" ;
    foreach $k ( sort keys %files_by_dir ) {
        if ($files_by_dir{$k}{"writable"}) {
            print MAKEFILE " \\\n\t$k/object_\${TRICK_HOST_CPU}" ;
        }
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

    # print out the sim directory libraries
    print MAKEFILE "\n\nSIM_LIBS = ";
    foreach my $l ( @{$$sim_ref{sim_libraries}} ) {
        my $lib_name = $l ;
        $lib_name =~ s/.*SIM_// ;
        print MAKEFILE " \\\n\tlib_\${TRICK_HOST_CPU}/lib_${lib_name}.a" ;
    }

    print MAKEFILE "\n\n\
TRICK_IO_CXXFLAGS := \\
   -Wno-invalid-offsetof \\
   -Wno-old-style-cast \\
   -Wno-write-strings \\
   -Wno-unused-variable

ifeq (\$(IS_CC_CLANG), 0)
 GCCVERSIONGTEQ48 := \$(shell perl -e 'printf \"\%d\\n\", (\$(GCC_MAJOR)>4)||((\$(GCC_MAJOR)==4)&&(\$(GCC_MINOR)>=8)) ;' )
 ifeq (\$(GCCVERSIONGTEQ48), 1)
  TRICK_IO_CXXFLAGS += -Wno-unused-but-set-variable
 endif
endif

test_all: TRICK_CXXFLAGS += -DTRICK_UNIT_TEST
test_all: TRICK_CFLAGS += -DTRICK_UNIT_TEST

\$(OBJECT_DIR)/io_S_source.o: TRICK_CXXFLAGS += \$(TRICK_IO_CXXFLAGS)\n\n" ;

    print MAKEFILE "all: S_main\n\n" ;

    print MAKEFILE "test_all: all\n\n" ;

    print MAKEFILE "ICG:\n" ;
    print MAKEFILE "\t\${TRICK_HOME}/bin/trick-ICG -m \${TRICK_CXXFLAGS} S_source.hh\n" ;
    print MAKEFILE "force_ICG:\n" ;
    print MAKEFILE "\t\${TRICK_HOME}/bin/trick-ICG -f -m \${TRICK_CXXFLAGS} S_source.hh\n" ;

    print MAKEFILE "
S_main : objects \$(LIB_DIR) \$(S_MAIN) \$(OBJECT_DIR)/S_define.deps S_sie.resource
\t@ echo \"\"
\t@ echo \"[32m=== Simulation make complete ===[00m\"

objects : \$(MODEL_OBJ_DIRS) model_objects build_user_lib

build_user_lib : \$(BUILD_USER_OBJ_DIRS) \$(BUILD_USER_LIBS)

" ;

    if ( $num_src_files + $num_icg_files > 1 ) {
        print MAKEFILE "\
\$(S_MAIN): \$(BUILD_USER_LIBS) \${TRICK_STATIC_LIB} \$(LIB_DIR)/lib_${sim_dir_name}.a \$(S_OBJECT_FILES) \$(SIM_LIBS)
\t\$(TRICK_LD) \$(TRICK_LDFLAGS) -o \$@ \\
\t\t\$(S_OBJECT_FILES)\\
\t\t\$(LD_WHOLE_ARCHIVE) \$(LIB_DIR)/lib_${sim_dir_name}.a \$(LD_NO_WHOLE_ARCHIVE)\\
\t\t\${BUILD_USER_LIBS} \${TRICK_USER_LINK_LIBS} \${READ_ONLY_LIBS} \\
\t\t\$(LD_WHOLE_ARCHIVE) \$(SIM_LIBS) \${TRICK_LIBS} \$(LD_NO_WHOLE_ARCHIVE)\\
\t\t\$(HDF5_LIB) \${TRICK_EXEC_LINK_LIBS}" ;
    } else {
        print MAKEFILE "\
\$(S_MAIN): \$(BUILD_USER_LIBS) \${TRICK_STATIC_LIB} \$(S_OBJECT_FILES) \$(SIM_LIBS)
\t\$(TRICK_LD) \$(TRICK_LDFLAGS) -o \$@ \\
\t\t\$(OBJECT_DIR)/*.o\\
\t\t\${BUILD_USER_LIBS} \${TRICK_USER_LINK_LIBS} \${READ_ONLY_LIBS} \\
\t\t\$(LD_WHOLE_ARCHIVE) \$(SIM_LIBS) \${TRICK_LIBS} \$(LD_NO_WHOLE_ARCHIVE)\\
\t\t\$(HDF5_LIB) \${TRICK_EXEC_LINK_LIBS}" ;
    }

    print MAKEFILE "\n
\$(OBJECTS) \$(IO_OBJECTS) : | \$(LIB_DIR) \$(OBJECT_DIR)

\$(LIB_DIR) :
\t@ mkdir -p \$@
" ;

    print MAKEFILE "
\$(OBJECT_DIR)/S_source.o: S_source.cpp | \$(OBJECT_DIR)
\t\$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -MMD -MP -c S_source.cpp -o \$(OBJECT_DIR)/S_source.o
\t\@\${TRICK_HOME}/\$(LIBEXEC)/trick/depend_objs S_source.cpp

\$(OBJECT_DIR)/class_map.o: \$(CURDIR)/io_src/class_map.cpp | \$(OBJECT_DIR)
\t\$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -MMD -MP -c \$(CURDIR)/io_src/class_map.cpp -o \$(OBJECT_DIR)/class_map.o
\t\@\${TRICK_HOME}/\$(LIBEXEC)/trick/depend_objs class_map.cpp

\$(OBJECT_DIR)/enum_map.o: \$(CURDIR)/io_src/enum_map.cpp | \$(OBJECT_DIR)
\t\$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -MMD -MP -c \$(CURDIR)/io_src/enum_map.cpp -o \$(OBJECT_DIR)/enum_map.o
\t\@\${TRICK_HOME}/\$(LIBEXEC)/trick/depend_objs enum_map.cpp

-include \$(OBJECT_DIR)/S_source.dep
-include \$(OBJECT_DIR)/class_map.dep
-include \$(OBJECT_DIR)/enum_map.dep

\$(OBJECT_DIR)/S_define.deps: | \$(OBJECT_DIR)
\t\$(TRICK_CPPC) \$(TRICK_SFLAGS) -M -MT Makefile_sim -MF \$(OBJECT_DIR)/S_define.deps -x c++ S_define

#\$(OBJECT_DIR):
#\t@ mkdir -p \$@
" ;

    print MAKEFILE "
S_source.cpp S_default.dat: S_define
\t\$(PERL) \${TRICK_HOME}/bin/CP -s -d

sie: S_sie.resource

S_sie.resource: \$(S_MAIN)
\t@ echo \"Generating S_sie.resource...\"
\t\$(S_MAIN) sie

S_define_exp:
\t\$(TRICK_CC) -E -C -xc++ \${TRICK_SFLAGS} S_define > \$@

help:
\t@ echo -e \"\\n\\
Simulation make options:\\n\\
    make                 - Makes everything: S_main, S_sie.resource,\\n\\
\\t\\t\\tand S_default.dat\\n\\
    make S_sie.resource  - Rebuilds the S_sie.resource file.\\n\\
    make S_source.cpp    - Rebuilds the S_source.cpp file.\\n\\
    make clean           - Removes all auto-generated files in\\n\\
\\t\\t\\tsimulation directory\\n\\
    make clean_model_io_src - Removes all io_src code in the model directories\\n\\
    make clean_model_io_obj - Removes all io_obj code in the model directories\\n\\
    make clean_obj       - Removes all object code in the model directories\\n\\
    make depend          - Performs make_depend for the entire simulation\\n\\
    make real_clean      - Does a make clean and clean_obj and removes\\n\\
\\t\\t\\tall io_src files, flex/bison files and \\n\\
\\t\\t\\tTrick built libraries \\n\\
    make spotless        - Does everthing in real_clean and\\n\\
\\t\\t\\tremoves all S_mains and lib directories\\n\"

tidy:
\t-rm -rf \$(LIB_DIR) Default_data
\t-rm -rf lib
\t-rm -f S_default.dat S_document.xml S_main_\${TRICK_HOST_CPU}.exe
\t-rm -f S_rif_\${TRICK_HOST_CPU}.exe S_sie.resource S_sie_*.xml S_structures
\t-rm -f DP_Product/DP_rt_frame DP_Product/DP_rt_itimer
\t-rm -f DP_Product/DP_rt_jobs DP_Product/DP_rt_timeline DP_Product/DP_mem_stats
\t-rm -f $$sim_ref{args}{o}
\t-rm -f Makefile_sim MAKE_out S_library_* .S_library_* .icg_no_found
\t-rm -f S_source.cpp S_source.hh
\t-rm -rf io_src xml
\t-rm -rf lib_* object_* S_main* T_main* tags

clean: tidy clean_obj clean_model_io_src clean_lex_yacc_src

real_clean: clean

spotless: clean
\t@ echo \"Removed all S_mains\"\n" ;

    foreach $k ( sort keys %files_by_dir ) {
        if ($files_by_dir{$k}{"writable"}) {
            if ( ! exists $$sim_ref{dir_src_only}{$k} ) {
                print MAKEFILE "\t-cd $k ; rm -rf xml io_src object_*\n" ;
            }
        }
    }

    print MAKEFILE "\t@ echo \"Removed all xml and io_src dirs\"

clean_obj: clean_model_c_obj clean_model_cpp_obj clean_model_io_obj clean_lex_yacc_obj

" ;

    # write out the clean rules
    print MAKEFILE "\nclean_model_c_obj:\n" ;
    foreach $k ( sort keys %files_by_dir ) {
        if ( ($#{$files_by_dir{$k}{c}} ne -1 ) and $files_by_dir{$k}{"writable"} eq 1 ) {
            if ( ! exists $$sim_ref{dir_src_only}{$k} ) {
                print MAKEFILE "\trm -f \$(MODEL_C_OBJ_$files_by_dir{$k}{dir_num})\n" ;
                if ( exists $files_by_dir{$k}{full_name} ) {
                    print MAKEFILE "\trm -f $files_by_dir{$k}{full_name}\n" ;
                }
            }
        }
    }
    print MAKEFILE "\t@ echo \"Model c_obj files have been removed\"\n" ;

    print MAKEFILE "\nclean_model_cpp_obj:\n" ;
    foreach $k ( sort keys %files_by_dir ) {
        if ( ($#{$files_by_dir{$k}{cpp}} ne -1 ) and $files_by_dir{$k}{"writable"} eq 1 ) {
            if ( ! exists $$sim_ref{dir_src_only}{$k} ) {
                print MAKEFILE "\t@ rm -f \$(MODEL_CPP_OBJ_$files_by_dir{$k}{dir_num})\n" ;
                if ( exists $files_by_dir{$k}{full_name} ) {
                    print MAKEFILE "\t@ rm -f $files_by_dir{$k}{full_name}\n" ;
                }
            }
        }
    }
    print MAKEFILE "\t@ echo \"Model cpp_obj files have been removed\"\n" ;

    print MAKEFILE "\nclean_model_io_src:\n" ;
    foreach $k ( sort keys %files_by_dir ) {
        if ( ($#{$files_by_dir{$k}{h}} ne -1 ) and $files_by_dir{$k}{"writable"}) {
            if ( ! exists $$sim_ref{dir_src_only}{$k} ) {
                print MAKEFILE "\t@ rm -f \$(MODEL_IO_SRC_$files_by_dir{$k}{dir_num})\n" ;
            }
        }
    }
    print MAKEFILE "\t@ echo \"Model io_src files have been removed\"\n" ;

    print MAKEFILE "\nclean_model_io_obj:\n" ;
    foreach $k ( sort keys %files_by_dir ) {
        if ( ($#{$files_by_dir{$k}{h}} ne -1 ) and $files_by_dir{$k}{"writable"}) {
            if ( ! exists $$sim_ref{dir_src_only}{$k} ) {
                print MAKEFILE "\t@ rm -f \$(MODEL_IO_OBJ_$files_by_dir{$k}{dir_num})\n" ;
            }
        }
    }
    print MAKEFILE "\t@ echo \"Model io_obj files have been removed\"\n" ;

    print MAKEFILE "\nclean_lex_yacc_obj:\n" ;
    foreach $k ( sort keys %files_by_dir ) {
        if ( ($#{$files_by_dir{$k}{y}} ne -1 ) and $files_by_dir{$k}{"writable"}) {
            if ( ! exists $$sim_ref{dir_src_only}{$k} ) {
                print MAKEFILE "\t@ rm -f \$(MODEL_YACC_OBJ_$files_by_dir{$k}{dir_num})\n" ;
            }
        }
    }
    print MAKEFILE "\t@ echo \"Model lex/yacc obj files have been removed\"\n"  ;

    print MAKEFILE "\nclean_lex_yacc_src:\n" ;
    foreach $k ( sort keys %files_by_dir ) {
        if ( ($#{$files_by_dir{$k}{y}} ne -1 ) and $files_by_dir{$k}{"writable"}) {
            if ( ! exists $$sim_ref{dir_src_only}{$k} ) {
                print MAKEFILE "\t@ rm -f \$(MODEL_LEX_SRC_$files_by_dir{$k}{dir_num})\n" ;
                print MAKEFILE "\t@ rm -f \$(MODEL_YACC_SRC_$files_by_dir{$k}{dir_num})\n" ;
            }
        }
    }
    print MAKEFILE "\t@ echo \"Model lex/yacc src files have been removed\"\n"  ;

    my $num_src_dirs = 0 ;
    foreach $k ( keys %files_by_dir ) {
        $num_src_dirs++ if ( ! exists $$sim_ref{dir_src_only}{$k} ) ;
    }


    print MAKEFILE "\nlib_\${TRICK_HOST_CPU}/libtrick.so:\n" ;
    print MAKEFILE "\tln -s -f \$(TRICK_LIB_DIR)/libtrick.so lib_\${TRICK_HOST_CPU}/libtrick.so\n" ;

    #print MAKEFILE "\nlib_\${TRICK_HOST_CPU}/libhdf5.so:\n" ;
    #print MAKEFILE "\tln -s -f \$(TRICK_LIB_DIR)/libhdf5.so lib_\${TRICK_HOST_CPU}/libhdf5.so\n" ;

    # write out the the rules to make the libraries
    print MAKEFILE "\n#LIBS\n\n"  ;
    print MAKEFILE "\$(LIB_DIR)/lib_${sim_dir_name}.a : \$(OBJECTS) \$(IO_OBJECTS) \$(DEFAULT_DATA_OBJECTS)\n" ;
    print MAKEFILE "\t@ echo \"[36mCreating libraries...[00m\"\n" ;
    print MAKEFILE "\t\@ rm -rf \$\@\n" ;
    print MAKEFILE "\tar crs \$\@ \$(LIB_DIR)/*.o || touch \$\@\n\n" ;

    # print out the sim directory libraries
    foreach my $l ( @{$$sim_ref{sim_libraries}} ) {
        my $lib_name = $l ;
        $lib_name =~ s/.*SIM_// ;
        print MAKEFILE "\nlib_\${TRICK_HOST_CPU}/lib_${lib_name}.a: $l/lib_\${TRICK_HOST_CPU}/lib_${lib_name}.a\n" ;
        print MAKEFILE "\tln -s -f $l/lib_\${TRICK_HOST_CPU}/lib_${lib_name}.a lib_\${TRICK_HOST_CPU}/lib_${lib_name}.a\n" ;

        print MAKEFILE "$l/lib_\${TRICK_HOST_CPU}/lib_${lib_name}.a: $l/lib_\${TRICK_HOST_CPU}\n" ;
        print MAKEFILE "\t@ echo \"\033[31mSIM lib $l not compiled... touching empty \$@\033[0m\"\n" ;
        print MAKEFILE "\t@ touch \$@\n" ;

        print MAKEFILE "$l/lib_\${TRICK_HOST_CPU}:\n" ;
        print MAKEFILE "\t@ mkdir -p \$\@\n" ;
    }

    print MAKEFILE "\napocalypse: spotless\n" ;
    foreach my $l ( @{$$sim_ref{sim_libraries}} ) {
        print MAKEFILE "\t- \$(MAKE) -C $l spotless\n" ;
    }
    print MAKEFILE "\t\@echo \"\033[31mI love the smell of napalm in the morning\033[0m\"\n" ;
    print MAKEFILE "\n" ;

    # write out the rules to link objects to lib_${TRICK_HOST_CPU}
    $m = 0 ;
    foreach $k ( sort keys %files_by_dir ) {
        next if ( $k =~ /\/io_src/ ) ;
        if ( ! exists $files_by_dir{$k}{full_name} ) {
            @temp_array = () ;
            push @temp_array ,  @{$files_by_dir{$k}{c}} , @{$files_by_dir{$k}{cpp}} ,
            @{$files_by_dir{$k}{f}} , @{$files_by_dir{$k}{y}} ;
            foreach $n ( sort @temp_array ) {
                print MAKEFILE "\$(LIB_DIR)/o$m.o : $k/object_\${TRICK_HOST_CPU}/${n}o\n" ;
                print MAKEFILE "\tln -f -s $k/object_\${TRICK_HOST_CPU}/${n}o \$@\n" ;
                $m++;
            }
        }
    }

    # write out the rules to link io_objects to lib_${TRICK_HOST_CPU}
    $m = 0;
    foreach $k ( sort keys %files_by_dir ) {
        if ( ! exists $files_by_dir{$k}{full_name} ) {
            foreach $n ( @{$files_by_dir{$k}{h}} ) {
                next if ( $n =~ /S_source./ ) ;
                print MAKEFILE "\$(LIB_DIR)/i$m.o : $k/object_\${TRICK_HOST_CPU}/io_${n}o\n" ;
                print MAKEFILE "\tln -f -s $k/object_\${TRICK_HOST_CPU}/io_${n}o \$@\n" ;
                $m++;
            }
        }
    }

    print MAKEFILE "\n#MODEL_OBJ_DIRS\n\n" ;
    foreach $k ( sort keys %files_by_dir ) {
        if ($files_by_dir{$k}{"writable"}) {
            print MAKEFILE "$k/object_\${TRICK_HOST_CPU} :\n" ;
            print MAKEFILE "\t@ mkdir -p \$\@\n" ;
        }
    }

    $m = 0;
    foreach $k ( sort keys %files_by_dir ) {
        if ( ! exists $files_by_dir{$k}{full_name} ) {
            foreach $n ( @{$files_by_dir{$k}{h}} ) {
                print MAKEFILE "lib/i$m.o: $k/object_\${TRICK_HOST_CPU}/io_$n" , "o\n" ;
                print MAKEFILE "\tln -f -s \$? \$\@\n" ;
                $m++;
            }
        }
    }

    # write out the rules to compile a directory
    print MAKEFILE "\n\nmodel_objects :" ;
    foreach $k ( sort keys %files_by_dir ) {
        print MAKEFILE " \\\n\tmodel_obj_$files_by_dir{$k}{dir_num}" ;
    }

    foreach $k ( sort keys %files_by_dir ) {
        print MAKEFILE "\n\nmodel_obj_$files_by_dir{$k}{dir_num} :" ;
        foreach ( "c" , "cpp" , "f" , "h" , "y" ) {
            if ( $#{$files_by_dir{$k}{$_}} ne -1 ) {
                print MAKEFILE " model_$_" . "_obj_$files_by_dir{$k}{dir_num}" ;
            }
        }
    }
    foreach $k ( sort keys %files_by_dir ) {
        foreach ( "c" , "cpp" , "f" , "y" ) {
            if ( $#{$files_by_dir{$k}{$_}} ne -1 ) {
                print MAKEFILE "\n\nmodel_$_" . "_obj_$files_by_dir{$k}{dir_num} :" ;
                foreach $f ( @{$files_by_dir{$k}{$_}} ) {
                    print MAKEFILE " \\\n\t$k/object_\$\{TRICK_HOST_CPU\}/$f" . "o" ;
                }
            }
        }
        if ( $#{$files_by_dir{$k}{h}} ne -1 ) {
            print MAKEFILE "\n\nmodel_h_obj_$files_by_dir{$k}{dir_num} :" ;
            foreach $f ( @{$files_by_dir{$k}{h}} ) {
                print MAKEFILE " \\\n\t$k/object_\$\{TRICK_HOST_CPU\}/io_$f" . "o" ;
            }
        }
    }

    # write out the rules to compile each individual file
    foreach $k ( sort keys %files_by_dir ) {
        if ( ! exists $$sim_ref{dir_src_only}{$k} ) {
            write_rules($k , \%{$files_by_dir{$k}}, $sim_ref, \%real_extension_h, \%real_extension_cpp ) ;
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

    print MAKEFILE "\ninclude Makefile_swig\n" ;
    print MAKEFILE "\n-include S_overrides.mk\n" ;

    close MAKEFILE ;

    # write out all of the files we used to S_library_list
    open LIB_LIST, ">S_library_list" or die "Could not open S_library_list" ;
    foreach $k ( sort keys %files_by_dir ) {
        foreach (@{$files_by_dir{$k}{h}}) {
            my $path = $k ;
            $path .= "/include" if ( -e "$k/include" ) ;
            my $ext = $real_extension_h{"$path/$_"} ;
            push @temp_array , "$k/$files_by_dir{$k}{include_dir}$_" . "$ext" ;
        }
        foreach (@{$files_by_dir{$k}{c}}) {
            push @temp_array , "$k/$files_by_dir{$k}{src_dir}$_" . "c" ;
        }
        foreach (@{$files_by_dir{$k}{cpp}}) {
            my $ext = $real_extension_cpp{"$k/$files_by_dir{$k}{src_dir}$_"} ;
            push @temp_array , "$k/$files_by_dir{$k}{src_dir}$_" . "$ext" ;
        }
        foreach (@{$files_by_dir{$k}{f}}) {
            push @temp_array , "$k/$files_by_dir{$k}{src_dir}$_" . "f" ;
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

sub write_rules ($$$$) {

    my ($k , $dir_info_ref, $sim_ref, $re_h_ref, $re_cpp_ref ) = @_ ;
    my (%dir_info) = %$dir_info_ref ;
    my ($n ) ;

    # This block of code is pretty similar.  Each part writes out the individual compile line
    # for a source file

    print MAKEFILE "\n\n#MODEL_C_OBJ_$dir_info{dir_num}\n\n" ;
    foreach $n ( @{$dir_info{c}} ) {
        print MAKEFILE "$k/object_\${TRICK_HOST_CPU}/$n" . "o : $k/$dir_info{src_dir}$n" . "c\n" ;
        if ( $dir_info{writable} ) {
            print MAKEFILE "\tcd $k/$dir_info{src_dir} ; \$(TRICK_CC) \$(TRICK_CFLAGS) -MMD -MP -c \${\@F:.o=.c} -o \$\@\n" ;
            print MAKEFILE "\t\@cd $k/$dir_info{src_dir} ; \${TRICK_HOME}/\$(LIBEXEC)/trick/depend_objs ${n}c\n" ;

            print MAKEFILE "-include $k/object_\${TRICK_HOST_CPU}/${n}dep\n" ;
        }
        else {
            print MAKEFILE "\t@ echo \"[31m\$\@ is out of date and is in a read-only directory... aborting[00m\"\n" ;
            print MAKEFILE "\t@ exit 1\n" ;
        }
    }
    print MAKEFILE "\n\n#MODEL_CPP_OBJ_$dir_info{dir_num}\n\n" ;
    foreach $n ( @{$dir_info{cpp}} ) {
        my $ext = $$re_cpp_ref{"$k/$dir_info{src_dir}$n"} ;
        print MAKEFILE "$k/object_\${TRICK_HOST_CPU}/$n" . "o : $k/$dir_info{src_dir}$n$ext\n" ;
        if ( $dir_info{writable} ) {
            #print MAKEFILE "\t\@echo \"Compiling $k/$dir_info{src_dir}[33m\${\@F:.o=.$ext}[00m\"\n" ;
            print MAKEFILE "\tcd $k/$dir_info{src_dir} ; \$(TRICK_CPPC) \$(TRICK_CXXFLAGS) -MMD -MP -c \${\@F:.o=.$ext} -o \$\@\n" ;
            print MAKEFILE "\t\@cd $k/$dir_info{src_dir} ; \${TRICK_HOME}/\$(LIBEXEC)/trick/depend_objs $n$ext\n" ;

            print MAKEFILE "\n-include $k/object_\${TRICK_HOST_CPU}/${n}dep\n\n" ;
        }
        else {
            print MAKEFILE "\t@ echo \"[31m\$\@ is out of date and is in a read-only directory... aborting[00m\"\n" ;
            print MAKEFILE "\t@ exit 1\n" ;
        }
    }
    print MAKEFILE "\n\n#MODEL_LEX_YACC_OBJ_$dir_info{dir_num}\n\n" ;
    foreach $n ( @{$dir_info{y}} ) {
        print MAKEFILE "$k/$dir_info{src_dir}$n" . "clex : $k/$dir_info{src_dir}$n" . "l\n" ;
        if ( $dir_info{writable} ) {
            print MAKEFILE "\t\$(LEX) -o\$\@ \$\?\n\n"
        }
        else {
            print MAKEFILE "\t@ echo \"[31m\$\@ is out of date and is in a read-only directory... aborting[00m\"\n" ;
            print MAKEFILE "\t@ exit 1\n" ;
        }
        print MAKEFILE "$k/$dir_info{src_dir}$n" . "c : $k/$dir_info{src_dir}$n" . "y\n" ;
        if ( $dir_info{writable} ) {
            print MAKEFILE "\t\$(YACC) -o\$\@ \$\?\n\n"
        }
        else {
            print MAKEFILE "\t@ echo \"[31m\$\@ is out of date and is in a read-only directory... aborting[00m\"\n" ;
            print MAKEFILE "\t@ exit 1\n" ;
        }
        print MAKEFILE "$k/object_\${TRICK_HOST_CPU}/$n" . "o : $k/$dir_info{src_dir}$n" .
                       "c $k/$dir_info{src_dir}$n" . "clex\n" ;
        if ( $dir_info{writable} ) {
            print MAKEFILE "\tcd $k/$dir_info{src_dir} ; \$(TRICK_CC) \$(TRICK_CFLAGS) -c \${\@F:.o=.c} -o \$\@\n\n"
        }
        else {
            print MAKEFILE "\t@ echo \"[31m\$\@ is out of date and is in a read-only directory... aborting[00m\"\n" ;
            print MAKEFILE "\t@ exit 1\n" ;
        }
    }
    print MAKEFILE "\n\n#MODEL_IO_OBJ_$dir_info{dir_num}\n\n" ;
    foreach $n ( @{$dir_info{h}} ) {
        my $full_name =  "$k/$dir_info{include_dir}${n}h" ;
        my $path = $k ;
        $path .= "/include" if ( -e "$k/include" ) ;
        my $ext = $$re_h_ref{"$path/$n"} ;
        my $extension = "cpp" ;
        print MAKEFILE "$k/object_\${TRICK_HOST_CPU}/io_$n" . "o : $k/io_src/io_$n$extension\n";
        if ( $dir_info{writable} ) {
            print MAKEFILE "\tcd $k ; \$(TRICK_CPPC) \$(TRICK_CXXFLAGS) \$(TRICK_IO_CXXFLAGS) -c $k/io_src/\${\@F:.o=.$extension} -o \$\@\n\n" ;
        }
        else {
            print MAKEFILE "\t@ echo \"[31m\$\@ is out of date and is in a read-only directory... aborting[00m\"\n" ;
            print MAKEFILE "\t@ exit 1\n" ;
        }
    }

    # if this directory is to be a library, addd a rule to make it.
    if ( exists $dir_info{full_name} ) {
        print MAKEFILE "$dir_info{full_name} : \${MODEL_C_OBJ_$dir_info{dir_num}} \${MODEL_CPP_OBJ_$dir_info{dir_num}} " ,
                           "\${MODEL_YACC_OBJ_$dir_info{dir_num}} " , "\${MODEL_IO_OBJ_$dir_info{dir_num}}\n" ;
        print MAKEFILE "\tar crs \$\@ \$?
\t@ if [ \"\${TRICK_RANLIB}\" != \"\" ] ; then \\
\t     echo \${TRICK_RANLIB} $dir_info{full_name} ; \\
\t     \${TRICK_RANLIB} $dir_info{full_name} ; \\
\t  fi\n\n" ;
    }
}

1;
