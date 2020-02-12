#!/usr/bin/perl
package launch_java ;
@ISA = qw(Exporter) ;
@EXPORT = qw(launch_java) ;

# launch_java determines trick_home based on execution location of the calling script.
use File::Basename ;
use Cwd 'abs_path' ;
use gte ;

sub launch_java($$) {

    my ($name, $application ) = @_ ;

    if ( ! exists $ENV{TRICK_HOME} ) {
        $trick_bin = dirname(abs_path($0)) ;
        $trick_home = dirname($trick_bin) ;

        # set TRICK_HOME based on the value of trick_home
        $ENV{TRICK_HOME} = $trick_home ;
    }

    if ( -e "$ENV{TRICK_HOME}/libexec/trick" ) {
        $lib_dir = "libexec" ;
    } else {
        $lib_dir = "lib" ;
    }
    $java_dir = "$ENV{TRICK_HOME}/$lib_dir/trick/java" ;
    
    $host_cpu = gte("TRICK_HOST_CPU") ;
    chomp($host_cpu) ;
    $ENV{TRICK_HOST_CPU} = $host_cpu ;

    $command = "java -jar ";

    if ( $^O eq "darwin" ) {
        $command .= "\\
             -Xdock:name=\"$name\" \\
             -Xdock:icon=$java_dir/build/classes/trick/common/resources/trick_icon.png \\
             -Djava.net.preferIPv4Stack=true \\" ;        
    }

    $command .= "$java_dir/build/$application.jar ";

    foreach (@ARGV) {
       $command .= "$_ ";
    }

    system $command ;
    exit $? >> 8 ;
}

1 ;
