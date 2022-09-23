package get_paths ;

use Exporter ();
use Cwd 'abs_path' ;
@ISA = qw(Exporter);
@EXPORT = qw(get_paths get_include_paths get_defines get_containing_path);

use strict ;

# Get environment variable, split on colons, strip leading/trailing whitespace, remove empty elements, get absolute paths, remove more empty elements
sub get_paths {
    return grep { $_ ne ''} map abs_path($_), grep { $_ ne '' } map { s/(^\s+|\s+$)//g ; $_ } split /:/, $ENV{$_[0]} ;
}

sub get_include_paths {
    return "$ENV{TRICK_CFLAGS} $ENV{TRICK_CXXFLAGS}" =~ /-(?:I|isystem)(\S+)/g
}

sub get_defines {
    return "$ENV{TRICK_CFLAGS} $ENV{TRICK_CXXFLAGS}" =~ /-D\S+/g
}

sub get_containing_path {
    my $path = shift;
    foreach my $exclude_path (@_) {
        if ( $path =~ /^\Q$exclude_path\E(.*)/ or abs_path($path) =~ /^\Q$exclude_path\E(.*)/ ) {
            return $exclude_path ;
        }
    }
    return 0 ;
}

1;
