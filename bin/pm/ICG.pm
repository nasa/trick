package ICG ;

use Exporter ();
@ISA = qw(Exporter);
use vars qw($arg_lang);
@EXPORT = qw(ICG);

use File::Basename ;
use Text::Balanced qw ( extract_bracketed );
use Cwd ;
use strict ;
use gte ;
use MIS ;
use html ;
use trick_print ;
use trick_version ;
use Cwd 'abs_path';

##############################################################################
# Important Hashes
##############################################################################
#
#     VALID-TYPES HASH
#     Key               Value
#     <type-specifier>  Ref to TYPE-INFO HASH
#
# ----------------------------------------------------------------------------
#     TYPE-INFO HASH
#     Key               Value
#     value             Trick parameter type as defined in parameter_types.h
#                       Examples: "TRICK_CHARACTER", "TRICK_INTEGER", etc.
#     name              C/C++ type specifier.
#                       Examples: "char", "int", etc.
#     intrinsic         0= User-defined type. 1= Intrinsic type.
#     class             Parent class.
#     file              File where this type was declared.
#     typedef           0 or 1. Whether declaration starts with struct or union.
#     inherit_class     @ of strings that name the classes from which this class is inherits.
#     virtual           Whether this class inherits virtually.
#     abstract          Whether this class is abstract (and therefore cannot be instantiated). 0=no, 1=yes.
#     def_construct     0 or 1. Indicates whether we should create an io_src_allocate_ routine.

##############################################################################
# Global Variables
##############################################################################

# Variable: $top_file
# Description:
my $top_file ;

# Variable: all_icg_depends_ref
# Description:

my ( $all_icg_depends_ref ) ;

my $verbose ;
my %header ;
my $indent = 3 ;
my %already_processed ;

my @file_levels ;

