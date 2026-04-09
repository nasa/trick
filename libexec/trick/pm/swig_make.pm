package swig_make ;

use FindBin qw($RealBin);
use lib "$RealBin/" ;

use File::Basename ;
use File::Path qw(make_path);
use Cwd ;
use Cwd 'abs_path';
use gte ;
use html ;
use verbose_build ;
use get_lib_deps ;
use get_paths ;
use Digest::MD5 qw(md5_hex) ;

@ISA = qw(Exporter);
@EXPORT = qw(read_files_to_process write_makefile_swig_deps get_trick_headers purge_swig_no_files write_makefile_swig write_lib_files trickify_map_fake_deps replace_files_to_process write_fake_deps_map) ;

use strict ;

my @files_to_process ;
my @ext_lib_files ;
my %md5s ;
my $verbose_build = verbose_build() ;
my %trick_headers ;
my %python_modules ;
my %python_module_dirs ;

#trickify vars
my %trickify_files_to_replace ;

sub read_files_to_process()
{
    my ($files_to_process, $ext_lib_files, $md5s) = get_s_source_deps() ;
    @files_to_process = @$files_to_process ;
    @ext_lib_files = @$ext_lib_files ;
    %md5s = %$md5s ;
}

sub write_makefile_swig_deps() 
{
    open DEPENDENCIES_FILE , ">$ENV{TRICK_BUILD_DIR}build/Makefile_swig_deps" or die "Could not open $ENV{TRICK_BUILD_DIR}build/Makefile_swig_deps for writing" ;
    print DEPENDENCIES_FILE "$ENV{TRICK_BUILD_DIR}build/Makefile_swig:" ;
    foreach my $file ( @files_to_process, @ext_lib_files ) 
    {
        print DEPENDENCIES_FILE " \\\n    " . $file ;
    }
    close DEPENDENCIES_FILE ;
}

sub get_trick_headers() 
{
    foreach my $f (  @files_to_process, @ext_lib_files ) 
    {
        my %trick_header = extract_trick_header( $f, do { local( @ARGV, $/ ) = $f ; <> }, 0, 0 ) ;
        if ( exists $trick_header{python_module} ) 
        {
            $trick_headers{$f}{python_module} = $trick_header{python_module} ;
            ($trick_headers{$f}{python_module_dir} = $trick_header{python_module}) =~ s/\./\//g ;
            $python_modules{$trick_headers{$f}{python_module}} = 1 ;
            $python_module_dirs{$trick_headers{$f}{python_module_dir}} = 1 ;
        }
    }
}

sub has_swig_no 
{
    my $result = $trick_headers{$_[0]}{swig} =~ /^NO$/i ;
    print "[95mSWIG Skip[39m  SWIG: (NO): $_[0]\n" if $verbose_build and $result ;
    return $result ;
}

sub purge_swig_no_files() 
{
    @files_to_process = grep { !has_swig_no($_) } @files_to_process ;
    @ext_lib_files = grep { !has_swig_no($_) } @ext_lib_files ;
}

