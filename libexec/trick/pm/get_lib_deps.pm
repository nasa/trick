package get_lib_deps ;

use File::Basename ;
use Cwd 'abs_path';
use File::Path qw(make_path) ;
use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(get_lib_deps write_lib_deps);

use strict ;

sub get_lib_deps ($$) {
    my ($contents, $source_file_name) = @_ ;
    my ($lib_deps) ;
    my (@lib_list) ;
    my (@inc_paths) ;
    my (@raw_lib_deps) ;

    # library dependency regular expression will match all the way through last parenthesis followed by
    # another field in the trick header, a doxygen style keyword, or the end of comment *.
    # we capture all library dependencies at once into raw_lib_deps
    @raw_lib_deps = ($contents =~ /LIBRARY[ _]DEPENDENC(?:Y|IES)\s*:[^(]*(.*?)\)(?:[A-Z _\t\n\r]+:|\s*[\*@])/gsi) ;
    foreach ( @raw_lib_deps ) {
        push @lib_list , (split /\)[ \t\n\r\*]*\(/ , $_)  ;
    }

    @inc_paths = $ENV{"TRICK_CFLAGS"} =~ /-I\s*(\S+)/g ;     # get include paths from TRICK_CFLAGS
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
    foreach my $l (@lib_list) {
        my $found = 0 ;
        $l =~ s/\(|\)|\s+//g ;
        $l =~ s/\${(.+?)}/$ENV{$1}/eg ;
        next if ( $l eq "" ) ;

        if ( $l =~ /\.a$/ ) {
            my ($rel_dir) = dirname($l) ;
            foreach my $inc ( dirname($source_file_name) , @inc_paths) {
                if ( -e "$inc/$rel_dir" ) {
                    my $f = abs_path("$inc/$rel_dir") . "/" . basename($l) ;
                    $resolved_files{$f} = 1 ;
                    $found = 1 ;
                    last ;
                }
            }
        } elsif ( $l !~ /\.o$/ ) {
            foreach my $inc ( dirname($source_file_name) , @inc_paths) {
                if ( -e "$inc/$l" ) {
                    #print "found $inc/$l$ext\n" ;
                    my $f = abs_path(dirname("$inc/$l")) . "/" . basename("$inc/$l") ;
                    $resolved_files{$f} = 1 ;
                    $found = 1 ;
                    last ;
                }
            }
        } else {
            $l =~ s/o$// ;
            my ($rel_dir) = dirname($l) ;
            my ($base) = basename($l) ;
            foreach my $inc ( $file_path_dir , @inc_paths) {
                foreach my $ext ( "cpp" , "cc" , "c" , "c++" , "cxx" , "C" ) {
                    if ( -e "$inc/$rel_dir/$base$ext" ) {
                        #print "found $inc/$l$ext\n" ;
                        my $f = abs_path("$inc/$rel_dir") . "/$base$ext" ;
                        $resolved_files{$f} = 1 ;
                        $found = 1 ;
                        last ;
                    }
                    elsif ( -e "$inc/$rel_dir/src/$base$ext" ) {
                        #print "found $inc/src/$l$ext\n" ;
                        my $f = abs_path("$inc/$rel_dir/src") . "/$base$ext" ;
                        $resolved_files{$f} = 1 ;
                        $found = 1 ;
                        last ;
                    }
                }
                last if ( $found == 1 ) ;
            }
        }

        if ( $found == 0 ) {
            print STDERR "[33mWarning: Could not find dependency $l[0m\n" ;
        }
    }
    return (sort keys %resolved_files) ;
}

sub write_lib_deps($) {
    my $deps_changed ;
    my ($source_file_name) = @_ ;
    my $contents ;
    {
        # read file in slurp mode.  Keep the scope of undefining $/ to just this read
        local $/ = undef ;
        open SOURCE, $source_file_name or warn 'cannot read $source_file_name' ;
        $contents = <SOURCE> ;
        close SOURCE ;
    }
    my (@resolved_files) = get_lib_deps($contents, $source_file_name) ;

    my ( $file, $dir, $suffix) = fileparse($source_file_name, qr/\.[^.]*/) ;
    my ($lib_dep_file_name) = "build$dir${file}.lib_deps" ;
    if ( ! -e "build$dir" ) {
        make_path("build$dir") ;
    }

    if ( -e $lib_dep_file_name ) {
        open OLDLIBDEP, "$lib_dep_file_name" ;
        my @old_resolved = <OLDLIBDEP> ;
        close OLDLIBDEP ;
        chomp @old_resolved ;
        if ( @old_resolved ~~ @resolved_files ) {
            print "Library dependencies unchanged for $source_file_name\n" ;
            $deps_changed = 0 ;
        } else {
            print "Library dependencies changed for $source_file_name\n" ;
            $deps_changed = 1 ;
        }
    } else {
        $deps_changed = 1 ;
    }

    if ( $deps_changed ) {
        open LIBDEP, ">$lib_dep_file_name" ;
        print LIBDEP map {"$_\n"} @resolved_files ;
        close LIBDEP ;
    }

    return $deps_changed , @resolved_files ;
}

1
