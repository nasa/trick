use strict;
use Getopt::Long;

sub get_sim_values ($$);
sub get_sim_result ($$$);


my $program = "S_main_$ENV{TRICK_HOST_CPU}.exe";
my @techniques = qw (
   abm
   beeman
   euler
   euler_cromer
   mm4
   nl2
   position_verlet
   rk2
   rk2_midpoint
   rk4
   rkf45
   rkf78
   rkg4
   velocity_verlet
);

my $priority = -20;
my $repeat_count = 4;

# Parse the command line.
GetOptions (
   'priority=i'   => \$priority,
   'repeat=i'     => \$repeat_count)
   or die "Bad option";

if (scalar @ARGV) {
   my %techniques;
   @techniques{@techniques} = (1) x scalar @techniques;
   @techniques = ();
   foreach my $technique (@ARGV) {
      if (defined $techniques{$technique}) {
         push @techniques, $technique;
      }
      else {
         warn "Unknown technique $technique\n";
      }
   }
}

setpriority 1, 0, $priority;

foreach my $technique (@techniques) {
   my @er7_vals   = get_sim_values $technique, 'er7';
   my @trick_vals = get_sim_values $technique, 'trick';

   if (scalar @trick_vals) {
      warn "End time mismatch\n" unless $er7_vals[0] == $trick_vals[0];
      warn "Results mismatch\n" if abs($er7_vals[1] - $trick_vals[1]) > 1e-4;
   }

   if ($repeat_count > 0) {
      if (scalar @trick_vals) {
         printf "%-15s  %6d  %11.6f  %7.3f %7.3f  %6.2f\n",
                $technique,
                $er7_vals[0], $er7_vals[1], $er7_vals[2], $trick_vals[2],
                100 * (1 - $er7_vals[2]/$trick_vals[2]);
      }
      else {
         printf "%-15s  %6d  %11.6f  %7.3f  ------   -----\n",
                $technique,
                $er7_vals[0], $er7_vals[1], $er7_vals[2];
      }
   }
   else {
      printf "%-15s  %6d  %11.6f\n", $technique, $er7_vals[0], $er7_vals[1];
   }
}


sub get_sim_values ($$) {
   my ($technique, $integ) = @_;
   my $input = "RUN_test/${integ}_${technique}.py";
   return () unless (-f $input);

   my $x300 = get_sim_result $input, 'time = *300', 's/.*, *//';

   $input = "RUN_test/long_${integ}_${technique}.py";

   `grep 'end_time =' $input` =~ /.*= *(.*)/;
   my $end = eval "$1 + 0";

   my @times = ();
   foreach (1 .. $repeat_count) {
      push @times, get_sim_result ($input, 'ACTUAL', 's/.*: *//');
   }
   @times = sort {$a <=> $b} @times;

   return ($end, $x300, $times[0]);
}


sub get_sim_result ($$$) {
   my ($input, $grep, $sed) = @_;
   my $exec = "./$program $input | grep '$grep' | sed -e '$sed'";
   open RUN, "$exec |" or die;
   my @output = <RUN>;
   close RUN;
   scalar @output == 1 or do { die "$input\n@output" };
   chomp $output[0];
   return $output[0] + 0;
}
