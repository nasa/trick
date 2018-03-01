/*
   PURPOSE: (Units Conversion C Language Interface)

   PROGRAMMERS: (((John M. Penn) (L-3Com/Titan) (May 2006) (v1.0) (Initial Release))) */

#include <stdio.h>
#include <string.h>
#include "trick/units_conv.h"

typedef enum { No_Op, Mult_Op, Div_Op, Square_Op, Cube_Op,
    Units_Primitive, End_of_input, Unknown
} Token_t;

typedef struct {
    Token_t token;
    int ix;                     /* conversion record index, if and only if token==Units_Primitive */
    int n;                      /* position of the next character in the string being parsed */
    const char *s;
    Units_t *units;
} parse_context_t;

typedef enum { Time,
    Length,
    Angle,
    Mass,
    Force,
    Voltage,
    Current,
    Resistance,
    Temperature,
    Sound,
    Energy,
    Power,
    Pressure,
    Volume,
    Unitless,
    Frequency
} Measure_t;

char *measure_name[] = {
    "Time",
    "Length",
    "Angle",
    "Mass",
    "Force",
    "Voltage",
    "Current",
    "Resistance",
    "Temperature",
    "Sound",
    "Energy",
    "Power",
    "Pressure",
    "Volume",
    "Unitless",
    "Frequency"
};

typedef enum {
    Metric,
    English,
    Standard
} Unit_System_t;

char *unit_system_name[] = {
    "Metric",
    "English",
    "Standard"
};

typedef struct {
    int top;
    UnitsConvFn_t a[MAX_TRIADS];
} UCF_stack_t;



#define PICO     1e-12
#define NANO     1e-09
#define MICRO    1e-06
#define MILLI    1e-03
#define CENTI    1e-02
#define DECI     1e-01
#define DECA     1e+01
#define HECTO    1e+2
#define KILO     1e+3
#define MEGA     1e+6
#define GIGA     1e+9
#define TERA     1e+12

/**
 @page UNITS_TIME Units of Time
 <table>
 <tr><th>Primitive Name</th> <th>Full Name</th> </tr>
 <tr><td>s</td><td>seconds</td></tr>
 <tr><td>ps</td><td>pico-seconds</td></tr>
 <tr><td>ns</td><td>nano-seconds</td></tr>
 <tr><td>us</td><td>micro-seconds</td></tr>
 <tr><td>ms</td><td>milli-seconds</td></tr>
 <tr><td>cs</td><td>centi-seconds</td></tr>
 <tr><td>ds</td><td>deci-seconds</td></tr>
 <tr><td>das</td><td>deca-seconds</td></tr>
 <tr><td>hs</td><td>hecto-seconds</td></tr>
 <tr><td>ks</td><td>kilo-seconds</td></tr>
 <tr><td>Ms</td><td>mega-seconds</td></tr>
 <tr><td>Gs</td><td>giga-seconds</td></tr>
 <tr><td>Ts</td><td>tera-seconds</td></tr>
 <tr><td>min</td><td>minutes</td></tr>
 <tr><td>hr</td><td>hours</td></tr>
 <tr><td>day</td><td>days</td></tr>
</table>

    @page UNITS_LENGTH Units of Length
    <table>
    <tr><th>Primitive Name</th> <th>Full Name</th> </tr>
    <tr><td>m</td><td>meters</td></tr>
    <tr><td>pm</td><td>pico-meters</td></tr>
    <tr><td>nm</td><td>nano-meters</td></tr>
    <tr><td>um</td><td>micro-meters</td></tr>
    <tr><td>mm</td><td>milli-meters</td></tr>
    <tr><td>cm</td><td>centi-meters</td></tr>
    <tr><td>dm</td><td>deci-meters</td></tr>
    <tr><td>dam</td><td>deca-meters</td></tr>
    <tr><td>hm</td><td>hecto-meters</td></tr>
    <tr><td>km</td><td>kilo-meters</td></tr>
    <tr><td>Mm</td><td>mega-meters</td></tr>
    <tr><td>Gm</td><td>giga-meters</td></tr>
    <tr><td>Tm</td><td>tera-meters</td></tr>
    <tr><td>ft</td><td>feet</td></tr>
    <tr><td>kft</td><td>kilo-feet</td></tr>
    <tr><td>in</td><td>inches</td></tr>
    <tr><td>yd</td><td>yards</td></tr>
    <tr><td>mi</td><td>miles</td></tr>
    <tr><td>n.m.</td><td>nautical-miles
    </table>

    @page UNITS_ANGLE Units of Angle
    @verbatim
    Primitive   Name
    ---------   --------------
      "r"       radians
      "pr"      pico-radians
      "nr"      nano-radians
      "ur"      micro-radians
      "mr"      milli-radians
      "cr"      centi-radians
      "dr"      deci-radians
      "d"       degrees
      "as"      arc-seconds
      "am"      arc-minutes
      "rev"     revolutions
    @endverbatim

    @page UNITS_MASS Units of Mass
    @verbatim
    Primitive   Name
    ---------   --------------
      "g"       grams
      "pg"      pico-grams
      "ng"      nano-grams
      "ug"      micro-grams
      "mg"      milli-grams
      "cg"      centi-grams
      "dg"      deci-grams
      "dag"     deca-grams
      "hg"      hecto-grams
      "kg"      kilo-grams
      "Mg"      mega-grams
      "Gg"      giga-grams
      "Tg"      tera-grams
      "mt"      metric-tons
      "sl"      slugs
      "lbm"     pound(mass)
    @endverbatim

    @page UNITS_FORCE Units of Force
    @verbatim
    Primitive   Name
    ---------   --------------
      "N"       newtons
      "pN"      pico-newtons
      "nN"      nano-newtons
      "uN"      micro-newtons
      "mN"      milli-newtons
      "cN"      centi-newtons
      "dN"      deci-newtons
      "daN"     deca-newtons
      "hN"      hecto-newtons
      "kN"      kilo-newtons
      "MN"      mega-newtons
      "GN"      giga-newtons
      "TN"      tera-newtons
      "oz"      ounces
      "lbf"     pound-force
    @endverbatim

    @page UNITS_ELECTRIC_POTENTIAL Units of Electric Potential
    @verbatim
    Primitive   Name
    ---------   --------------
      "v"       volts
      "pv"      pico-volts
      "nv"      nano-volts
      "uv"      micro-volts
      "mv"      milli-volts
      "cv"      centi-volts
      "dv"      deci-volts
      "dav"     deca-volts
      "hv"      hecto-volts
      "kv"      kilo-volts
      "Mv"      mega-volts
      "Gv"      giga-volts
      "Tv"      tera-volts
    @endverbatim

    @page UNITS_ELECTRIC_CURRENT Units of Electric Current
    @verbatim
    Primitive   Name
    ---------   --------------
      "amp"     amperes"
      "pamp     pico-amperes
      "namp     nano-amperes
      "uamp"    micro-amperes
      "mamp"    milli-amperes
      "camp"    centi-amperes
      "damp"    deci-amperes
      "daamp"   deca-amperes
      "hamp"    hecto-amperes
      "kamp"    kilo-amperes
      "Mamp"    mega-amperes
      "Gamp"    giga-amperes
      "Tamp"    tera-amperes
    @endverbatim

    @page UNITS_ELECTRIC_RESISTANCE Units of Electric Resistance
    @verbatim
    Primitive   Name
    ---------   --------------
      "ohm"     ohms
      "pohm"    pico-ohms
      "nohm"    nano-ohms
      "uohm"    micro-ohms
      "mohm"    milli-ohms
      "cohm"    centi-ohms
      "dohm"    deci-ohms
      "daohm"   deca-ohms
      "hohm"    hecto-ohms
      "kohm"    kilo-ohms
      "Mohm"    mega-ohms
      "Gohm"    giga-ohms
      "Tohm"    tera-ohms
    @endverbatim

    @page UNITS_TEMPERATURE Units of Temperature
    @verbatim
    Primitive   Name
    ---------   --------------
      "C"       degrees-celsius
      "K"       degrees-kelvin
      "F"       degrees-fahrenheit
      "R"       degrees-rankine
    @endverbatim

    @page UNITS_GAIN Units of Gain
    @verbatim
    Primitive   Name
    ---------   --------------
      "dB"      decibels
    @endverbatim

    @page UNITS_ENERGY Units of Energy
    @verbatim
    Primitive   Name
    ---------   --------------
      "J"       joule
      "pJ"      pico-joule
      "nJ"      nano-joule
      "uJ"      micro-joule
      "mJ"      milli-joule
      "cJ"      centi-joule
      "dJ"      deci-joule
      "daJ"     deca-joule
      "hJ"      hecto-joule
      "kJ"      kilo-joule
      "MJ"      mega-joule
      "GJ"      giga-joule
      "TJ"      tera-joule
      "BTU"     british-thermal
      "TNT"     ton-tnt
    @endverbatim

    @page UNITS_POWER Units of Power
    @verbatim
    Primitive   Name
    ---------   --------------
      "W"       watt
      "pW"      pico-watt
      "nW"      nano-watt
      "uW"      micro-watt
      "mW"      milli-watt
      "cW"      centi-watt
      "dW"      deci-watt
      "daW"     deca-watt
      "hW"      hecto-watt
      "kW"      kilo-watt
      "MW"      mega-watt
      "GW"      giga-watt
      "TW"      tera-watt
      "hp"      horse-power
    @endverbatim

    @page UNITS_PRESSURE Units of Pressure
    @verbatim
    Primitive   Name
    ---------   --------------
      "Pa"      pascals
      "pPa"     pico-pascals
      "nPa"     nano-pascals
      "uPa"     micro-pascals
      "mPa"     milli-pascals
      "cPa"     centi-pascals
      "dPa"     deci-pascals
      "daPa"    deca-pascals
      "hPa"     hecto-pascals
      "kPa"     kilo-pascals
      "MPa"     mega-pascals
      "GPa"     giga-pascals
      "TPa"     tera-pascals
      "psi"     ponds-sq-inch
      "atm"     atmospheres
      "mmHg"    milli-meters-mercury
      "inHg"    inches-mercury
      "mmAq"    milli-meters-water
      "inAq"    inches-water
    @endverbatim

    @page UNITS_VOLUME Units of Volume
    @verbatim
    Primitive   Name
    ---------   --------------
      "l"       liter
      "pl"      pico-liter
      "nl"      nano-liter
      "ul"      micro-liter
      "ml"      milli-liter
      "cl"      centi-liter
      "dl"      deci-liter
      "dal"     deca-liter
      "hl"      hecto-liter
      "kl"      kilo-liter
      "Ml"      mega-liter
      "Gl"      giga-liter
      "Tl"      tera-liter
      "gal"     gallon
      "floz"    fluid ounce
    @endverbatim

    @page UNITS_UNITLESS Unitless
    @verbatim
    Primitive   Name
    ---------   --------------
    "--"        <Unitless>
    "cnt"       <Unitless> count
    "one"       <Unitless> one
    "1"         <Unitless> 1
    "mol"       <Unitless> mole
    @endverbatim

    @page UNITS_FREQUENCY Frequency
    @verbatim
    Primitive   Name
    ---------   --------------
    "Hz"        hertz
    "Mhz"       mega-hertz
    "Ghz"       giga-hertz
    @endverbatim
*/

