package get_paths ;

use Exporter ();
use Cwd 'abs_path' ;
@ISA = qw(Exporter);
@EXPORT = qw(get_paths);

use strict ;

# Get environment variables, split on colons, strip leading/trailing whitespace, remove empty elements, get absolute paths then remove more empty elements
sub get_paths {
    return grep { $_ ne ''} map abs_path($_), grep { $_ ne '' } map { s/(^\s+|\s+$)//g ; $_ } split /:/, $ENV{$_[0]} ;
}

1;