sub write_makefile_swig() 
{
    my $s_source_md5 = md5_hex(abs_path("$ENV{TRICK_BUILD_DIR}S_source.hh")) ;
    my $swig_sim_dir = "$ENV{'TRICK_BUILD_DIR'}.trick" ;
    my $swig_sim_zip = "trick.zip" ;
    my $swig_src_dir = "$ENV{'TRICK_BUILD_DIR'}build" ;

    open MAKEFILE , ">$ENV{TRICK_BUILD_DIR}build/Makefile_swig" or die "Could not open $ENV{TRICK_BUILD_DIR}build/Makefile_swig for writing" ;
    print MAKEFILE "TRICK_SYSTEM_SWIG_CFLAGS := -I../include \${PYTHON_INCLUDES} -Wno-shadow -Wno-missing-field-initializers

ifeq (\$(IS_CC_CLANG), 1)
    TRICK_SYSTEM_SWIG_CFLAGS += -Wno-self-assign -Wno-sometimes-uninitialized -Wno-deprecated-register -Wno-unused-variable -Wno-unused-but-set-variable -Wno-cast-function-type-mismatch
else
    TRICK_SYSTEM_SWIG_CFLAGS += -Wno-unused-but-set-variable -Wno-maybe-uninitialized
    ifeq (\$(shell test \$(GCC_MAJOR) -ge 8; echo \$\$?), 0)
        TRICK_SYSTEM_SWIG_CFLAGS += -Wno-cast-function-type
    endif
endif

ifndef TRICK_VERBOSE_BUILD
    PRINT_SWIG         = \$(info \$(call COLOR,SWIGing)    \$<)
    PRINT_COMPILE_SWIG = \$(info \$(call COLOR,Compiling)  \$<)
endif

# TRICK_FIXED_PYTHON ===========================================================

TRICK_FIXED_PYTHON = \\
    $swig_sim_dir/swig_double.py \\
    $swig_sim_dir/swig_int.py \\
    $swig_sim_dir/swig_ref.py \\
    $swig_sim_dir/shortcuts.py \\
    $swig_sim_dir/unit_test.py \\
    $swig_sim_dir/sim_services.py \\
    $swig_sim_dir/exception.py

\$(TRICK_FIXED_PYTHON): $swig_sim_dir/\% : \${TRICK_HOME}/share/trick/swig/\%
\t\$(call ECHO_AND_LOG,/bin/cp -f \$< \$@)

# SWIG_I =======================================================================

SWIG_I =" ;

    foreach my $file ( @files_to_process ) 
    {
        (my $swig_file = $file) =~ s/(\.[^.]*)?$/_py/ ;
        print MAKEFILE " \\\n    $ENV{TRICK_BUILD_DIR}build$swig_file.i" ;
    }

    print MAKEFILE "

define create_convert_swig_rule
$ENV{TRICK_BUILD_DIR}build/%_py.i: /%.\$1
\t\$\$(call ECHO_AND_LOG,\${TRICK_HOME}/\$(LIBEXEC)/trick/convert_swig \$\${TRICK_CONVERT_SWIG_FLAGS} \$\$<)
endef

\$(foreach EXTENSION,H h hh hxx h++ hpp,\$(eval \$(call create_convert_swig_rule,\$(EXTENSION))))

$ENV{TRICK_BUILD_DIR}build/top.i: $ENV{TRICK_BUILD_DIR}build/CP_instances
\t\$(call ECHO_AND_LOG,\${PYTHON} \${TRICK_HOME}/\${LIBEXEC}/trick/create_top_dot_i)

# SWIG_SRC =====================================================================

SWIG_SRC = \$(subst .i,.cpp,\$(SWIG_I)) ${swig_src_dir}/top.cpp

\$(SWIG_SRC) : %.cpp: %.i | %.d \$(SWIG_I)
\t\$(PRINT_SWIG)
\t\$(call ECHO_AND_LOG,\$(SWIG) \$(TRICK_INCLUDE) \$(TRICK_DEFINES) \$(TRICK_VERSIONS) \$(TRICK_SWIG_FLAGS) -c++ -python -includeall -ignoremissing -w201 -w303 -w315 -w325 -w362 -w389 -w401 -w451 -MMD -MP -outdir $swig_sim_dir -o \$@ \$<)

\$(SWIG_SRC:.cpp=.d): ;

-include \$(SWIG_SRC:.cpp=.d)

# SWIG_OBJECTS =================================================================

SWIG_OBJECTS = \$(subst .cpp,.o,\$(SWIG_SRC)) ${swig_src_dir}/init_swig_modules.o

\$(SWIG_OBJECTS): %.o: %.cpp
\t\$(PRINT_COMPILE_SWIG)
\t\$(call ECHO_AND_LOG,\$(TRICK_CXX) \$(TRICK_CXXFLAGS) \$(TRICK_SYSTEM_CXXFLAGS) \$(TRICK_SWIG_CFLAGS) \$(TRICK_SYSTEM_SWIG_CFLAGS) -Wno-unused-parameter -c -o \$@ \$<)

\$(S_MAIN): \$(SWIG_OBJECTS)

LINK_LISTS += \$(LD_FILELIST)$ENV{TRICK_BUILD_DIR}build/py_link_list

# $ENV{'TRICK_BUILD_DIR'}$swig_sim_zip ===================================================================

$ENV{'TRICK_BUILD_DIR'}$swig_sim_zip: \$(SWIG_SRC) \$(TRICK_FIXED_PYTHON) $swig_sim_dir/__init__.py
\t\$(info \$(call COLOR,Compiling)  Python modules)
\t\$(call ECHO_AND_LOG,\$(PYTHON) -m compileall -q $swig_sim_dir)
\t\$(info \$(call COLOR,Zipping)    Python modules into \$@)
\t\$(call ECHO_AND_LOG,ln -sf $swig_sim_dir trick)
\t\$(call ECHO_AND_LOG,zip -rq $ENV{'TRICK_BUILD_DIR'}$swig_sim_zip trick)
\t\$(call ECHO_AND_LOG,rm -f trick)


all: $ENV{'TRICK_BUILD_DIR'}$swig_sim_zip
" ;

    open INITSWIGFILE , ">$ENV{TRICK_BUILD_DIR}build/init_swig_modules.cpp" or die "Could not open $ENV{TRICK_BUILD_DIR}build/init_swig_modules.cpp for writing" ;
    print INITSWIGFILE "#include <Python.h>\n" ;
    print INITSWIGFILE "#if PY_VERSION_HEX >= 0x03000000\n" ;
    print INITSWIGFILE "extern \"C\" {\n\n" ;

    foreach my $file ( @files_to_process, @ext_lib_files ) 
    {
        print INITSWIGFILE "PyObject * PyInit__m$md5s{$file}(void) ; /* $file */\n" ;
    }

    print INITSWIGFILE "PyObject * PyInit__sim_services(void) ;\n" ;
    print INITSWIGFILE "PyObject * PyInit__top(void) ;\n" ;
    print INITSWIGFILE "PyObject * PyInit__swig_double(void) ;\n" ;
    print INITSWIGFILE "PyObject * PyInit__swig_int(void) ;\n" ;
    print INITSWIGFILE "PyObject * PyInit__swig_ref(void) ;\n" ;

    print INITSWIGFILE "\nvoid init_swig_modules(void) {\n\n" ;
    foreach my $file ( @files_to_process, @ext_lib_files ) 
    {
        next if ( $file =~ /S_source.hh/ ) ;
        print INITSWIGFILE "    PyImport_AppendInittab(\"_m$md5s{$file}\", PyInit__m$md5s{$file}) ;\n" ;
    }
    print INITSWIGFILE "    PyImport_AppendInittab(\"_m${s_source_md5}\", PyInit__m${s_source_md5}) ;\n" ;
    print INITSWIGFILE "    PyImport_AppendInittab(\"_sim_services\", PyInit__sim_services) ;\n" ;
    print INITSWIGFILE "    PyImport_AppendInittab(\"_top\", PyInit__top) ;\n" ;
    print INITSWIGFILE "    PyImport_AppendInittab(\"_swig_double\", PyInit__swig_double) ;\n" ;
    print INITSWIGFILE "    PyImport_AppendInittab(\"_swig_int\", PyInit__swig_int) ;\n" ;
    print INITSWIGFILE "    PyImport_AppendInittab(\"_swig_ref\", PyInit__swig_ref) ;\n" ;
    print INITSWIGFILE "    return ;\n}\n\n}\n" ;
    print INITSWIGFILE "#else\n" ;

    print INITSWIGFILE "extern \"C\" {\n\n" ;

    foreach my $file ( @files_to_process, @ext_lib_files ) 
    {
        print INITSWIGFILE "void init_m$md5s{$file}(void) ; /* $file */\n" ;
    }

    print INITSWIGFILE "void init_sim_services(void) ;\n" ;
    print INITSWIGFILE "void init_top(void) ;\n" ;
    print INITSWIGFILE "void init_swig_double(void) ;\n" ;
    print INITSWIGFILE "void init_swig_int(void) ;\n" ;
    print INITSWIGFILE "void init_swig_ref(void) ;\n" ;

    print INITSWIGFILE "\nvoid init_swig_modules(void) {\n\n" ;
    foreach my $file ( @files_to_process, @ext_lib_files) 
    {
        next if ( $file =~ /S_source.hh/ ) ;
        print INITSWIGFILE "    init_m$md5s{$file}() ;\n" ;
    }
    print INITSWIGFILE "    init_m${s_source_md5}() ;\n" ;
    print INITSWIGFILE "    init_sim_services() ;\n" ;
    print INITSWIGFILE "    init_top() ;\n" ;
    print INITSWIGFILE "    init_swig_double() ;\n" ;
    print INITSWIGFILE "    init_swig_int() ;\n" ;
    print INITSWIGFILE "    init_swig_ref() ;\n" ;
    print INITSWIGFILE "    return ;\n}\n\n}\n" ;
    print INITSWIGFILE "#endif\n" ;
    close INITSWIGFILE ;

    if ( ! -e $swig_sim_dir) 
    {
        mkdir $swig_sim_dir ;
    }
    open INITFILE , ">$swig_sim_dir/__init__.py" or die "Could not open $swig_sim_dir/__init__.py for writing" ;

    print INITFILE "from pkgutil import extend_path\n" ;
    print INITFILE "__path__ = extend_path(__path__, __name__)\n" ;
    print INITFILE "import sys\n" ;
    print INITFILE "import os\n" ;
    print INITFILE "sys.path.append(os.getcwd() + \"/$swig_sim_zip/trick\")\n" ;
    foreach my $dir ( keys %python_module_dirs ) 
    {
       print INITFILE "sys.path.append(os.getcwd() + \"/$swig_sim_zip/trick/$dir\")\n" ;
    }

    print INITFILE "\n" ;
    print INITFILE "import _sim_services\n" ;
    print INITFILE "from sim_services import *\n\n" ;

    print INITFILE "# create \"all_cvars\" to hold all global/static vars\n" ;
    print INITFILE "all_cvars = new_cvar_list()\n" ;
    print INITFILE "combine_cvars(all_cvars, cvar)\n" ;
    print INITFILE "cvar = None\n\n" ;

    foreach my $file ( @files_to_process, @ext_lib_files ) 
    {
        print INITFILE "# $file\n" ;
        print INITFILE "import _m$md5s{$file}\n" ;
        print INITFILE "combine_cvars(all_cvars, cvar)\n" ;
        print INITFILE "cvar = None\n\n" ;
    }

    foreach my $file ( @files_to_process, @ext_lib_files ) 
    {
        print INITFILE "# $file\n" ;
        print INITFILE "from m$md5s{$file} import *\n" ;
    }

    foreach my $mod ( keys %python_modules ) 
    {
        print INITFILE "import trick.$mod\n" ;
    }

    print INITFILE "\n" ;
    print INITFILE "# S_source.hh\n" ;
    print INITFILE "import _m${s_source_md5}\n" ;
    print INITFILE "from m${s_source_md5} import *\n\n" ;
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

    foreach my $dir ( keys %python_module_dirs ) 
    {
        system("mkdir -p $swig_sim_dir/$dir");
        open MODULE_INITFILE, ">$swig_sim_dir/$dir/__init__.py";
        foreach my $file ( @files_to_process ) 
        {
            if ( exists $trick_headers{$file}{python_module_dir} and $trick_headers{$file}{python_module_dir} eq $dir ) 
            {
                print MODULE_INITFILE "# $file\n" ;
                print MODULE_INITFILE "import _m$md5s{$file}\n" ;
                print MODULE_INITFILE "from m$md5s{$file} import *\n" ;
            }
        }
        close MODULE_INITFILE;
    }

    return ;
}