struct {
    Measure_t measure;
    char *c_unit;
    char *s_unit;
    char *desc;
    int era;                    /* Trick Release in which the units primitive was introduced. */
    Unit_System_t system;
    UnitsConvFn_t poly;         /* to 1st order poly to convert c_unit to the standard unit */
} conv_tab[] = {

    { Time       , "s",     "s",    "seconds",        1,Standard, {{ 0.0                , 1.0                   }} },
    { Time       , "ps",    "s",    "pico-seconds",   7,Standard, {{ 0.0                , PICO                  }} },
    { Time       , "ns",    "s",    "nano-seconds",   7,Standard, {{ 0.0                , NANO                  }} },
    { Time       , "us",    "s",    "micro-seconds",  7,Standard, {{ 0.0                , MICRO                 }} },
    { Time       , "ms",    "s",    "milli-seconds",  7,Standard, {{ 0.0                , MILLI                 }} },
    { Time       , "cs",    "s",    "centi-seconds",  7,Standard, {{ 0.0                , CENTI                 }} },
    { Time       , "ds",    "s",    "deci-seconds",   7,Standard, {{ 0.0                , DECI                  }} },
    { Time       , "das",   "s",    "deca-seconds",   7,Standard, {{ 0.0                , DECA                  }} },
    { Time       , "hs",    "s",    "hecto-seconds",  7,Standard, {{ 0.0                , HECTO                 }} },
    { Time       , "ks",    "s",    "kilo-seconds",   7,Standard, {{ 0.0                , KILO                  }} },
    { Time       , "Ms",    "s",    "mega-seconds",   7,Standard, {{ 0.0                , MEGA                  }} },
    { Time       , "Gs",    "s",    "giga-seconds",   7,Standard, {{ 0.0                , GIGA                  }} },
    { Time       , "Ts",    "s",    "tera-seconds",   7,Standard, {{ 0.0                , TERA                  }} },
    { Time       , "min",   "s",    "minutes",        1,Standard, {{ 0.0                , 60.0                  }} },
    { Time       , "hr",    "s",    "hours",          1,Standard, {{ 0.0                , 3600.0                }} },
    { Time       , "day",   "s",    "days",           1,Standard, {{ 0.0                , 86400.0               }} },

    { Length     , "M",     "m",    "meters",         1,Metric,   {{ 0.0                , 1.0                   }} },
    { Length     , "m",     "m",    "meters",         7,Metric,   {{ 0.0                , 1.0                   }} },
    { Length     , "pm",    "m",    "pico-meters",    7,Metric,   {{ 0.0                , PICO                  }} },
    { Length     , "nm",    "m",    "nano-meters",    1,Metric,   {{ 0.0                , NANO                  }} },
    { Length     , "um",    "m",    "micro-meters",   7,Metric,   {{ 0.0                , MICRO                 }} },
    { Length     , "mm",    "m",    "milli-meters",   1,Metric,   {{ 0.0                , MILLI                 }} },
    { Length     , "cm",    "m",    "centi-meters",   1,Metric,   {{ 0.0                , CENTI                 }} },
    { Length     , "dm",    "m",    "deci-meters",    7,Metric,   {{ 0.0                , DECI                  }} },
    { Length     , "dam",   "m",    "deca-meters",    7,Metric,   {{ 0.0                , DECA                  }} },
    { Length     , "hm",    "m",    "hecto-meters",   7,Metric,   {{ 0.0                , HECTO                 }} },
    { Length     , "km",    "m",    "kilo-meters",    1,Metric,   {{ 0.0                , KILO                  }} },
    { Length     , "Mm",    "m",    "mega-meters",    7,Metric,   {{ 0.0                , MEGA                  }} },
    { Length     , "Gm",    "m",    "giga-meters",    7,Metric,   {{ 0.0                , GIGA                  }} },
    { Length     , "Tm",    "m",    "tera-meters",    7,Metric,   {{ 0.0                , TERA                  }} },
    { Length     , "ft",    "m",    "feet",           1,English,  {{ 0.0                , 0.3048                }} },
    { Length     , "kft",   "m",    "kilo-feet",      1,English,  {{ 0.0                , 0.3048 * KILO         }} },
    { Length     , "in",    "m",    "inches",         1,English,  {{ 0.0                , 0.0254                }} },
    { Length     , "yd",    "m",    "yards",          1,English,  {{ 0.0                , 0.9144                }} },
    { Length     , "mi",    "m",    "miles",          1,English,  {{ 0.0                , 1609.344              }} },
    { Length     , "n.m.",  "m",    "nautical-miles", 7,English,  {{ 0.0                , 1852.0                }} },
    { Length     , "nmile", "m",    "nautical-miles", 7,English,  {{ 0.0                , 1852.0                }} },

    { Angle      , "r",      "r",    "radians",       1,Standard, {{ 0.0                , 1.0                   }} },
    { Angle      , "rad",    "r",    "radians",       1,Standard, {{ 0.0                , 1.0                   }} },
    { Angle      , "pr",     "r",    "pico-radians",  7,Standard, {{ 0.0                , 1.0 * PICO            }} },
    { Angle      , "nr",     "r",    "nano-radians",  7,Standard, {{ 0.0                , 1.0 * NANO            }} },
    { Angle      , "ur",     "r",    "micro-radians", 7,Standard, {{ 0.0                , 1.0 * MICRO           }} },
    { Angle      , "mr",     "r",    "milli-radians", 1,Standard, {{ 0.0                , 1.0 * MILLI           }} },
    { Angle      , "cr",     "r",    "centi-radians", 7,Standard, {{ 0.0                , 1.0 * CENTI           }} },
    { Angle      , "dr",     "r",    "deci-radians",  7,Standard, {{ 0.0                , 1.0 * DECI            }} },
    { Angle      , "dar",    "r",    "deca-radians",  7,Standard, {{ 0.0                , 1.0 * DECA            }} },
    { Angle      , "kr",     "r",    "kilo-radians",  7,Standard, {{ 0.0                , 1.0 * KILO            }} },
    { Angle      , "Mr",     "r",    "mega-radians",  7,Standard, {{ 0.0                , 1.0 * MEGA            }} },
    { Angle      , "Gr",     "r",    "giga-radians",  7,Standard, {{ 0.0                , 1.0 * GIGA            }} },
    { Angle      , "Tr",     "r",    "tera-radians",  7,Standard, {{ 0.0                , 1.0 * TERA            }} },
    { Angle      , "d",      "r",    "degrees",       1,Standard, {{ 0.0                , 0.0174532925199433    }} },
    { Angle      , "degree", "r",    "degrees",       1,Standard, {{ 0.0                , 0.0174532925199433    }} },
    { Angle      , "as",     "r",    "arc-seconds",   1,Standard, {{ 0.0                , 4.848136811095362e-06 }} },
    { Angle      , "arcsecond", "r", "arc-seconds",   1,Standard, {{ 0.0                , 4.848136811095362e-06 }} },
    { Angle      , "am",     "r",    "arc-minutes",   1,Standard, {{ 0.0                , 0.0002908882086657216 }} },
    { Angle      , "arcminute", "r", "arc-minutes",   1,Standard, {{ 0.0                , 0.0002908882086657216 }} },
    { Angle      , "rev",    "r",    "revolutions",   1,Standard, {{ 0.0                , 6.28318530717958647   }} },
    { Angle      , "revolution", "r","revolutions",   1,Standard, {{ 0.0                , 6.28318530717958647   }} },

    { Mass       , "g",      "kg",    "grams",        1,Metric,   {{ 0.0               , MILLI                 }} },
    { Mass       , "pg",     "kg",    "pico-grams",   1,Metric,   {{ 0.0               , MILLI * PICO          }} },
    { Mass       , "ng",     "kg",    "nano-grams",   7,Metric,   {{ 0.0               , MILLI * NANO          }} },
    { Mass       , "ug",     "kg",    "micro-grams",  7,Metric,   {{ 0.0               , MILLI * MICRO         }} },
    { Mass       , "mg",     "kg",    "milli-grams",  7,Metric,   {{ 0.0               , MILLI * MILLI         }} },
    { Mass       , "cg",     "kg",    "centi-grams",  7,Metric,   {{ 0.0               , MILLI * CENTI         }} },
    { Mass       , "dg",     "kg",    "deci-grams",   7,Metric,   {{ 0.0               , MILLI * DECI          }} },
    { Mass       , "dag",    "kg",    "deca-grams",   7,Metric,   {{ 0.0               , MILLI * DECA          }} },
    { Mass       , "hg",     "kg",    "hecto-grams",  7,Metric,   {{ 0.0               , MILLI * HECTO         }} },
    { Mass       , "kg",     "kg",    "kilo-grams",   1,Metric,   {{ 0.0               , MILLI * KILO          }} },
    { Mass       , "Mg",     "kg",    "mega-grams",   7,Metric,   {{ 0.0               , MILLI * MEGA          }} },
    { Mass       , "Gg",     "kg",    "giga-grams",   7,Metric,   {{ 0.0               , MILLI * GIGA          }} },
    { Mass       , "Tg",     "kg",    "tera-grams",   7,Metric,   {{ 0.0               , MILLI * TERA          }} },
    { Mass       , "mt",     "kg",   "metric-tons",   1,Metric,   {{ 0.0                , 1000.0               }} },
    { Mass       , "t",      "kg",   "metric-tons",   1,Metric,   {{ 0.0                , 1000.0               }} },
    { Mass       , "sl",     "kg",   "slugs",         1,English,  {{ 0.0                , 14.59390             }} },
    { Mass       , "slug",   "kg",   "slugs",         1,English,  {{ 0.0                , 14.59390             }} },
    { Mass       , "lbm",    "kg",   "pound(mass)",   1,English,  {{ 0.0                , 0.45359237           }} },
    { Mass       , "lb",     "kg",   "pound(mass)",   1,English,  {{ 0.0                , 0.45359237           }} },

    { Force      , "N",      "N",    "newtons",       1,Metric,   {{ 0.0                , 1.0                   }} },
    { Force      , "pN",     "N",    "pico-newtons",  7,Metric,   {{ 0.0                , PICO                  }} },
    { Force      , "nN",     "N",    "nano-newtons",  7,Metric,   {{ 0.0                , NANO                  }} },
    { Force      , "uN",     "N",    "micro-newtons", 7,Metric,   {{ 0.0                , MICRO                 }} },
    { Force      , "mN",     "N",    "milli-newtons", 7,Metric,   {{ 0.0                , MILLI                 }} },
    { Force      , "cN",     "N",    "centi-newtons", 7,Metric,   {{ 0.0                , CENTI                 }} },
    { Force      , "dN",     "N",    "deci-newtons",  7,Metric,   {{ 0.0                , DECI                  }} },
    { Force      , "daN",    "N",    "deca-newtons",  7,Metric,   {{ 0.0                , DECA                  }} },
    { Force      , "hN",     "N",    "hecto-newtons", 7,Metric,   {{ 0.0                , HECTO                 }} },
    { Force      , "kN",     "N",    "kilo-newtons",  1,Metric,   {{ 0.0                , KILO                  }} },
    { Force      , "MN",     "N",    "mega-newtons",  7,Metric,   {{ 0.0                , MEGA                  }} },
    { Force      , "GN",     "N",    "giga-newtons",  7,Metric,   {{ 0.0                , GIGA                  }} },
    { Force      , "TN",     "N",    "tera-newtons",  7,Metric,   {{ 0.0                , TERA                  }} },
    { Force      , "oz",     "N",    "ounces",        1,English,  {{ 0.0                , 0.2780139             }} },
    { Force      , "ozf",    "N",    "ounces",        1,English,  {{ 0.0                , 0.2780139             }} },
    { Force      , "lbf",    "N",    "pound(force)",  1,English,  {{ 0.0                , 4.4482216152605       }} },

    { Voltage    , "v",      "v",    "volts",         1,Standard, {{ 0.0                , 1.0   }} },
    { Voltage    , "V",      "v",    "volts",         1,Standard, {{ 0.0                , 1.0   }} },
    { Voltage    , "pv",     "v",    "pico-volts",    7,Standard, {{ 0.0                , PICO  }} },
    { Voltage    , "nv",     "v",    "nano-volts",    7,Standard, {{ 0.0                , NANO  }} },
    { Voltage    , "uv",     "v",    "micro-volts",   7,Standard, {{ 0.0                , MICRO }} },
    { Voltage    , "mv",     "v",    "milli-volts",   7,Standard, {{ 0.0                , MILLI }} },
    { Voltage    , "cv",     "v",    "centi-volts",   7,Standard, {{ 0.0                , CENTI }} },
    { Voltage    , "dv",     "v",    "deci-volts",    7,Standard, {{ 0.0                , DECI  }} },
    { Voltage    , "dav",    "v",    "deca-volts",    7,Standard, {{ 0.0                , DECA  }} },
    { Voltage    , "hv",     "v",    "hecto-volts",   7,Standard, {{ 0.0                , HECTO }} },
    { Voltage    , "kv",     "v",    "kilo-volts",    1,Standard, {{ 0.0                , KILO  }} },
    { Voltage    , "Mv",     "v",    "mega-volts",    7,Standard, {{ 0.0                , MEGA  }} },
    { Voltage    , "Gv",     "v",    "giga-volts",    7,Standard, {{ 0.0                , GIGA  }} },
    { Voltage    , "Tv",     "v",    "tera-volts",    7,Standard, {{ 0.0                , TERA  }} },

    { Current    , "amp",    "amp",  "amperes",       1,Standard, {{ 0.0                , 1.0   }} },
    { Current    , "pamp",   "amp",  "pico-amperes",  7,Standard, {{ 0.0                , PICO  }} },
    { Current    , "namp",   "amp",  "nano-amperes",  7,Standard, {{ 0.0                , NANO  }} },
    { Current    , "uamp",   "amp",  "micro-amperes", 7,Standard, {{ 0.0                , MICRO }} },
    { Current    , "mamp",   "amp",  "milli-amperes", 1,Standard, {{ 0.0                , MILLI }} },
    { Current    , "camp",   "amp",  "centi-amperes", 7,Standard, {{ 0.0                , CENTI }} },
    { Current    , "damp",   "amp",  "deci-amperes",  7,Standard, {{ 0.0                , DECI  }} },
    { Current    , "daamp",  "amp",  "deca-amperes",  7,Standard, {{ 0.0                , DECA  }} },
    { Current    , "hamp",   "amp",  "hecto-amperes", 7,Standard, {{ 0.0                , HECTO }} },
    { Current    , "kamp",   "amp",  "kilo-amperes",  7,Standard, {{ 0.0                , KILO  }} },
    { Current    , "Mamp",   "amp",  "mega-amperes",  7,Standard, {{ 0.0                , MEGA  }} },
    { Current    , "Gamp",   "amp",  "giga-amperes",  7,Standard, {{ 0.0                , GIGA  }} },
    { Current    , "Tamp",   "amp",  "tera-amperes",  7,Standard, {{ 0.0                , TERA  }} },

    { Resistance , "ohm",    "ohm",  "ohms",          1,Standard, {{ 0.0                , 1.0   }} },
    { Resistance , "pohm",   "ohm",  "pico-ohms",     7,Standard, {{ 0.0                , PICO  }} },
    { Resistance , "nohm",   "ohm",  "nano-ohms",     7,Standard, {{ 0.0                , NANO  }} },
    { Resistance , "uohm",   "ohm",  "micro-ohms",    7,Standard, {{ 0.0                , MICRO }} },
    { Resistance , "mohm",   "ohm",  "milli-ohms",    7,Standard, {{ 0.0                , MILLI }} },
    { Resistance , "cohm",   "ohm",  "centi-ohms",    7,Standard, {{ 0.0                , CENTI }} },
    { Resistance , "dohm",   "ohm",  "deci-ohms",     7,Standard, {{ 0.0                , DECI  }} },
    { Resistance , "daohm",  "ohm",  "deca-ohms",     7,Standard, {{ 0.0                , DECA  }} },
    { Resistance , "hohm",   "ohm",  "hecto-ohms",    7,Standard, {{ 0.0                , HECTO }} },
    { Resistance , "kohm",   "ohm",  "kilo-ohms",     7,Standard, {{ 0.0                , KILO  }} },
    { Resistance , "Mohm",   "ohm",  "mega-ohms",     7,Standard, {{ 0.0                , MEGA  }} },
    { Resistance , "Gohm",   "ohm",  "giga-ohms",     7,Standard, {{ 0.0                , GIGA  }} },
    { Resistance , "Tohm",   "ohm",  "tera-ohms",     7,Standard, {{ 0.0                , TERA  }} },

    { Temperature, "C",   "C",    "degrees-celsius",  1,Metric,   {{ 0.0                , 1.0                   }} },
    { Temperature, "degC","C",    "degrees-celsius",  1,Metric,   {{ 0.0                , 1.0                   }} },
    { Temperature, "K",   "C",    "degrees-kelvin",   1,Metric,   {{ -273.15            , 1.0                   }} },
    { Temperature, "F",   "C",    "degrees-fahrenheit",1,English, {{ -17.777777777777778, 0.55555555555555556   }} },
    { Temperature, "degF","C",    "degrees-fahrenheit",1,English, {{ -17.777777777777778, 0.55555555555555556   }} },
    { Temperature, "R",   "C",    "degrees-rankine",  1,English,  {{ -273.15            , 0.55555555555555556   }} },
    { Temperature, "degR","C",    "degrees-rankine",  1,English,  {{ -273.15            , 0.55555555555555556   }} },

    { Sound,       "dB",  "dB",   "decibels",         1,Standard, {{ 0.0                , 1.0                   }} },

    {Energy,       "J",   "J",    "joule",            9,Metric,   {{ 0.0                , 1.0                   }} },
    {Energy,       "pJ",  "J",    "pico-joule",       9,Metric,   {{ 0.0                , PICO                  }} },
    {Energy,       "nJ",  "J",    "nano-joule",       9,Metric,   {{ 0.0                , NANO                  }} },
    {Energy,       "uJ",  "J",    "micro-joule",      9,Metric,   {{ 0.0                , MICRO                 }} },
    {Energy,       "mJ",  "J",    "milli-joule",      9,Metric,   {{ 0.0                , MILLI                 }} },
    {Energy,       "cJ",  "J",    "centi-joule",      9,Metric,   {{ 0.0                , CENTI                 }} },
    {Energy,       "dJ",  "J",    "deci-joule",       9,Metric,   {{ 0.0                , DECI                  }} },
    {Energy,       "daJ", "J",    "deca-joule",       9,Metric,   {{ 0.0                , DECA                  }} },
    {Energy,       "hJ",  "J",    "hecto-joule",      9,Metric,   {{ 0.0                , HECTO                 }} },
    {Energy,       "kJ",  "J",    "kilo-joule",       9,Metric,   {{ 0.0                , KILO                  }} },
    {Energy,       "MJ",  "J",    "mega-joule",       9,Metric,   {{ 0.0                , MEGA                  }} },
    {Energy,       "GJ",  "J",    "giga-joule",       9,Metric,   {{ 0.0                , GIGA                  }} },
    {Energy,       "TJ",  "J",    "tera-joule",       9,Metric,   {{ 0.0                , TERA                  }} },
    {Energy,       "BTU", "J",    "british-thermal",  9,English,  {{ 0.0                , 1055.056              }} },
    {Energy,       "TNT", "J",    "ton-tnt",          9,Metric,   {{ 0.0                , 4.184 * GIGA          }} },
    {Energy,       "ton_TNT", "J","ton-tnt",          9,Metric,   {{ 0.0                , 4.184 * GIGA          }} },
    {Energy,       "Wh",  "J",    "watt-hour",        9,Metric,   {{ 0.0                , 3600                  }} },
    {Energy,       "kWh", "J",    "kilo-watt-hour",   9,Metric,   {{ 0.0                , 3600000               }} },

    {Power,        "W",   "W",    "watt",             9,Metric,   {{ 0.0                , 1.0                   }} },
    {Power,        "pW",  "W",    "pico-watt",        9,Metric,   {{ 0.0                , PICO                  }} },
    {Power,        "nW",  "W",    "nano-watt",        9,Metric,   {{ 0.0                , NANO                  }} },
    {Power,        "uW",  "W",    "micro-watt",       9,Metric,   {{ 0.0                , MICRO                 }} },
    {Power,        "mW",  "W",    "milli-watt",       9,Metric,   {{ 0.0                , MILLI                 }} },
    {Power,        "cW",  "W",    "centi-watt",       9,Metric,   {{ 0.0                , CENTI                 }} },
    {Power,        "dW",  "W",    "deci-watt",        9,Metric,   {{ 0.0                , DECI                  }} },
    {Power,        "daW", "W",    "deca-watt",        9,Metric,   {{ 0.0                , DECA                  }} },
    {Power,        "hW",  "W",    "hecto-watt",       9,Metric,   {{ 0.0                , HECTO                 }} },
    {Power,        "kW",  "W",    "kilo-watt",        9,Metric,   {{ 0.0                , KILO                  }} },
    {Power,        "MW",  "W",    "mega-watt",        9,Metric,   {{ 0.0                , MEGA                  }} },
    {Power,        "GW",  "W",    "giga-watt",        9,Metric,   {{ 0.0                , GIGA                  }} },
    {Power,        "TW",  "W",    "tera-watt",        9,Metric,   {{ 0.0                , TERA                  }} },
    {Power,        "hp",  "W",    "horse-power",      9,English,  {{ 0.0                , 745.69987             }} },

    {Pressure,     "Pa",  "Pa",   "pascals",          9,Metric,   {{ 0.0                , 1.0                   }} },
    {Pressure,     "pPa", "Pa",   "pico-pascals",     9,Metric,   {{ 0.0                , PICO                  }} },
    {Pressure,     "nPa", "Pa",   "nano-pascals",     9,Metric,   {{ 0.0                , NANO                  }} },
    {Pressure,     "uPa", "Pa",   "micro-pascals",    9,Metric,   {{ 0.0                , MICRO                 }} },
    {Pressure,     "mPa", "Pa",   "milli-pascals",    9,Metric,   {{ 0.0                , MILLI                 }} },
    {Pressure,     "cPa", "Pa",   "centi-pascals",    9,Metric,   {{ 0.0                , CENTI                 }} },
    {Pressure,     "dPa", "Pa",   "deci-pascals",     9,Metric,   {{ 0.0                , DECI                  }} },
    {Pressure,     "daPa","Pa",   "deca-pascals",     9,Metric,   {{ 0.0                , DECA                  }} },
    {Pressure,     "hPa", "Pa",   "hecto-pascals",    9,Metric,   {{ 0.0                , HECTO                 }} },
    {Pressure,     "kPa", "Pa",   "kilo-pascals",     9,Metric,   {{ 0.0                , KILO                  }} },
    {Pressure,     "MPa", "Pa",   "mega-pascals",     9,Metric,   {{ 0.0                , MEGA                  }} },
    {Pressure,     "GPa", "Pa",   "giga-pascals",     9,Metric,   {{ 0.0                , GIGA                  }} },
    {Pressure,     "TPa", "Pa",   "tera-pascals",     9,Metric,   {{ 0.0                , TERA                  }} },
    {Pressure,     "psi", "Pa",   "pounds-sq-inch",   9,English,  {{ 0.0                , 6894.7569999999996    }} },
    {Pressure,     "atm", "Pa",   "atmospheres",      9,Metric,   {{ 0.0                , 101325.00000          }} },
    {Pressure,     "mmHg","Pa",   "milli-meters-mercury",10,Metric,{{ 0.0               , MILLI*133322.387415 }} },
    {Pressure,     "inHg","Pa",   "inches-mercury",  13,English,  {{ 0.0                , 3386.389 }} },
    {Pressure,     "mmWater","Pa","milli-meters-water",13,Metric, {{ 0.0                , MILLI*9806.65 }} },
    {Pressure,     "inWater","Pa","inches-water",    13,English,  {{ 0.0                , 249.08891 }} },

    {Volume,       "l",   "l",    "liter",           10,Metric,   {{ 0.0                , 1.0                   }} },
    {Volume,       "pl",  "l",    "pico-liter",      10,Metric,   {{ 0.0                , PICO                  }} },
    {Volume,       "nl",  "l",    "nano-liter",      10,Metric,   {{ 0.0                , NANO                  }} },
    {Volume,       "ul",  "l",    "micro-liter",     10,Metric,   {{ 0.0                , MICRO                 }} },
    {Volume,       "ml",  "l",    "milli-liter",     10,Metric,   {{ 0.0                , MILLI                 }} },
    {Volume,       "cl",  "l",    "centi-liter",     10,Metric,   {{ 0.0                , CENTI                 }} },
    {Volume,       "dl",  "l",    "deci-liter",      10,Metric,   {{ 0.0                , DECI                  }} },
    {Volume,       "dal", "l",    "deca-liter",      10,Metric,   {{ 0.0                , DECA                  }} },
    {Volume,       "hl",  "l",    "hecto-liter",     10,Metric,   {{ 0.0                , HECTO                 }} },
    {Volume,       "kl",  "l",    "kilo-liter",      10,Metric,   {{ 0.0                , KILO                  }} },
    {Volume,       "Ml",  "l",    "mega-liter",      10,Metric,   {{ 0.0                , MEGA                  }} },
    {Volume,       "Gl",  "l",    "giga-liter",      10,Metric,   {{ 0.0                , GIGA                  }} },
    {Volume,       "Tl",  "l",    "tera-liter",      10,Metric,   {{ 0.0                , TERA                  }} },
    {Volume,       "gal", "l",    "gallon",          10,English,  {{ 0.0                , 3.785412              }} },
    {Volume,       "gallon", "l", "gallon",          10,English,  {{ 0.0                , 3.785412              }} },
    {Volume,       "floz","l",    "fluid ounce",     10,English,  {{ 0.0                , 0.02957353            }} },

    { Unitless,    "1",   "--",   "<Unitless>",       1,Standard, {{ 0.0                , 1.0                   }} },
    { Unitless,    "--",  "--",   "<Unitless>",       1,Standard, {{ 0.0                , 1.0                   }} },
    { Unitless,    "cnt", "--",   "<Unitless>",       1,Standard, {{ 0.0                , 1.0                   }} },
    { Unitless,    "count", "--", "<Unitless>",       1,Standard, {{ 0.0                , 1.0                   }} },
    { Unitless,    "mol", "--",   "<Unitless>",       7,Standard, {{ 0.0                , 6.02214179e+23        }} },
    { Unitless,    "one", "--",   "<Unitless>",       1,Standard, {{ 0.0                , 1.0                   }} },

    {Frequency,     "Hz",  "Hz",   "hertz",           10,Metric,  {{ 0.0                , 1.0                   }} },
    {Frequency,    "pHz",  "Hz",   "pico-hertz",      10,Metric,  {{ 0.0                , 1.0 * PICO            }} },
    {Frequency,    "nHz",  "Hz",   "nano-hertz",      10,Metric,  {{ 0.0                , 1.0 * NANO            }} },
    {Frequency,    "uHz",  "Hz",   "micro-hertz",     10,Metric,  {{ 0.0                , 1.0 * MICRO           }} },
    {Frequency,    "mHz",  "Hz",   "milli-hertz",     10,Metric,  {{ 0.0                , 1.0 * MILLI           }} },
    {Frequency,    "cHz",  "Hz",   "centi-hertz",     10,Metric,  {{ 0.0                , 1.0 * CENTI           }} },
    {Frequency,    "dHz",  "Hz",   "deci-hertz",      10,Metric,  {{ 0.0                , 1.0 * DECI            }} },
    {Frequency,    "daHz", "Hz",   "deca-hertz",      10,Metric,  {{ 0.0                , 1.0 * DECA            }} },
    {Frequency,    "hHz",  "Hz",   "hecto-hertz",     10,Metric,  {{ 0.0                , 1.0 * HECTO           }} },
    {Frequency,    "kHz",  "Hz",   "kilo-hertz",      10,Metric,  {{ 0.0                , 1.0 * KILO            }} },
    {Frequency,    "MHz",  "Hz",   "mega-hertz",      10,Metric,  {{ 0.0                , 1.0 * MEGA            }} },
    {Frequency,    "GHz",  "Hz",   "giga-hertz",      10,Metric,  {{ 0.0                , 1.0 * GIGA            }} },
    {Frequency,    "THz",  "Hz",   "tera-hertz",      10,Metric,  {{ 0.0                , 1.0 * TERA            }} },
														   };

