package trick_version ;

use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(get_trick_version);


use strict;

sub get_trick_version() {

    my ($current_version , $thread_version , $service_issues ) ;
    my (@all_lines , $file_contents) ;
 
    if ( !exists $ENV{"TRICK_HOME"} ) {
        die "TRICK_HOME is not set\n" ;
    }
 
    open FILE, "$ENV{\"TRICK_HOME\"}/share/trick/trick_ver.txt" or
           die "trick_version: Couldn't find $ENV{\"TRICK_HOME\"}/share/trick/trick_ver.txt\n" ;
    @all_lines = <FILE> ;
    close FILE ;
    $file_contents  = join  "" , @all_lines ;
 
    ($current_version) = $file_contents =~ /current_version\s*=\s*"([^"]+)"/ ;
    ($thread_version) = $file_contents =~ /thread_version\s*=\s*"([^"]+)"/ ;
    ($service_issues) = $file_contents =~ /service_issues\s*=\s*"(.+)"/s ;
 
    return ( $current_version , $thread_version , $service_issues ) ;
}

1 ;
