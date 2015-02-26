#!/usr/bin/perl

use strict ;

my (@all_lines) ;
my ($contents) ;

foreach my $f ( @ARGV ) {
    print "$f\n" ;
    open (FH, "+< $f") ;

    @all_lines = <FH> ;
    $contents = join "" , @all_lines ;

    $contents =~ s/(<dl[^\n]+Python Usage:.*?<\/div>)//igs ;

    seek(FH,0,0) ;
    print FH $contents ;
    truncate(FH, tell(FH)) ;
    close(FH) ;
}

