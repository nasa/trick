package find_module ;

use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(find_header_file );

use strict ;

sub find_header_file ($$) {
    my ($relpath, $inc_paths_ref) = @_ ;
    my $file_name ;
    my $rel_dir ;

    foreach ( @{$inc_paths_ref} ) {
        (my $path = $_) =~ s/\\//g ;
        return ( "$path/$relpath" ) if ( -e "$path/$relpath" ) ;
    }

    # Didn't find file
    return("");

}

1;
