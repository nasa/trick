/*
PURPOSE:
    (Test Trick input processor memory allocation and input field syntax and
     capabilities.)
 
REFERENCE:
    (((Bailey, R.W)
      (User's Guide and Operational Procedures Volume ...
       of the Trick Simulation Environment) (MDSS-HD TM-6.24.26-04)
      (McDonnell Dougla Space Systems - Houston Division) (March 1993) ()))

ASSUMPTIONS AND LIMITATIONS:
    ((What you see is what you get.))

PROGRAMMERS:
    (((Robert W. Bailey) (LinCom) (9/1/90 (Trick-CR-00000) (Initial Release)))
*/

/*
 * $Log: ip_test.d,v $
 * Revision 7.3  2007-01-08 11:22:17-06  dstrauss
 * Bugs in parsing new units specs
 *
 * Revision 7.2  2006-08-31 13:59:12-05  hchen
 * SIM_test_ip error - convert the old style of allocation statements to the new style and update the file due to the variable units changed to units_test
 *
 * Revision 7.1  2006-06-22 14:14:14-05  lin
 * Bump version number for 07
 *
 * Revision 5.2  2004-09-07 11:48:42-05  lin
 * New #define handling does not pass SIM_test_ip
 *
 * Revision 5.1  2004-08-05 13:06:55-05  lin
 * Bump
 *
 * Revision 4.3  2004/02/18 21:54:41  lin
 * remove "STRING" as the special case for "char *"
 *
 * Revision 4.2  2004/01/15 22:36:42  lin
 * test sims do not work
 *
 * Revision 4.1  2003/10/21 21:49:19  lin
 * Bump version number for 04
 *
 * Revision 1.5  2003/06/09 21:03:13  vetter
 * Forget Fortran For 4
 *
 * Revision 1.4  2003/05/29 18:57:21  vetter
 * Add Bitfield Test To IP Test
 *
 * Revision 1.3  2003/03/14 20:16:51  lin
 * Try and rewrite ip_alloc scheme to remove extra 8 byte requirement
 *
 * Revision 1.2  2002/10/07 15:16:27  lin
 * Add rcs version info to all trick_models files
 *
 */
 
#define TWO     (2.00000000)
#define TWO2    ((4*2.0)-6.00000000)
#define THREE   3.00000000
#define DTR     0.0174532925199432957

IP_TEST.strings_good = 1 ;

IP_TEST.bits[0].bits_a = 1 ;
IP_TEST.bits[0].bits_b = 2 ;
IP_TEST.bits[0].bits_c = 3 ;
IP_TEST.bits[0].bits_d = 4 ;
IP_TEST.bits[0].doub_x = 777.0 ;

IP_TEST.bits[1].bits_a = 5 ;
IP_TEST.bits[1].bits_b = 6 ;
IP_TEST.bits[1].bits_c = 7 ;
IP_TEST.bits[1].bits_d = 8 ;
IP_TEST.bits[1].doub_x = 999.0 ;

/* The following are the old style of allocation statements.
IP_TEST.c_pointer_types.cpp[ 2 ][ 2 ][ 2 ] ;
IP_TEST.c_pointer_types.ucpp[ 2 ][ 2 ][ 2 ] ;
IP_TEST.c_pointer_types.spp[ 2 ][ 2 ][ 2 ] ;
IP_TEST.c_pointer_types.uspp[ 2 ][ 2 ][ 2 ] ;
IP_TEST.c_pointer_types.ipp[ 2 ][ 2 ][ 2 ] ;
IP_TEST.c_pointer_types.uipp[ 2 ][ 2 ][ 2 ] ;
IP_TEST.c_pointer_types.lpp[ 2 ][ 2 ][ 2 ] ;
IP_TEST.c_pointer_types.ulpp[ 2 ][ 2 ][ 2 ] ;
IP_TEST.c_pointer_types.fpp[ 2 ][ 2 ][ 2 ] ;
IP_TEST.c_pointer_types.dpp[ 2 ][ 2 ][ 2 ] ;

IP_TEST.c_mixed_types.cpa[ 0 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.cpa[ 1 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.ucpa[ 0 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.ucpa[ 1 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.spa[ 0 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.spa[ 1 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.uspa[ 0 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.uspa[ 1 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.ipa[ 0 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.ipa[ 1 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.uipa[ 0 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.uipa[ 1 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.lpa[ 0 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.lpa[ 1 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.ulpa[ 0 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.ulpa[ 1 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.fpa[ 0 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.fpa[ 1 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.dpa[ 0 ][ 2 ][ 2 ] ;
IP_TEST.c_mixed_types.dpa[ 1 ][ 2 ][ 2 ] ;

IP_TEST.units_test[ 0 ][ 2 ][ 2 ] ;
IP_TEST.units_test[ 1 ][ 2 ][ 2 ] ;*/

    /* NEW ALLOCATION STATEMENTS */
