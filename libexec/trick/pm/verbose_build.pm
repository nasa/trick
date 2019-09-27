package verbose_build ;

use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(verbose_build);

use strict ;

sub verbose_build {
    return (exists($ENV{'TRICK_VERBOSE_BUILD'}) or exists($ENV{'VERBOSE'}));
}

1;
