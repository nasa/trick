#!/usr/bin/perl

#********************************************************************
# Program: condense_list.awk
# 
#    This is an PERL script used to condense a list of parameters
#
#********************************************************************
# Creation:
#    Author:  Alex Lin
#    Company: NASA
#    Date:    3/21/2001
#********************************************************************

#USAGE condense_list.pl [s=<char>] env vars or values

#get separator character
if ($ARGV[0] =~ /^s=(\S+)/) {
    $sp = $1;
    shift @ARGV
}
else {
    $sp = " ";
}

while (@ARGV) {
    $curr = shift @ARGV ;
    if (exists $ENV{$curr} ) {
        $curr = $ENV{$curr};
    }
    @curr_sp = (split /$sp/ , $curr) ;
    splice @all, $#all + 1 , 0 , @curr_sp ;
}

print "$all[0]";
for ($i = 1 ; $i <= $#all ; $i++) {
    $found = 0 ;
    for ($j = 0 ; $j < $i ; $j++) {
        if ( $all[$i] eq $all[$j] ) {
            $found = 1;
        }
    }
    if ( $found eq 0 ) {
        print "$sp$all[$i]"
    }
}
print "\n"

