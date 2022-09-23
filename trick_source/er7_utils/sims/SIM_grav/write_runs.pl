use strict;

## my $order = 32;
## my $steps_scale = 4;
my $order = 8;
my $steps_scale = 1;

my %integration = (
    abm             => { id     => 'ABM_Method',              steps =>  2.5},
    beeman          => { er7_id => 'Beeman',                  steps =>  2},
    euler           => { id     => 'Euler',                   steps =>  1.5},
    euler_cromer    => { id     => 'Euler_Cromer',            steps =>  1.5},
    mm4             => { id     => 'Modified_Midpoint_4',     steps =>  3},
    position_verlet => { er7_id => 'PositionVerlet',          steps =>  1.5},
    nl2             => { id     => 'Nystrom_Lear_2',          steps =>  2},
    rk2             => { id     => 'Runge_Kutta_2',           steps =>  2},
    rk2_midpoint    => { er7_id => 'RK2Midpoint',             steps =>  2},
    rk4             => { id     => 'Runge_Kutta_4',           steps =>  4},
    rkf45           => { id     => 'Runge_Kutta_Fehlberg_45', steps =>  6},
    rkf78           => { id     => 'Runge_Kutta_Fehlberg_78', steps => 12},
    rkg4            => { id     => 'Runge_Kutta_Gill_4',      steps =>  4},
    velocity_verlet => { er7_id => 'VelocityVerlet',          steps =>  2}
);

sub make_files ($$) {
   my ($technique, $use_trick_integ) = @_;
   my $prefix = join '_', +($use_trick_integ ? 'trick' : 'er7'), $technique;
   my $TECHNIQUE       = $integration{$technique}->{id};
   my $USE_TRICK_INTEG = $use_trick_integ ? 'True' : 'False';
   my $DERIVS_PER_STEP = $integration{$technique}->{steps};

   if (defined $TECHNIQUE) {
      my $sed = join ' ',
                "-e 's/DEGREE_ORDER/$order/'",
                "-e 's/TRICK_TECHNIQUE/$TECHNIQUE/'",
                "-e 's/ER7_TECHNIQUE/Integration.Unspecified/'",
                "-e 's/USE_TRICK_INTEG/$USE_TRICK_INTEG/'",
                "-e 's/DERIVS_PER_STEP/($steps_scale*$DERIVS_PER_STEP)/'";

      system "sed $sed long_template.py > RUN_test/long_$prefix.py";
      system "sed $sed short_template.py > RUN_test/$prefix.py";
   }

   elsif (! $use_trick_integ) {
      $TECHNIQUE = $integration{$technique}->{er7_id};
      my $sed = join ' ',
                "-e 's/DEGREE_ORDER/$order/'",
                "-e 's/TRICK_TECHNIQUE/User_Defined/'",
                "-e 's/ER7_TECHNIQUE/Integration.$TECHNIQUE/'",
                "-e 's/USE_TRICK_INTEG/$USE_TRICK_INTEG/'",
                "-e 's/DERIVS_PER_STEP/($steps_scale*$DERIVS_PER_STEP)/'";
      system "sed $sed long_template.py > RUN_test/long_$prefix.py";
      system "sed $sed short_template.py > RUN_test/$prefix.py";
   }
}

foreach my $technique (keys %integration) {
   foreach my $use_trick_integ (0, 1) {
      make_files $technique, $use_trick_integ;
   }
}
