The Data Products (DP) is a simulation data post processor designed to allow visualization of data recorded in the Trick simulation.

The data products can plot ASCII, Binary & HDF5 data. HDF5 is the new data format supported since Trick 10.

Normally, a user will create DP formatted files for viewing by using the trick_dp and quickplot (trick_qp) GUIs (see 6.4 Data Products GUIs), and will not have to worry about DP file formats as well as the underlying Trick applications "fxplot" or "gxplot", which do the work of displaying plots and tables. However, there are times that a user may want to program scripts or do something command line and need to know the usage of these internal applications. There are two basic DP programs for displaying data: fxplot, and gxplot. "fxplot" creates X-Y plots using a 3rd party Fermi-Lab plot widget as well as creates ASCII tabular data. "gxplot" uses gnuplot for generating report quality plots.

The usage for fxplot and gxplot are seen below:

```
UNIX Prompt> fxplot <session_file>
UNIX Prompt> gxplot <session_file>
```

This session is arranged as following:

- [DP Session File Format](DP-Session-File-Format)
- [DP Product File Format](DP-Product-File-Format)
- [Plot Printing](Plot-Printing)
- [Data Products GUIs](Data-Products-GUIs)

[Continue to DP Session File Format](DP-Session-File-Format)
