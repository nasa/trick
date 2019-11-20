Since Trick 10, the DP Product Specification File Format is changed to XML. The DP Product XML file
DTD is defined as following:


### Product.dtd

```
<!-- Trick DP Product definition -->
<!-- V 1.0 -->
<!ELEMENT fname       (#PCDATA)>
<!ELEMENT title       (#PCDATA)>
<!ELEMENT tstart      (#PCDATA)>
<!ELEMENT tstop       (#PCDATA)>
<!ELEMENT frequency   (#PCDATA)>
<!ELEMENT label       (#PCDATA)>
<!ELEMENT units       (#PCDATA)>
<!ELEMENT measurement (var, units)>
<!ELEMENT var         (#PCDATA)>

<!-- A varcase construct is used to indicate that two or variable names mean the same thing.
     If the data for a curve can't be found for the first name, try the second and so forth.
-->
<!ELEMENT varcase     (var, var, var?)>
<!ELEMENT xaxis       (label?, units?)>
<!ELEMENT yaxis       (label?, units?)>
<!ELEMENT zaxis       (label?, units?)>
<!ELEMENT column      (label?, units?, var)>
<!ELEMENT curve       ((var, var, var?)|varcase+)>
<!ELEMENT table       (title?, tstart?, tstop?, frequency?, column+)>
<!ELEMENT plot        (title?, tstart?, tstop?, frequency?, (xaxis|yaxis|zaxis)*, curve+)>
<!ELEMENT page        (title?, tstart?, tstop?, frequency?, plot+)>
<!ELEMENT inputs      (var+)>
<!ELEMENT outputs     (measurement+)>
<!ELEMENT extfn       (fname, inputs, outputs)>
<!ELEMENT product     (title?, tstart?, tstop?, frequency?, extfn*, (page|table)+)>

<!ATTLIST product version                 CDATA #REQUIRED>
<!ATTLIST product foreground_color        CDATA #IMPLIED>
<!ATTLIST product background_color        CDATA #IMPLIED>

<!ATTLIST page foreground_color           CDATA #IMPLIED>
<!ATTLIST page background_color           CDATA #IMPLIED>
<!ATTLIST page hcells                     CDATA #IMPLIED>
<!ATTLIST page vcells                     CDATA #IMPLIED>

<!ATTLIST page presentation               CDATA #IMPLIED>
<!ATTLIST page gnuplot_template           CDATA #IMPLIED>
<!ATTLIST page gnuplot_object             CDATA #IMPLIED>
<!ATTLIST page gnuplot_geom               CDATA #IMPLIED>
<!ATTLIST page gnuplot_plot_ratio         CDATA #IMPLIED>
<!ATTLIST page gnuplot_page_orientation   CDATA #IMPLIED>

<!ATTLIST table foreground_color          CDATA #IMPLIED>
<!ATTLIST table background_color          CDATA #IMPLIED>

<!ATTLIST plot x_scale                    CDATA #IMPLIED>
<!ATTLIST plot xmin                       CDATA #IMPLIED>
<!ATTLIST plot xmax                       CDATA #IMPLIED>
<!ATTLIST plot y_scale                    CDATA #IMPLIED>
<!ATTLIST plot ymin                       CDATA #IMPLIED>
<!ATTLIST plot ymax                       CDATA #IMPLIED>
<!ATTLIST plot grid                       CDATA #IMPLIED>
<!ATTLIST plot grid_color                 CDATA #IMPLIED>
<!ATTLIST plot foreground_color           CDATA #IMPLIED>
<!ATTLIST plot background_color           CDATA #IMPLIED>
<!ATTLIST plot font                       CDATA #IMPLIED>

<!-- The format attribute specifies the printf() format string to    -->
<!-- be used for printing y variable data. (e.g., "%18.6e")              -->
<!ATTLIST yaxis format                    CDATA #IMPLIED>

<!-- The format attribute specifies the printf() format string to    -->
<!-- be used for printing column data. (e.g., "%18.6e")              -->
<!ATTLIST column format                   CDATA #IMPLIED>

<!-- =============================================================== -->
<!-- <var> ATTRIBUTES                                                -->
<!-- =============================================================== -->
<!ATTLIST var label                       CDATA #IMPLIED>

<!-- <var>'s from_units attribute specifies the units to be assumed  -->
<!-- for the recorded data if and only if the recorded data doesn't  -->
<!-- contain units information for it's variables. -->
<!-- This attribute is used for data streams that don't supply       -->
<!-- actual units. For streams that supply actual units (like .trk   -->
<!-- files) this attribute is ignored. -->
<!ATTLIST var from_units                  CDATA #IMPLIED>

<!-- <var>'s units attribute indicates the units that recorded data -->
<!-- should be converted to, below being displayed.                 -->
<!ATTLIST var units                       CDATA #IMPLIED>

<!ATTLIST var bias                        CDATA #IMPLIED>
<!ATTLIST var scale                       CDATA #IMPLIED>
<!ATTLIST var max                         CDATA #IMPLIED>
<!ATTLIST var symbol_style                CDATA #IMPLIED>
<!ATTLIST var symbol_size                 CDATA #IMPLIED>
<!ATTLIST var line_style                  CDATA #IMPLIED>
<!ATTLIST var line_color                  CDATA #IMPLIED>
<!ATTLIST var gnuplot_line_style          CDATA #IMPLIED>
```