int i,j;

IP_TEST.c_pointer_types.cpp = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_pointer_types.cpp[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_pointer_types.cpp[i][j] = alloc(2);
        }
}

IP_TEST.c_pointer_types.ucpp = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_pointer_types.ucpp[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_pointer_types.ucpp[i][j] = alloc(2);
        }
}

IP_TEST.c_pointer_types.spp = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_pointer_types.spp[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_pointer_types.spp[i][j] = alloc(2);
        }
}

IP_TEST.c_pointer_types.uspp = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_pointer_types.uspp[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_pointer_types.uspp[i][j] = alloc(2);
        }
}

IP_TEST.c_pointer_types.ipp = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_pointer_types.ipp[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_pointer_types.ipp[i][j] = alloc(2);
        }
}

IP_TEST.c_pointer_types.uipp = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_pointer_types.uipp[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_pointer_types.uipp[i][j] = alloc(2);
        }
}

IP_TEST.c_pointer_types.lpp = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_pointer_types.lpp[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_pointer_types.lpp[i][j] = alloc(2);
        }
}

IP_TEST.c_pointer_types.ulpp = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_pointer_types.ulpp[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_pointer_types.ulpp[i][j] = alloc(2);
        }
}

IP_TEST.c_pointer_types.fpp = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_pointer_types.fpp[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_pointer_types.fpp[i][j] = alloc(2);
        }
}

IP_TEST.c_pointer_types.dpp = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_pointer_types.dpp[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_pointer_types.dpp[i][j] = alloc(2);
        }
}

IP_TEST.c_mixed_types.cpa = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_mixed_types.cpa[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_mixed_types.cpa[i][j] = alloc(2);
        }
}

IP_TEST.c_mixed_types.ucpa = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_mixed_types.ucpa[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_mixed_types.ucpa[i][j] = alloc(2);
        }
}

IP_TEST.c_mixed_types.spa = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_mixed_types.spa[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_mixed_types.spa[i][j] = alloc(2);
        }
}

IP_TEST.c_mixed_types.uspa = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_mixed_types.uspa[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_mixed_types.uspa[i][j] = alloc(2);
        }
}

IP_TEST.c_mixed_types.ipa = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_mixed_types.ipa[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_mixed_types.ipa[i][j] = alloc(2);
        }
}

IP_TEST.c_mixed_types.uipa = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_mixed_types.uipa[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_mixed_types.uipa[i][j] = alloc(2);
        }
}

IP_TEST.c_mixed_types.lpa = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_mixed_types.lpa[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_mixed_types.lpa[i][j] = alloc(2);
        }
}

IP_TEST.c_mixed_types.ulpa = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_mixed_types.ulpa[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_mixed_types.ulpa[i][j] = alloc(2);
        }
}

IP_TEST.c_mixed_types.fpa = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_mixed_types.fpa[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_mixed_types.fpa[i][j] = alloc(2);
        }
}

IP_TEST.c_mixed_types.dpa = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.c_mixed_types.dpa[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.c_mixed_types.dpa[i][j] = alloc(2);
        }
}


