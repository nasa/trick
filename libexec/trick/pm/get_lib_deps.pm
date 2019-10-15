package get_lib_deps ;

use File::Basename ;
use Cwd 'abs_path';
use File::Path qw(make_path) ;
use Exporter ();
use gte ;
use get_paths ;
use verbose_build ;
@ISA = qw(Exporter);
@EXPORT = qw(get_lib_deps write_lib_deps);

use strict ;

my $verbose_build = verbose_build() ;
my @ext_lib_paths = get_paths( "TRICK_EXT_LIB_DIRS" ) ;

sub get_lib_deps ($$) {
    my ($contents, $source_file_name) = @_ ;
    my ($lib_deps) ;
    my (@lib_list) ;
    my (@inc_paths) ;
    my (@raw_lib_deps) ;

    # Doxygen style
    @lib_list = ($contents =~ /(?:@|\\)trick_li(?:nk|b)_dependency\s*{\s*(.*?)\s*}/gs) ;

    # Classic style
    # library dependency regular expression will match all the way through last parenthesis followed by
    # another field in the trick header, a doxygen style keyword, or the end of comment *.
    # we capture all library dependencies at once into raw_lib_deps
    @raw_lib_deps = ($contents =~ /LIBRARY[ _]DEPENDENC(?:Y|IES)\s*:[^(]*(.*?)\)(?:[A-Z _\t\n\r]+:|\s*[\*@])/gsi) ;
    foreach my $r ( @raw_lib_deps ) {
        # if there is preprocessor directive in the library dependencies, run the text through cpp.
        if ( $r =~ /#/ ) {
            (my $cc = gte("TRICK_CC")) =~ s/\n// ;
            my @defines = get_defines() ;
            my $temp ;
            open FILE, "echo \"$r\" | cpp -P @defines |" ;
            while ( <FILE> ) {
                $temp .= $_ ;
            }
            $r = $temp ;
        }
        push @lib_list , (split /\)[ \t\n\r\*]*\(/ , $r)  ;
    }

    @inc_paths = get_include_paths() ;
    # Get only the include paths that exist
    my @valid_inc_paths ;
    foreach (@inc_paths) {
        push @valid_inc_paths , $_ if ( -e $_ ) ;
    }
    @inc_paths = @valid_inc_paths ;

    my ($file_path_dir) = dirname($source_file_name) ;
    $file_path_dir =~ s/\/+$// ;                 # remove trailing slash
    $file_path_dir =~ s/\/include$// ;

    my %resolved_files ;
    my @ordered_resolved_files ;
    foreach my $l (@lib_list) {
        my $found = 0 ;
        $l =~ s/\(|\)|\s+//g ;
        $l =~ s/\$\{(.+?)\}/$ENV{$1}/eg ;
        next if ( $l eq "" ) ;

        if ( $l =~ /\.a$/ ) {
            my ($rel_dir) = dirname($l) ;
            foreach my $inc ( dirname($source_file_name) , @inc_paths) {
                if ( -e "$inc/$rel_dir" ) {
                    my $f = abs_path("$inc/$rel_dir") . "/" . basename($l) ;
                    if ( ! exists $resolved_files{$f} ) {
                        $resolved_files{$f} = 1 ;
                        push @ordered_resolved_files , $f ;
                    }
                    $found = 1 ;
                    last ;
                }
            }
        } elsif ( $l !~ /\.o$/ ) {
            foreach my $inc ( dirname($source_file_name) , @inc_paths) {
                if ( -e "$inc/$l" ) {
                    #print "found $inc/$l$ext\n" ;
                    my $f = abs_path(dirname("$inc/$l")) . "/" . basename("$inc/$l") ;
                    if ( ! exists $resolved_files{$f} ) {
                        $resolved_files{$f} = 1 ;
                        push @ordered_resolved_files , $f ;
                    }
                    $found = 1 ;
                    last ;
                }
            }
        } else {
            $l =~ s/o$// ;
            my ($rel_dir) = dirname($l) ;
            my ($base) = basename($l) ;
            if ( $rel_dir =~ /^\// ) {
                # if the directory name of the dependency is absolute
                foreach my $ext ( "cpp" , "cc" , "c" , "c++" , "cxx" , "C" ) {
                    if ( -e "$rel_dir/$base$ext" ) {
                        my $f = abs_path("$rel_dir") . "/$base$ext" ;
                        #print "found $f\n" ;
                        if ( ! exists $resolved_files{$f} ) {
                            $resolved_files{$f} = 1 ;
                            push @ordered_resolved_files , $f ;
                        }
                        $found = 1 ;
                    }
                    elsif ( -e "$rel_dir/src/$base$ext" ) {
                        my $f = abs_path("$rel_dir/src") . "/$base$ext" ;
                        #print "found $f\n" ;
                        if ( ! exists $resolved_files{$f} ) {
                            $resolved_files{$f} = 1 ;
                            push @ordered_resolved_files , $f ;
                        }
                        $found = 1 ;
                    }
                }
            } else {
                # if the directory name of the dependency is a relative directory
                foreach my $inc ( $file_path_dir , @inc_paths) {
                    foreach my $ext ( "cpp" , "cc" , "c" , "c++" , "cxx" , "C" ) {
                        if ( -e "$inc/$rel_dir/$base$ext" ) {
                            my $f = abs_path("$inc/$rel_dir") . "/$base$ext" ;
                            #print "found $f\n" ;
                            if ( ! exists $resolved_files{$f} ) {
                                $resolved_files{$f} = 1 ;
                                push @ordered_resolved_files , $f ;
                            }
                            $found = 1 ;
                            last ;
                        }
                        elsif ( -e "$inc/$rel_dir/src/$base$ext" ) {
                            my $f = abs_path("$inc/$rel_dir/src") . "/$base$ext" ;
                            #print "found $f\n" ;
                            if ( ! exists $resolved_files{$f} ) {
                                $resolved_files{$f} = 1 ;
                                push @ordered_resolved_files , $f ;
                            }
                            $found = 1 ;
                            last ;
                        }
                    }
                    last if ( $found == 1 ) ;
                }
            }
            # file not found, append the "o" we stripped for the error message
            $l .= "o" ;
        }
        if ( $found == 0 ) {
            print STDERR "[1m[33mWarning    [39m$source_file_name[0m\n           " ;
            if ( $l =~ /^(sim_services)/ or $l =~ /^(er7_utils)/ ) {
                print STDERR "It is not necessary to list dependencies found in $1: \"[1m$l[0m\"\n" ;
            } else {
                print STDERR "Could not find dependency \"[1m$l[0m\"\n" ;
            }
        }
    }

    my @included_ordered_resolved_files;
    foreach (@ordered_resolved_files) {
        if ( my $exclude_path = get_containing_path( $_, @ext_lib_paths ) ) {
            print "[95mDep Skip[39m   TRICK_EXT_LIB_DIRS: [4m$exclude_path[24m" . substr($_, length $exclude_path) . "\n" if $verbose_build ;
            next ;
        }
        push @included_ordered_resolved_files, $_ ;
    }
    return @included_ordered_resolved_files ;
}