#define N_conversions  ( sizeof conv_tab / sizeof conv_tab[0] )


static int conversion_record(char *c_unit)
{
    unsigned int i;
    int ix = -1;

    for (i = 0; i < N_conversions; i++) {
        if (!strcmp(c_unit, conv_tab[i].c_unit)) {
            ix = i;
            break;
        }
    }
    if (ix < 0) {
        fprintf(stderr, "Unknown Unit \"%s\".\n", c_unit);
    }
    return (ix);
}


static Conv_Status_t convert_primitive(char *from, char *to, UnitsConvFn_t * p)
{
    int ix_from, ix_to;

    if ((ix_from = conversion_record(from)) < 0)
        return (CONV_ERR);
    if ((ix_to = conversion_record(to)) < 0)
        return (CONV_ERR);

    if (conv_tab[ix_from].measure == conv_tab[ix_to].measure) {
        p->C[0] = (conv_tab[ix_from].poly.C[0] - conv_tab[ix_to].poly.C[0]) / conv_tab[ix_to].poly.C[1];
        p->C[1] = conv_tab[ix_from].poly.C[1] / conv_tab[ix_to].poly.C[1];
    } else {
        fprintf(stderr, "ERROR: Units dont measure the same things.\n");
        return CONV_ERR;
    }
    return CONV_OK;
}

