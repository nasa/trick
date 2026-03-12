package get_lib_deps ;

use File::Basename ;
use Cwd 'abs_path';
use File::Path qw(make_path) ;
use Exporter ();
use gte ;
use get_paths ;
use verbose_build ;
use trick_version ;
use Digest::MD5 qw(md5_hex) ;
@ISA = qw(Exporter);
@EXPORT = qw(get_lib_deps write_lib_deps get_s_source_deps trickify_map_fake_deps) ;

use strict ;

my $verbose_build = verbose_build() ;
my @ext_lib_paths = get_paths( "TRICK_EXT_LIB_DIRS" ) ;
my @ext_lib_paths_overrides = get_paths( "TRICK_EXT_LIB_DIRS_OVERRIDES" ) ;

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
            if ( get_containing_path( $_, @ext_lib_paths_overrides ) eq 0 ) {
                print "[95mDep Skip[39m   TRICK_EXT_LIB_DIRS: [4m$exclude_path[24m" . substr($_, length $exclude_path) . "\n" if $verbose_build ;
                next ;
            }
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
        open SOURCE, $source_file_name or warn "cannot read $source_file_name: $!" ;
        $contents = <SOURCE> ;
        close SOURCE ;
    }
    # Get the library dependencies
    my (@resolved_files) = get_lib_deps($contents, $source_file_name) ;
    # Remove a self dependency if it exists
    @resolved_files = grep { $_ ne $source_file_name } @resolved_files ;

    # Build the library dependencies file name to store results
    my ( $file, $dir, $suffix) = fileparse($source_file_name, qr/\.[^.]*/) ;
    my ($lib_dep_file_name) = "$ENV{TRICK_BUILD_DIR}build$dir${file}${suffix}.lib_deps" ;
    if ( ! -e "build$dir" ) {
        make_path("$ENV{TRICK_BUILD_DIR}build$dir") ;
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

sub get_s_source_deps() {
    (my $version, my $thread) = get_trick_version() ;
    (my $year) = $version =~ /^(\d+)/ ;
    (my $cc = gte("TRICK_CC")) =~ s/\n// ;

    # Prepend -I to each include path before we pass them to the compiler
    my @include_paths = map("-I$_", (get_include_paths(), "$ENV{TRICK_HOME}", "$ENV{TRICK_HOME}/include", "$ENV{TRICK_HOME}/include/trick/compat", "$ENV{TRICK_HOME}/trick_source", "../include")) ;
    my @defines = (get_defines(), "-DTRICK_VER=$year", "-DSWIG") ;

    my @exclude_paths = get_paths( "TRICK_EXCLUDE" ) ;
    my @swig_exclude_paths = get_paths( "TRICK_SWIG_EXCLUDE" ) ;
    my %local_md5s ;

    # get the list of header files from the compiler
    open FILE_LIST, "$cc -MM @include_paths @defines $ENV{'TRICK_BUILD_DIR'}S_source.hh |" ;
    my $dir = dirname(abs_path("S_define")) ;
    my %files ;
    my %ext_libs ;
    while ( <FILE_LIST> ) {
        next if ( /^#/ or /^\s+\\/ ) ;
        outer:
        foreach my $word ( split ) {
            next if ( $word eq "\\" or $word =~ /o:/ ) ;

            # skip unsupported extensions
            next if not $word =~ /\.(H|h|hh|hxx|h++|hpp)$/ ;

            # get the absolute path
            if ( $word !~ /^\// and $dir ne "\/" ) {
                $word = "$dir/$word" ;
            }
            $word = abs_path(dirname($word)) . "/" . basename($word) ;

            # skip duplicate files
            next if (exists($local_md5s{$word})) ;
            $local_md5s{$word} = md5_hex($word) ;

            # skip system headers that are missed by the compiler -MM flag
            next if ( $word =~ /^\/usr\/include/ ) ;
            next if ( $word =~ /^\/usr\/local\/include/ ) ;

            # skip Trick headers
            my $trick_home = $ENV{'TRICK_HOME'} ;
            next if ( $word =~ /^\Q$trick_home\/include/ ) ;
            next if ( $word =~ /^\Q$trick_home\/trick_source/ ) ;

            # skip paths in TRICK_EXCLUDE
            foreach my $path ( @exclude_paths ) {
                if ( $word =~ /^\Q$path\E(.*)/ or abs_path($word) =~ /^\Q$path\E(.*)/ ) {
                    print "[95mSWIG Skip[39m  TRICK_EXCLUDE: [4m$path[24m$1\n" if $verbose_build ;
                    next outer ;
                }
            }

            # skip paths in TRICK_SWIG_EXCLUDE
            foreach my $path ( @swig_exclude_paths ) {
                if ( $word =~ /^\Q$path\E(.*)/ or abs_path($word) =~ /^\Q$path\E(.*)/ ) {
                    print "[95mSWIG Skip[39m  TRICK_SWIG_EXCLUDE: [4m$path[24m$1\n" if $verbose_build ;
                    next outer ;
                }
            }

            # separate paths in TRICK_EXT_LIB_DIRS
            foreach my $path ( @ext_lib_paths ) {
                if ( $word =~ /^\Q$path\E(.*)/ or abs_path($word) =~ /^\Q$path\E(.*)/ ) {
                    if ( get_containing_path( $word, @ext_lib_paths_overrides) eq 0 ) {
                        print "[95mSWIG Skip[39m  TRICK_EXT_LIB_DIRS: [4m$path[24m$1\n" if $verbose_build ;
                        $ext_libs{$word} = 1 ;
                        next outer ;
                    }
                }
            }
            $files{$word} = 1 ;
        }
    }
    my @local_ext_lib_files = sort keys %ext_libs ;
    my @local_files_to_process = sort keys %files ;

    return (\@local_ext_lib_files, \@local_files_to_process, \%local_md5s) ;
}

# Read in the list of all trickify deps, so that we can make sure no fake python dependencies are listed.
# This problem occurs when users use -I voodoo to make their local model files take precdent over a libraries files.
# When trickiying the respective library, trick will list the libraries version of the swig module as a dependency, which will cause mayhem at run time.
# Only do this on the trickify.mk passthrough
sub trickify_map_fake_deps($) {
    (my $files_to_process) = @_ ;

    # Generate the true list of dependencies
    my %src_deps ;
    open SRC_DEPS , "$ENV{TRICK_BUILD_DIR}tmp_build/build/trickify_deps" or die "Failed to open $ENV{TRICK_BUILD_DIR}tmp_build/build/trickify_deps\n";
    while ( <SRC_DEPS> ) {
        my $key = s/\s*$//;
        my $key = s/^\s*//;
        $src_deps{$_} = 1 ;
    }
    close SRC_DEPS ;

    # Grab our includes, for validating the replacement path
    my @includes = get_include_paths() ;

    my @new_file_list ;
    my %replacements ;
    foreach my $file (@$files_to_process) {
        # file is fine, keep it
        if ($src_deps{$file} == 1 or $file =~ /S_source.hh/) {
            push @new_file_list, $file ;
        }
        # fake dependency, find the real one
        else {
            print "SKIPPING: $file\n";
            my (@src_dep, @file, @common, $tmp_src_dep, $tmp_file) ;
            my $found_match = 0 ;
            my %possible_replacements = ();
            foreach my $src_dep (sort keys %src_deps) {
                @src_dep = File::Spec->splitdir($src_dep) ;
                @file = File::Spec->splitdir($file) ;

                # search for the real dependencies, by checking the file path starting at the filename
                # if at least the file name matches, we have a match. Otherwise move onto the next dependency
                # for a match, split the paths into matching and non matching portions
                $found_match = 0 ;
                do {
                    $tmp_src_dep = pop @src_dep ;
                    $tmp_file = pop @file ;
                    if ($tmp_src_dep eq $tmp_file) {
                        unshift @common, $tmp_file ;
                        $found_match = 1 ;
                    }
                    else {
                        push @src_dep, $tmp_src_dep ;
                        push @file, $tmp_file ;
                        if ($found_match) {
                            $possible_replacements{$src_dep} = join '/', @common ;
                        }
                    }
                } while ($tmp_src_dep eq $tmp_file and @src_dep and @file) ;
            }

            if( (scalar (keys %possible_replacements)) == 1) {
                print "WE'VE GOT A MATCH: \n\t", keys %possible_replacements, " | $file\n" ;
                my ($key) = (keys %possible_replacements) ; 
                $replacements{$file} = $key ;
                push @new_file_list, $key ;              
            }
            else {
                print "You did something bad...\n" ;
            }
        }  
    }
    return %replacements ; ;
}

1
