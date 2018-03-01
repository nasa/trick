package gte ;

use File::Basename ;
use Cwd 'abs_path';
use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(gte);

use strict ;

sub gte (@) {
    my (@args) = @_ ;
    my $ret ;
    my ( $system_type , $version , $major_ver , $gcc_version , $glibc_version ) ;
    my ( $machine_hardware ) ;
    my (%gte , %def );
    my @externals ;
    my $search ;
    my $e ;
 
    my $trick_bin = dirname(abs_path($0)) ;
    my $trick_home = dirname($trick_bin) ;

    $ret = `uname -s -r` ;
    chomp($ret);

    ($system_type , $version) = split / / , $ret ;

    if ( $system_type eq "SunOS" ) {
        $gte{"TRICK_HOST_CPU"} = $system_type . "_" . $version ;
        $gte{"TRICK_HOST_TYPE"} = $system_type . "_5.6_plus" ;
    }
    elsif ( $system_type eq "IRIX" or $system_type eq "IRIX64" ) {
        $gte{"TRICK_HOST_TYPE"} = "IRIX_6.x" ;
        $gte{"TRICK_HOST_CPU"} = "IRIX_" . $version ;
    }
    else { # Linux or Darwin
        ($major_ver) = $version =~ /(\d+)/ ;
        $def{"TRICK_HOST_CPU"} = $system_type . "_" . $major_ver ;
        $def{"TRICK_HOST_TYPE"} = $system_type ;
        $ret = "" ;

        # need to append the gcc version as different gcc's are not compatible
        if ( exists $ENV{"TRICK_CC"} ) {
            if ( $ENV{"TRICK_CC"} =~ /ccintppc/ ) {
                $ret = "0.0" ;
            } elsif ( $ENV{"TRICK_CC"} =~ /^\// ) {
                my ($temp) = $ENV{"TRICK_CC"} ;
                # remove possible ccache from TRICK_CC
                $temp =~ s/.*?ccache\s+// ;
                if ( -e $temp ) {
                    $ret = `$temp -dumpversion` ;
                }
                else {
                    printf STDERR "[33mCannot find TRICK_CC = $temp, using /usr/bin/gcc\n" ;
                    $ret = `/usr/bin/gcc -dumpversion` ;
                }
            } else {
                # remove possible ccache from TRICK_CC
                my ($temp) = $ENV{TRICK_CC} ;
                $temp =~ s/.*?ccache\s+// ;
                $ret = `$temp -dumpversion` ;
            }
        }
        else {
            $ret = `gcc -dumpversion` ;
        }
        ($gcc_version) = $ret =~ /(\d+\.\d+)/ ;

        if ( $system_type eq "Linux" ) {
            $def{"TRICK_HOST_CPU"} = $system_type . "_" . $gcc_version ;
            $machine_hardware = `uname -m` ;
            if ( (! exists $ENV{"TRICK_FORCE_32BIT"} or $ENV{"TRICK_FORCE_32BIT"} == 0) and $machine_hardware eq "x86_64\n") {
                $def{"TRICK_HOST_CPU"} .= "_x86_64" ;
            }
        }
    }

    # defaults common to all architectures
    $def{"TRICK_CC"} = "cc" ;
    $def{"TRICK_CONVERT_SWIG_FLAGS"} = "" ;
    $def{"TRICK_CFLAGS"} = "" ;
    $def{"TRICK_CXXFLAGS"} = "" ;
    $def{"TRICK_CPPC"} = "c++" ;
    $def{"TRICK_DEBUG"} = "0" ;
    $def{"TRICK_EDITOR"} = "" ;
    $def{"TRICK_EXEC_LINK_LIBS"} = "" ;
    $def{"TRICK_FORCE_32BIT"} = "0" ;
    $def{"TRICK_GTE_EXT"} = "" ;
    $def{"TRICK_HOME"} = "$trick_home" ;
    $def{"TRICK_HOST_CPU_USER_SUFFIX"} = "" ;
    $def{"TRICK_ICGFLAGS"} = "" ;
    $def{"TRICK_ICG_COMPAT15"} = "" ;
    $def{"TRICK_ICG_NOCOMMENT"} = "" ;
    $def{"TRICK_ICG_EXCLUDE"} = "" ;
    $def{"TRICK_ICG_IGNORE_TYPES"} = "" ;
    $def{"TRICK_SWIG_EXCLUDE"} = "" ;
    $def{"TRICK_EXT_LIB_DIRS"} = "" ;
    $def{"TRICK_PYTHON_PATH"} = "" ;
    $def{"TRICK_LDFLAGS"} = "" ;
    $def{"TRICK_MAKE"} = "" ;
    $def{"TRICK_PATH"} = "$trick_home/bin" ;
    $def{"TRICK_PRINT_CMD"} = "lpr" ;
    $def{"TRICK_PRINTER_NAME"} = "" ;
    $def{"TRICK_SFLAGS"} = "" ;
    $def{"TRICK_SYSTEM_CFLAGS"} = "" ;
    $def{"TRICK_SYSTEM_CXXFLAGS"} = "" ;
    $def{"TRICK_SYSTEM_LDFLAGS"} = "" ;
    $def{"TRICK_USER_HOME"} = "$ENV{HOME}/trick_sims" ;
    $def{"TRICK_USER_CSHRC"} = "$ENV{HOME}/.Trick_user_cshrc" ;
    $def{"TRICK_USER_LINK_LIBS"} = "" ;
    $def{"TRICK_USER_PROFILE"} = "$ENV{HOME}/.Trick_user_profile" ;
    $def{"TRICK_VER"} = "trick_dev" ;
    $def{"XML_CATALOG_FILES"} = "$trick_home/trick_source/data_products/DPX/XML/catalog.xml" ;
 
    # set gte variables... if variable in environment use it, else use default
    foreach ( keys %def ) {
        $gte{$_} = ( exists $ENV{$_} ) ? $ENV{$_} : $def{$_} ;
    }
 
    $gte{"TRICK_HOST_CPU"} .= $gte{"TRICK_HOST_CPU_USER_SUFFIX"} ;

    # Flip -g/-O in TRICK_CFLAGS according to TRICK_DEBUG if we are not asking for whole list
    if ( scalar(@args) != 0 ) {
        if ( $gte{"TRICK_DEBUG"} eq "1" ) {
            $gte{"TRICK_CFLAGS"} =~ s/-[gO]\d?(\s+|$)/-g /g ;
            if ($gte{"TRICK_CFLAGS"} !~ /-g/ ) {
                $gte{"TRICK_CFLAGS"} =~ s/^\s*/-g / ;
            }
            $gte{"TRICK_CXXFLAGS"} =~ s/-[gO]\d?(\s+|$)/-g /g ;
            if ($gte{"TRICK_CXXFLAGS"} !~ /-g/ ) {
                $gte{"TRICK_CXXFLAGS"} =~ s/^\s*/-g / ;
            }
        }
        elsif ( $gte{"TRICK_DEBUG"} eq "2" ) {
            $gte{"TRICK_CFLAGS"} =~ s/-g(\s+|$)/-O / ;
            if ($gte{"TRICK_CFLAGS"} !~ /-O/ ) {
                $gte{"TRICK_CFLAGS"} =~ s/^\s*/-O / ;
            }
            $gte{"TRICK_CXXFLAGS"} =~ s/-g(\s+|$)/-O / ;
            if ($gte{"TRICK_CXXFLAGS"} !~ /-O/ ) {
                $gte{"TRICK_CXXFLAGS"} =~ s/^\s*/-O / ;
            }
        }
        elsif ( $gte{"TRICK_DEBUG"} eq "3" ) {
            $gte{"TRICK_CFLAGS"} =~ s/-[gO](\s+|$)/-O2 / ;
            if ($gte{"TRICK_CFLAGS"} !~ /-O2/ ) {
                $gte{"TRICK_CFLAGS"} =~ s/^\s*/-O2 / ;
            }
            $gte{"TRICK_CXXFLAGS"} =~ s/-[gO](\s+|$)/-O2 / ;
            if ($gte{"TRICK_CXXFLAGS"} !~ /-O2/ ) {
                $gte{"TRICK_CXXFLAGS"} =~ s/^\s*/-O2 / ;
            }
        }
        elsif ( $gte{"TRICK_DEBUG"} eq "4" ) {
            $gte{"TRICK_CFLAGS"} =~ s/-[gO]\d?(\s+|$)/-O3 / ;
            if ($gte{"TRICK_CFLAGS"} !~ /-O3/ ) {
                $gte{"TRICK_CFLAGS"} =~ s/^\s*/-O3 / ;
            }
            $gte{"TRICK_CXXFLAGS"} =~ s/-[gO]\d?(\s+|$)/-O3 / ;
            if ($gte{"TRICK_CXXFLAGS"} !~ /-O3/ ) {
                $gte{"TRICK_CXXFLAGS"} =~ s/^\s*/-O3 / ;
            }
        }
        else {
            $gte{"TRICK_CFLAGS"} =~ s/-[gO]\d?(\s+|$)// ;
            $gte{"TRICK_CXXFLAGS"} =~ s/-[gO]\d?(\s+|$)// ;
        }
    }
 
    $gte{"TRICK_CFLAGS"} =~ s/\s+$// ;
    $gte{"TRICK_CXXFLAGS"} =~ s/\s+$// ;
 
    # Add any external variables that the user wanted added
    @externals = split / / , $gte{"TRICK_GTE_EXT"} ;
    foreach (@externals) {
        if ( exists $ENV{$_} ) {
            $gte{$_} = $ENV{$_}
        }
    }
 
    $ret = "" ;
    # List of variables that we want the local_env value to override the environment
    my %vars_override_env = qw( TRICK_CXXFLAGS 1 TRICK_CFLAGS 1) ;
    # if arguments given print arguments, else print whole list
    if ( $#args >= 0 )  {
        if ( $args[0] eq "S_source") {
            foreach $e (sort keys %gte) {
                $ret .= "    local_env[\"" . $e .  "\"] = \"" . $gte{$e} . "\";\n";
                $ret .= "    setenv(\"" . $e . "\", local_env[\"" . $e . "\"].c_str(), ";
                if ( exists $vars_override_env{$e} ) {
                    $ret .= "1";
                } else {
                    $ret .= "0";
                }
                $ret .= ");\n";
            }
            $ret =~ s/,\n+$/\n/s ;
        }
        else {
            foreach $e (@args) {
                # print variable from gte
                if (exists $gte{$e}) {
                    $ret .= $gte{$e} . "\n" ;
                }
                elsif (exists $ENV{$e}) {
                    # print variable from environment if not a trick variable
                    $ret .= $ENV{$e} . "\n" ;
                }
            }
        }
        
        if ( $args[0] =~ /^-e(.*)/ ) {
            $search = $1 ;
            if ($search eq "") {
                $search = $args[1] ;
            }
                
            # print all variables in gte that match
            foreach $e (sort keys %gte) {
                if ( $e =~ /$search/ ) {
                    $ret .= $e . "=" . $gte{$e} . "\n" ;
                }
            }
        }
    }
    else  {
        # print all variables in gte
        foreach $e (sort keys %gte) {
            $ret .= $e . "=" . $gte{$e} . "\n" ;
        }
    }
 
    return $ret ;
}

1;