static Conv_Status_t next_lexeme(parse_context_t * context)
{
    int status = CONV_OK;

    switch (context->s[context->n]) {
        case '*':
            context->token = Mult_Op;
            context->n++;
            break;
        case '/':
            context->token = Div_Op;
            context->n++;
            break;
        case '2':
            context->token = Square_Op;
            context->n++;
            break;
        case '3':
            context->token = Cube_Op;
            context->n++;
            break;
        case '^':
            context->n++;
            status = next_lexeme(context) ;
            break;
        default:{
                unsigned int i;
                int maxlen = 0;
                for (i = 0; i < N_conversions; i++) {
                    int len = 0;

                    while ((context->s[context->n + len] == conv_tab[i].c_unit[len]) &&
                           (context->s[context->n + len] != '\0')) {
                        len++;
                    }
                    if ((conv_tab[i].c_unit[len] == '\0') && (len > maxlen)) {
                        maxlen = len;
                        context->ix = i;
                    }
                }
                if (maxlen > 0) {
                    context->token = Units_Primitive;
                    context->n += maxlen;
                } else if (context->s[context->n] == '\0') {
                    context->token = End_of_input;
                } else {
                    context->token = Unknown;
                    fprintf(stderr, "Syntax error: Expected a units primitive in \"%s\" at character position [%d].\n",
                            context->s, context->n);
                    status = CONV_ERR;
                }
            }                          /* default */
    }                                  /* switch */

    return ((Conv_Status_t) status);
}


