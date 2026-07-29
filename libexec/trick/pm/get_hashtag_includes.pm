package get_hashtag_includes;

use strict;
use warnings;

use FindBin qw($RealBin);
use lib "$RealBin/";

use Exporter ();

our @ISA    = qw(Exporter);
our @EXPORT = qw(get_hashtag_includes);

sub get_hashtag_includes($) {
    my ($file_list) = @_;
    my %includes;

    foreach my $file (@$file_list) {
        open( my $fh, '<', $file ) or die "Failed to open $file: $!\n";
        while (<$fh>) {
            chomp;
            if ( $_ =~ /^\s*#\s*include\s*"([^"]*)"/ || $_ =~ /^\s*#\s*include\s*<([^>]*)>/ ) {
                $includes{$1} = 1;
            }
        }
        close($fh);
    }
    return \%includes;
}

1;
