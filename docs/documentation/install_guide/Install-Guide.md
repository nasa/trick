| [Home](/trick/index) → Install Guide |
|------------------------------|

# Introduction
This document will walk you through the process of installing Trick on your computer. Please read each section carefully.

# Package Dependencies
Trick requires various free third party utilities in order to function. All the following products are used by Trick and may already be installed as part of your OS distribution. **Install any missing dependencies with your operating system's [package manager](https://en.wikipedia.org/wiki/Package_manager).** For most operating systems, the default version of a dependency will be compatitable with Trick. Please check the specific OS instructions below for your operating system for more details.

| Utility       | Version   | Description               | Usage                                                         |
|-:|:-:|:-:|:-|
| [gcc] and g++   | 4.8+      | C/C++ Compiler            | Compiles Trick and Trick simulations.                         |
| [clang]/[llvm]    | 3.4.2+     | C/C++ Compiler            | Utilized by the interface code generator.                     |
| [python]        | 2.7+      | Programming Language      | Lets the user interact with a simulation.                  |
| [perl]          | 5.6+      | Programming Language      | Allows executable scripts in the bin directory to run.        |
| [java]          | 1.8+      | Programming Language      | Necessary for Trick GUIs.                                     |
| [swig]          | 2.0+      | Language Interfacing      | Connects the python input processor with Trick's C code.      |
| [make]          | 3.78+     | Build Automation          | Automates the building and cleaning of Trick.                 |
| [openmotif]     | 2.2.0+    | GUI Toolkit               | Covers Trick GUIs not made with Java.                         |
| [udunits]       | 2.x+      | C Unit Library/Database   | Provides support for units of physical quantities.            |

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

## Notes
### Clang/LLVM compiler and libraries
Clang/LLVM can be installed and located manually should your package manager fail to acquire it. You can tell Trick where to find Clang/LLVM with the "--with-llvm" configuration option specified [below](TODO).
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

### Java
Trick needs the javac compiler included in the Java Development Kit (JDK). Trick will work with either the Oracle JDK or OpenJDK, but we prefer the Oracle JDK located [here](http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html).
**Installing both the Oracle JDK and OpenJDK may lead to problems and confusion.** Both use the alternatives program to tell the system which *java* to use, but the OpenJDK packages are inconsistent in their alternatives priority settings and result in unintuitive installation results.

# Operating Systems
Trick runs on GNU/Linux and MacOSX, though any System V/POSIX compatible UNIX workstation should accept the Trick software with very little source code porting. Below are instructions for installing the prerequisites on popular operating systems here at NASA.