static Conv_Status_t emit(parse_context_t * context, int op, int a1, int a2)
{

    Units_t *units = context->units;

    if (units->nt < MAX_TRIADS) {
        units->t[units->nt].op = op;
        units->t[units->nt].a1 = a1;
        units->t[units->nt].a2 = a2;
        units->nt++;
        return CONV_OK;
    } else {
        fprintf(stderr, "Unit specification string is too long.\n");
        return CONV_ERR;
    }
}

static Conv_Status_t units_factor(parse_context_t * context)
{
    int conversion_ix;
    int exponent = 1;

    if (context->token != Units_Primitive) {
        fprintf(stderr, "Syntax error: Expected a units primitive in \"%s\" before character position [%d].\n",
                context->s, context->n);
        return (CONV_ERR);
    }
    conversion_ix = context->ix;
    if (next_lexeme(context) != CONV_OK) {
        return (CONV_ERR);
    }
    if (context->token == Square_Op) {
        exponent = 2;
        if (next_lexeme(context) != CONV_OK) {
            return (CONV_ERR);
        }
    } else if (context->token == Cube_Op) {
        exponent = 3;
        if (next_lexeme(context) != CONV_OK) {
            return (CONV_ERR);
        }
    }
    return (emit(context, Units_Primitive, conversion_ix, exponent));
}