The root element of the DP Product XML file is <b>product</b>. It contains all other elements.
There are three main elements under product element that are: <b>page</b>, <b>table</b>, and
<b>extfn</b>. <b>page</b> refers to a page of X-Y-(Z) plots. <b>table</b> refers to ASCII
text formats. The product specification file may contain any number of pages and tables, but at least one
page or table. <b>extfn</b> refers to an external program designed to manipulate recorded data
into a format which is more easily displayed and its occurrence is not required.

### DP Page Element Specifications
A DP product file may have one or more <b>page</b> elements. Each <b>page</b> element must
have at least one <b>plot</b> elment and may have more plots sepcified. All attributes of a
<b>page</b> element as stated earlier in @ref product_dtd "Product.dtd" are:
de foreground_color, background_color, hcells, vcells, presentation, gnuplot_template, gnuplot_object, gnuplot_geom, gnuplot_plot_ratio, gnuplot_page_orientation@endcode
The <b>page</b> element will be discussed in following sections: @ref plot_element_specifictions "6.2.1.1 Plot Element Specifications",
@ref general_variable_options "6.2.1.2 General Variable Options",
@ref specific_variable_options "6.2.1.3 DP Specific Y (or Z) Variable Options", and
@ref curves "6.2.1.4 Curves".

#### DP Plot Element Specifications

The <b>tstart</b> and <b>tstop</b> options have the same function as in the session file. If either of these options
is specified, they will override any values specified in the session file for this particular plot page.
Each plot page specification can include up to nine individual plot specifications. The size of each
of the plots on a plot page is automatically sized to fit within the plot page window regardless of
the number of plots specified for the plot page.

#### General Variable Options

The general variable options are options that apply to a variable regardless when it's for X, or Y, or Z.
They are:

- The <b>label</b> attribte of var element allows the user to give the parameter
(or program token) a name to be used in the legend of a plot.
- The <b>units</b> attribute of <b>var</b> allows the user to specify the measurement
units in which the specified parameter will be displayed. The measurement units specification
syntax is identical to that used in the input processor and ICG parameter comments.
- The <b>bias</b> attribute of <b>var</b> allows the user to shift the plot.
It is applied after the scaling and unit conversion. X or Y or Z may be shifted.
- The <b>max</b> attribute of <b>var</b> allows the user to override the max range options
specified at the plot level.
- The <b>scale</b> attribute of <b>var</b> allows the user to scale the specified parameter
value by a factor of the value specified by this attribute. The scale factor is applied after the
measurement units (if specified) conversion is performed.

#### DP Specific Y (or Z) Variable Options

The Y (Z) variable specification has additional options which allow the user to specify distinct line, symbol,
and color attributes. Even though <b>var</b> element XML specification doesn't limit a X variable having
all these options, such restriction is implemented at GUI level.

These options are:

- <b>ymbol_style</b>:        None|Square|Circle|Star|XX|Triangle|Solid_Square|Solid_Circle|Thick_Square|Thick_Circle
- <b>symbol_size</b>:        Tiny|Small|Medium|Large
- <b>line_sytle</b>:         Plain|Dash|No_Line|X_Thick_Line|Fine_Dash|Med_Fine_Dash|Long_Dash|X_Long_Dash|Dot_Dash|2_Dot_Dash|3_Dot_Dash|4_Dot_Dash
- <b>line_color</b>:         system supported color
- <b>gnuplot_line_sytle</b>: lines|points|linespoints|impulses|dots|steps|fsteps|histeps|boxes

