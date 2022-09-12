# How to Dump a Core File on MacOS (Monterey 12.5)

A core dump (file) records the memory and state of a program at some moment in time, usually when it terminates abnormally. Core dumps can be useful in debugging in certain situations.

On MacOS, the ability to dump a core file must be enabled. The following steps describe how. 

### 1. Ensure ```/cores``` Directory is Writable
Core-files are stored in the ```/cores``` directory on MacOS. You must have permission to write to it. To ensure this:

```% sudo chmod 1777 /cores```

### 2. Set Kernel State to Allow Core-dumps
The kernel state variable ```kern.coredump``` must be set in order to dump core-files. This needs to be set only once; it is persistent between reboots. To set it:

```% sudo sysctl kern.coredump=1```

To check its current value:

```% sudo sysctl kern.coredump```

### 3. Code-sign the Executable

For an executable to dump a core-file it must be signed. To do this, create an ```.entitlements``` file with the com.apple.security.get-task-allow entitlement set:

```/usr/libexec/PlistBuddy -c "Add :com.apple.security.get-task-allow bool true" segv.entitlements```

Using this ```.entitlements``` file, sign the executable:

```codesign -s - -f --entitlements segv.entitlements segv```

### 4. Set ```ulimit -c unlimited```

Before running the executable from which you want a core-dump run the following :

```% ulimit -c unlimited```

This sets shell resources necessary for a core-dump. This setting is not persistent between shells.

## Example Code:


### segv.c

```C
int main() {
    int *p = (void*)0;
    *p = 0;
    return 0;
}
```

### makefile

```Makefile
all: segv

segv: segv.c segv.entitlements
        clang -g segv.c -o segv
        codesign -s - -f --entitlements segv.entitlements segv

segv.entitlements:
        /usr/libexec/PlistBuddy -c "Add :com.apple.security.get-task-allow bool true" segv.entitlements

clean:
        rm segv
        rm segv.entitlements
        rm -rf segv.dSYM
```
