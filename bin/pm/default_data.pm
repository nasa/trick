package default_data ;

use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(make_default_data);

use strict ;

use lib $ENV{"TRICK_HOME"} . "/bin/pm" ;
use gte ;
use trick_print ;

#--------------------------------------------------------------
# Make Default Data

sub make_default_data($) {

    my ($sim_ref) = @_ ;
    my $title ;
    my @all_lines ;
    my $date = localtime() ;
    my $user ;
    my @include_paths ;

    if (open FILE, "Title") {
        @all_lines = <FILE> ;
        $title = join "" , @all_lines ;
        chomp $title ;
        close FILE ;
    }
    else {
        $title = "No Title Specified" ;
    }

    $user = gte("USER");
    chomp $user ;

    open S_DEF, ">S_default.dat" or die "Could not open S_default.dat for writing" ;

    my ($sec,$min,$hour,$mday,$mon,$year) = localtime ;
    $year += 1900 ;
    $mon += 1 ;
    my ($login) = getpwuid($<) ;

    #---------------------------
    # Print version - date
    printf S_DEF "\n// Created $year/%02d/%02d %02d\:%02d\:%02d $login\n",
     $mon , $mday , $hour , $min , $sec ;

    print S_DEF "
// TITLE: $title
// DATE:  $date
// USER:  $user\n\n" ;

    my @default_data_list ;
    @include_paths = $ENV{"TRICK_CFLAGS"} =~ /-I\s*(\S+)/g ; # get include paths from TRICK_CFLAGS

    foreach my $dd_comment ( @{$$sim_ref{default_data}} ) {
        $dd_comment =~ s/\(\s*\(/\(/sg ;
        $dd_comment =~ s/\)\s*\)/\)/sg ;
        @default_data_list = $dd_comment =~  m/\((.+?)\)/sg ;
        foreach my $dd ( @default_data_list ) {
            my $file_found = 0 ;
            my ($type , $var , $file_name) = split /\s+/ , $dd ;
            if ( $file_name eq "" ) {
                # not enough fields
                trick_print($$sim_ref{fh},"Default data does not have \"TYPE VAR FILE\" syntax\n$dd\n", "title_red", $$sim_ref{args}{v}) ;
                exit -1 ;
            }
            print S_DEF "\n// from $type $var $file_name\n\n" ;
            (my $cc = gte("TRICK_CC")) =~ s/\n// ;
            foreach my $inc ( @include_paths ) {
                if ( -e  "$inc/$file_name") {
                    open DEFDATA, "$cc -E -x c -D$type=$var $inc/$file_name |" ;
                    while ( <DEFDATA> ) {
                        print S_DEF if ( !/^#\s\d+/ )
                    }
                    $file_found = 1 ;
                    last ;
                }
            }
            if ( $file_found == 0 ) {
                print "\033[31mCould not find default data file $file_name\033[00m\n" ;
            }
        }
    }

    return ;

}

1;
