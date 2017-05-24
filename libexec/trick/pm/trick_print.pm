package trick_print ;

use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(trick_print trick_formatted_print);

use strict ;

my %message_type = ( 
                     "title_white"    , { "color" , 00 , "level" , 1 } ,
                     "title_red"      , { "color" , 31 , "level" , 1 } ,
                     "title_green"    , { "color" , 32 , "level" , 1 } ,
                     "title_yellow"   , { "color" , 33 , "level" , 1 } ,
                     "title_blue"     , { "color" , 34 , "level" , 1 } ,
                     "title_magenta"  , { "color" , 35 , "level" , 1 } ,
                     "title_cyan"     , { "color" , 36 , "level" , 1 } ,
                     "normal_white"   , { "color" , 00 , "level" , 2 } ,
                     "normal_red"     , { "color" , 31 , "level" , 2 } ,
                     "normal_green"   , { "color" , 32 , "level" , 2 } ,
                     "normal_yellow"  , { "color" , 33 , "level" , 2 } ,
                     "normal_blue"    , { "color" , 34 , "level" , 2 } ,
                     "normal_magenta" , { "color" , 35 , "level" , 2 } ,
                     "normal_cyan"    , { "color" , 36 , "level" , 2 } ,
                     "debug_white"    , { "color" , 00 , "level" , 3 } ,
                     "debug_red"      , { "color" , 31 , "level" , 3 } ,
                     "debug_green"    , { "color" , 32 , "level" , 3 } ,
                     "debug_yellow"   , { "color" , 33 , "level" , 3 } ,
                     "debug_blue"     , { "color" , 34 , "level" , 3 } ,
                     "debug_magenta"  , { "color" , 35 , "level" , 3 } ,
                     "debug_cyan"     , { "color" , 36 , "level" , 3 } 
                   ) ;

sub trick_print($$$$) {
        
    my ($fh, $message, $mt , $verbose) = @_ ;
 
    # print at least lvl 2 messages to the file
    if ( defined $fh ) {
        my ($temp_verbose) = $verbose ;
        $temp_verbose = 2 if ($temp_verbose < 2) ;
        if ( $temp_verbose >= $message_type{$mt}{level} ) {
            print $fh "$message" ;
        }
    }

    # print the message to the screen
    if ( $verbose >= $message_type{$mt}{level} ) {
        $message =~ s/(\n)?$/[00m$1/s ;
        print "[$message_type{$mt}{color}m$message" ;
    }
}

sub trick_formatted_print {
    my ($file, @strings) = @_;
    for (my $i = 0; $i < @strings; ++$i) {
        print $strings[$i];
        if ($i % 2) {
            print $file $strings[$i];
        }
    }
}

1;
