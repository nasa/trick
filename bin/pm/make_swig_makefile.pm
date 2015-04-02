package make_swig_makefile ;

use Exporter ();
use trick_version ;
use File::Path ;

@ISA = qw(Exporter);
@EXPORT = qw(make_swig_makefile);

use lib $ENV{"TRICK_HOME"} . "/bin/pm" ;
use File::Basename ;
use gte ; 
use trick_print ;
use Cwd ;
use Cwd 'abs_path';
use strict ;

sub make_swig_makefile($$$) {

    my ($h_ref , $sim_ref , $make_cwd ) = @_ ;
    my ($n , $f , $k , $i , $j, $m);
    my (%all_icg_depends) = %{$$sim_ref{all_icg_depends}} ;
    my %temp_hash ;
    my @all_h_files ;
    my (@temp_array , @temp_array2) ; 
    my ($ii) ;
    my ($swig_sim_dir, $swig_src_dir) ;
    my (%py_module_map) ;
    my @exclude_dirs ;
    my @swig_exclude_dirs ;

    my (@include_paths) ;
    my (@s_inc_paths) ;
    my (@defines) ;
    my ($version, $thread, $year) ;
    my ($swig_module_i, $swig_module_source, $py_wrappers) ;
    my $s_source_full_path = abs_path("S_source.hh") ;
    my $s_library_swig = ".S_library_swig" ;
    my $s_library_swig_ext = ".S_library_swig_ext" ;
    my ( $dev, $inode ) ;

    ( $dev, $inode ) = stat($s_source_full_path) ;
    my $s_source_uniq_id = "${dev}_$inode" ;

    ($version, $thread) = get_trick_version() ;
    ($year) = $version =~ /^(\d+)/ ;
    (my $cc = gte("TRICK_CC")) =~ s/\n// ;
    @include_paths = $ENV{"TRICK_CFLAGS"} =~ /(-I\s*\S+)/g ; # get include paths from TRICK_CFLAGS
    push @include_paths , ("-I".$ENV{"TRICK_HOME"}."/trick_source" , "-I../include") ;

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

    @swig_exclude_dirs = split /:/ , $ENV{"TRICK_SWIG_EXCLUDE"};
    # See if there are any elements in the swig_exclude_dirs array
    if (scalar @swig_exclude_dirs) {
        @swig_exclude_dirs = sort(@swig_exclude_dirs );
        # Error check - delete any element that is null
        # (note: sort forced all blank names to front of array
        @swig_exclude_dirs = map { s/(^\s+|\s+$)//g ; $_ } @swig_exclude_dirs ;
        while ( not length @swig_exclude_dirs[0] ) {
            # Delete an element from the left side of an array (element zero)
            shift @swig_exclude_dirs ;
        }
        @swig_exclude_dirs = map { (-e $_) ? abs_path($_) : $_ } @swig_exclude_dirs ;
    }

    # If there were no directories listed in TRICK_SWIG_EXCLUDE then copy the ones from ICG_EXCLUDE.
    if ( scalar @swig_exclude_dirs == 0 ) {
        @swig_exclude_dirs = split /:/ , $ENV{"TRICK_ICG_EXCLUDE"};
        # See if there are any elements in the swig_exclude_dirs array
        if (scalar @swig_exclude_dirs) {
            @swig_exclude_dirs = sort(@swig_exclude_dirs );
            # Error check - delete any element that is null
            # (note: sort forced all blank names to front of array
            @swig_exclude_dirs = map { s/(^\s+|\s+$)//g ; $_ } @swig_exclude_dirs ;
            while ( not length @swig_exclude_dirs[0] ) {
                # Delete an element from the left side of an array (element zero)
                shift @swig_exclude_dirs ;
            }
            @swig_exclude_dirs = map { (-e $_) ? abs_path($_) : $_ } @swig_exclude_dirs ;
        }
    }

    @defines = $ENV{"TRICK_CFLAGS"} =~ /(-D\S+)/g ;       # get defines from TRICK_CFLAGS
    push @defines , "-DTRICK_VER=$year" ;
    push @defines , "-DSWIG" ;

    @s_inc_paths = $ENV{"TRICK_SFLAGS"} =~ /-I\s*(\S+)/g ;     # get include paths from TRICK_CFLAGS

    # The sim_libraries are full paths at this point
    if ( exists $$sim_ref{sim_libraries} ) {
        for ($i = 0 ; $i < scalar @{$$sim_ref{sim_libraries}} ; $i++ ) {
            open SLIB , "@{$$sim_ref{sim_libraries}}[$i]/$s_library_swig" ;
            while ( <SLIB> ) {
                chomp ;
                $$sim_ref{sim_lib_swig_files}{$_} = 1 ;
            }
        }
    }

    open SLIB_EXT, ">$s_library_swig_ext" ;
    foreach my $f ( sort keys %{$$sim_ref{sim_lib_swig_files}} ) {
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
    @all_h_files = sort (grep !/trick_source/ , @all_h_files) ;

    $swig_sim_dir = abs_path("trick") ;

    if ( ! -e $swig_sim_dir ) {
        mkdir $swig_sim_dir, 0775 ;
    }

    $swig_src_dir = abs_path("swig") ;

    if ( ! -e $swig_src_dir ) {
        mkdir $swig_src_dir, 0775 ;
    }

    undef @temp_array2 ;
    foreach $n (sort @$h_ref) {
        if ( $n !~ /trick_source/ ) {
            undef @temp_array ;
            if ( !exists  $all_icg_depends{$n}{$n} ) {
                @temp_array = ($n) ;
            }
            push @temp_array , keys %{$all_icg_depends{$n}} ;
            @temp_array = grep !/\/trick_source\// , @temp_array ;
            @temp_array = grep !/C$/ , @temp_array ;

            # check to see if the parent directory of each file is writable.
            # If it isn't, then don't add it to the list of files to requiring ICG
            foreach my $f ( @temp_array ) {
                $f = abs_path(dirname($f)) . "/" . basename($f) ;
                if (exists $$sim_ref{icg_no}{$f}) {
                    trick_print($$sim_ref{fh}, "CP(swig) skipping $f (ICG No found)\n" , "normal_yellow" , $$sim_ref{args}{v}) ;
                    next ;
                }
                my ($continue) = 1 ;
                foreach my $ie ( @swig_exclude_dirs ) {
                    # if file location begins with $ie (an IGC exclude dir)
                    if ( $f =~ /^\Q$ie/ ) {
                        trick_print($$sim_ref{fh}, "CP(swig) skipping $f (ICG exclude dir $ie)\n" , "normal_yellow" , $$sim_ref{args}{v}) ;
                        $continue = 0 ;
                        last ;  # break out of loop
                    }
                }
                next if ( $continue == 0 ) ;
                my $temp_str ;
                $temp_str = dirname($f) ;
                $temp_str =~ s/\/include$// ;
                if ( -w $temp_str ) {
                    push @temp_array2 , $f ;
                }
            }
        }
    }

    undef %temp_hash ;
    @temp_array2 = grep ++$temp_hash{$_} < 2, @temp_array2 ;

    # Get the list header files from the compiler to compare to what get_headers processed.
    open FILE_LIST, "$cc -MM -DSWIG @include_paths @defines S_source.hh |" ;
    my $dir ;
    $dir = dirname($s_source_full_path) ;
    while ( <FILE_LIST> ) {
        next if ( /^#/ or /^\s+\\/ ) ;
        my $word ;
        foreach $word ( split ) {
            next if ( $word eq "\\" or $word =~ /o:/ ) ;
            if ( $word !~ /^\// and $dir ne "\/" ) {
                $word = "$dir/$word" ;
            }
            $word = abs_path(dirname($word)) . "/" . basename($word) ;
            # filter out system headers that are missed by the compiler -MM flag
            next if ( $word =~ /^\/usr\/include/) ;
            #print "gcc found $word\n" ;
            $$sim_ref{gcc_all_includes}{$word} = 1 ;
        }
    }

    # Only use header files that the compiler says are included.
    undef %temp_hash ;
    foreach my $k ( @temp_array2 ) {
        if ( exists $$sim_ref{gcc_all_includes}{$k} and
             $k !~ /$ENV{TRICK_HOME}\/trick_source/ ) {
            $temp_hash{$k} = 1 ;
        }
    }
    @temp_array2 = sort keys %temp_hash ;
    #print map { "$_\n" } @temp_array2 ;

    open MAKEFILE , ">Makefile_swig" or return ;

    print MAKEFILE "# SWIG rules\n" ;
    print MAKEFILE "SWIG_CFLAGS := -I../include \${PYTHON_INCLUDES} -Wno-shadow -Wno-missing-field-initializers\n" ;
    print MAKEFILE "ifeq (\$(IS_CC_CLANG), 1)\n" ;
    print MAKEFILE " SWIG_CFLAGS += -Wno-self-assign -Wno-sometimes-uninitialized\n" ;
    print MAKEFILE "endif\n" ;
    print MAKEFILE "SWIG_MODULE_OBJECTS = " ;
    for ( $ii = 0 ; $ii < scalar @temp_array2 ; $ii++ ) {

        my ($continue) = 1 ;
        foreach my $ie ( @exclude_dirs ) {
            # if file location begins with $ie (an IGC exclude dir)
            if ( @temp_array2[$ii] =~ /^\Q$ie/ ) {
                $continue = 0 ;
                last ;  # break out of loop
            }
        }
        next if ( $continue == 0 ) ;

        if ( ! exists $$sim_ref{sim_lib_swig_files}{@temp_array2[$ii]} ) {
            if ( @temp_array2[$ii] !~ /S_source/ )  {
                print MAKEFILE "\\\n\t\$(LIB_DIR)/p${ii}.o" ;
            }
        }
    }

    print MAKEFILE "\n\n" ;
    print MAKEFILE "SIM_SWIG_OBJECTS = \\\n" ;
    print MAKEFILE "\t\$(OBJECT_DIR)/init_swig_modules.o\\\n" ;
    print MAKEFILE "\t\$(OBJECT_DIR)/py_S_source.o\\\n" ;
    print MAKEFILE "\t\$(OBJECT_DIR)/py_top.o\n" ;
    print MAKEFILE "S_OBJECT_FILES += \$(SIM_SWIG_OBJECTS)\n\n" ;
    print MAKEFILE "ALL_SWIG_OBJECTS = \\\n" ;
    print MAKEFILE "\t\$(SWIG_MODULE_OBJECTS)\\\n" ;
    print MAKEFILE "\t\$(SIM_SWIG_OBJECTS)\n\n" ;

    print MAKEFILE "\$(ALL_SWIG_OBJECTS) : | \$(LIB_DIR) \$(OBJECT_DIR)\n\n" ;

    print MAKEFILE "# SWIG_PY_OBJECTS is a convienince list to modify rules for compilation\n" ;
    print MAKEFILE "SWIG_PY_OBJECTS =" ;
    foreach my $f ( @temp_array2 ) {
        my ($continue) = 1 ;
        foreach my $ie ( @exclude_dirs ) {
            # if file location begins with $ie (an IGC exclude dir)
            if ( $f =~ /^\Q$ie/ ) {
                $continue = 0 ;
                $ii++ ;
                last ;  # break out of loop
            }
        }
        next if ( $continue == 0 ) ;

        my ($swig_dir, $swig_object_dir , $swig_module_dir , $swig_file_only) ;
        my ($swig_f) = $f ;
        $swig_f =~ s/(?:include\/)?([^\/]*)(?:\.h|\.H|\.hh|\.h\+\+|\.hxx)$/swig\/$1/ ;
        $swig_dir = dirname($swig_f) ;
        ($swig_object_dir = $swig_dir) =~ s/swig$/object_\${TRICK_HOST_CPU}/ ;
        $swig_file_only = basename($swig_f) ;
        print MAKEFILE" \\\n\t$swig_object_dir/py_${swig_file_only}.o" ;
    }
    print MAKEFILE"\n\n" ;

    print MAKEFILE "convert_swig:\n" ;
    print MAKEFILE "\t\${TRICK_HOME}/bin/convert_swig \${TRICK_CONVERT_SWIG_FLAGS} S_source.hh\n" ;
    print MAKEFILE "\n\n" ;

    my %swig_dirs ;
    my %python_modules ;
    $ii = 0 ;
    foreach my $f ( @temp_array2 ) {

        my ($continue) = 1 ;
        foreach my $ie ( @exclude_dirs ) {
            # if file location begins with $ie (an IGC exclude dir)
            if ( $f =~ /^\Q$ie/ ) {
                $continue = 0 ;
                $ii++ ;
                last ;  # break out of loop
            }
        }
        next if ( $continue == 0 ) ;

        my ($swig_dir, $swig_object_dir , $swig_module_dir , $swig_file_only) ;
        my ($swig_f) = $f ;

        if ( $$sim_ref{python_module}{$f} ne "" ) {
            #print "[31mpython module for $f = $$sim_ref{python_module}{$f}[0m\n" ;
            my ($temp_str) = $$sim_ref{python_module}{$f} ;
            $temp_str =~ s/\./\//g ;
            $swig_module_dir = "$temp_str/" ;
            $temp_str =~ $$sim_ref{python_module}{$f} ;
            $temp_str =~ s/\\/\./g ;
            push @{$python_modules{$temp_str}} , $f ;
        } else {
            $swig_module_dir = "" ;
            push @{$python_modules{"root"}} , $f ;
        }

        ( $dev, $inode ) = stat($f) ;
        my $uniq_id = "${dev}_$inode" ;

        # check if .sm file was accidentally ##included instead of #included
        if ( rindex($swig_f,".sm") != -1 ) {
           trick_print($$sim_ref{fh}, "\nError: $swig_f should be in a #include not a ##include  \n\n", "title_red", $$sim_ref{args}{v}) ;
           exit -1 ;
        }
        $swig_f =~ s/(?:include\/)?([^\/]*)(?:\.h|\.H|\.hh|\.h\+\+|\.hxx)$/swig\/$1.i/ ;
        $swig_dir = dirname($swig_f) ;
        ($swig_object_dir = $swig_dir) =~ s/swig$/object_\${TRICK_HOST_CPU}/ ;
        $swig_file_only = basename($swig_f) ;
        $swig_file_only =~ s/\.i$// ;
        $swig_dirs{$swig_dir} = 1 ;

        $swig_module_i .= "\\\n    $swig_f" ;
        $swig_module_source .= "\\\n    $swig_dir/py_${swig_file_only}.cpp\\\n    $swig_dir/m${uniq_id}.py" ;
        $py_wrappers .= " \\\n    $swig_sim_dir/${swig_module_dir}m${uniq_id}.py" ;

        if ( ! exists $$sim_ref{sim_lib_swig_files}{$f} ) {
            print MAKEFILE "$swig_object_dir/py_${swig_file_only}.o : $swig_f\n" ;
            print MAKEFILE "\t\$(SWIG) \$(TRICK_INCLUDE) \$(TRICK_DEFINES) \$(TRICK_VERSIONS) -c++ -python -includeall -ignoremissing -w201,303,362,389,401,451 -outdir $swig_dir -o $swig_dir/py_${swig_file_only}.cpp $swig_f\n" ;
            print MAKEFILE "\t\$(TRICK_CPPC) \$(TRICK_CXXFLAGS) \$(TRICK_IO_CXXFLAGS) \$(SWIG_CFLAGS) -c $swig_dir/py_${swig_file_only}.cpp -o \$@\n\n" ;

            if ( @temp_array2[$ii] !~ /S_source/ )  {
                print MAKEFILE "\$(LIB_DIR)/p${ii}.o : $swig_object_dir/py_${swig_file_only}.o\n" ;
                print MAKEFILE "\tln -s -f \$< \$@\n\n" ;
            }
        }

        if ( $swig_module_dir ne "" ) {
            print MAKEFILE "$swig_sim_dir/${swig_module_dir}m${uniq_id}.py : | $swig_sim_dir/$swig_module_dir\n" ;
        }
        if ( ! exists $$sim_ref{sim_lib_swig_files}{$f} ) {
            print MAKEFILE "$swig_sim_dir/${swig_module_dir}m${uniq_id}.py : | $swig_object_dir/py_${swig_file_only}.o\n" ;
        } else {
            print MAKEFILE "$swig_sim_dir/${swig_module_dir}m${uniq_id}.py :\n" ;
        }
        print MAKEFILE "\t/bin/cp $swig_dir/m${uniq_id}.py \$@\n\n" ;

        $ii++ ;
    }

    foreach $m ( keys %python_modules ) {
        next if ( $m eq "root") ;
        my ($temp_str) = $m ;
        $temp_str =~ s/\./\//g ;
        print MAKEFILE "$swig_sim_dir/$m:\n" ;
        print MAKEFILE "\tmkdir -p \$@\n\n" ;
    }

    print MAKEFILE "PY_WRAPPERS = $py_wrappers\n\n" ;

    my $wd = abs_path(cwd()) ;
    my $sim_dir_name = basename($wd) ;
    $sim_dir_name =~ s/SIM_// ;

    print MAKEFILE "
SWIG_SRC_FILES = \$(addprefix $swig_src_dir/,\$(notdir \$(subst .o,.cpp,\$(ALL_SWIG_OBJECTS))))
#SWIG_MODULE_SRC_FILES = \$(filter p%,\$(SWIG_SRC_FILES))

\$(ALL_SWIG_OBJECTS) : TRICK_CXXFLAGS += -Wno-unused-parameter -Wno-redundant-decls

\$(SWIG_SRC_FILES): | $swig_src_dir

.PHONY: swig_objects 

\$(S_MAIN) : \$(OBJECT_DIR)/py_top.o \$(OBJECT_DIR)/init_swig_modules.o \$(OBJECT_DIR)/py_S_source.o\n
\$(LIB_DIR)/lib_${sim_dir_name}.a : \$(SWIG_MODULE_OBJECTS)
#\$(LIB_DIR)/lib_${sim_dir_name}.a : \$(SWIG_MODULE_SOURCE) \$(ALL_SWIG_OBJECTS)
#\$(LIB_DIR)/lib_${sim_dir_name}.so : \$(SWIG_MODULE_SOURCE) \$(ALL_SWIG_OBJECTS)\n\n" ;

    print MAKEFILE "$swig_src_dir/py_top.cpp : $swig_src_dir/top.i\n" ;
    print MAKEFILE "\t\$(SWIG) \$(TRICK_INCLUDE) \$(TRICK_DEFINES) \$(TRICK_VERSIONS) -c++ -python -includeall -ignoremissing -w201,303,362,389,401,451 -outdir $swig_sim_dir -o \$@ $swig_src_dir/top.i\n\n" ;

    print MAKEFILE "\$(OBJECT_DIR)/py_top.o : $swig_src_dir/py_top.cpp | \$(OBJECT_DIR)\n" ;
    print MAKEFILE "\t\$(TRICK_CPPC) \$(TRICK_CXXFLAGS) \$(SWIG_CFLAGS) -c \$< -o \$@\n\n" ;

    print MAKEFILE "\$(OBJECT_DIR)/init_swig_modules.o : $swig_src_dir/init_swig_modules.cpp | \$(OBJECT_DIR)\n" ;
    print MAKEFILE "\t\$(TRICK_CPPC) \$(TRICK_CXXFLAGS) \$(SWIG_CFLAGS) -c \$< -o \$@\n\n" ;

    print MAKEFILE "TRICK_FIXED_PYTHON = $swig_sim_dir/swig_double.py $swig_sim_dir/swig_int.py $swig_sim_dir/swig_ref.py $swig_sim_dir/shortcuts.py $swig_sim_dir/unit_test.py $swig_sim_dir/sim_services.py $swig_sim_dir/exception.py\n" ;
    print MAKEFILE "S_main: \$(TRICK_FIXED_PYTHON) \$(PY_WRAPPERS)\n\n" ;

    print MAKEFILE "$swig_sim_dir/sim_services.py : \${TRICK_HOME}/trick_source/trick_swig/swig_\${TRICK_HOST_CPU}/sim_services.py\n" ;
    print MAKEFILE "\t/bin/cp \$< \$@\n\n" ;

    print MAKEFILE "$swig_sim_dir/swig_double.py : \${TRICK_HOME}/trick_source/trick_swig/swig_\${TRICK_HOST_CPU}/swig_double.py\n" ;
    print MAKEFILE "\t/bin/cp \$< \$@\n\n" ;

    print MAKEFILE "$swig_sim_dir/swig_int.py : \${TRICK_HOME}/trick_source/trick_swig/swig_\${TRICK_HOST_CPU}/swig_int.py\n" ;
    print MAKEFILE "\t/bin/cp \$< \$@\n\n" ;

    print MAKEFILE "$swig_sim_dir/swig_ref.py : \${TRICK_HOME}/trick_source/trick_swig/swig_\${TRICK_HOST_CPU}/swig_ref.py\n" ;
    print MAKEFILE "\t/bin/cp \$< \$@\n\n" ;

    print MAKEFILE "$swig_sim_dir/shortcuts.py : \${TRICK_HOME}/trick_source/trick_swig/shortcuts.py\n" ;
    print MAKEFILE "\t/bin/cp \$< \$@\n\n" ;

    print MAKEFILE "$swig_sim_dir/exception.py : \${TRICK_HOME}/trick_source/trick_swig/exception.py\n" ;
    print MAKEFILE "\t/bin/cp \$< \$@\n\n" ;

    print MAKEFILE "$swig_sim_dir/unit_test.py : \${TRICK_HOME}/trick_source/trick_swig/unit_test.py\n" ;
    print MAKEFILE "\t/bin/cp \$< \$@\n\n" ;

    foreach (keys %swig_dirs) {
        print MAKEFILE "$_:\n" ;
        print MAKEFILE "\tmkdir -p $_\n\n" ;
    }

    print MAKEFILE "
tidy: tidy_swig

tidy_swig:
\t -rm -rf Makefile_swig $swig_src_dir trick $s_library_swig

clean: clean_swig

clean_swig:\n" ;
    foreach (keys %swig_dirs) {
        print MAKEFILE "\t-rm -rf $_\n" ;
    }

    print MAKEFILE "\n" ;
    close MAKEFILE ;

    open SWIGLIB , ">$s_library_swig" or return ;
    foreach my $f ( @temp_array2 ) {
        print SWIGLIB "$f\n" ;
    }
    close SWIGLIB ;

    open TOPFILE , ">$swig_src_dir/top.i" or return ;
    print TOPFILE "\%module top\n\n" ;
    print TOPFILE "\%{\n#include \"../S_source.hh\"\n\n" ;
    foreach my $inst ( @{$$sim_ref{instances}} ) {
        print TOPFILE "extern $$sim_ref{instances_type}{$inst} $inst ;\n" ;
    }
    foreach my $integ_loop ( @{$$sim_ref{integ_loop}} ) {
       print TOPFILE "extern IntegLoopSimObject $$integ_loop{name} ;" ;
    }

    print TOPFILE "\n\%}\n\n" ;
    print TOPFILE "\%import \"S_source.i\"\n\n" ;
    foreach my $inst ( @{$$sim_ref{instances}} ) {
        print TOPFILE "$$sim_ref{instances_type}{$inst} $inst ;\n" ;
    }
    foreach my $integ_loop ( @{$$sim_ref{integ_loop}} ) {
       print TOPFILE "IntegLoopSimObject $$integ_loop{name} ;" ;
    }
    close TOPFILE ;

    open INITSWIGFILE , ">$swig_src_dir/init_swig_modules.cpp" or return ;
    print INITSWIGFILE "extern \"C\" {\n\n" ;
    foreach $f ( @temp_array2 ) {

        ( $dev, $inode ) = stat($f) ;
        my $uniq_id = "${dev}_$inode" ;

        print INITSWIGFILE "void init_m${uniq_id}(void) ; /* $f */\n" ;
    }
    print INITSWIGFILE "void init_sim_services(void) ;\n" ;
    print INITSWIGFILE "void init_top(void) ;\n" ;
    print INITSWIGFILE "void init_swig_double(void) ;\n" ;
    print INITSWIGFILE "void init_swig_int(void) ;\n" ;
    print INITSWIGFILE "void init_swig_ref(void) ;\n" ;

    print INITSWIGFILE "\nvoid init_swig_modules(void) {\n\n" ;
    foreach $f ( @temp_array2 ) {
        next if ( $f =~ /S_source.hh/ ) ;
        ( $dev, $inode ) = stat($f) ;
        my $uniq_id = "${dev}_$inode" ;

        print INITSWIGFILE "    init_m${uniq_id}() ;\n" ;
    }

    print INITSWIGFILE "    init_m${s_source_uniq_id}() ;\n" ;

    print INITSWIGFILE "    init_sim_services() ;\n" ;
    print INITSWIGFILE "    init_top() ;\n" ;
    print INITSWIGFILE "    init_swig_double() ;\n" ;
    print INITSWIGFILE "    init_swig_int() ;\n" ;
    print INITSWIGFILE "    init_swig_ref() ;\n" ;
    print INITSWIGFILE "    return ;\n}\n\n}\n" ;
    close INITSWIGFILE ;

    open INITFILE , ">$swig_sim_dir/__init__.py" or return ;

    print INITFILE "import sys\n" ;
    print INITFILE "import os\n" ;
    print INITFILE "sys.path.append(os.getcwd() + \"/trick\")\n" ;

    foreach $m ( keys %python_modules ) {
        next if ( $m eq "root") ;
        my ($temp_str) = $m ;
        $temp_str =~ s/\./\//g ;
        print INITFILE "sys.path.append(os.getcwd() + \"/trick/$temp_str\")\n" ;
    }
    print INITFILE "\n" ;
    print INITFILE "import _sim_services\n" ;
    print INITFILE "from sim_services import *\n\n" ;

    print INITFILE "# create \"all_cvars\" to hold all global/static vars\n" ;
    print INITFILE "all_cvars = new_cvar_list()\n" ;
    print INITFILE "combine_cvars(all_cvars, cvar)\n" ;
    print INITFILE "cvar = None\n\n" ;

    foreach $m ( keys %python_modules ) {
        next if ( $m eq "root") ;
        my ($temp_str) = $m ;
        $temp_str =~ s/\//\./g ;
        print INITFILE "import $temp_str\n" ;
    }
    print INITFILE "\n" ;

    foreach $f ( @{$python_modules{"root"}} ) {
        next if ( $f =~ /S_source.hh/ ) ;
        ( $dev, $inode ) = stat($f) ;
        my $uniq_id = "${dev}_$inode" ;

        print INITFILE "# $f\n" ;
        print INITFILE "import _m${uniq_id}\n" ;
        print INITFILE "from m${uniq_id} import *\n" ;
        print INITFILE "combine_cvars(all_cvars, cvar)\n" ;
        print INITFILE "cvar = None\n\n" ;
    }

    print INITFILE "# S_source.hh\n" ;
    print INITFILE "import _m${s_source_uniq_id}\n" ;
    print INITFILE "from m${s_source_uniq_id} import *\n\n" ;
    print INITFILE "import _top\n" ;
    print INITFILE "import top\n\n" ;
    print INITFILE "import _swig_double\n" ;
    print INITFILE "import swig_double\n\n" ;
    print INITFILE "import _swig_int\n" ;
    print INITFILE "import swig_int\n\n" ;
    print INITFILE "import _swig_ref\n" ;
    print INITFILE "import swig_ref\n\n" ;
    print INITFILE "from shortcuts import *\n\n" ;
    print INITFILE "from exception import *\n\n" ;
    print INITFILE "cvar = all_cvars\n\n" ;
    close INITFILE ;

    foreach $m ( keys %python_modules ) {
        next if ( $m eq "root") ;
        my ($temp_str) = $m ;
        $temp_str =~ s/\./\//g ;
        if ( ! -e "$swig_sim_dir/$temp_str" ) {
            #make_path("$swig_sim_dir/$temp_str", {mode=>0775}) ;
            mkpath("$swig_sim_dir/$temp_str", {mode=>0775}) ;
        }
        open INITFILE , ">$swig_sim_dir/$temp_str/__init__.py" or return ;
        foreach $f ( @{$python_modules{$m}} ) {
            next if ( $f =~ /S_source.hh/ ) ;
            ( $dev, $inode ) = stat($f) ;
            my $uniq_id = "${dev}_$inode" ;

            print INITFILE "# $f\n" ;
            print INITFILE "import _m${uniq_id}\n" ;
            print INITFILE "from m${uniq_id} import *\n\n" ;
        }
        close INITFILE ;

        while ( $temp_str =~ s/\/.*?$// ) {
            open INITFILE , ">$swig_sim_dir/$temp_str/__init__.py" or return ;
            close INITFILE ;
        }
    }

    return ;

}

1;

