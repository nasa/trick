package trick_units ;

# $Id: trick_units.pm 3449 2014-02-25 22:07:15Z dstrauss $

%units_info = (
"s" => ["seconds", "Standard", "Time"],
"ps" => ["pico-seconds", "Standard", "Time"],
"ns" => ["nano-seconds", "Standard", "Time"],
"us" => ["micro-seconds", "Standard", "Time"],
"ms" => ["milli-seconds", "Standard", "Time"],
"cs" => ["centi-seconds", "Standard", "Time"],
"ds" => ["deci-seconds", "Standard", "Time"],
"das" => ["deca-seconds", "Standard", "Time"],
"hs" => ["hecto-seconds", "Standard", "Time"],
"ks" => ["kilo-seconds", "Standard", "Time"],
"Ms" => ["mega-seconds", "Standard", "Time"],
"Gs" => ["giga-seconds", "Standard", "Time"],
"Ts" => ["tera-seconds", "Standard", "Time"],
"min" => ["minutes", "Standard", "Time"],
"hr" => ["hours", "Standard", "Time"],
"day" => ["days", "Standard", "Time"],
"m" => ["meters", "Metric", "Length"],
"M" => ["meters", "Metric", "Length"],
"pm" => ["pico-meters", "Metric", "Length"],
"nm" => ["nano-meters", "Metric", "Length"],
"um" => ["micro-meters", "Metric", "Length"],
"mm" => ["milli-meters", "Metric", "Length"],
"cm" => ["centi-meters", "Metric", "Length"],
"dm" => ["deci-meters", "Metric", "Length"],
"dam" => ["deca-meters", "Metric", "Length"],
"hm" => ["hecto-meters", "Metric", "Length"],
"km" => ["kilo-meters", "Metric", "Length"],
"Mm" => ["mega-meters", "Metric", "Length"],
"Gm" => ["giga-meters", "Metric", "Length"],
"Tm" => ["tera-meters", "Metric", "Length"],
"ft" => ["feet", "English", "Length"],
"kft" => ["kilo-feet", "English", "Length"],
"in" => ["inches", "English", "Length"],
"yd" => ["yards", "English", "Length"],
"mi" => ["miles", "English", "Length"],
"n.m." => ["nautical-miles", "English", "Length"],
"r" => ["radians", "Standard", "Angle"],
"pr" => ["pico-radians", "Standard", "Angle"],
"nr" => ["nano-radians", "Standard", "Angle"],
"ur" => ["micro-radians", "Standard", "Angle"],
"mr" => ["milli-radians", "Standard", "Angle"],
"cr" => ["centi-radians", "Standard", "Angle"],
"dr" => ["deci-radians", "Standard", "Angle"],
"dar" => ["deca-radians", "Standard", "Angle"],
"kr" => ["kilo-radians", "Standard", "Angle"],
"Mr" => ["mega-radians", "Standard", "Angle"],
"Gr" => ["giga-radians", "Standard", "Angle"],
"Tr" => ["tera-radians", "Standard", "Angle"],
"d" => ["degrees", "Standard", "Angle"],
"as" => ["arc-seconds", "Standard", "Angle"],
"am" => ["arc-minutes", "Standard", "Angle"],
"rev" => ["revolutions", "Standard", "Angle"],
"g" => ["grams", "Metric", "Mass"],
"pg" => ["pico-grams", "Metric", "Mass"],
"ng" => ["nano-grams", "Metric", "Mass"],
"ug" => ["micro-grams", "Metric", "Mass"],
"mg" => ["milli-grams", "Metric", "Mass"],
"cg" => ["centi-grams", "Metric", "Mass"],
"dg" => ["deci-grams", "Metric", "Mass"],
"dag" => ["deca-grams", "Metric", "Mass"],
"hg" => ["hecto-grams", "Metric", "Mass"],
"kg" => ["kilo-grams", "Metric", "Mass"],
"Mg" => ["mega-grams", "Metric", "Mass"],
"Gg" => ["giga-grams", "Metric", "Mass"],
"Tg" => ["tera-grams", "Metric", "Mass"],
"mt" => ["metric-tons", "Metric", "Mass"],
"sl" => ["slugs", "English", "Mass"],
"lbm" => ["pound(mass)", "English", "Mass"],
"N" => ["newtons", "Metric", "Force"],
"pN" => ["pico-newtons", "Metric", "Force"],
"nN" => ["nano-newtons", "Metric", "Force"],
"uN" => ["micro-newtons", "Metric", "Force"],
"mN" => ["milli-newtons", "Metric", "Force"],
"cN" => ["centi-newtons", "Metric", "Force"],
"dN" => ["deci-newtons", "Metric", "Force"],
"daN" => ["deca-newtons", "Metric", "Force"],
"hN" => ["hecto-newtons", "Metric", "Force"],
"kN" => ["kilo-newtons", "Metric", "Force"],
"MN" => ["mega-newtons", "Metric", "Force"],
"GN" => ["giga-newtons", "Metric", "Force"],
"TN" => ["tera-newtons", "Metric", "Force"],
"oz" => ["ounzes", "English", "Force"],
"lbf" => ["pound(force)", "English", "Force"],
"v" => ["volts", "Standard", "Voltage"],
"pv" => ["pico-volts", "Standard", "Voltage"],
"nv" => ["nano-volts", "Standard", "Voltage"],
"uv" => ["micro-volts", "Standard", "Voltage"],
"mv" => ["milli-volts", "Standard", "Voltage"],
"cv" => ["centi-volts", "Standard", "Voltage"],
"dv" => ["deci-volts", "Standard", "Voltage"],
"dav" => ["deca-volts", "Standard", "Voltage"],
"hv" => ["hecto-volts", "Standard", "Voltage"],
"kv" => ["kilo-volts", "Standard", "Voltage"],
"Mv" => ["mega-volts", "Standard", "Voltage"],
"Gv" => ["giga-volts", "Standard", "Voltage"],
"Tv" => ["tera-volts", "Standard", "Voltage"],
"amp" => ["amperes", "Standard", "Current"],
"pamp" => ["pico-amperes", "Standard", "Current"],
"namp" => ["nano-amperes", "Standard", "Current"],
"uamp" => ["micro-amperes", "Standard", "Current"],
"mamp" => ["milli-amperes", "Standard", "Current"],
"camp" => ["centi-amperes", "Standard", "Current"],
"damp" => ["deci-amperes", "Standard", "Current"],
"daamp" => ["deca-amperes", "Standard", "Current"],
"hamp" => ["hecto-amperes", "Standard", "Current"],
"kamp" => ["kilo-amperes", "Standard", "Current"],
"Mamp" => ["mega-amperes", "Standard", "Current"],
"Gamp" => ["giga-amperes", "Standard", "Current"],
"Tamp" => ["tera-amperes", "Standard", "Current"],
"ohm" => ["ohms", "Standard", "Resistance"],
"pohm" => ["pico-ohms", "Standard", "Resistance"],
"nohm" => ["nano-ohms", "Standard", "Resistance"],
"uohm" => ["micro-ohms", "Standard", "Resistance"],
"mohm" => ["milli-ohms", "Standard", "Resistance"],
"cohm" => ["centi-ohms", "Standard", "Resistance"],
"dohm" => ["deci-ohms", "Standard", "Resistance"],
"daohm" => ["deca-ohms", "Standard", "Resistance"],
"hohm" => ["hecto-ohms", "Standard", "Resistance"],
"kohm" => ["kilo-ohms", "Standard", "Resistance"],
"Mohm" => ["mega-ohms", "Standard", "Resistance"],
"Gohm" => ["giga-ohms", "Standard", "Resistance"],
"Tohm" => ["tera-ohms", "Standard", "Resistance"],
"C" => ["degrees-celsius", "Metric", "Temperature"],
"K" => ["degrees-kelvin", "Metric", "Temperature"],
"F" => ["degrees-fahrenheit", "English", "Temperature"],
"R" => ["degrees-rankine", "English", "Temperature"],
"dB" => ["decibels", "Standard", "Sound"],
"J" => ["joule", "Metric", "Energy"],
"pJ" => ["pico-joule", "Metric", "Energy"],
"nJ" => ["nano-joule", "Metric", "Energy"],
"uJ" => ["micro-joule", "Metric", "Energy"],
"mJ" => ["milli-joule", "Metric", "Energy"],
"cJ" => ["centi-joule", "Metric", "Energy"],
"dJ" => ["deci-joule", "Metric", "Energy"],
"daJ" => ["deca-joule", "Metric", "Energy"],
"hJ" => ["hecto-joule", "Metric", "Energy"],
"kJ" => ["kilo-joule", "Metric", "Energy"],
"MJ" => ["mega-joule", "Metric", "Energy"],
"GJ" => ["giga-joule", "Metric", "Energy"],
"TJ" => ["tera-joule", "Metric", "Energy"],
"BTU" => ["british-thermal", "English", "Energy"],
"TNT" => ["ton-tnt", "English", "Energy"],"MHz" => ["mega-hertz", "Metric", "Frequency"],
"GHz" => ["giga-hertz", "Metric", "Frequency"],
"THz" => ["tera-hertz", "Metric", "Frequency"],
"W" => ["watt", "Metric", "Power"],
"pW" => ["pico-watt", "Metric", "Power"],
"nW" => ["nano-watt", "Metric", "Power"],
"uW" => ["micro-watt", "Metric", "Power"],
"mW" => ["milli-watt", "Metric", "Power"],
"cW" => ["centi-watt", "Metric", "Power"],
"dW" => ["deci-watt", "Metric", "Power"],
"daW" => ["deca-watt", "Metric", "Power"],
"hW" => ["hecto-watt", "Metric", "Power"],
"kW" => ["kilo-watt", "Metric", "Power"],
"MW" => ["mega-watt", "Metric", "Power"],
"GW" => ["giga-watt", "Metric", "Power"],
"TW" => ["tera-watt", "Metric", "Power"],
"hp" => ["horse-power", "English", "Power"],
"Pa" => ["pascal", "Metric", "Pressure"],
"pPa" => ["pico-pascal", "Metric", "Pressure"],
"nPa" => ["nano-pascal", "Metric", "Pressure"],
"uPa" => ["micro-pascal", "Metric", "Pressure"],
"mPa" => ["milli-pascal", "Metric", "Pressure"],
"cPa" => ["centi-pascal", "Metric", "Pressure"],
"dPa" => ["deci-pascal", "Metric", "Pressure"],
"daPa" => ["deca-pascal", "Metric", "Pressure"],
"hPa" => ["hecto-pascal", "Metric", "Pressure"],
"kPa" => ["kilo-pascal", "Metric", "Pressure"],
"MPa" => ["mega-pascal", "Metric", "Pressure"],
"GPa" => ["giga-pascal", "Metric", "Pressure"],
"TPa" => ["tera-pascal", "Metric", "Pressure"],
"psi" => ["pounds-sq-inch", "English", "Pressure"],
"atm" => ["atmospheres", "Metric", "Pressure"],
"mmHg" => ["milli-meters-mercury", "Metric", "Pressure"],
"inHg" => ["inches-mercury", "English", "Pressure"],
"mmWater" => ["milli-meters-water", "Metric", "Pressure"],
"inWater" => ["inches-water", "English", "Pressure"],
"l" => ["liter", "Metric", "Volume"],
"pl" => ["pico-liter", "Metric", "Volume"],
"nl" => ["nano-liter", "Metric", "Volume"],
"ul" => ["micro-liter", "Metric", "Volume"],
"ml" => ["milli-liter", "Metric", "Volume"],
"cl" => ["centi-liter", "Metric", "Volume"],
"dl" => ["deci-liter", "Metric", "Volume"],
"dal" => ["deca-liter", "Metric", "Volume"],
"hl" => ["hecto-liter", "Metric", "Volume"],
"kl" => ["kilo-liter", "Metric", "Volume"],
"Ml" => ["mega-liter", "Metric", "Volume"],
"Gl" => ["giga-liter", "Metric", "Volume"],
"Tl" => ["tera-liter", "Metric", "Volume"],
"gal" => ["gallon", "English", "Volume"],
"floz" => ["fluid-ounce", "English", "Volume"],
"--" => ["<Unitless>", "Standard", "Unitless"],
"cnt" => ["<Unitless>", "Standard", "Unitless"],
"1" => ["<Unitless>", "Standard", "Unitless"],
"one" => ["<Unitless>", "Standard", "Unitless"],
"mol" => ["<Unitless>", "Standard", "Unitless"],
"Hz" => ["hertz", "Metric", "Frequency"],
"pHz" => ["pico-hertz", "Metric", "Frequency"],
"nz" => ["nano-hertz", "Metric", "Frequency"],
"uHz" => ["micro-hertz", "Metric", "Frequency"],
"mHz" => ["milli-hertz", "Metric", "Frequency"],
"cHz" => ["centi-hertz", "Metric", "Frequency"],
"dHz" => ["deci-hertz", "Metric", "Frequency"],
"daHz" => ["deca-hertz", "Metric", "Frequency"],
"hHz" => ["hecto-hertz", "Metric", "Frequency"],
"kHz" => ["kilo-hertz", "Metric", "Frequency"],
"MHz" => ["mega-hertz", "Metric", "Frequency"],
"GHz" => ["giga-hertz", "Metric", "Frequency"],
"THz" => ["tera-hertz", "Metric", "Frequency"],
);