| Quick Jump Menu |
|---|
|[RedHat Enterprise Linux (RHEL) 7](#redhat7)|
|[RedHat Enterprise Linux (RHEL) 6](#redhat6)|
|[Fedora 32, 30, 28, 24](#fedora)|
|[Ubuntu 16.04/15.10](#ubuntu16.04)|
|[Ubuntu 15.04](#ubuntu15.04)|
|[MacOSX 10.12/10.11](#macosx)|
|[Windows 10.0.15063 (Creators Update)](#windows10)|

---
<a name="redhat7"></a>

### RedHat Enterprise Linux (RHEL) 7
Trick requires the clang/llvm compiler to compile and link the Trick Interface Code Generator.  clang/llvm is available through the [Extra Packages for Enterprise Linux](https://fedoraproject.org/wiki/EPEL) repository.  Download and install the 'epel-release' package.

```bash
# From the EPEL repository
yum install llvm llvm-devel llvm-static clang clang-devel

```

Trick also requires development packages from the base and epel repositories

```bash
# From the base repository
yum install bison flex gcc gcc-c++ libxml2-devel make ncurses-devel \
 openmotif openmotif-devel python-devel perl perl-Digest-MD5 swig zlib-devel 
#UDUnits is required and is in the EPEL repository
yum install udunits2 udunits2-devel
```

The javac compiler must be installed.  The javac compiler is included with the Java Development Kit (JDK).  Trick will work with either the Oracle JDK or OpenJDK, but we prefer the Oracle JDK. 
At the time of this writing (March 2015) the Oracle JDK 1.8 is available from the [Oracle download site](http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html).

Alternatively OpenJDK is available through yum

```bash
yum install java-1.8.0-openjdk java-1.8.0-openjdk-devel
```

Trick makes use of several optional packages if they are present on the system.  These include using the HDF5 package for logging, the GSL packages for random number generation, and google test (gtest) for Trick's unit testing.  These are available from the EPEL repository

```bash
yum install hdf5-devel gsl-devel gtest-devel
```

---
<a name="redhat6"></a>

### RedHat Enterprise Linux (RHEL) 6
Trick requires gcc version 4.8+ to compile.  gcc 4.9 is available through RHEL's [Software Collections](https://access.redhat.com/documentation/en/red-hat-software-collections/) capability.  To install the software collections, the software collections reposotory needs to be added as a yum repository and the scl-utils package needs to be optionally installed.  The instructions below are specific to Scientific Linux 6, an RHEL derivative.  Repository location will be different for the official RHEL 6 and CentOS.

```bash
wget http://ftp.scientificlinux.org/linux/scientific/6x/external_products/softwarecollections/yum-conf-softwarecollections-2.0-1.el6.noarch.rpm
rpm -ivh yum-conf-softwarecollections-2.0-1.el6.noarch.rpm

# From the software collections repository
yum install devtoolset-3-gcc-c++
```

Trick requires the clang/llvm compiler to compile and link the Trick Interface Code Generator.  clang/llvm is available through the [Extra Packages for Enterprise Linux](https://fedoraproject.org/wiki/EPEL) repository.  Download and install the 'epel-release' package.

```bash
# From the EPEL repository
yum install llvm llvm-devel llvm-static clang clang-devel
```

Trick also requires development packages from the base and epel repositories

```bash
# From the base repository
yum install bison flex gcc gcc-c++ libxml2-devel make \
 openmotif openmotif-devel python-devel perl swig zlib-devel
#UDUnits is required and is in the EPEL repository
yum install udunits2 udunits2-devel
```

The javac compiler must be installed.  The javac compiler is included with the Java Development Kit (JDK).  Trick will work with either the Oracle JDK or OpenJDK, but we prefer the Oracle JDK. 
At the time of this writing (March 2015) the Oracle JDK 1.8 is available from the [Oracle download site](http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html).

Alternatively OpenJDK is available through yum

```bash
yum install java-1.8.0-openjdk java-1.8.0-openjdk-devel
```

Trick makes use of several optional packages if they are present on the system.  These include using the HDF5 package for logging, the GSL packages for random number generation, and google test (gtest) for Trick's unit testing.  These are available from the EPEL repository

```bash
yum install hdf5-devel gsl-devel gtest-devel
```

---
<a name="fedora"></a>

### Fedora 31, 30, 28, 24
Trick requires development packages from the base repositories.

```bash
dnf install bison flex gcc gcc-c++ libxml2-devel make openmotif \
 openmotif-devel python-devel perl swig zlib-devel llvm llvm-devel \
 llvm-static clang clang-devel perl-Text-Balanced perl-Digest-MD5 \
 udunits2 udunits2-devel ncurses-devel
```

Be sure to install the Oracle JDK. Alternatively, install OpenJDK with this command:
```bash
dnf install java-1.8.0-openjdk-devel
```

Trick makes use of several optional packages if they are present on the system.  These include using the HDF5 package for logging, the GSL packages for random number generation, and google test (gtest) for Trick's unit testing.  These are available from the EPEL repository

```bash
dnf install hdf5-devel gsl-devel gtest-devel
```

---
<a name="ubuntu16.04"></a>

### Ubuntu 18.04/16.04/15.10
All packages required for Trick may be installed through apt-get.

```bash
sudo apt-get install bison curl flex g++ libx11-dev libxml2-dev libxt-dev \
 libmotif-common libmotif-dev make openjdk-8-jdk python2.7-dev swig \
 zlib1g-dev llvm llvm-dev clang libclang-dev libudunits2-dev
```

---
<a name="ubuntu15.04"></a>

### Ubuntu 15.04
Follow the 16.04 instructions above.

Trick requires the clang/llvm compiler to compile and link the Trick Interface Code Generator. The instructions below install clang 3.6. 

```bash
sudo apt-get install clang-3.6 llvm-3.6 llvm-3.6-dev
```

---

<a name="macosx"></a>

### MacOSX 10.14

1. Install XCode from the App Store.

2. Download and install Command Line Tools for MacOSX by opening a terminal and running the following command.

```bash
xcode-select --install
```

3. Install system header files into /usr/include
```bash
sudo installer -pkg /Library/Developer/CommandLineTools/Packages/macOS_SDK_headers_for_macOS_10.14.pkg -target /
```

4. Install Homebrew, MacOSX's unofficial package manager.

```bash
# bash  
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```
or
```csh
# csh
curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install | ruby
```

5. Install cask to get java and xquartz.

```bash
# brew install caskroom may not be required anymore
# brew install caskroom/cask/brew-cask
brew cask install java xquartz
```

6. Finally, install the remaining dependencies.

```bash
brew install llvm swig udunits openmotif  
```

Openmotif may install dependent packages that conflict with other installations, fontconfig and freetype.  Use the following command to skip installing these packages if you encounter conflicts.
```bash
brew install --ignore-dependencies openmotif
```

---

<a name="macosx"></a>

### MacOSX 10.13/10.12/10.11

1. Install XCode from the App Store.

2. Download and install Command Line Tools for MacOSX by opening a terminal and running the following command.

```bash
xcode-select --install
```

3. Install Homebrew, MacOSX's unofficial package manager.

```bash
# bash  
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```
or
```csh
# csh
curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install | ruby
```

4. Install cask to get java and xquartz.

```bash
# brew install caskroom may not be required anymore
# brew install caskroom/cask/brew-cask
brew cask install java xquartz
```

5. Finally, install the remaining dependencies.

```bash
brew install llvm swig udunits openmotif  
```

Openmotif may install dependent packages that conflict with other installations, fontconfig and freetype.  Use the following command to skip installing these packages if you encounter conflicts.
```bash
brew install --ignore-dependencies openmotif
```

To configure Trick with an older llvm (No longer needed now that llvm 6 works with Xcode 9.2).
```bash
brew install llvm@5
./configure --with-llvm=/usr/local/opt/llvm@5
```

---
<a name="windows10"></a>

### Windows 10 version 1803 and 1709

1.  Install Ubuntu 18.04 in the Windows Linux Subsystem following these [instructions.](https://msdn.microsoft.com/en-us/commandline/wsl/install_guide).

2. Open a bash shell and install the following packages

```bash
sudo apt-get update
sudo apt install bison curl flex g++ libx11-dev libxml2-dev \
 libxt-dev libxtst6 libxi6 libmotif-common libmotif-dev make python2.7-dev \
 swig zlib1g-dev llvm-6.0-dev llvm clang libclang-dev libudunits2-dev
```

*Note This will install about 150 dependent packages on a new machine.

3.  Install Java JDK 11 through the ppa repository
```bash
sudo apt-add-repository ppa:linuxuprising/java
sudo apt update
sudo apt install oracle-java10-installer
```

4. Install an X-windows server like [Xming.](https://sourceforge.net/projects/xming/?source=typ_redirect)

5. Ensure hostname resolves to an address.  
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

6. You may have to change the sslVersion that git uses.
```bash
# Edit ${HOME}/.gitconfig
<edit_cmd> ${HOME}/.gitconfig
```

Add the following text to ${HOME}/.gitconfig

```
[httpd]
    sslVersion = tlsv1.2
```

### Windows 10 Version 1703 OS build 15063 (Creators Update)

1.  Set up the Ubuntu Linux Subsystem following these [instructions.](https://msdn.microsoft.com/en-us/commandline/wsl/install_guide)

2. Open a bash shell and install the following packages.with
```bash
sudo apt install bison curl flex g++ libx11-dev libxml2-dev \
 libxt-dev libmotif-common libmotif-dev make python2.7-dev \
 swig zlib1g-dev llvm llvm-dev clang libclang-dev libudunits2-dev
```

3.  Install Java JDK 8 through the ppa repository
```bash
sudo apt-add-repository ppa:webupd8team/java
sudo apt-get update
sudo apt install oracle-java8-installer
```

4. Install an X-windows server like [Xming.](https://sourceforge.net/projects/xming/?source=typ_redirect)

5. Ensure hostname resolves to an address.  
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

The *configure* script will generate makefiles and locate project dependencies automatically. It may be necessary to specify dependency paths manually. Run the following command to see the possible options *configure* will accept.

```bash
./configure --help
```

## 3.) Compile Trick
Now that Trick has been configured and a makefile has been generated, we can run *make* to compile Trick. To build Trick in 32-bit mode, first set the `TRICK_FORCE_32BIT` environment variable to `1`.

```bash
make
```

### Install Trick
You can also install Trick on your machine by running *sudo make install* after you compile Trick.

```bash
sudo make install
```

You will need super user privileges in order to copy files directly to the default **/usr/local** directory. This install directory can be modified with the configuration script.


## 4.) Optionally Update Your Environment
Gone are the days when you needed to set several environment variables to use Trick. Trick can now be used completely environmentlessly*. You no longer need to set `TRICK_HOME` and friends.

Trick still makes use of shell variables, but their existence is only required during simulation compilation and execution. If they are not set, Trick will infer them without polluting your environment. Furthermore, they will be available to any processes that are spawned as part of compilation or execution, so even your own tools may no longer need these variables to be manually set.

Similarly, Trick does not require its executables to be on your `PATH`, but you may find it convenient to add them if you prefer to not specify the full path to `trick-CP` every time you build a sim. They are located in `bin` under Trick's root directory. However, if you frequently work with multiple versions of Trick, it is often easier to use a full path than to keep changing an environment variable.

Finally, although setting `TRICK_CFLAGS` and `TRICK_CXXFLAGS` is not necessary, it can be useful to do so if you want a set of flags (`-g` or `-Wall`, for instance) to be applied to all simulation builds.

*The exception to this is if you're building in 32-bit mode, in which case the `TRICK_FORCE_32BIT` environment variable must be set to `1` before you build Trick or any simulation.

[Continue to Building A Simulation](../building_a_simulation/Building-a-Simulation)