sub write_lib_files() 
{
    open PY_LINK_LIST , ">$ENV{TRICK_BUILD_DIR}build/py_link_list" or die "Could not open $ENV{TRICK_BUILD_DIR}build/py_link_list for writing" ;
    open TRICKIFY_PY_LINK_LIST , ">$ENV{TRICK_BUILD_DIR}build/trickify_py_link_list" or die "Could not open $ENV{TRICK_BUILD_DIR}build/trickify_py_link_list for writing" ;
    open SWIGLIB , ">$ENV{TRICK_BUILD_DIR}build/S_library_swig" or die "Could not open $ENV{TRICK_BUILD_DIR}build/S_library_swig for writing" ;

    print PY_LINK_LIST "$ENV{TRICK_BUILD_DIR}build/init_swig_modules.o\n" ;
    print PY_LINK_LIST "$ENV{TRICK_BUILD_DIR}build/top.o\n" ;

    foreach my $file ( @files_to_process ) 
    {
        print SWIGLIB "$file\n" ;

        (my $swig_file = $file) =~ s/(\.[^.]*)?$/_py/ ;
        print PY_LINK_LIST "$ENV{TRICK_BUILD_DIR}build$swig_file.o\n" ;
        if ( !($swig_file =~ /(.*)S_source_py$/) ) 
        {
            print TRICKIFY_PY_LINK_LIST "$ENV{TRICK_BUILD_DIR}build$swig_file.o\n" ;
        }
    }

    close MAKEFILE ;
    close PY_LINK_LIST ;
    close TRICKIFY_PY_LINK_LIST ;
    close SWIGLIB ;
}

