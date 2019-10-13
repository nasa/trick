package get_paths ;

use Exporter ();
use Cwd 'abs_path' ;
@ISA = qw(Exporter);
@EXPORT = qw(get_paths get_include_paths get_defines is_path_in);

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

sub is_path_in {
    foreach my $path (get_paths($_[1])) {
        if ( $_[0] =~ /^\Q$path\E(.*)/ or abs_path($_[0]) =~ /^\Q$path\E(.*)/ ) {
            return 1 ;
        }
    }
    return 0 ;
}

1;