The <b>symbol_style</b> attribute of <b>var</b> allows the user to mark each data point with a specific symbol.
The default is None.
The <b>line_style attribute</b> of <b>var</b> allows the user to change the line style which connects the X-Y(-Z) data points.
The default is Plain.
The <b>line_color</b> attribute of <b>var</b> allows the user to specify a color to be used for the X-Y(-Z) plot line and symbol.
the <b>gnuplot_line_sytle</b> attribute of <b>var</b> allows the user to change the line style in a Gnuplot.
You may specify the line style by name. The default is lines.

#### Curves
Each curve has either specified 2 or 3 variables stated as <b>var</b> or has <b>
varcase</b> specified. The first <b>var</b> element is for X, the second <b>var</b>
element is for Y and the third is for Z. A <b>curve</b>  element can not have both <b>var</b>
and <b>varcase</b> elements at same time.
If you have specified multiple RUN_ directories that contain the same X, Y, and (Z) variable names, Trick can
generate a curve for each RUN_ directory, or a single comparison plot. However, the only <b>var</b>
specification makes it very difficult to compare a parameter with one variable name in RUN_A and a different
variable name in RUN_B. The <b>varcase</b> specification, on the other hand, allows multiple X, Y, and (Z),
which are lists of possible XY(Z) variables that define the curve for each RUN_ directory. If <b>varcase</b>
elements do not have unique variable names, DP will use the first <b>varcase</b> that it finds in the logged
data and ignores the other <b>varcase</b> elements.
As shown in the following DP specification example, it compares sys.exec.out.time (X) and
ball.obj.state.output.acceleration[1] (Y) variables from RUN_A and my_other_data.time (X)
and my_other_data.acceleration[1] (Y) variables from RUN_B. It also generates curves for
sys.exec.out.time (X) and ball.obj.state.output.external_force[0] (Y) from
both RUN_A and RUN_B.

```
<?xml version="1.0" encoding="ISO-8859-1" standalone="no"?>
<!DOCTYPE product PUBLIC "-//Tricklab//DTD Product V1.0//EN" "Product.dtd">
<product background_color="#ede9e3" foreground_color="#000000" version="1.0">
    <tstart>-1.0E20</tstart>
    <tstop>1.0E20</tstop>
    <frequency>0.0</frequency>
    <page background_color="#ede9e3" foreground_color="#000000">
        <title>Page</title>
        <plot background_color="#ede9e3" foreground_color="#000000" grid="Yes" grid_color="#ffffff">
            <title>Plot</title>
            <curve>
                <varcase>
                    <var units="--">sys.exec.out.time</var>
                    <var units="m/s2">ball.obj.state.output.acceleration[1]</var>
                </varcase>
                <varcase>
                    <var units="--">my_other_data.time</var>
                    <var units="m/s2">my_other_data.acceleration[1]</var>
                </varcase>
            </curve>
            <curve>
                <var units="--">sys.exec.out.time</var>
                <var units="N">ball.obj.state.output.external_force[0]</var>
            </curve>
        </plot>
    </page>
</product>
```

### DP Table Specifications

Each table is comprised of one or more columns and each column is only for one variable. Each <b>column</b> element
has an optional <b>format</b> attribute that allows the user to specify the text format for the variable's data. The
syntax of <b>format</b> is the same as that for a C language @p printf format field. Each variable element of
<b>column</b> element has those general variable options as stated in
Section @ref general_variable_options "6.2.1.2 GeneralVariable Options".

As shown in the following product example XML file, it has one <b>table</b> defined. This <b>table</b> has
4 different columns and each column is corresponding to a specific <b>var</b>.

```
<?xml version="1.0" encoding="ISO-8859-1" standalone="no"?>
<!DOCTYPE product PUBLIC "-//Tricklab//DTD Product V1.0//EN" "Product.dtd">
<product background_color="#ede9e3" foreground_color="#000000" version="1.0">
    <tstart>-1.0E20</tstart>
    <tstop>1.0E20</tstop>
    <frequency>0.0</frequency>
    <table>
        <title>Table</title>
        <column>
            <units>--</units>
            <var units="m/s2">ball.obj.state.output.acceleration[0]</var>
        </column>
        <column>
            <units>--</units>
            <var units="m/s2">ball.obj.state.output.acceleration[1]</var>
        </column>
        <column>
            <units>--</units>
            <var units="N">ball.obj.state.output.external_force[0]</var>
        </column>
        <column>
            <units>--</units>
            <var units="N">ball.obj.state.output.external_force[1]</var>
        </column>
    </table>
</product>
```

