| [Home](/trick/index) → Install Guide |
|------------------------------|

# Introduction
This document will walk you through the process of installing Trick on your computer. Please read each section carefully.

# Package Dependencies
Trick requires various free third party utilities in order to function. All the following products are used by Trick and may already be installed as part of your OS distribution. **Install any missing dependencies with your operating system's [package manager](https://en.wikipedia.org/wiki/Package_manager).** For most operating systems, the default version of a dependency will be compatitable with Trick. **We strongly recommend that you use your package manager's default versions if they meet Trick's requirements.** Please check the specific OS instructions below for your operating system for more details.

| Utility        | Version | Description             | Usage                                                     | Notes                                                 |
|---------------:|:-------:|:-----------------------:|:---------------------------------------------------------:|:------------------------------------------------------|
| [gcc] and g++  | 4.8+    | C/C++ Compiler          | Compiles Trick and Trick simulations.                     |                                                        |
| [clang]/[llvm] | <=14    | C/C++ Compiler          | Utilized by the interface code generator.                 | Trick Versions <= 19.3 should use LLVM <= 9        |
| [python]       | 2.7+    | Programming Language    | Lets the user interact with a simulation.                 | Trick has been tested up to python 3.11 as of 04/23  |
| [perl]         | 5.6+    | Programming Language    | Allows executable scripts in the bin directory to run.    |                                                        |
| [java]         | 11+     | Programming Language    | Necessary for Trick GUIs.                                 |                                                        |
| [swig]         | 3.x-4.x | Language Interfacing    | Connects the python input processor with Trick's C code.  | 3.0+ is now required for Trick. SWIG 4.x is compatible with Trick, but has some [issues](https://github.com/nasa/trick/issues/1288). Please open an issue if you encounter a problem related to SWIG 4. |
| [make]         | 3.78+   | Build Automation        | Automates the building and cleaning of Trick.             |                                                        |
| [openmotif]    | 2.2.0+  | GUI Toolkit             | Covers Trick GUIs not made with Java.                     |                                                        |
| [udunits]      | 2.x+    | C Unit Library/Database | Provides support for units of physical quantities.        |                                                        |
| [maven]        | x.x     | Java package manager    | Downloads Java dependencies and builds trick GUIs         |                                                        |

[gcc]: https://gcc.gnu.org/
[clang]: https://clang.llvm.org/
[llvm]: https://llvm.org/
[python]: https://www.python.org/
[perl]: https://www.perl.org/
[java]: https://www.java.com/
[swig]: http://www.swig.org/
[make]: https://www.gnu.org/software/make/
[openmotif]: http://www.opengroup.org/openmotif/
[udunits]: https://www.unidata.ucar.edu/software/udunits/
[maven]: https://maven.apache.org/

## Notes
### Clang/LLVM compiler and libraries
Clang/LLVM can be installed and located manually should your package manager fail to acquire it. You can tell Trick where to find Clang/LLVM with the "--with-llvm" configuration option specified [below](TODO).

### Java
Trick needs the javac compiler included in the Java Development Kit (JDK). Trick will work with either the Oracle JDK or OpenJDK.


**Installing both the Oracle JDK and OpenJDK may lead to problems and confusion.**

# Operating Systems
Trick runs on GNU/Linux and macOS, though any System V/POSIX compatible UNIX workstation should accept the Trick software with very little source code porting. Below are instructions for installing the prerequisites on popular operating systems here at NASA.