IP_TEST.units_test = alloc(2);
for (i = 0; i < 2; i++) {
        IP_TEST.units_test[i] = alloc(2);
        for (j=0; j < 2; j++) {
                IP_TEST.units_test[i][j] = alloc(2);
        }
}


    /* UNITS CONVERSION */
IP_TEST.units_test[0][0][0].second[0] = 1.0 ;
IP_TEST.units_test[0][0][0].second[1] {s} = 1.0 ;
IP_TEST.units_test[0][0][0].second[2] {min} = 1.0 ;
IP_TEST.units_test[0][0][0].second[3] {hr} = 1.0 ;
IP_TEST.units_test[0][0][0].second[4] {day} = 1.0 ;
IP_TEST.units_test[0][0][0].meter[0] = 1.0 ;
IP_TEST.units_test[0][0][0].meter[1] {mm} = 1.0 ;
IP_TEST.units_test[0][0][0].meter[2] {cm} = 1.0 ;
IP_TEST.units_test[0][0][0].meter[3] {M} = 1.0 ;
IP_TEST.units_test[0][0][0].meter[4] {km} = 1.0 ;
IP_TEST.units_test[0][0][0].meter[5] {in} = 1.0 ;
IP_TEST.units_test[0][0][0].meter[6] {ft} = 1.0 ;
IP_TEST.units_test[0][0][0].meter[7] {yd} = 1.0 ;
IP_TEST.units_test[0][0][0].meter[8] {mi} = 1.0 ;
IP_TEST.units_test[0][0][0].meter[9] {n.m.} = 1.0 ;
IP_TEST.units_test[0][0][0].radian[0] = 1.0 ;
IP_TEST.units_test[0][0][0].radian[1] {r} = 1.0 ;
IP_TEST.units_test[0][0][0].radian[2] {as} = 1.0 ;
IP_TEST.units_test[0][0][0].radian[3] {am} = 1.0 ;
IP_TEST.units_test[0][0][0].radian[4] {d} = 1.0 ;
IP_TEST.units_test[0][0][0].kilogram[0] = 1.0 ;
IP_TEST.units_test[0][0][0].kilogram[1] {g} = 1.0 ;
IP_TEST.units_test[0][0][0].kilogram[2] {kg} = 1.0 ;
IP_TEST.units_test[0][0][0].kilogram[3] {mt} = 1.0 ;
IP_TEST.units_test[0][0][0].kilogram[4] {sl} = 1.0 ;
IP_TEST.units_test[0][0][0].kilogram[5] {lbm} = 1.0 ;
IP_TEST.units_test[0][0][0].newton[0] = 1.0 ;
IP_TEST.units_test[0][0][0].newton[1] {N} = 1.0 ;
IP_TEST.units_test[0][0][0].newton[2] {kN} = 1.0 ;
IP_TEST.units_test[0][0][0].newton[3] {oz} = 1.0 ;
IP_TEST.units_test[0][0][0].newton[4] {lbf} = 1.0 ;
IP_TEST.units_test[0][0][0].volt[0] = 1.0 ;
IP_TEST.units_test[0][0][0].volt[1] {v} = 1.0 ;
IP_TEST.units_test[0][0][0].volt[2] {kv} = 1.0 ;
IP_TEST.units_test[0][0][0].amp[0] = 1.0 ;
IP_TEST.units_test[0][0][0].amp[1] {amp} = 1.0 ;
IP_TEST.units_test[0][0][0].amp[2] {mamp} = 1.0 ;
IP_TEST.units_test[0][0][0].ohm[0] = 1.0 ;
IP_TEST.units_test[0][0][0].ohm[1] {ohm} = 1.0 ;
IP_TEST.units_test[0][0][0].celsius[0] = 1.0 ;
IP_TEST.units_test[0][0][0].celsius[1] {K} = 1.0 ;
IP_TEST.units_test[0][0][0].celsius[2] {R} = 1.0 ;
IP_TEST.units_test[0][0][0].celsius[3] {F} = 1.0 ;
IP_TEST.units_test[0][0][0].fahrenheit[0] = 1.0 ;
IP_TEST.units_test[0][0][0].fahrenheit[1] {C} = 1.0 ;
IP_TEST.units_test[0][0][0].fahrenheit[2] {K} = 1.0 ;
IP_TEST.units_test[0][0][0].fahrenheit[3] {R} = 1.0 ;
IP_TEST.units_test[0][0][0].temp_rate[0] = 1.0 ;
IP_TEST.units_test[0][0][0].temp_rate[1] {C/s} = 1.0 ;
IP_TEST.units_test[0][0][0].temp_rate[2] {R/min} = 1.0 ;
IP_TEST.units_test[0][0][0].temp_rate[3] {F/s} = 1.0 ;
IP_TEST.units_test[0][0][0].area[0] = 1.0 ;
IP_TEST.units_test[0][0][0].area[1] {ft2} = 1.0 ;
IP_TEST.units_test[0][0][0].area[2] {cm2} = 1.0 ;
IP_TEST.units_test[0][0][0].volume[0] = 1.0 ;
IP_TEST.units_test[0][0][0].volume[1] {cm3} = 1.0 ;
IP_TEST.units_test[0][0][0].volume[2] {ft3} = 1.0 ;
IP_TEST.units_test[0][0][0].velocity[0] = 1.0 ;
IP_TEST.units_test[0][0][0].velocity[1] {mi/hr} = 1.0 ;
IP_TEST.units_test[0][0][0].velocity[2] {yd/min} = 1.0 ;
IP_TEST.units_test[0][0][0].velocity_eng[0] {mi/hr} = 1.0 ;
IP_TEST.units_test[0][0][0].velocity_eng[1] {M/s} = 1.0 ;
IP_TEST.units_test[0][0][0].accel[0] = 1.0 ;
IP_TEST.units_test[0][0][0].accel[1] {ft/s2} = 1.0 ;
IP_TEST.units_test[0][0][0].accel[2] {n.m./day2} = 1.0 ;
IP_TEST.units_test[0][0][0].inertia[0] = 1.0 ;
IP_TEST.units_test[0][0][0].inertia[1] {lbm*ft2} = 1.0 ;
IP_TEST.units_test[0][0][0].inertia[2] {sl*in2} = 1.0 ;
IP_TEST.units_test[0][0][0].torque[0] = 1.0 ;
IP_TEST.units_test[0][0][0].torque[1] {lbf*ft} = 1.0 ;
IP_TEST.units_test[0][0][0].torque[2] {oz*in} = 1.0 ;
IP_TEST.units_test[0][0][0].pressure[0] = 1.0 ;
IP_TEST.units_test[0][0][0].pressure[1] {oz/in2} = 1.0 ;
IP_TEST.units_test[0][0][0].pressure[2] {lbf/ft2} = 1.0 ;
IP_TEST.units_test[0][0][0].density[0] = 1.0 ;
IP_TEST.units_test[0][0][0].density[1] {g/cm3} = 1.0 ;
IP_TEST.units_test[0][0][0].density[2] {sl/ft3} = 1.0 ;


