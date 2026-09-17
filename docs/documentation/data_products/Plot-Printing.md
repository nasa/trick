| [Home](/trick) → [Documentation Home](../Documentation-Home) → [Data Products](Data-Products) → Plot Printing |
|------------------------------------------------------------------|

To print a plot drawn by the Java plotting utility (`trick-jxplot`), bring up the plot and press the "Print..." button on the plot window. This opens the standard Java print dialog, from which you choose the printer.

To print plots drawn by gnuplot (`trick-gxplot`), set the device to "printer", either through the device attribute of the session element in a DP `<session_file>` or through the "trick_dp" GUI, and all plotting will go to the specified printer automatically. In order for this to work you should set two environment variables:

```
UNIX Prompt> setenv TRICK_PRINT_CMD <your print command e.g. lpr>
UNIX Prompt> setenv TRICK_PRINTER_NAME <printer name e.g. xerox>
```

In all other cases, just choose the device to be "file", then print the file.

[Continue to Data Products GUIs](Data-Products-GUIs)
