To print fermi plots, simply bring up the fermi plot, and press either the "Print" (printer icon) button or the individual "Print" (printer icon) buttons on the plots themselves. In order for this to work you should set two environment variables:

```
UNIX Prompt> setenv TRICK_PRINT_CMD <your print command e.g. lpr>
UNIX Prompt> setenv TRICK_PRINTER_NAME <printer name e.g. xerox>
```

In all other cased, just choose the device to be "file", then print the file. If the device is set to be "printer" either in a DP <session_file> (device attribute of session element) or through -device option of "fxplot" application as following or through "trick_dp" gui, all plotting will go to the specified printer automatically.

To specify the -device option of "fxplot" command for sending the plots to a printer (-device option overrides the device setting in the <session_file>):

```
UNIX Prompt> fxplot -device=printer <session_file>
```

[Continue to Data Products GUIs](Data-Products-GUIs)
