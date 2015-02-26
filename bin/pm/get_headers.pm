package get_headers ;

use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(get_headers);

use File::Basename ;
use strict ;
use Cwd 'abs_path';

my ( @inc_paths , @valid_inc_paths ) ;
my %immediate_includes ;
my %visited_files ;

sub parse_file($$) {
    my ($file_name, $sim_ref) = @_ ;
    my $curr_path ;

    if ( ! exists $visited_files{$file_name} ) {
        $visited_files{$file_name} = 1 ;
        $curr_path = dirname($file_name) ;

        open my $fh, $file_name or warn "Could not open file $file_name" ;
        while ( <$fh> ) {
            if ( /^\s*#\s*include\s+\"(.*)\"/ ) {
                #print "[31m$1[00m\n" ;
                my ($include_file) = $1 ;
                my $include_file_full_path ;
                if ( -e "$curr_path/$include_file") {
                    $include_file_full_path = abs_path(dirname("$curr_path/$include_file")) . "/" . basename("$include_file");
                    $immediate_includes{$file_name}{$include_file_full_path} = 1 ;
                    #print "1 $include_file_full_path\n" ;
                    parse_file( $include_file_full_path , $sim_ref );
                } else {
                    my ($found) = 0 ;
                    foreach my $p ( @inc_paths ) {
                        if ( -e "$p/$include_file" || -e "$include_file" ) {
                            if ( -e "$p/$include_file" ) {
                                $include_file_full_path = abs_path(dirname("$p/$include_file")) . "/" . basename("$include_file");
                            } else {
                                $include_file_full_path = $include_file ;
                            }
                            #print "2 $include_file_full_path\n" ;
                            $immediate_includes{$file_name}{$include_file_full_path} = 1 ;
                            $found = 1 ;
                            parse_file( $include_file_full_path , $sim_ref );
                            last ;
                        }
                    }
                    if ( $found == 0 ) {
                        #print "[31mcould not file $include_file in $file_name[00m\n" ;
                    }
                }
            } elsif ( /ICG:/i ) {
                my ($line) = $_ ;
                my ($next_line) ;
                do {
                    $next_line = <$fh> ;
                    $line .= $next_line ;
                } while ( $next_line =~ /^\s*$/ and !eof($fh) ) ;
                if ( $line =~ /\(\s*No\s*\)/si ) {
                    #print "[31mICG no found in $file_name[00m\n" ;
                    $$sim_ref{icg_no}{$file_name} = 1 ;
                }
            }
        }
    }
}

sub get_all_includes($$$) {
    my ($sim_ref , $top_file , $curr_file ) = @_ ;

    foreach my $f ( keys %{$immediate_includes{$curr_file}} ) {
        if ( ! exists $$sim_ref{all_includes}{$top_file}{$f} ) {
             $$sim_ref{all_includes}{$top_file}{$f} = 1 ;
             get_all_includes( $sim_ref , $top_file , $f ) ;
        }
    }
    
}

sub get_headers($$) {

    my ( $sim_ref, $top_file ) = @_ ;

    @inc_paths = $ENV{"TRICK_CFLAGS"} =~ /-I\s*(\S+)/g ;     # get include paths from TRICK_CFLAGS
    push @inc_paths , "$ENV{TRICK_HOME}/trick_source" ;     
    push @inc_paths , "../include" ;     

    # Get only the include paths that exist
    foreach (@inc_paths) {
        push @valid_inc_paths , $_ if ( -e $_ ) ;
    }
    @inc_paths = @valid_inc_paths ;
    
    parse_file($top_file, $sim_ref) ;
    
    # make hash of all downward files that are included by $f
    foreach my $f ( keys %immediate_includes ) {
        get_all_includes($sim_ref , $f , $f) ;
    }
    
    # get all file modification times.  The hash of $top_file plus S_source.hh constitute all include files
    foreach my $f ( (keys %{$$sim_ref{all_includes}{$top_file}}) , $top_file ) {
        $$sim_ref{mod_date}{$f} = (stat $f)[9] ;
    }

}

1 ;