static Conv_Status_t units_product(parse_context_t * context, int *n_factors)
{

    if (units_factor(context) != CONV_OK) {
        return CONV_ERR;
    }
    (*n_factors)++;

    while (context->token == Mult_Op) {
        if (next_lexeme(context) != CONV_OK) {
            return (CONV_ERR);
        }
        if (units_factor(context) != CONV_OK) {
            return CONV_ERR;
        }
        (*n_factors)++;
    }

    return CONV_OK;
}

static Conv_Status_t units_specifier(parse_context_t * context)
{
    int n_factors = 0;
    int n_products = 0;

    if (units_product(context, &n_factors) != CONV_OK) {
        return CONV_ERR;
    }
    if (n_factors > 1) {
        if (emit(context, Mult_Op, n_factors, 0) != CONV_OK) {
            return (CONV_ERR);
        }
    }
    n_factors = 0;
    n_products++;
    while (context->token == Div_Op) {
        if ((next_lexeme(context) != CONV_OK) || (units_product(context, &n_factors) != CONV_OK)) {
            return (CONV_ERR);
        }
        n_products++;
    }
    if (context->token != End_of_input) {
        return (CONV_ERR);
    } else {
        if (n_factors > 1) {
            if (emit(context, Mult_Op, n_factors, 0) != CONV_OK) {
                return (CONV_ERR);
            }
        }
        if (n_products > 1) {
            if (emit(context, Div_Op, n_products, 0) != CONV_OK) {
                return (CONV_ERR);
            }
        }
    }
    return CONV_OK;
}


