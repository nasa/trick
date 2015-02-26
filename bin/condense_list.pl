#!/usr/bin/perl

#*****************************************************************************#
#                                                                             #
# Trick Simulation Environment Software                                       #
#                                                                             #
# Copyright (c) 1996,1997 LinCom Corporation, Houston, TX                     #
# All rights reserved.                                                        #
#                                                                             #
# Copyrighted by LinCom Corporation and proprietary to it. Any unauthorized   #
# use of Trick Software including source code, object code or executables is  #
# strictly prohibited and LinCom assumes no liability for such actions or     #
# results thereof.                                                            #
#                                                                             #
# Trick Software has been developed under NASA Government Contracts and       #
# access to it may be granted for Government work by the following contact:   #
#                                                                             #
# Contact: Charles Gott, Branch Chief                                         #
#          Simulation and Graphics Branch                                     #
#          Automation, Robotics, & Simulation Division                        #
#          NASA, Johnson Space Center, Houston, TX                            #
#                                                                             #
#*****************************************************************************#
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

# $Id: condense_list.pl 49 2009-02-02 22:37:59Z lin $

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