| Quick Jump Menu |
|---|
|[RedHat Enterprise Linux (RHEL) 8](#redhat8)|
|[Oracle Linux 8](#redhat8)|
|[AlmaLinux 8](#redhat8)|
|[Rocky Linux 8](#redhat8)|
|[RedHat Enterprise Linux (RHEL) 7](#redhat7)|
|[CentOS 7](#redhat7)|
|[Fedora](#fedora)|
|[Ubuntu](#ubuntu)|
|[macOS (Intel)](#macos)|
|[macOS (Silicon)](#macos-silicon)|
|[Windows 10 (Linux Subsystem Only)](#windows10)|
|[Troubleshooting](#trouble)|

---
<a name="trouble"></a>
### Troubleshooting

#### Environment Variables
Sometimes environment variables affect the Trick build and can cause it to fail. If you find one that isn't listed here, please create an issue and we'll add it to the list.


```
JAVA_HOME # Trick and Maven will use JAVA_HOME to build the GUIs instead of javac in PATH if it is set.
TRICK_HOME # This variable is optional but may cause a Trick build to fail if it is set to the wrong directory.
CFLAGS, CXXFLAGS, LDFLAGS # If these flags are set they may affect flags passed to your compiler and linker
```
#### If You Think The Install Instructions Do Not Work Or Are Outdated
If the Trick tests are passing, you can see *exactly* how we configure our test machines on Github's test integration platform, Github Actions.

If logged into any github account on github.com, you can access the [Actions](https://github.com/nasa/trick/actions) tab on the Trick repo page. Go to [Trick-CI](https://github.com/nasa/trick/actions?query=workflow%3A%22Trick+CI%22), and click the latest passing run. Here you can access a log of our shell commands to configure each OS with dependencies and also the commands we use to install Trick. In fact, that is exactly where I go when I want to update the install guide! @spfennell

The configuration for these tests can be found in the [trick/.github/workflow/test_linux.yml](https://github.com/nasa/trick/blob/master/.github/workflows/test_linux.yml) file.

#### Weird Linker Error
It is possible you may have an old version of Trick installed, and Trick's libraries are on your LDPATH and interfering with your new build. The solution is to uninstall the old version before building the new one. Call `sudo make uninstall` from any Trick top level directory and it will remove the old libraries.

---
<a name="redhat8"></a>

### RedHat Enterprise Linux (RHEL) 8, Oracle Linux 8, Rocky Linux 8, AlmaLinux 8
Trick requires the clang/llvm compiler to compile and link the Trick Interface Code Generator.  clang/llvm is available through the [Extra Packages for Enterprise Linux](https://fedoraproject.org/wiki/EPEL) repository.  Download and install the 'epel-release' package.


```bash
dnf install epel-release
dnf update
dnf install -y bison clang flex git llvm make maven swig cmake clang-devel \
gcc gcc-c++ java-11-openjdk-devel libxml2-devel llvm-devel llvm-static \
ncurses-devel openmotif openmotif-devel perl perl-Digest-MD5 udunits2 \
udunits2-devel which zlib-devel gtest-devel libX11-devel libXt-devel  \
python3-devel diffutils
```



Trick makes use of several optional packages if they are present on the system.  These include using the HDF5 package for logging, the GSL packages for random number generation, and google test (gtest) for Trick's unit testing.  These are available from the EPEL repository. In order to access gtest-devel in the epel repository on RHEL 8 you need to enable the dnf repo CodeReady Linux Builder. In Rocky Linux and Alma Linux you can instead enable the Power Tools Repo. On Oracle Linux 8 you must enable OL8 CodeReady Builder. 

See RedHat's documentation to enable the CodeReady Linux Builder repository:
https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/8/html/package_manifest/codereadylinuxbuilder-repository

On AlmaLinux 8, Rocky Linux 8:
```bash
dnf config-manager --enable powertools
  dnf install -y gtest-devel
```

On Oracle Linux 8:
```
dnf config-manager --enable ol8_codeready_builder
dnf install -y gtest-devel
```

proceed to [Install Trick](#install) section of the install guide

---
<a name="redhat7"></a>

### RedHat Enterprise Linux (RHEL) 7, CentOS 7
Trick requires the clang/llvm compiler to compile and link the Trick Interface Code Generator.  clang/llvm is available through the [Extra Packages for Enterprise Linux](https://fedoraproject.org/wiki/EPEL) repository.  Download and install the 'epel-release' package.

```bash
# install epel-release
Run yum -y install epel-release && yum -y update

```

Trick also requires development packages from the base and epel repositories

```bash
yum install -y bison clang flex git llvm make maven swig3 cmake clang-devel \
gcc gcc-c++ java-11-openjdk-devel libxml2-devel llvm-devel llvm-static \
ncurses-devel openmotif openmotif-devel perl perl-Digest-MD5 udunits2 \
udunits2-devel which zlib-devel gtest-devel libX11-devel libXt-devel python-devel \
zip
```

Trick makes use of several optional packages if they are present on the system.  These include using the HDF5 package for logging, the GSL packages for random number generation, and google test (gtest) for Trick's unit testing.  These are available from the EPEL repository

```bash
yum install hdf5-devel gsl-devel gtest-devel
```

proceed to [Install Trick](#install) section of the install guide

---
<a name="fedora"></a>

### Fedora
Trick requires development packages from the base repositories.

```bash
dnf install -y bison clang flex git llvm make maven swig cmake clang-devel \ 
gcc gcc-c++ java-11-openjdk-devel libxml2-devel llvm-devel llvm-static \
ncurses-devel openmotif openmotif-devel perl perl-Digest-MD5 udunits2 udunits2-devel \
which zlib-devel gtest-devel perl-Text-Balanced python-devel diffutils zip 
```

Trick makes use of several optional packages if they are present on the system.  These include using the HDF5 package for logging, the GSL packages for random number generation, and google test (gtest) for Trick's unit testing.  These are available from the EPEL repository

```bash
dnf install hdf5-devel gsl-devel gtest-devel
```
<a name="ubuntu"></a>

proceed to [Install Trick](#install) section of the install guide

---
### Ubuntu
All packages required for Trick may be installed through apt-get. If your package manager cannot find these packages, try searching for alternatives, or your Ubuntu version may be too old.

```bash
#update apt
apt-get update

# install packages
apt-get install -y bison clang flex git llvm make maven swig cmake \
curl g++ libx11-dev libxml2-dev libxt-dev libmotif-common libmotif-dev \
python3-dev zlib1g-dev llvm-dev libclang-dev libudunits2-dev \
libgtest-dev openjdk-11-jdk zip

# On some versions of Ubuntu (18.04 as of 04/2021), there may be multiple installations of python.
# Our new python3-dev will be linked to python3 and python3-config in your bin.
# To help trick find this instead of python2.7, we set an environment variable in our shell before calling configure:
export PYTHON_VERSION=3
```

proceed to [Install Trick](#install) section of the install guide

---
<a name="macos"></a>
### macOS Monterey, Big Sur, Catalina
#### These instructions are for Intel-based macs. For the latest Apple silicon (M1) instructions see this issue: https://github.com/nasa/trick/issues/1283 or [macOS (Silicon)](#macos-silicon)
1. Install the latest Xcode. I recommend installing Xcode through the App Store.

2. Download and install Xcode Command Line Tools for macOS. The following command in the terminal should do the job:

```bash
xcode-select --install
```

3. Install Homebrew, macOS's unofficial package manager. They typically have a single line that can be executed in your terminal to install brew at their homepage at https://brew.sh/


4. Install the following dependencies using brew (note, we do not currently support installing llvm through brew. Trick WILL NOT work with brew's llvm. See step 5). 

```bash
brew install python java xquartz swig maven udunits openmotif 

```
IMPORTANT: Make sure to follow the instructions for adding java and swig to your `PATH` provided by brew. If you missed them, you can see them again by using `brew info java` and `brew info swig`. Remember,  you may need to restart your terminal for these `PATH` changes to take effect. Note that `swig@3` is now deprecated on Mac.

5. Download and un-compress the latest pre-built clang+llvm from llvm-project github. Go to https://github.com/llvm/llvm-project/releases
and download the latest version llvm that matches your Xcode version from the release assets. For example, if your Xcode version is 14 then you will want the latest 14.x.x release of llvm. 13.0.1 is the latest as of the writing of this guide, the link I used is below:
https://github.com/llvm/llvm-project/releases/download/llvmorg-13.0.1/clang+llvm-13.0.1-x86_64-apple-darwin.tar.xz
Tip: I suggest renaming the untar'd directory to something simple like llvm13 and putting it in your home directory or development environment.

6. Read the following macOS optional steps/caveats and then go to the Install Trick section of this document. 

IMPORTANT: Your mac might complain during configuration or build that llvm is downloaded from the internet and can not be trusted. You may need to find a safe solution for this on your own. DO THIS AT YOUR OWN RISK: What worked for us was enabling Settings->Security & Privacy->Privacy->Developer Tools->Terminal. 

IMPORTANT: when doing the configure step in the install trick section, you need to point trick to llvm. It is also possible that the current iteration of our configure script will not be able to find the udunits package, so you may need to point trick to udunits as well (I believe this is only an issue on M1 macs).
You can find the path of udunits by executing the following command:
```
brew info udunits
```

Then enter the path to llvm (and udunits) when you execute the configure command in place of the placeholders:
```
./configure --with-llvm=<enter path to llvm> --with-udunits=<path to udunits> <other configure flags (if any)>
```
e.g.
```
./configure --with-llvm=/Users/trickguy/llvm13 --with-udunits=/usr/local/Cellar/udunits/2.2.28
```

OPTIONAL: Trick uses google test (gtest) version 1.8 for unit testing. To install gtest:
```
brew install cmake wget
wget https://github.com/google/googletest/archive/release-1.8.0.tar.gz
tar xzvf release-1.8.0.tar.gz
cd googletest-release-1.8.0/googletest
cmake .
make
make install
```

proceed to [Install Trick](#install) section of the install guide

---

<a name="macos-silicon"></a>
### macOS M1, M2, M3 Silicon Processors
#### These instructions are for Silicon-based macs. For intel based installations, see [macOS (Intel)](#macos)
1. Install the latest Xcode. I recommend installing Xcode through the App Store.

2. Download and install Xcode Command Line Tools for macOS. The following command in the terminal should do the job:
```
xcode-select --install
```

3. Install Homebrew, macOS's unofficial package manager. They have a single line to run that will install brew located at https://brew.sh/

4. Install the following dependencies using brew (Note, we do not currently support installing llvm through brew. Trick WILL NOT work with brew's llvm).
```
brew install python java xquartz swig maven udunits openmotif ninja
```

IMPORTANT: Steps 5-9 are to update your env variables. The following is what I used, but make sure that your file paths align with your current system & versions.

5. Set your PATH variables for Java, Swig, and Trick/bin.
```
echo 'export PATH="/opt/homebrew/opt/openjdk/bin:/opt/homebrew/opt/swig/bin:${HOME}/trick/bin:$PATH"' >> ~/.zshrc
```

6. Set the default Python version to Python 3 for any scripts or commands executed from the terminal.
```
echo 'export PYTHON_VERSION=3' >> ~/.zshrc
```

7. Add lib path for homebrew to library path, solves linking error
```
echo 'export LIBRARY_PATH="${LIBRARY_PATH}:/opt/homebrew/Cellar/zstd/1.5.5/lib"' >> ~/.zshrc
```

8. Add these flags to your environment
```
echo 'export TRICK_CFLAGS="-g -Wall -Wmissing-prototypes -Wextra -Wshadow -I/opt/homebrew/include -L/opt/homebrew/lib -Wno-unused-command-line-argument"' >> ~/.zshrc
```

```
echo 'export TRICK_CXXFLAGS="-g -Wall -Wextra -Wshadow -I/opt/homebrew/include -L/opt/homebrew/lib -Wno-unused-command-line-argument"' >> ~/.zshrc
```

9. Either run `source ~/.zshrc` or restart your terminal for the environment variables to be updated.

10. Download and un-compress the latest pre-built "clang+llvm-*VERSION#*-arm64-apple-darwin22.0.tar.xz" from llvm-project github. Go to https://github.com/llvm/llvm-project/releases
and download the latest version llvm that matches your Xcode version from the release assets. Tip: I suggest renaming the untar'd directory to something simple like llvmVERSION# and putting it in your home directory or development environment.

11. Download and un-compress the latest source code zip from llvm-project github. Go to https://github.com/llvm/llvm-project/releases
and download the latest version llvm that matches your Xcode version from the release assets.

12. Open a terminal and navigate to a directory where you want to perform the build (not inside the LLVM source tree). Create and move into a new build directory:
```
mkdir mybuilddir && cd mybuilddir
```

14. Run cmake to configure the build. Make sure to replace VERSION# with your version and again make sure that your file paths align with your current system & versions.
```
cmake -G Ninja -DLLVM_ENABLE_PROJECTS=clang -DLLVM_TARGETS_TO_BUILD=AArch64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${HOME}/llvmVERSION#-aarch64 ${HOME}/llvmVERSION#/llvm
```

15. Build LLVM.
```
cmake --build .
```

16. Configure LLVM. Make sure that your file paths align with your current system & versions.
```
./configure --with-llvm=/Users/ethanmaxey/Downloads/clang+llvm-VERSION#-arm64-apple-darwin22.0 --with-udunits=/opt/homebrew/Cellar/udunits/2.2.28
```

IMPORTANT: Your mac might complain during configuration or build that llvm is downloaded from the internet and can not be trusted. You may need to find a safe solution for this on your own. DO THIS AT YOUR OWN RISK: What worked for us was enabling Settings->Security & Privacy->Privacy->Developer Tools->Terminal. 

OPTIONAL: Trick uses google test (gtest) version 1.8 for unit testing. To install gtest:
```
brew install cmake wget
wget https://github.com/google/googletest/archive/release-1.8.0.tar.gz
tar xzvf release-1.8.0.tar.gz
cd googletest-release-1.8.0/googletest
cmake .
make
make install
```

proceed to [Install Trick](#install) section of the install guide

---
<a name="windows10"></a>
### Windows 10 (Linux Subsystem Only)

1.  Set up the Windows Subsystem for Linux by following the Microsoft Install Guide:
(link current as of September 2020)
https://docs.microsoft.com/en-us/windows/wsl/install-win10

2. Install the Ubuntu dependencies from above on the WSL: [Ubuntu](#ubuntu)

3. Install an X-windows server like [Xming.](https://sourceforge.net/projects/xming/?source=typ_redirect)

4. Ensure hostname resolves to an address.  
```bash
# Get name of machine
hostname
# Get IP of name
hostname -i
# If hostname -i returns an error find IP address
ifconfig
# Add an entry to /etc/hosts to associate IP address to hostname "numeric.ip.address hostname"
sudo <edit_cmd> /etc/hosts
```

proceed to [Install Trick](#install) section of the install guide

<a name="install"></a>
# Install Trick

## 1.) Clone Trick

The following commands will clone the Trick repository into a folder named *trick* in your home directory. You can install multiple copies of Trick in different locations to isolate your simulation environments from one another. 

```bash
cd ${HOME}
git clone https://github.com/nasa/trick
```

## 2.) Configure Trick 
Navigate to the *trick* directory you just created and run the *configure* script.

```bash
cd ${HOME}/trick
./configure
```

The *configure* script will generate makefiles and locate project dependencies automatically. It may be necessary to specify dependency paths manually. Run the following command to see the possible options *configure* will accept. If you are having trouble with this step, make sure there were not details in the OS section of this document that you missed.

```bash
./configure --help
```

## 3.) Compile Trick
Now that Trick has been configured and a makefile has been generated, we can run *make* to compile Trick. To build Trick in 32-bit mode, first set the `TRICK_FORCE_32BIT` environment variable to `1`.

```bash
make
```


## 4.) Optionally Update Your Environment
Gone are the days when you needed to set several environment variables to use Trick. Trick can now be used completely environmentlessly*. You no longer need to set `TRICK_HOME` and friends.

Trick still makes use of shell variables, but their existence is only required during simulation compilation and execution. If they are not set, Trick will infer them without polluting your environment. Furthermore, they will be available to any processes that are spawned as part of compilation or execution, so even your own tools may no longer need these variables to be manually set.

Similarly, Trick does not require its executables to be on your `PATH`, but you may find it convenient to add them if you prefer to not specify the full path to `trick-CP` every time you build a sim. They are located in `bin` under Trick's root directory. However, if you frequently work with multiple versions of Trick, it is often easier to use a full path than to keep changing an environment variable.

Finally, although setting `TRICK_CFLAGS` and `TRICK_CXXFLAGS` is not necessary, it can be useful to do so if you want a set of flags (`-g` or `-Wall`, for instance) to be applied to all simulation builds.

*The exception to this is if you're building in 32-bit mode, in which case the `TRICK_FORCE_32BIT` environment variable must be set to `1` before you build Trick or any simulation.

[Continue to Building A Simulation](../building_a_simulation/Building-a-Simulation)

## Notes
### 32-bit Mode
If you intend to build Trick in 32-bit mode, you will need 32-bit versions of the libraries in the above table. If a 32-bit version of udunits is not available through your package manager, you can build it from [source](ftp://ftp.unidata.ucar.edu/pub/udunits/udunits-2.2.25.tar.gz):
```bash
tar xfvz udunits-2.2.25.tar.gz
cd udunits-2.2.25
export CFLAGS="-m32"
./configure --prefix=/usr
make
make install
```

### Offline Mode
#### (No maven) (19.1 and up)
Because java is virtual machine code and is portable, you can copy the java applications that have already been built on a different machine into your trick installation on the target machine. If trick is configured in this way, it no longer relies on maven or calls it (in the target environment only). If you know someone trustworthy who has built Trick already, they can provide the built java code to you (you can skip step 1 below).
 
Trick jars are all-in-one and contain everything they need to run. You will still need maven on the machine where you build the trick java jars.
 
1. Pre-build your java code on a machine with trick dependencies, including maven and internet access.
```
# On source machine with trick dependencies, internet, and maven
cd prebuiltTrick && ./configure && make java
```
 
2. Copy the java jars to the environment that you need to build trick on. They are nested in the libexec directory as specified below. The directory should be at the top level of trick, called trick/trick-offline.

``` 
mkdir trick/trick-offline
cp prebuiltTrick/libexec/trick/java/build/*.jar trick/trick-offline
```

3. When you configure, use the flag --enable-offline.
```
./configure --enable-offline …<other flags>
```

4. Follow regular install instructions above. 

### Python Version

If you would like to use Python 2 with Trick please first make sure Python 2 and the Python 2 libs are installed. Then you will likely need to set `PYTHON_VERSION=2` in your shell environment before executing the `configure` script so that Trick will use Python 2 instead of Python 3. This can be done in bash or zsh with the following commands:

```
export PYTHON_VERSION=2
./configure
```
