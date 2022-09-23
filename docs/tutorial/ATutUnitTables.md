## Common Units & Unit Prefixes

The following tables list all of the available unit prefixes, and many of the most
commonly used units in simulations at the NASA, Johnson Space Center - Engineering Branch. 

Tables of units are provided for measuring [time](#time), [length](#length),
[angle](#angle), [mass](#mass), [force](#force), [volt](#volt), [current](#current),
[resistance](#resistance), [temperature](#temperature), [energy](#energy),
[power](#power), [pressure](#pressure), [volume](#volume), and [frequency](#frequency).
Each of the rows in each of the tables contain a list of identifiers that refer to a
particular unit. Each of the identifiers in a list is synonymous. Also, characters
between brackets are optional. For example, "s", "second", "seconds" are treated as
being identical.

It is **important** to note that these tables are not complete lists of all available units.

If you're really curious, or if you just can't find the units you need, you could run
`udunits2 -h` on the command line, follow the default database path listed at the bottom
of the help text, and examine the (fairly easy to read) UDUNITS2 xml database files.
You could also use `udunits2` with the `-H` option to determine whether the units you need exist, by guessing.

<a id=unit-prefixes></a>
### Unit Prefixes
| Prefix            | Value|
|-------------------|------|
| Y, yotta          | 1e24 |
| Z, zetta          | 1e21 |
| E, exa            | 1e18 |
| P, peta           | 1e15 |
| T, tera           | 1e12 |
| G, giga           | 1e9  |
| M, mega           | 1e6  |
| k, kilo           | 1e3  |
| h, hecto          | 100  |
| da, deka          | 10   |
| d, deci           | 0.1  |
| c, centi          | 0.01 |
| m, milli          | 1e-3 |
| &#x3BC;, u, micro | 1e-6 |
| n, nano           | 1e-9 |
| p, pico           | 1e-12|
| f, femto          | 1e-15|
| a, atto           | 1e-18|
| z, zepto          | 1e-21|
| y, yocto          | 1e-24|

<a id=common-predefined-units></a>
###Common Predefined Units

<a id=time></a>
#### Time
| Unit Identifier                 |
|---------------------------------|
| s, second[s]                    |
| ms, millisecond[s]              |
| &#x3BC;s, microsecond[s]        |
| min, minute[s]                  |
| h, hour[s]                      |
| d, day[s]                       |

<a id=length></a>
#### Length
| Unit Identifier                 |
|---------------------------------|
| m, meter[s]                     |
| au, astronomical_unit[s]        |
| nmile, nautical_mile[s]         |
| ft, foot, feet                  | 
| in, inch[es]                    |
| yd, yard[s]                     |
| mi, mile[s]                     |
| km, kilometer[s]                |
| cm, centimeter[s]               |
| mm, millimeter[s]               |
| um, &#x3BC;m, micrometer[s]     |
         
<a id=angle></a>
### Angle
| Unit Identifier                 |
|---------------------------------|
| rad, radian[s]                  |
| &#xB0;, degree[s]               |
| &#x2032;, arcminute[s]          |
| &#x2033;, arcsecond[s]          |
         
<a id=mass></a>
### Mass
| Unit Identifier                 |
|---------------------------------|
| kg, kilogram[s]                 |
| g, gram[s]                      |
| lb, pound[s]                    |
| slug, slug[s]                   |

NOTE: For Trick 13 and earlier, use 'sl' instead of 'slug'.
         
<a id=force></a>
### Force
| Unit Identifier                 |
|---------------------------------|
| N, newton[s], kg.m/s^2, kg.m/s&#xB2;|
| lbf, pound[s]\_force             |
         
<a id=voltage></a>
### Voltage
| Unit Identifier                 |
|---------------------------------|
| V, volt[s]                      |
| mV, millivolt[s]                |
| uV, &#x3BC;V, microvolt[s]      |

<a id=current></a>
### Current
| Unit Identifier                 |
|---------------------------------|
| A, amp, ampere                  |
| mA, milliamp                    |
| uA, &#x3BC;A, microamp          |
         
<a id=resistance></a>
### Resistance
| Unit Identifier                 |
|---------------------------------|
| &#x2126;, ohm[s]                |
| k&#x2126;, kohm[s], kiloohm[s]  |
| M&#x2126;, Mohm[s], megaohm[s]  |
         
<a id=temperature></a>
### Temperature
| Unit Identifier                 |
|---------------------------------|
| K, &#xB0;K, degK                |
| &#xB0;C, degC                   |
| &#xB0;F, degF                   |
| &#xB0;R, degR                   |
         
<a id=energy></a>
### Energy
| Unit Identifier                 |
|---------------------------------|
| J, joule[s]                     |
| MJ, Mjoule[s], megajoule[s]     |
| W.h, watt.hour[s]               |
| kW.h, kilowatt.hour[s]          |
| BTU[s]                          |
         
<a id=power></a>
### Power
| Unit Identifier                 |
|---------------------------------|
| W, watt                         |
| kW, kilowatt                    |
| MW, megawatt                    |
| hp, horsepower                  |
         
<a id=pressure></a>
### Pressure
| Unit Identifier                 |
|---------------------------------|
| Pa, pascal[s]                   |
| psi                             |
| atm, atmosphere                 |
| mmHg, millimeter[s]\_Hg         |
| inHg, inch[es]\_Hg              |
         
<a id=volume></a>
### Volume
| Unit Identifier                 |
|---------------------------------|
| l, liter                        |
| cm&#xB3;, cm^3, (cm)&#xB3;  
| gallon                          |
| oz                              |
         
<a id=frequency></a>
### Frequency
| Unit Identifier                 |
|---------------------------------|
| Hz, hertz, 1/s                  |
| kHz, kilohertz, kiloHz
| MHz, megahertz, megaHz
         
<a id=counting></a>
### Counting
| Unit Identifier                 |
|---------------------------------|
| 1                               |
| count[s]                        |
| bit[s]                          |
| byte[s]                         |