static void normalize(Units_t * units)
{

    int i, j, k;
    /* Sort factors in the same product by their measure. Since reals are commutative under multiplication "oz*in" and
       "in*oz" (for example) should mean the same thing and be convertable to "M*N" or "N*M". */
    i = units->nt;
    while (i > 0) {
        if (units->t[i].op == Mult_Op) {
            int sorting = 1;
            int n_operands = units->t[i].a1;
            while (sorting) {
                sorting = 0;
                for (j = 1; j < n_operands; j++) {
                    k = i - j;
                    if (conv_tab[units->t[k].a1].measure > conv_tab[units->t[k - 1].a1].measure) {
                        triad_t temp;
                        temp = units->t[k];
                        units->t[k] = units->t[k - 1];
                        units->t[k - 1] = temp;
                        sorting = 1;
                    }
                }
            }
            i = i - (1 + n_operands);
        } else {
            i--;
        }
    }
    /* Combine like factors (having the same units_primitives) in the same product (and sum their exponents). Ex: sss
       would become s3. */
    i = units->nt;
    while (i > 0) {
        if (units->t[i].op == Mult_Op) {
            int n_operands = units->t[i].a1;
            for (j = 1; j < n_operands; j++) {
                k = i - j;
                if (units->t[k - 1].a1 == units->t[k].a1) {
                    units->t[k - 1].a2 += units->t[k].a2;
                    units->t[k].op = No_Op;
                    units->t[i].a1--;
                    if (units->t[i].a1 < 2)
                        units->t[i].op = No_Op;
                }
            }
            i = i - (1 + n_operands);
        } else {
            i--;
        }
    }
    /* Remove any No_Ops */
    for (i = 0, j = 0; i < units->nt; i++) {
        if (units->t[i].op != No_Op) {
            if (i != j) {
                units->t[j] = units->t[i];
            }
        }
        if (units->t[j].op != No_Op) {
            j++;
        }
    }
    units->nt = j;
}


Units_t *new_units(const char *units_spec)
{

    Units_t *units;
    parse_context_t context;

    if ((units = (Units_t *) CONV_ALLOC((size_t) 1, (size_t) sizeof(Units_t))) == NULL) {
        fprintf(stderr, "init_parse_context: Out of memory.\n");
        exit(1);
    }
    units->nt = 0;
    context.token = Unknown;
    context.ix = 0;
    context.n = 0;
    context.s = units_spec;
    context.units = units;
    if ((next_lexeme(&context) != CONV_OK) || (units_specifier(&context) != CONV_OK)) {
        CONV_FREE(units);
        return NULL;
    }
    normalize(units);
    return (units);
}

static void init_stack(UCF_stack_t * stack)
{
    stack->top = 0;
}

static Conv_Status_t push(UCF_stack_t * stack, UnitsConvFn_t item)
{
    if (stack->top >= MAX_TRIADS) {
        fprintf(stderr, "Stack overflow.\n");
        return (CONV_ERR);
    } else {
        stack->a[stack->top] = item;
        stack->top++;
    }
    return CONV_OK;
}

static Conv_Status_t pop(UCF_stack_t * stack, UnitsConvFn_t * item)
{
    if (stack->top == 0) {
        fprintf(stderr, "Stack underflow.\n");
        return (CONV_ERR);
    } else {
        stack->top--;
        *item = stack->a[stack->top];
    }
    return CONV_OK;
}