IP_TEST.units_test[0][0][0].measure = meter ;
IP_TEST.units_test[0][0][1].measure = centimeter ;
IP_TEST.units_test[0][1][0].measure = millimeter ;
IP_TEST.units_test[0][1][1].measure = kilometer ;
IP_TEST.units_test[1][0][0].measure = inches ;
IP_TEST.units_test[1][0][1].measure = feet ;
IP_TEST.units_test[1][1][0].measure = yard ;
IP_TEST.units_test[1][1][1].measure = mile ;

IP_TEST.c_types.c = 'c' ;
IP_TEST.c_types.uc = 'u' ;
IP_TEST.c_types.cp = "Testing Char Pointers." ;
IP_TEST.c_types.s = abs( -1 ) ;
IP_TEST.c_types.us = 2 % THREE ;
IP_TEST.c_types.i = -3 % TWO ;
IP_TEST.c_types.ui = pow ( 4 , TWO ) ;
IP_TEST.c_types.l = pow ( -5 , THREE ) ;
IP_TEST.c_types.ul = 6 * TWO2 ;
IP_TEST.c_types.f = THREE/7.7777777 ;
IP_TEST.c_types.d = 8.88888888888888888888888 + TWO ;

IP_TEST.c_array_types.ca[0][0][0] = 'a' , 'b' ;
IP_TEST.c_array_types.ca[0][1][0] = 'c' , 'd' ;
IP_TEST.c_array_types.ca[1][0][0] = 'e' , 'f' ;
IP_TEST.c_array_types.ca[1][1][0] = 'g' , 'h' ;
IP_TEST.c_array_types.uca[0][0][0] = 'i' , 'j' ;
IP_TEST.c_array_types.uca[0][1][0] = 'k' , 'l' ;
IP_TEST.c_array_types.uca[1][0][0] = 'm' , 'n' ;
IP_TEST.c_array_types.uca[1][1][0] = 'o' , 'p' ;
IP_TEST.c_array_types.sa[0][0][0] = -1 - THREE , - THREE ;
IP_TEST.c_array_types.sa[0][1][0] = TWO2 * { -3 , -4 } ;
IP_TEST.c_array_types.sa[1][0][0] = { -5 , -6 } * ( 1.0 / TWO ) ;
IP_TEST.c_array_types.sa[1][1][0] = -7  , -8  ;
IP_TEST.c_array_types.usa[0][0][0] = 0x0009 , 0x000A ;
IP_TEST.c_array_types.usa[0][1][0] = 0x000b , 0x000C ;
IP_TEST.c_array_types.usa[1][0][0] = 0x000d ,  14 ;
IP_TEST.c_array_types.usa[1][1][0] = 15 ,  16 ;
IP_TEST.c_array_types.ia[0][0][0] = -20 , -21 ;
IP_TEST.c_array_types.ia[0][1][0] = -22 , -23 ;
IP_TEST.c_array_types.ia[1][0][0] = -24 , -25 ;
IP_TEST.c_array_types.ia[1][1][0] = -26 , -27 ;
IP_TEST.c_array_types.uia[0][0][0] = 28 ,  29 ;
IP_TEST.c_array_types.uia[0][1][0] = 30 ,  31 ;
IP_TEST.c_array_types.uia[1][0][0] = 32 ,  33 ;
IP_TEST.c_array_types.uia[1][1][0] = 34 ,  35 ;
IP_TEST.c_array_types.la[0][0][0] = -200001 ,-200002 ;
IP_TEST.c_array_types.la[0][1][0] = -200003 ,-200004 ;
IP_TEST.c_array_types.la[1][0][0] = -200005 ,-200006 ;
IP_TEST.c_array_types.la[1][1][0] = -200007 ,-200008 ;
IP_TEST.c_array_types.ula[0][0][0] = 200009 , 200010 ;
IP_TEST.c_array_types.ula[0][1][0] = 200011 , 200012 ;
IP_TEST.c_array_types.ula[1][0][0] = 200013 , 200014 ;
IP_TEST.c_array_types.ula[1][1][0] = 200015 , 200016 ;
IP_TEST.c_array_types.fa[0][0][0] = abs( cos( 30.0 * DTR ) ) ;
IP_TEST.c_array_types.fa[0][1][0] = pow( 3.33333333 , 4.44444444 ) ;
IP_TEST.c_array_types.fa[1][0][0] = exp( 5.55555555 ) ;
IP_TEST.c_array_types.fa[1][1][0] = log( 7.77777777 ) ;
IP_TEST.c_array_types.da[0][0][0] = cos( 30.0 * DTR ) ;
IP_TEST.c_array_types.da[0][1][0] = sin( 30.0 * DTR ) ;
IP_TEST.c_array_types.da[1][0][0] = tan( 30.0 * DTR ) ;
IP_TEST.c_array_types.da[1][1][0] = acos( 0.5 ) ;

