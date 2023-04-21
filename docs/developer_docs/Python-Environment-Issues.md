| [Home](/trick) → [Developer Docs](Developer-Docs-Home) → Python Environment Issues |
|------------------------------------------------------------------|

# Python Environment in Trick

In the `./configure` step, Trick finds the Python environment that is used as the embedded interpreter to execute input files and events. 

The configure script first tries to find the Python executable and the python<version>-config script. If the `--with-python=[DIR]` option is used, it will only look in that given directory. Otherwise, it will use the system path. The python<version>-config script is used to find the location of Python.h, installed python libraries, etc.

## Help! The packages that I installed with pip are not being found by my Trick input files!

This often happens when a machine has multiple Python installations, particularly on Mac with brew python vs system python. To debug this:
 
Find where the `python` and `pip` (or `python3` and `pip3`) symlinks are – this is the version that Trick will pick up unless you have specified something else using the `--with-python=<directory>` configure option.

``` 
which python3
which pip
```

These should match. cd into that directory. It was `/usr/local/bin` for me, which is the default Brew install directory for python.

Read the path for the symlinks for `python3`, `pip3`, and `python3-config` (or whichever preferred version). These paths should match as well, but often this is the problem.
 
```
readlink python3
    /Library/Frameworks/Python.framework/Versions/3.10/bin/python3
readlink pip3
    /usr/local/Cellar/python@3.11/3.11.3/bin/pip3
```

Brew is supposed to be able to detect problems like this with `brew doctor`. Mine detected that `python3.11` was unlinked:
 
```
Warning: You have unlinked kegs in your Cellar.
Leaving kegs unlinked can lead to build-trouble and cause formulae that depend on
those kegs to fail to run properly once built. Run `brew link` on these:
  python@3.11
```

Run with the `--overwrite` flag:
 
```
brew link --overwrite python@3.11
```

Running readlink should now show the correct Brew install (the one with Cellar in the path) for all the Python executables.
 
Trick will search for the python3 and python3-config executables first on the user provided directory (if given) --with-python=<directory> and then along the system path. It then uses python3-config to find installed modules and the CPython headers and libraries. Trick assumes that these symlinks point to the same Python install, and when this assumption is violated it can cause problems with finding pip installed libaries.
 
This is a common problem for systems with multiple Python installs, particularly for Macs with Brew python and Xcode python. Another option is just to completely remove the Python install in /Library/Frameworks/Python.framework.

Helpful links:

https://faun.pub/the-right-way-to-set-up-python-on-your-mac-e923ffe8cf8e

https://stackoverflow.com/questions/5157678/how-do-i-use-brew-installed-python-as-the-default-python