An example data of the specified table:

```
------------------------------------------------------------------------
   acceleration[0]   acceleration[1] external_force[0] external_force[1]
                --                --                --                --
------------------------------------------------------------------------
     -6.859943e-01     -4.115966e-01     -6.859943e+00     -4.115966e+00
     -6.853979e-01     -4.125891e-01     -6.853979e+00     -4.125891e+00
     -6.848069e-01     -4.135693e-01     -6.848069e+00     -4.135693e+00
     -6.842212e-01     -4.145375e-01     -6.842212e+00     -4.145375e+00
     -6.836408e-01     -4.154939e-01     -6.836408e+00     -4.154939e+00
     -6.830657e-01     -4.164388e-01     -6.830657e+00     -4.164388e+00
     -6.824956e-01     -4.173724e-01     -6.824956e+00     -4.173724e+00
     -6.819306e-01     -4.182949e-01     -6.819306e+00     -4.182949e+00
     -6.813706e-01     -4.192065e-01     -6.813706e+00     -4.192065e+00
     -6.808155e-01     -4.201075e-01     -6.808155e+00     -4.201075e+00
     -6.802652e-01     -4.209980e-01     -6.802652e+00     -4.209980e+00
     -6.797196e-01     -4.218782e-01     -6.797196e+00     -4.218782e+00
     -6.791788e-01     -4.227484e-01     -6.791788e+00     -4.227484e+00
     -6.786426e-01     -4.236086e-01     -6.786426e+00     -4.236086e+00
     -6.781109e-01     -4.244592e-01     -6.781109e+00     -4.244592e+00
     -6.775837e-01     -4.253003e-01     -6.775837e+00     -4.253003e+00
     -6.770610e-01     -4.261320e-01     -6.770610e+00     -4.261320e+00
     -6.765426e-01     -4.269545e-01     -6.765426e+00     -4.269545e+00
     -6.760285e-01     -4.277680e-01     -6.760285e+00     -4.277680e+00
     -6.755186e-01     -4.285727e-01     -6.755186e+00     -4.285727e+00
     -6.750130e-01     -4.293687e-01     -6.750130e+00     -4.293687e+00
     -6.745114e-01     -4.301562e-01     -6.745114e+00     -4.301562e+00
     -6.740139e-01     -4.309353e-01     -6.740139e+00     -4.309353e+00
```

### DP External Programs

The <b>extfn</b> element provides a means for transforming data. Users build a program that is dynamically linked into
Trick data products for manipulating data specified in the DP Product XML file and its document type definition file
is stated as @ref product_dtd "Product.dtd".

#### Element extfn Specifications
The <b>extfn</b> needs to have 3 element specifications associated with it. These elements are <b>fname</b>, <b>inputs</b>, and <b>outputs</b>.
The <b>fname</b> is a full path to a program which accepts the inputs and generates the outputs. This
program must adhere to strict interface requirements. This program will be dynamically linked into the data products,
which implies it must be built under specific guidelines. Only ONE <b>extfn</b> may be defined per DP XML file.
The <b>inputs</b> are specified as a list of simulation variable names or <b>var</b> elements. The <b>outputs</b> is a user defined name list which
provides a unique variable name for each of the external program output arguments. These outputs variables may be used throughout
the product XML specification file wherever a simulation variable name or <b>var</b> element is required.
Inputs will be cast to doubles going to the external program, and outputs must be doubles as well.
The following example shows an external program that takes 3 double inputs and return the addition of these 3 inputs.
The product specification file (DP_* file) might look like this:

