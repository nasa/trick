package edit ;

use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(edit_only edit_and_exit);

use strict ;
use gte ;

sub edit_only {
   my ($program, $f, $line_num) = @_ ;
   my $editor ;
   
   $editor = gte("TRICK_EDITOR") ;
   chomp $editor ;
   # Edit the offending file with the user's editor of choice. Use vi as default
   if    ($editor eq "emacs") { system "emacs +$line_num $f &" ; }
   elsif ($editor eq "nedit") { system "nedit +$line_num $f &" ; }
   elsif ($editor eq "vi" or $editor eq "vim" ) { system "xterm -geometry 80x50 -e $editor +$line_num $f &" ; }
   elsif ($editor ne "none" and $editor ne "")  { system "$editor $f &" ; }

}

sub edit_and_exit {
   my ($program, $f, $line_num) = @_ ;
   edit_only(@_) ;

   print "\n\n[31m$program aborted[00m\n\n" ;
   exit(1) ;
}

1;