# Read in the list of all trickify deps, so that we can make sure no fake python dependencies are listed.
# This problem occurs when users use -I voodoo to make their local model files take precdent over a libraries files.
# When trickiying the respective library, trick will list the libraries version of the swig module as a dependency, which will cause mayhem at run time.
# Only do this on the trickify.mk passthrough
sub trickify_map_fake_deps() 
{

    # Generate the true list of dependencies
    my %src_deps ;
    open SRC_DEPS , "$ENV{TRICK_BUILD_DIR}tmp_build/build/trickify_deps" or die "Failed to open $ENV{TRICK_BUILD_DIR}tmp_build/build/trickify_deps\n";
    while ( <SRC_DEPS> ) 
    {
        my $key = s/\s*$//;
        my $key = s/^\s*//;
        $src_deps{$_} = 1 ;
    }
    close SRC_DEPS ;

    # Grab our includes, for validating the replacement path
    my @includes = get_include_paths() ;

    my @new_file_list ;
    my %replacements ;
    foreach my $file (@files_to_process) 
    {
        # file is fine, keep it
        if ($src_deps{$file} == 1 or $file =~ /S_source.hh/) 
        {
            push @new_file_list, $file ;
        }
        # fake dependency, find the real one
        else 
        {
            print "SKIPPING: $file\n";
            my (@src_dep, @file, @common, $tmp_src_dep, $tmp_file) ;
            my $found_match = 0 ;
            my %possible_replacements = ();
            foreach my $src_dep (sort keys %src_deps) 
            {
                @src_dep = File::Spec->splitdir($src_dep) ;
                @file = File::Spec->splitdir($file) ;

                # search for the real dependencies, by checking the file path starting at the filename
                # if at least the file name matches, we have a match. Otherwise move onto the next dependency
                # for a match, split the paths into matching and non matching portions
                $found_match = 0 ;
                do {
                    $tmp_src_dep = pop @src_dep ;
                    $tmp_file = pop @file ;
                    if ($tmp_src_dep eq $tmp_file) 
                    {
                        unshift @common, $tmp_file ;
                        $found_match = 1 ;
                    }
                    else 
                    {
                        push @src_dep, $tmp_src_dep ;
                        push @file, $tmp_file ;
                        if ($found_match) {
                            $possible_replacements{$src_dep} = join '/', @common ;
                        }
                    }
                } while ($tmp_src_dep eq $tmp_file and @src_dep and @file) ;
            }

            if( (scalar (keys %possible_replacements)) == 1) 
            {
                print "WE'VE GOT A MATCH: \n\t", keys %possible_replacements, " | $file\n" ;
                my ($key) = (keys %possible_replacements) ; 
                $trickify_files_to_replace{$file} = $key ;
                push @new_file_list, $key ;              
            }
            else 
            {
                print "You did something bad...\n" ;
            }
        }  
    }
}

sub replace_files_to_process()
{
    foreach my $old_file (keys %trickify_files_to_replace) 
    {
        foreach (@files_to_process) 
        {
            $_ = $trickify_files_to_replace{$old_file} if $_ eq $old_file ; 
        }
    }

}

sub write_fake_deps_map()
{
    make_path("$ENV{TRICK_BUILD_DIR}build/trickify/swig/") unless -d "$ENV{TRICK_BUILD_DIR}build/trickify/swig/" ;

    open FAKEDEPSMAP, ">$ENV{TRICK_BUILD_DIR}build/trickify/fake_deps_map" or die "Could not open $ENV{TRICK_BUILD_DIR}build/trickify/fake_deps_map" ;
    print FAKEDEPSMAP map { "$_:$trickify_files_to_replace{$_}\n" } (sort keys %trickify_files_to_replace) ;
    close FAKEDEPSMAP ;  
}

1 ;