Conv_Status_t conv_fn_u(const Units_t * f_units,        /* IN */
                        const Units_t * t_units,        /* IN */
                        UnitsConvFn_t * conv_fn)
{                                      /* OUT */

    int i, j;
    UCF_stack_t u_stack;

    init_stack(&u_stack);

    if (f_units->nt != t_units->nt) {
        fprintf(stderr, "Units specifications have different number of units_primitives.\n");
        return (CONV_ERR);
    }

    for (i = 0; i < f_units->nt; i++) {
        if (f_units->t[i].op != t_units->t[i].op) {
            fprintf(stderr, "Units specifications are incompatible.\n");
            return (CONV_ERR);
        }
        switch (f_units->t[i].op) {

            case Units_Primitive:{
                    UnitsConvFn_t p;

                    if (conv_tab[f_units->t[i].a1].measure != conv_tab[t_units->t[i].a1].measure) {


                        fprintf(stderr, "Units primitives don't measure the same thing.\n");
                        fprintf(stderr, "\"%s\" is a measure of \"%s\", whereas \"%s\" is a measure of \"%s\".\n",
                                conv_tab[f_units->t[i].a1].c_unit,
                                measure_name[conv_tab[f_units->t[i].a1].measure],
                                conv_tab[t_units->t[i].a1].c_unit, measure_name[conv_tab[t_units->t[i].a1].measure]);
                        return (CONV_ERR);
                    }

                    if (f_units->t[i].a2 != t_units->t[i].a2) {
                        fprintf(stderr, "Units primitives are not the same dimension.\n");
                        return (CONV_ERR);
                    }

                    if (convert_primitive(conv_tab[f_units->t[i].a1].c_unit,
                                          conv_tab[t_units->t[i].a1].c_unit, &p) != CONV_OK) {
                        return (CONV_ERR);
                    }

                    switch (f_units->t[i].a2) { /* the exponent */
                        case 3:
                            if (p.C[0] != 0.0) {
                                fprintf(stderr, "Unable represent the conversion to %s3 as a linear function of %s3.\n",
                                        conv_tab[t_units->t[i].a1].c_unit, conv_tab[f_units->t[i].a1].c_unit);
                                return (CONV_ERR);
                            };
                            p.C[1] = p.C[1] * p.C[1] * p.C[1];
                            if (push(&u_stack, p) != CONV_OK)
                                return (CONV_ERR);
                            break;
                        case 2:
                            if (p.C[0] != 0.0) {
                                fprintf(stderr, "Unable represent the conversion to %s2 as a linear function of %s2.\n",
                                        conv_tab[t_units->t[i].a1].c_unit, conv_tab[f_units->t[i].a1].c_unit);
                                return (CONV_ERR);
                            };
                            p.C[1] = p.C[1] * p.C[1];
                            if (push(&u_stack, p) != CONV_OK)
                                return (CONV_ERR);
                            break;
                        case 1:
                            if (push(&u_stack, p) != CONV_OK)
                                return (CONV_ERR);
                            break;
                        default:
                            fprintf(stderr, "Exponent is outside the range [1..3].\n");
                            return (CONV_ERR);
                    }

                };
                break;

            case Mult_Op:{
                    UnitsConvFn_t product = { {0.0, 1.0}
                    };
                    UnitsConvFn_t factor;

                    if (f_units->t[i].a1 != t_units->t[i].a1) {
                        fprintf(stderr, "Numbers of factors in a product dont match.\n");
                        return CONV_ERR;
                    }
                    // For each of the factors in this product
                    for (j = 0; j < f_units->t[i].a1; j++) {

                        if (pop(&u_stack, &factor) != CONV_OK)
                            return CONV_ERR;

                        if (product.C[0] == 0.0) {
                            double a = product.C[1];
                            product.C[1] = a * factor.C[1];
                            product.C[0] = a * factor.C[0];
                        } else if (factor.C[0] == 0.0) {
                            double a = factor.C[1];
                            product.C[1] = a * product.C[1];
                            product.C[0] = a * product.C[0];
                        } else {
                            fprintf(stderr, "Unable to create a linear conversion.\n");
                            return CONV_ERR;
                        }
                    }
                    if (push(&u_stack, product) != CONV_OK)
                        return CONV_ERR;
                };
                break;

            case Div_Op:{
                    UnitsConvFn_t denominator;
                    UnitsConvFn_t numerator;
                    UnitsConvFn_t ratio;

                    if (pop(&u_stack, &denominator) != CONV_OK)
                        return CONV_ERR;
                    if (pop(&u_stack, &numerator) != CONV_OK)
                        return CONV_ERR;

                    ratio.C[1] = numerator.C[1] / denominator.C[1];
                    ratio.C[0] = 0.0;

                    if (push(&u_stack, ratio) != CONV_OK)
                        return CONV_ERR;

                };
                break;

            default:{
                    fprintf(stderr, "Internal Error 1. Unable to complete the conversion.\n");
                    return CONV_ERR;
                }
        }                              /* switch */

    }                                  /* for each triad */

    if ((pop(&u_stack, conv_fn) != CONV_OK) /* || (u_stack.top != 0) */ ) {
        conv_fn->C[0] = 0.0;
        conv_fn->C[1] = 1.0;
        fprintf(stderr, "Internal Error 2. Unable to complete the conversion.\n");
        return CONV_ERR;
    }
    return CONV_OK;
}


Conv_Status_t conv_fn_s(const char *f_units_s,  /* IN */
                        const char *t_units_s,  /* IN */
                        UnitsConvFn_t * conv_fn)
{                                      /* OUT */
    Conv_Status_t rc;
    Units_t *f_units;
    Units_t *t_units;

    if ((f_units = new_units(f_units_s)) == NULL) {
        return CONV_ERR;
    }
    if ((t_units = new_units(t_units_s)) == NULL) {
        return CONV_ERR;
    }
    rc = conv_fn_u(f_units, t_units, conv_fn);
    CONV_FREE(f_units);
    CONV_FREE(t_units);

    return (rc);
}


double convert_units(double from_value, const UnitsConvFn_t * conv_fn)
{
    return (from_value * conv_fn->C[1] + conv_fn->C[0]);
}

void list_units(void)
{
    unsigned int i;
    char  * curr_name = NULL ;

    for (i = 0; i < N_conversions; i++) {
        if ( curr_name != measure_name[conv_tab[i].measure] ) {
            printf("\n%s\n", measure_name[conv_tab[i].measure]) ;
            printf("%-10s %-20s %-10s\n", "Unit", "Description", "System") ;
            printf("---------------------------------------------\n") ;
            curr_name = measure_name[conv_tab[i].measure] ;
        }
        printf("%-10s %-20s %-10s\n", conv_tab[i].c_unit, conv_tab[i].desc,
                unit_system_name[conv_tab[i].system]);
    }
}

/*
 *
 */
int convert_units_spec(const char *old_units_spec, char *new_units_spec)
{

    parse_context_t context;
    int done = 0;
    int state = 0;
    int rc = 0;

    context.token = Unknown;
    context.ix = 0;
    context.n = 0;
    context.s = old_units_spec;
    context.units = NULL;              /* We are are not creating a units object here. */

    new_units_spec[0] = 0;

    while (!done) {
        if (next_lexeme(&context) != CONV_OK) {
            rc = 1;
            done = 1;
        }
        switch (state) {
            case 0:{
                    if (context.token == Units_Primitive) {
                        if (conv_tab[context.ix].era == 1) {
                            if (strcmp("M", conv_tab[context.ix].c_unit) == 0) {
                                strcat(new_units_spec, "m");
                            } else if (strcmp("nm", conv_tab[context.ix].c_unit) == 0) {
                                strcat(new_units_spec, "n.m.");
                            } else {
                                strcat(new_units_spec, conv_tab[context.ix].c_unit);
                            }
                            state = 1;
                        } else {
                            rc = 1;
                            done = 1;
                        }
                    } else {
                        rc = 1;
                        done = 1;
                    }
                }
                break;
            case 1:{
                    if (context.token == Div_Op) {
                        strcat(new_units_spec, "/");
                        state = 0;

                    } else if (context.token == Square_Op) {
                        strcat(new_units_spec, "2");

                    } else if (context.token == Cube_Op) {
                        strcat(new_units_spec, "3");

                    } else if (context.token == Units_Primitive) {
                        if (conv_tab[context.ix].era == 1) {
                            strcat(new_units_spec, "*");
                            if (strcmp("M", conv_tab[context.ix].c_unit) == 0) {
                                strcat(new_units_spec, "m");
                            } else if (strcmp("nm", conv_tab[context.ix].c_unit) == 0) {
                                strcat(new_units_spec, "n.m.");
                            } else {
                                strcat(new_units_spec, conv_tab[context.ix].c_unit);
                            }
                        } else {
                            rc = 1;
                            done = 1;
                        }
                    } else if (context.token == End_of_input) {
                        done = 1;

                    } else if (context.token != Mult_Op) {
                        rc = 1;
                        done = 1;
                    }
                }
                break;
        }                              /* switch */
    }
    return (rc);
}