IP_TEST.c_pointer_types.cpp[0][0][0] = "I'd" , "rather" ;
IP_TEST.c_pointer_types.cpp[0][1][0] = "be" , "developing" ;
IP_TEST.c_pointer_types.cpp[1][0][0] = "than" , "testing" ;
IP_TEST.c_pointer_types.cpp[1][1][0] = "or" , "debugging." ;
IP_TEST.c_pointer_types.ucpp[0][0][0] = 'A' , 'B' ;
IP_TEST.c_pointer_types.ucpp[0][1][0] = 'C' , 'D' ;
IP_TEST.c_pointer_types.ucpp[1][0][0] = 'E' , 'F' ;
IP_TEST.c_pointer_types.ucpp[1][1][0] = 'G' , 'H' ;
IP_TEST.c_pointer_types.spp[0][0][0] = -1  , -2  ;
IP_TEST.c_pointer_types.spp[0][1][0] = -3  , -4  ;
IP_TEST.c_pointer_types.spp[1][0][0] = -5  , -6  ;
IP_TEST.c_pointer_types.spp[1][1][0] = -7  , -8  ;
IP_TEST.c_pointer_types.uspp[0][0][0] = 9  ,  10 ;
IP_TEST.c_pointer_types.uspp[0][1][0] = 11 ,  12 ;
IP_TEST.c_pointer_types.uspp[1][0][0] = 13 ,  14 ;
IP_TEST.c_pointer_types.uspp[1][1][0] = 15 ,  16 ;
IP_TEST.c_pointer_types.ipp[0][0][0] = -20 , -21 ;
IP_TEST.c_pointer_types.ipp[0][1][0] = -22 , -23 ;
IP_TEST.c_pointer_types.ipp[1][0][0] = -24 , -25 ;
IP_TEST.c_pointer_types.ipp[1][1][0] = -26 , -27 ;
IP_TEST.c_pointer_types.uipp[0][0][0] = 28 ,  29 ;
IP_TEST.c_pointer_types.uipp[0][1][0] = 30 ,  31 ;
IP_TEST.c_pointer_types.uipp[1][0][0] = 32 ,  33 ;
IP_TEST.c_pointer_types.uipp[1][1][0] = 34 ,  35 ;
IP_TEST.c_pointer_types.lpp[0][0][0] = -200001 ,-200002 ;
IP_TEST.c_pointer_types.lpp[0][1][0] = -200003 ,-200004 ;
IP_TEST.c_pointer_types.lpp[1][0][0] = -200005 ,-200006 ;
IP_TEST.c_pointer_types.lpp[1][1][0] = -200007 ,-200008 ;
IP_TEST.c_pointer_types.ulpp[0][0][0] = 200009 , 200010 ;
IP_TEST.c_pointer_types.ulpp[0][1][0] = 200011 , 200012 ;
IP_TEST.c_pointer_types.ulpp[1][0][0] = 200013 , 200014 ;
IP_TEST.c_pointer_types.ulpp[1][1][0] = 200015 , 200016 ;
IP_TEST.c_pointer_types.fpp[0][0][0] = asin( 0.5 ) ;
IP_TEST.c_pointer_types.fpp[0][1][0] = atan( 0.5 ) , atan2( -1.0 , 2.0 ) ;
IP_TEST.c_pointer_types.fpp[1][0][0] = sqrt( THREE ) ;
IP_TEST.c_pointer_types.fpp[1][1][0] = 7.77777777 , 8.88888888 ;
IP_TEST.c_pointer_types.dpp[0][0][0] = -1.1111111111111111, -2.2222222222222222;
IP_TEST.c_pointer_types.dpp[0][1][0] = -3.3333333333333333, -4.4444444444444444;
IP_TEST.c_pointer_types.dpp[1][0][0] = -5.5555555555555555, -6.6666666666666666;
IP_TEST.c_pointer_types.dpp[1][1][0] = -7.777E-03         , -8.888e+003 ;