sub list_units {
    foreach $unit_primitive ( sort ( keys %units_info ) ) {
        printf ( STDOUT "\"%s\" means \"%s\", a %s unit of %s.\n",
              $unit_primitive,
              $units_info{$unit_primitive}[0],
              $units_info{$unit_primitive}[1],
              $units_info{$unit_primitive}[2]
         );
    }
}


sub is_valid_primitive {
    my ($unit_primitive) = @_;

    if ( $units_info{$unit_primitive} ) {
         return 1;
    } else {
         return 0;
    }
}


sub validate_unit_spec {
    my ($unit_spec) = @_;

    my $warn_msg = "" ;
    my @valid_parts = ();

    # If the string starts with a parenthesis, then it is a release-07 dual units spec.
    if ( $unit_spec =~ /^\(/ ) {
        my @part_attrs = (0,0);
        my @part_class = (0,0);

        # Strip out right and left parens and whitespace.
        $unit_spec =~ s/^\(|\s|\)$//g ;
        $unit_spec =~ s/\^//g ;

        # Split the dual unit spec into its metric and english parts.
        @dual_unit_parts = split /\|/, $unit_spec ; 

        # There should be either one or two parts. 
        $number_of_parts = scalar( @dual_unit_parts );

        if ( $number_of_parts == 1 )  {

            for (my $ii = 0 ; $ii < $number_of_parts ; $ii ++ ) {
                my $unit_factor;

                # Split the unit spec, using '*' and '/' as delimiters. 
                my @unit_factors = split /\/|\*/ , $dual_unit_parts[$ii] ;

                foreach $unit_factor ( @unit_factors ) {


                    # Substitute out any exponents unless it is just "1" all by itself.
                    $unit_factor =~ s/[1-3]+$//g if ( $unit_factor ne "1" );
                    $unit_system = $units_info{$unit_factor}[1];

                    if ( $unit_system eq "Metric") {
                         # The ii'th part contains metric primitives.
                         $part_attrs[$ii] |= 1 ;
                    } elsif ( $unit_system eq "Standard") {
                         # The ii'th part contains english primitives.
                         $part_attrs[$ii] |= 2;
                    } elsif ( $unit_system eq "English") {
                         # The ii'th part contains standard primitives.
                         $part_attrs[$ii] |= 4;
                    } else {
                         # The ii'th part contains stuff we dont recognize.
                         # It's broke.
                         $part_attrs[$ii] |= 8;
                    }
                }

                if ( $part_attrs[$ii] == 2 ) {
                        $part_class[$ii] = 1; # pure standard
                } elsif ( $part_attrs[$ii] == 1 || $part_attrs[$ii] == 3 ) {
                        @part_class[$ii] = 2; # metric (possibly mixed with standard )"kHz" => ["kilo-hertz", "Metric", "Frequency"],
"pHz" => ["pico-hertz", "Metric", "Frequency"],
"nHz" => ["nano-hertz", "Metric", "Frequency"],
"uHz" => ["micro-hertz", "Metric", "Frequency"],
"mHz" => ["milli-hertz", "Metric", "Frequency"],
"cHz" => ["centi-hertz", "Metric", "Frequency"],
"dHz" => ["deci-hertz", "Metric", "Frequency"],
"daHz" => ["deca-hertz", "Metric", "Frequency"],
"hHz" => ["hecto-hertz", "Metric", "Frequency"],
"kHz" => ["kilo-hertz", "Metric", "Frequency"],
"MHz" => ["mega-hertz", "Metric", "Frequency"],
"GHz" => ["giga-hertz", "Metric", "Frequency"],
"THz" => ["tera-hertz", "Metric", "Frequency"],
                } elsif ( $part_attrs[$ii] == 4 || $part_attrs[$ii] == 6 ) {
                        $part_class[$ii] = 3; # english (possibly mixed with standard )
                } else {
                        $part_class[$ii] = 0; # invalid
                }

                if ( $part_class[$ii] ) {
                        push @valid_parts, $dual_unit_parts[$ii];
                }
             }

            if ( $number_of_parts == scalar ( @valid_parts )) {
                if ( $number_of_parts == 2 ) {
                    if ( ( $part_class[0] == $part_class[1] ) && ( $part_class[0] != 1 ) ) {
                        @valid_parts = ();
                    } elsif ( ($part_class[0] == 3) || ( ($part_class[0] == 1) && ($part_class[1] == 2) ) ) {
                        $temp           = $valid_parts[0]; 
                        $valid_parts[0] = $valid_parts[1]; 
                        $valid_parts[1] = $temp; 
                    }
                }                     
            } else {
                @valid_parts = ();
            }
        }
        elsif ( $number_of_parts == 2 )  {
            $warn_msg = "Dual units are no longer supported" ;
            return ($warn_msg , @valid_parts) ;
        }
    } else {

        my ($orig_unit_spec) = $unit_spec ;

        # Pre 07,  old style units
        $superset_units = "sl|rev" ;
        $std_units = "1|s|M|r|kg|N|v|amp|ohm|C|dB|J|W|Pa|--" ;
        $derived_units = "min|hr|day|ft|in|mm|cm|km|yd|mi|nm|n\.m\.|kft|d|as|am|mr|lbm|g|mt|kN|oz|lbf|kv|mamp|K|R|F|BTU|TNT|hp|psi|atm|cnt|one|mol|pJ|nJ|uJ|mJ|cJ
|dJ|daJ|hJ|kJ|MJ|GJ|TJ|pW|nW|uW|mW|cW|dW|daW|hW|kW|MW|GW|TW|Pa|pPa|nPa|uPa|mPa|cPa|dPa|daPa|hPa|kPa|MPa|GPa|TPa|pHz|nHz|uHz|mHz|cHz|dHz|daHz|hHz|kHz|MHz|GHz|THz" ;

        my $first_product = 0;
        my @elements = ();

        $unit_spec =~ s/\*//g;  # Remove any asterisks that may incorrectly represent an exponent/power.
        $unit_spec =~ s/\^//g;  # Remove any "^" that may incorrectly represent an exponent/power.

        if ( $unit_spec =~ s/^1?\/// ) {
           $first_product = 1;
           push @elements, "1";
        }

        while ( $unit_spec =~ s/([^\/]+)(\/|$)// ) {
            $unit_product = $1 ;
            if ( !$first_product ) {"pHz" => ["pico-hertz", "Metric", "Frequency"],
"nHz" => ["nano-hertz", "Metric", "Frequency"],
"uHz" => ["micro-hertz", "Metric", "Frequency"],
"mHz" => ["milli-hertz", "Metric", "Frequency"],
"cHz" => ["centi-hertz", "Metric", "Frequency"],
"dHz" => ["deci-hertz", "Metric", "Frequency"],
"daHz" => ["deca-hertz", "Metric", "Frequency"],
"hHz" => ["hecto-hertz", "Metric", "Frequency"],
"kHz" => ["kilo-hertz", "Metric", "Frequency"],
"MHz" => ["mega-hertz", "Metric", "Frequency"],
"GHz" => ["giga-hertz", "Metric", "Frequency"],
"THz" => ["tera-hertz", "Metric", "Frequency"],

                $first_product = 1;
            } else {
                push @elements, "/";
            }
            $unit_product =~ s/^(($superset_units|$std_units|$derived_units)([123]?))// ;
            if ( $1 ne "" ) {
                if ( $1 eq "nm" ) {
                    $warn_msg = "    Units \"nm\" assumed to be nautical miles." . 
                        "  Please clarify by using \"n.m.\" or \"(n.m.)\".  Use \"(nm)\" for nanometers." ;
                    push @elements, "n.m.";
                }
                else {
                    push @elements, $1;
                }

                while ( $unit_product =~ s/^(($superset_units|$std_units|$derived_units)([123]?))// ) {
                    push @elements, "*", $1;
                }
            } else {
                # Units product contains no primitives at all. Not good.
                @elements = (); last;
            }
            if ( $unit_product ne "" ) {
                @elements = (); last;
            }
        }
        $valid_parts[0] = join '', @elements;
        # convert old "M" to standard "m"
        $valid_parts[0] =~ s/M/m/g ;
        if ( $valid_parts[0] eq "" ) {
            $warn_msg = "    No valid units found.  If you are using \"m\" for meters, enclose the units in parenthesis. i.e. \"(m)\"\n" ;
            $warn_msg .="    Defaulting units to \"--\" instead of \"$orig_unit_spec\"" ;
            # Provide a default unit for validity
            $valid_parts[0] = "--" ;
        }
    }
    return ($warn_msg , @valid_parts) ;
}