################################################################################
# SUBROUTINE: ICG
#
# DESCRIPTION: FIXME
#
# PARAMETERS:
# temp_h_files   array of strings representing the names of header files to be processed.
# operation      scalar string that indicates what ICG is supposed to do.
#                Recognized values are "full",  "CP", "sim_services", "single", "s_source", "tree".
# user_defs      array of strings (e.g.: "-DFOO" representing user defines.
# sim_ref        Reference to global SIM-HASH.
#
# RETURNS: FIXME
################################################################################
sub ICG(\@$$$) {

    my ($temp_h_files , $operation, $user_defs, $sim_ref) = @_ ;
    my @h_files ;
    my $wd ;
    my $path_dir ;
    my @include_paths ;
    my @defines ;
    my $curr_file ;
    my (@file_stack , %file_contents ) ;
    my ($f) ;
    my $file_path_dir ;
    my @inc_paths ;
    my @file_order ;
    my %tree ;
    my @icg_exclude_dirs ;
    my ($version, $thread, $year) ;
    my $cc ;

    @h_files = @$temp_h_files ;

    # used by make_makefile
    $all_icg_depends_ref =  \%{$$sim_ref{all_icg_depends}} ;

    $wd = cwd();
    $path_dir = dirname($wd) ;
    if ($path_dir eq ".") {
        $path_dir = cwd();
    }

    ($version, $thread) = get_trick_version() ;
    ($year) = $version =~ /^(\d+)/ ;

    ($cc = gte("TRICK_CC")) =~ s/\n// ;
    @include_paths = $ENV{"TRICK_CFLAGS"} =~ /(-I\s*\S+)/g ; # get include paths from TRICK_CFLAGS
    push @include_paths , ("-I" . $ENV{"TRICK_HOME"} . "/trick_source" , "-I../include") ;
    @defines = $ENV{"TRICK_CFLAGS"} =~ /(-D\S+)/g ;       # get defines from TRICK_CFLAGS
    if ( $ENV{"TRICK_CFLAGS"} !~ /DTRICK_VER=/ ) {
        push @defines , "-DTRICK_VER=$year" ;
    }
    push @defines , "-DTRICK_ICG" ;

    # Break up the $TRICK_ICG_EXCLUDE string based on some delimiter (colon).
    # In an array context, split returns a list of the things that were found.
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

    @inc_paths = $ENV{"TRICK_CFLAGS"} =~ /-I\s*(\S+)/g ;     # get include paths from TRICK_CFLAGS
    # Get only the include paths that exist
    my @valid_inc_paths ;
    foreach (@inc_paths) {
        push @valid_inc_paths , $_ if ( -e $_ ) ;
    }
    @inc_paths = @valid_inc_paths ;

    # return if no header files are to be processed
    if (scalar @h_files == 0)  {
       return -1 ;
    }

    open SLIB , ".icg_no_found" ;
    while ( <SLIB> ) {
        chomp ;
        $$sim_ref{icg_no}{$_} = 1 ;
    }

    while (@h_files) {

        my %temp_hash ;
        my $cc_inc ;
        my $curr_dir ;

        #Clear out variables for new file
        undef %file_contents ;
        undef @file_stack ;
        undef @file_levels ;
        undef @file_order ;

        $curr_file = shift @h_files ;

        #get the absolute path to this header file... use this from now on.
        $curr_file = abs_path(dirname($curr_file)) . "/" . basename($curr_file) ;
        $top_file = $curr_file ;
        $curr_dir = dirname($curr_file) ;

        if ( $operation ne "treedep" and $operation ne "single" ) {
            trick_print($$sim_ref{fh}, "ICG $curr_file\n\n" , "normal_cyan" , $$sim_ref{args}{v}) ;
        }

        open FILE, "$curr_file" or die "Couldn't find file: $curr_file\n";

        # Use the C preprocessor to get all of the include dependencies for the current file
        # This way we can properly take care of #defines with h files
        # When you open a filehandle (FILE) that's already open, Perl
        # implicitly closes it first.
        if ( $curr_file =~ /\.h$/ ) {
           open FILE, "cd $curr_dir ; $cc -E -C @include_paths @defines @$user_defs $curr_file |" ;
        }
        elsif ( $curr_file =~ /(\.hh|\.H|\.h\+\+|\.hxx)$/ ) {
           open FILE, "cd $curr_dir ; $cc -E -C -x c++ @include_paths $cc_inc @defines @$user_defs $curr_file |" ;
        }
        else {
           open FILE, "cd $curr_dir ; $cc -E -C -x c @include_paths $cc_inc @defines @$user_defs $curr_file |" ;
        }

        # read the result of the preprocessor and separate the output to individual files
        while ( <FILE> )  {
            # change file we are in to include dependencies
            if ( /^#(\s*line)?\s*\d+\s*"(.*?)"/ ) {
                $curr_file = $2 ;
                $curr_file =~ s/\/(\/)+/\//g ;

                # Replace directories that are symbolic links
                # with real/absolute paths. Also substitute out
                # /dir_a/dir_b/../file.c with /dir_a/file.c
                $curr_file = abs_path(dirname($curr_file)) . "/" . basename($curr_file) ;

                if ($curr_file !~ /^\/usr\/include/ and
                    $curr_file !~ /^\/usr\/lib/ and
                    ($curr_file =~ /^\/usr\/local\/trick/ # Trick install PREFIX
                     or $curr_file !~ /^\/usr\/local/) and
                    $curr_file !~ /^\/usr\/X11/ and
                    $curr_file !~ /gcc-lib/ and
                    $curr_file !~ /^\/Developer/ and
                    $curr_file !~ /^\/Library\/Developer/ and
                    $curr_file !~ /^\/Applications\/Xcode.app/ and
                    $curr_file !~ /^\/usr\/llvm-gcc-4.2/ and
                    $curr_file !~ /opt\/sfw/ and
                    $curr_file !~ /<built-in>/ and
                    $curr_file !~ /^\/usr\/qnx.*?\/target/ and
                    $curr_file !~ /<command.line>/) {

                    # keep a record of the order the files were visited
                    push @file_order , $curr_file ;
                }
            }

            # add line to file we are in
            if ($curr_file !~ /^\/usr\/include/ and $curr_file !~ /^\/usr\/lib/ and
                $curr_file !~ /^\/usr\/X11/ and $curr_file !~ /gcc-lib/ ) {
               # string concatenate
               $file_contents{$curr_file} .= "$_" ;
            }
        }

        # Convert the flat file output from cc to a tree dependency hash
        foreach $f ( @file_order ) {
            # skip c++ .i files
            next if ( $f =~ /\.i$/ ) ;

            # Use absolute path
            if ( $f =~ /^\// ) {
                my $old_f = $f ;
                $f = abs_path(dirname($f)) . "/" . basename($f) ;
                if ( ! exists $file_contents{$f} ) {
                    $file_contents{$f} = $file_contents{$old_f} ;
                }
            }
            else {
                my $old_f = $f ;
                $f = abs_path(dirname("$curr_dir/$f")) . "/" . basename($f) ;
                if ( ! exists $file_contents{$f} ) {
                    $file_contents{$f} = $file_contents{$old_f} ;
                }
            }

            if (! defined $file_stack[$#file_stack] or $f ne $file_stack[$#file_stack]) {
                # if file name is equal to previous file on stack pop up to that file
                if ($#file_stack ge 1 and $file_stack[$#file_stack - 1] eq $f) {
                    pop @file_stack ;
                }
                else {
                    # make sure this is not the top level file
                    if ( $#file_stack ge 0 ) {
                        # if file name does not exist in it's parent's list add it
                        my $temp_f = quotemeta $f ;
                        if ( !(grep /$temp_f/, @{$$all_icg_depends_ref{$top_file}{$file_stack[$#file_stack]}}) ) {
                            push @{$$all_icg_depends_ref{$top_file}{$file_stack[$#file_stack]}} , $f ;
                        }
                        # if there is no entry yet for this file make one
                        if (!exists $$all_icg_depends_ref{$top_file}{$f}) {
                            @{$$all_icg_depends_ref{$top_file}{$f}} = () ;
                        }
                    }
                    # push this file name onto the stack
                    push @file_stack, $f ;
                    $tree{$f} = 1 ;
                    if ($operation eq "tree" ) {
                        print $$sim_ref{fh}, " " x ($#file_stack * 3) . $f . "\n" ;
                    }
                }
            }
        }

        # if ICG was called with the -tree option exit here.
        return %tree if ($operation eq "tree" or $operation eq "treedep" ) ;

        # Get the realpath of all the files we read in from the C preprocessor
        foreach $f ( keys %file_contents ) {
            my $new_name = abs_path(dirname($f)) . "/" . basename($f) ;
            if ( $new_name ne $f and !exists $file_contents{$new_name} ) {
                $file_contents{$new_name} = $file_contents{$f};
            }
        }

        my %all_file_names ;
        @file_levels = traverse_tree_to_file_levels($top_file, \%all_file_names , \%{$$all_icg_depends_ref{$top_file}} ) ;

        foreach $f (@file_levels) {

            my @structs_info ;
            my @enums_info ;
            my $continue ;

            # skip file if already processed this invocation of ICG
            if ( exists $already_processed{$f} ) {
                next ;
            }
            $already_processed{$f} = 1 ;

            if ( $f =~ /\.C$|\.cc$|\.cxx$|\.cpp$|\.c\+\+$/ ) {
                next ;
            }
            elsif ( $f =~ /\.c$/ ) {
                # MIS
                mis_c($f , $file_contents{$f} , $sim_ref) ;
                next ;
            }
            elsif ( $f =~ /\.i$/ ) {
                next ;
            }

            $continue = 1 ;
            foreach my $ie ( @icg_exclude_dirs ) {
                # if file location begins with $ie (an IGC exclude dir)
                my $dum = $f ;
                if ( $dum =~ s/^\Q$ie// ) {
                    if ( $dum =~ /^\// ) {
                        trick_print($$sim_ref{fh}, "ICG skipping $f (ICG exclude dir $ie)\n" , "normal_yellow" , $$sim_ref{args}{v}) ;
                        $continue = 0 ;
                        last ;  # break out of loop
                    }
                }
            }
            next if ( $continue == 0 ) ;

            if ( exists $$sim_ref{icg_no}{$f} ) {
                trick_print($$sim_ref{fh}, "ICG skipping $f (ICG No found)\n" , "normal_yellow" , $$sim_ref{args}{v}) ;
                next ;
            }

            $file_path_dir = dirname($f) ;
            if ($file_path_dir eq ".") {
                $file_path_dir = cwd();
            }
            $file_path_dir =~ s/\/+$// ;                 # remove trailing slash
            $file_path_dir =~ s/\/include$// ;

            # extract the Trick header
            %header = extract_trick_header( $f, $file_contents{$f}, $verbose , $indent );

            $$sim_ref{python_module}{$f} = $header{python_module} ;
            $$sim_ref{python_module}{$f} =~ s/\(|\)|\s+//g ;

            if ( $operation eq "CP" ) {

                trick_print($$sim_ref{fh}, "Getting dependencies for $f\n" , "normal_white" , $$sim_ref{args}{v});

                delete $$sim_ref{head_deps}{$f} ;

                # add any external dependencies from an .exd file
                my ($exd_file_name) = $f ;
                my @exd_file_list ;
                undef @exd_file_list ;
                $exd_file_name =~ s/\.[^\/]*$/\.exd/ ;
                if ( open EXD , "$exd_file_name" ) {
                    while ( <EXD> ) {
                        push @exd_file_list , $_ if ( /\.o\s*$/ and !/^\s*#/ ) ;
                    }
                    chomp @exd_file_list ;
                }

                # we have library dependencies in an h file. ( possible for c++ )
                if (exists $header{libdep} or (scalar @exd_file_list != 0) ) {
                    if ( $header{libdep} =~ /#/ ) {
                        (my $cc = gte("TRICK_CC")) =~ s/\n// ;
                        my @defines = $ENV{"TRICK_CFLAGS"} =~ /(-D\S+)/g ;
                        my $temp ;
                        open FILE, "echo \"$header{libdep}\" | cpp -P @defines |" ;
                        while ( <FILE> ) {
                            $temp .= $_ ;
                        }
                        $header{libdep} = $temp ;
                    }
                    my @lib_list = split /\)[ \t\n\r\*]*\(/ , $header{libdep} ;
                    push @lib_list , @exd_file_list ;
                    foreach my $l (@lib_list) {
                        my $found = 0 ;
                        $l =~ s/\(|\)|\s+//g ;
                        $l =~ s/\${(.+?)}/$ENV{$1}/eg ;
                        next if ( $l eq "" ) ;

                        if ( $l !~ /\.o$/ ) {
                            foreach my $inc ( dirname($f) , @inc_paths) {
                                if ( -e "$inc/$l" ) {
                                    #print "found $inc/$l$ext\n" ;
                                    my $f = abs_path(dirname("$inc/$l")) . "/" . basename("$inc/$l") ;
                                    push @{$$sim_ref{mis_entry_files}} , $f ;
                                    push @{$$sim_ref{head_deps}{$f}{files}} , $f ;
                                    $found = 1 ;
                                    last ;
                                }
                            }
                        } else {
                            $l =~ s/o$// ;
                            my $file_name = basename($l) ;
                            my $dir_name = dirname($l) ;
                            foreach my $inc ( $file_path_dir , @inc_paths) {
                                foreach my $ext ( "cpp" , "cc" , "c" , "c++" , "cxx" , "C" ) {
                                    if ( -e "$inc/$l$ext" ) {
                                        #print "found $inc/$l$ext\n" ;
                                        my $f = abs_path(dirname("$inc/$l$ext")) . "/" . basename("$inc/$l$ext") ;
                                        push @{$$sim_ref{mis_entry_files}} , $f ;
                                        push @{$$sim_ref{head_deps}{$f}{files}} , $f ;
                                        $found = 1 ;
                                        last ;
                                    }
                                    elsif ( -e "$inc/$dir_name/src/$file_name$ext" ) {
                                        #print "found $inc/$dir_name/src/$file_name$ext\n" ;
                                        my $f = abs_path("$inc/$dir_name/src/$file_name$ext") ;
                                        push @{$$sim_ref{mis_entry_files}} , $f ;
                                        push @{$$sim_ref{head_deps}{$f}{files}} , $f ;
                                        $found = 1 ;
                                        last ;
                                    }
                                }
                                last if ( $found == 1 ) ;
                            }
                        }

                        if ( $found == 0 ) {
                            trick_print($$sim_ref{fh}, "$f: Warning: Could not find dependency $l" . "o\n" ,
                                    "normal_yellow" , $$sim_ref{args}{v}) ;
                        }
                    }
                }
                next ;
            }

        } # End of foreach on filelevel

    } # End of gigantic while loop

    return 0 ;

} # end of ICG

sub traverse_tree_to_file_levels($$$$$) {
    my ($file, $all_file_names_ref , $curr_tree ) = @_ ;
    my $a ;
    my @lower_levels ;
    my $io_src_file ;
    my ($ret_stat , @ret_files) ;
    my $ret ;

    foreach $a ( @{$$curr_tree{$file}} ) {
        if ( ($a ne $file) and !(exists $$all_file_names_ref{$a}) ) {
            $$all_file_names_ref{$a} = 1 ;
            ( @ret_files ) = &traverse_tree_to_file_levels($a, $all_file_names_ref , $curr_tree ) ;
            push @lower_levels , @ret_files ;

        }
    }
    $$all_file_names_ref{$file} = 1 ;
    return ( @lower_levels , $file ) ;
}

1;