#define SURF surfing
#define COLOR blue
#define BEACH Wakiki


IP_TEST.c_mixed_types.cpa[0][0][0] = "I" , "would" ;
IP_TEST.c_mixed_types.cpa[0][1][0] = "rather" , "be" ;
IP_TEST.c_mixed_types.cpa[1][0][0] = "#(SURF)" , "today" ;
IP_TEST.c_mixed_types.cpa[1][1][0] = "in" , "the #{COLOR} oceans of #(BEACH)." ;
IP_TEST.c_mixed_types.ucpa[0][0][0] = 'A' , 'B' ;
IP_TEST.c_mixed_types.ucpa[0][1][0] = 'C' , 'D' ;
IP_TEST.c_mixed_types.ucpa[1][0][0] = 'E' , 'F' ;
IP_TEST.c_mixed_types.ucpa[1][1][0] = 'G' , 'H' ;
IP_TEST.c_mixed_types.spa[0][0][0] = -1  , -2  ;
IP_TEST.c_mixed_types.spa[0][1][0] = -3  , -4  ;
IP_TEST.c_mixed_types.spa[1][0][0] = -5  , -6  ;
IP_TEST.c_mixed_types.spa[1][1][0] = -7  , -8  ;
IP_TEST.c_mixed_types.uspa[0][0][0] = 9  ,  10 ;
IP_TEST.c_mixed_types.uspa[0][1][0] = 11 ,  12 ;
IP_TEST.c_mixed_types.uspa[1][0][0] = 13 ,  14 ;
IP_TEST.c_mixed_types.uspa[1][1][0] = 15 ,  16 ;
IP_TEST.c_mixed_types.ipa[0][0][0] = -20 , -21 ;
IP_TEST.c_mixed_types.ipa[0][1][0] = -22 , -23 ;
IP_TEST.c_mixed_types.ipa[1][0][0] = -24 , -25 ;
IP_TEST.c_mixed_types.ipa[1][1][0] = -26 , -27 ;
IP_TEST.c_mixed_types.uipa[0][0][0] = 28 ,  29 ;
IP_TEST.c_mixed_types.uipa[0][1][0] = 30 ,  31 ;
IP_TEST.c_mixed_types.uipa[1][0][0] = 32 ,  33 ;
IP_TEST.c_mixed_types.uipa[1][1][0] = 34 ,  35 ;
IP_TEST.c_mixed_types.lpa[0][0][0] = -200001 ,-200002 ;
IP_TEST.c_mixed_types.lpa[0][1][0] = -200003 ,-200004 ;
IP_TEST.c_mixed_types.lpa[1][0][0] = -200005 ,-200006 ;
IP_TEST.c_mixed_types.lpa[1][1][0] = -200007 ,-200008 ;
IP_TEST.c_mixed_types.ulpa[0][0][0] = 200009 , 200010 ;
IP_TEST.c_mixed_types.ulpa[0][1][0] = 200011 , 200012 ;
IP_TEST.c_mixed_types.ulpa[1][0][0] = 200013 , 200014 ;
IP_TEST.c_mixed_types.ulpa[1][1][0] = 200015 , 200016 ;
IP_TEST.c_mixed_types.fpa[0][0][0] = 1.11111111 , 2.22222222 ;
IP_TEST.c_mixed_types.fpa[0][1][0] = 3.33333333 , 4.44444444 ;
IP_TEST.c_mixed_types.fpa[1][0][0] = 5.55555555 , 6.66666666 ;
IP_TEST.c_mixed_types.fpa[1][1][0] = -7.777E-03 , -8.888e+03 ;
IP_TEST.c_mixed_types.dpa[0][0][0] = -1.1111111111111111 , -2.2222222222222222 ;
IP_TEST.c_mixed_types.dpa[0][1][0] = -3.3333333333333333 , -4.4444444444444444 ;
IP_TEST.c_mixed_types.dpa[1][0][0] = -5.5555555555555555 , -6.6666666666666666 ;
IP_TEST.c_mixed_types.dpa[1][1][0] = -7.7777777777777777 , -8.8888888888888888 ;
