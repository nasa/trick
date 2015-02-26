package mis_dep ;

# $Id: mis_dep.pm 3041 2013-06-19 15:55:06Z alin $

use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(mis_dep mis_catalog_dep traverse_tree);

use Cwd ;
use File::Basename ;
use strict ;
use Cwd 'abs_path';
use trick_print ;
use gte ;

my @mis_inc_paths ;
my %processed_files ;
my %mis_depends_tree ;

# Get TRICK_CFLAGS and parse out -I include paths
sub mis_dep ($@) {

    my $file ;
    my %all_depend_trees ;
    my ($k , $a) ;
    my ($sim_ref , @fileList) = @_;

    @mis_inc_paths = $ENV{"TRICK_CFLAGS"} =~ /-I\s*(\S+)/g ;
    push @mis_inc_paths , "../include" ;
    @mis_inc_paths = grep !/trick_source/, @mis_inc_paths ;
    push @mis_inc_paths , "$ENV{TRICK_HOME}/trick_source" ;
    my @valid_inc_paths ;
    foreach (@mis_inc_paths) {
            push @valid_inc_paths , $_ if ( -e $_ ) ;
    }
    @mis_inc_paths = @valid_inc_paths ;

    @fileList = grep !/\.h$|\.H$|\.hh$|\.h\+\+$/ , @fileList ;

    foreach $file ( @fileList ) {
        if ( $file ne "" ) {
            undef %mis_depends_tree ;
            $file = abs_path(dirname($file)) . "/" . basename($file) ;
            trick_print($$sim_ref{fh},  "Getting dependencies for $file\n" , "normal_cyan" , $$sim_ref{args}{v}) ;
            get_depends($file, $sim_ref);
            %{$all_depend_trees{$file}} = %mis_depends_tree ;
            $all_depend_trees{$file}{last_look} = time ;
        }
    }

    return \%all_depend_trees ;
}

sub traverse_tree {
    my ($file, $level, %curr_tree) = @_ ;
    my $a ;

    print "   " x $level , $file , "\n";
    foreach $a ( @{$curr_tree{$file}} ) {
        if ( $a ne $file ) {
            traverse_tree($a, $level+1, %curr_tree) ;
        }
    }
}

# Show Depends Recursive Algorithm
sub get_depends($$) {

    my $file ;
    my $sim_ref ;
    my @list_objects ;
    my @list_libs ;
    my $found_dep ;
    my @lib_list ;
    my $contents ;
    my $size ;
    my $libdep ;
    my $language ;
    my ( $full_path ) ;
    my ($rcs_file_name, $rcs_ver, $rcs_date, $rcs_owner ) ;

    ($file, $sim_ref) = @_;

    # If you have already seen this dependency, then don't process
    # Trim the tree ! And prevent infinite circular recursion !!! Yikes ...
    return if ( exists $processed_files{$file} ) ;

    # Add this file to processed list, if it is a full path
    $processed_files{$file} = 1 ;

    open FILE, $file or die "Could not process $file\n";
    seek FILE , 0 , 2 ;
    $size = tell FILE ;
    seek FILE , 0 , 0 ;
    read FILE , $contents , $size;
    close(FILE);

    while ( $contents =~ /LIBRARY(\s|_)DEPENDENC(Y|IES):[^(]*(.*?)\)([A-Z \t\n\r]+:|[ \t\n\r]*\*)/gsi ) {
        $libdep .= $3 ;
    }

    if ( $libdep =~ /#/ ) {
        (my $cc = gte("TRICK_CC")) =~ s/\n// ;
        my @defines = $ENV{"TRICK_CFLAGS"} =~ /(-D\S+)/g ;
        my $temp ;
        open FILE, "echo \"$libdep\" | cpp -P @defines |" ;
        while ( <FILE> ) {
            $temp .= $_ ;
        }
        $libdep = $temp ;
    }


    @lib_list = split /\)[ \t\n\r\*]*\(/ , $libdep ;
    foreach (@lib_list) {
        s/\(|\)|\s+//g ;
        s/\${(.+?)}/$ENV{$1}/eg ;
    }
    @list_objects = grep /\.o$/ , @lib_list ;
    @list_libs = grep /\.so$|\.a$/ , @lib_list ;

    # Always add self-dependency 
    push @{$mis_depends_tree{$file}} , $file ;

    # Now for each object, construct full path to object's src and find src file
    foreach my $o (@list_objects) {

        # look for object code with source
        $found_dep = find_depends_file($file,$o,$sim_ref) ;

        # Did not find dependency
        if ( $found_dep == 0 ) {
            $o =~ s/\.[\w\+]+$/\.o/ ;
            trick_print($$sim_ref{fh}, "ERROR: Dependency \"$o\" not found for \"$file\"\n", "title_red" , $$sim_ref{args}{v}) ;
            exit( -1 ) ;
        }
    }

    # Now for each lib, construct full path to lib's src and add to list
    foreach my $l ( @list_libs ) {

        my ($rel_path , $lib_name ) ;

        ($rel_path , $lib_name ) = $l =~ /(?:(.*)\/)?([^\/]+)/ ;

        if ($rel_path eq "") {
            $found_dep = 1 ;
            $lib_name =~ s,lib,,;
            $lib_name =~ s,\.so$|\.a$,,;
            $processed_files{"-l$lib_name"} = 1 ;
            push @{$mis_depends_tree{$file}} , "-l$lib_name" ;
            trick_print($$sim_ref{fh},  "  $lib_name\n" , "debug_white" , $$sim_ref{args}{v}) ;
        } else {

            $found_dep = 0 ;
            foreach my $ipath ( @mis_inc_paths ) {

                # Construct possible full paths to lib dir from include paths
                $full_path = $ipath . "/" . $rel_path;

                if ( -s $full_path ) {

                    $found_dep = 1;
                    $lib_name = $full_path ."/object_\${TRICK_HOST_CPU}/". $lib_name ;

                    # remove ../
                    while ( $lib_name =~ s,/[^/]+/\.\.,, ) {}

                    push @{$mis_depends_tree{$file}} , $lib_name ;
                    trick_print($$sim_ref{fh},  "  $lib_name\n" , "debug_white" , $$sim_ref{args}{v}) ;

                    $processed_files{$lib_name} = 1 ;
                    last;
                }
            }
        }

        # Did not find dependency
        if ( $found_dep == 0 ) {
            trick_print($$sim_ref{fh}, "ERROR: Dependency $l not found for $file\n", "title_red" , $$sim_ref{args}{v}) ;
            exit( -1 ) ;
        }
    }
}


