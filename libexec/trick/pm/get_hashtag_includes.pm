package get_hashtag_includes;

use FindBin qw($RealBin);
use lib "$RealBin/";

use File::Basename;
use Cwd;
use Cwd 'abs_path';
use trick_print;

@ISA    = qw(Exporter);
@EXPORT = qw(get_hashtag_includes);

use strict;

sub get_hashtag_includes($) {
    my ($file_list) = @_;
    my %includes;

    foreach my $file (@$file_list) {
        open FILE, "$file" or die "Failed to open $file\n";
        while (<FILE>) {
            chomp;
            if ( $_ =~ /^#\s*include\s*\"(.*)\"$/ || $_ =~ /^#\s*include\s*\<(.*)\>$/ ) {
                if ( defined($1) ) {
                    chomp $1;
                    $includes{$1} = 1;
                }
            }
        }
    }
    return \%includes;
}

1;