sub write_lib_deps($) {
    my $deps_changed ;
    my ($source_file_name) = @_ ;
    my $contents ;
    {
        # read source file in slurp mode.  Keep the scope of undefining $/ (slurp) to this read
        local $/ = undef ;
        open SOURCE, $source_file_name or warn 'cannot read $source_file_name' ;
        $contents = <SOURCE> ;
        close SOURCE ;
    }
    # Get the library dependencies
    my (@resolved_files) = get_lib_deps($contents, $source_file_name) ;
    # Remove a self dependency if it exists
    @resolved_files = grep { $_ ne $source_file_name } @resolved_files ;

    # Build the library dependencies file name to store results
    my ( $file, $dir, $suffix) = fileparse($source_file_name, qr/\.[^.]*/) ;
    my ($lib_dep_file_name) = "build$dir${file}${suffix}.lib_deps" ;
    if ( ! -e "build$dir" ) {
        make_path("build$dir") ;
    }

    if ( -e $lib_dep_file_name ) {
        # If the library dependency file exists open the old lib dep file
        # and compare the new and old lists.
        open OLDLIBDEP, "$lib_dep_file_name" ;
        my @old_resolved = <OLDLIBDEP> ;
        close OLDLIBDEP ;
        chomp @old_resolved ;

        if (join(", ", @old_resolved) eq join(", ", @resolved_files)) {
            #print "Library dependencies unchanged for $source_file_name\n" ;
            $deps_changed = 0 ;
        } else {
            #print "Library dependencies changed for $source_file_name\n" ;
            $deps_changed = 1 ;
        }
    } else {
        # If the library dependency does not exist, the deps changed.
        $deps_changed = 1 ;
    }

    # if the library dependencies changed, write out the new dependency list
    if ( $deps_changed ) {
        open LIBDEP, ">$lib_dep_file_name" ;
        print LIBDEP map {"$_\n"} @resolved_files ;
        close LIBDEP ;
    }

    # return the deps changed flag and the list of dependencies
    return $deps_changed , @resolved_files ;
}

1