```
<?xml version="1.0" encoding="ISO-8859-1" standalone="no"?>
<!DOCTYPE product PUBLIC "-//Tricklab//DTD Product V1.0//EN" "Product.dtd">
<product background_color="#edeceb" foreground_color="#1a1a1a" version="1.0">
    <tstart>-1.0E20</tstart>
    <tstop>1.0E20</tstop>
    <frequency>0.0</frequency>
    <extfn>
        <fname>/users/hchen3/trick_test/myextfn/dp_test.so</fname>
        <inputs>
            <var units="m">ball.obj.state.output.position[0]</var>
            <var units="m">ball.obj.state.output.position[1]</var>
            <var units="m">ball.obj.state.output.position[0]</var>
        </inputs>
        <outputs>
            <measurement>
                <var>out</var>
                <units>--</units>
            </measurement>
        </outputs>
    </extfn>
    <page background_color="#edeceb" foreground_color="#1a1a1a" hcells="0" vcells="0">
        <title>Page</title>
        <plot background_color="#edeceb" foreground_color="#1a1a1a" grid="Yes" grid_color="#ffffff">
            <title>Plot</title>
            <curve>
                <var units="s">sys.exec.out.time</var>
                <var units="--">out</var>
            </curve>
        </plot>
    </page>
</product>
```

#### External Program Source Code

To use the external program feature of the product specification file, a user must either access a previously written
program, or write their own.
The following is an example external program source file which reads the three inputs and generates one output as
specified above. The code example below contains comments which explain the function of the code segments.

```
/* Used for dp test */

// This line should not change from program to program. The content of this function is application specific and up to user to define.
int extGetNextRecord(double *in, int numIn, double *out, int numOut) ;

// This line should not change from program to program. The content of this function is application specific and up to user to define.
int extGetNextRecord(double *in, int numIn __attribute__ ((unused)), double *out, int numOut __attribute__ ((unused)))
{
        out[0] = in[0] + in[1] + in[2];
        return (0);
}
```

#### Building The External Program

##### Linux

- Step 1. cc -c <myprogram1>.c (compile all individual object this way)
- Step 2. ld -shared -o <myprogram>.so <myfunction1>.o <myfunction2>.o ... <myfunctionN>.o <myLib>.a -lc

##### MacOS X

- Step 1. cc -c <myprogram1>.c (compile all individual object this way)
- Step 2. cc -bundle -o <myprogram>.so <myfunction1>.o <myfunction2>.o ... <myfunctionN>.o <myLib>.a -lc

In the above example, <myprogram>.so is the name that needs to be specified in the DP specification file.
If LD_LIBRARY_PATH doesn't point to the location of your shared object, then you just need to sepcify the full path
of the shared object in the DP sepcification file.
The example above links in <myLib>.a too. It's that simple.
Do the following to see if your newly created shared object has unresolved dependencies:

- @b UNIX @b Prompt> nm <myprogram>.so

#### External Program Summary

To use an external program:
1. Build a DP spec file with the program name, inputs and outputs.
2. Write an external program (or scam one off a friend).
3. Build the external program.
4. Run the data products, and hopefully the results you expect will be there.

#### External Program Proglems And Caveats

- Can't load shared library!!! The external program (*.so program) may have unresolved dependencies.
Try "nm" on your external program, and look for "U"s. The objects that you have linked in might have extern definitions that aren't there.
- Make sure you specify the full path of the shared object if LD_LIBRARY_PATH is not defined or doesn't point to the location of the shared object.
- You cannot scale or bias X values with external programs.
- External programs convert everything to doubles, and only accept and output doubles.
- External programs have no notion of unit conversion.

### A general DP Product XML File Example

```
<?xml version="1.0" encoding="ISO-8859-1" standalone="no"?>
<!DOCTYPE product PUBLIC "-//Tricklab//DTD Product V1.0//EN" "Product.dtd">
<product background_color="#ede9e3" foreground_color="#000000" version="1.0">
    <tstart>-1.0E20</tstart>
    <tstop>1.0E20</tstop>
    <frequency>0.0</frequency>
    <page background_color="#ede9e3" foreground_color="#000000">
        <title>Page</title>
        <plot background_color="#ede9e3" foreground_color="#000000" grid="Yes" grid_color="#ffffff">
            <title>Plot</title>
            <curve>
                <var units="--">sys.exec.out.time</var>
                <var units="N">ball.obj.state.output.external_force[0]</var>
            </curve>
        </plot>
        <plot background_color="#ede9e3" foreground_color="#000000" grid="Yes" grid_color="#ffffff">
            <title>Plot</title>
            <curve>
                <var units="--">sys.exec.out.time</var>
                <var units="N">ball.obj.state.output.external_force[1]</var>
            </curve>
        </plot>
    </page>
</product>
```

[Continue to Plot Printing](Plot-Printing)