sub find_depends_file($$$) {

    my ($file, $o , $sim_ref) ;
    my @list_languages ;
    my $lex_yacc ;
    my ( $full_path , $full_path1 , $full_path2 ) ;
    my ( $curr_path ) ;
    my ( $dep_to_search ) ;

    ($file , $dep_to_search, $sim_ref) = @_;

    $o = $dep_to_search ;

    $curr_path = dirname($file) ;
    # Decide how to search through languages
    opendir THISDIR, $curr_path or die "Could not open the directory of $curr_path" ;
    $lex_yacc = grep /\.[ly]$/ , readdir THISDIR ;

    if ( $lex_yacc ) {
        @list_languages = ("y", "l", "c", "cpp" , "cc" , "cxx" , "C" , "c\+\+");
    }
    else {
        @list_languages = ("c", "cpp" , "cc" , "cxx" , "C" , "c\+\+", "y", "l");
    }

    # Otherwise use TRICK_CFLAGS and find it
    if ( $dep_to_search !~ /\.o$/ ) {
        foreach my $ipath ( $curr_path , @mis_inc_paths ) {
            if ( -s "$ipath/$dep_to_search" ) {
                my $f = "$ipath/$dep_to_search" ;
                $full_path = abs_path(dirname($f)) . "/" . basename($f) ;
                push @{$mis_depends_tree{$file}} , $full_path ;
                trick_print($$sim_ref{fh},  "  $full_path\n" , "debug_white" , $$sim_ref{args}{v}) ;
                get_depends($full_path , $sim_ref);
                return(1) ;
            }
        }
    } else {
        foreach my $ipath ( $curr_path , @mis_inc_paths ) {
            foreach my $lang ( @list_languages ) {

                my $found_dep ;
                my ($file_name ) ;
                my ($dir_name ) ;
                my ($rspath , $rpath ) ;

                $o =~ s/\.[\w\+]+$/\.$lang/ ;

                #
                # Construct possible full paths to src from include paths
                #
                $file_name = basename($o) ;
                $dir_name = dirname($o) ;

                # if full path sepcified (possible, but not recommended)
                # don't prepend include path.
                if ( $dir_name =~ /^\// ) {
                    $full_path1 = "$dir_name/src/$file_name" ;
                    $full_path2 = "$dir_name/$file_name" ;
                }
                else {
                    $full_path1 = "$ipath/$dir_name/src/$file_name" ;
                    $full_path2 = "$ipath/$dir_name/$file_name" ;
                }

                $found_dep = 0;
                # Construct full path
                if ( -s $full_path1 ) {
                    $full_path = abs_path(dirname($full_path1)) . "/" . basename($full_path1) ;
                    $found_dep = 1;
                }
                elsif ( -s $full_path2 ) {
                    $full_path = abs_path(dirname($full_path2)) . "/" . basename($full_path2);
                    $found_dep = 1;
                }

                if ( $found_dep == 1 ) {
                    push @{$mis_depends_tree{$file}} , $full_path ;
                    trick_print($$sim_ref{fh},  "  $full_path\n" , "debug_white" , $$sim_ref{args}{v}) ;
                    if ( $lang eq "y" ) {
                        ($full_path1 = $full_path ) =~ s/\.y$/\.l/ ;
                        push @{$mis_depends_tree{$file}} , $full_path1 ;
                    }
                    get_depends($full_path , $sim_ref);
                    return(1);

                }

            }
        }
    }

    return(0) ;
}


sub mis_catalog_dep ($$) {
    return ;
}

1;
