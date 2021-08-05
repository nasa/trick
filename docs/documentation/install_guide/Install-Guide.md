| [Home](/trick/index) → Install Guide |
|------------------------------|

# Introduction
This document will walk you through the process of installing Trick on your computer. Please read each section carefully.

# Package Dependencies
Trick requires various free third party utilities in order to function. All the following products are used by Trick and may already be installed as part of your OS distribution. **Install any missing dependencies with your operating system's [package manager](https://en.wikipedia.org/wiki/Package_manager).** For most operating systems, the default version of a dependency will be compatitable with Trick. **We strongly recommend that you use your package manager's default versions if they meet Trick's requirements.** Please check the specific OS instructions below for your operating system for more details.

| Utility        | Version | Description             | Usage                                                     | Notes                                                 |
|---------------:|:-------:|:-----------------------:|:---------------------------------------------------------:|:------------------------------------------------------|
| [gcc] and g++  | 4.8+    | C/C++ Compiler          | Compiles Trick and Trick simulations.                     |                                                       |
| [clang]/[llvm] | 3.4.2+ (<=11 on mac)  | C/C++ Compiler          | Utilized by the interface code generator.                 |                                                       |
| [python]       | 2.7+    | Programming Language    | Lets the user interact with a simulation.                 | Trick has been tested up to python 3.9 as of 02/21    |
| [perl]         | 5.6+    | Programming Language    | Allows executable scripts in the bin directory to run.    |                                                       |
| [java]         | 11+     | Programming Language    | Necessary for Trick GUIs.                                 |                                                       |
| [swig]         | 2.0+    | Language Interfacing    | Connects the python input processor with Trick's C code.  | 3.0+ required for some unit tests in make test target |
| [make]         | 3.78+   | Build Automation        | Automates the building and cleaning of Trick.             |                                                       |
| [openmotif]    | 2.2.0+  | GUI Toolkit             | Covers Trick GUIs not made with Java.                     |                                                       |
| [udunits]      | 2.x+    | C Unit Library/Database | Provides support for units of physical quantities.        |                                                       |
| [maven]        | x.x     | Java package manager    | Downloads Java dependencies and builds trick GUIs         |                                                       |

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
Trick runs on GNU/Linux and MacOSX, though any System V/POSIX compatible UNIX workstation should accept the Trick software with very little source code porting. Below are instructions for installing the prerequisites on popular operating systems here at NASA.

| Quick Jump Menu |
|---|
|[RedHat Enterprise Linux (RHEL) 8](#redhat8)|
|[CentOS 8](#redhat8)|
|[RedHat Enterprise Linux (RHEL) 7](#redhat7)|
|[CentOS 7](#redhat7)|
|[Fedora](#fedora)|
|[Ubuntu](#ubuntu)|
|[MacOS](#macos)|
|[Windows 10 (Linux Subsystem Only)](#windows10)|

---
### Troubleshooting
If the Trick tests are passing, you can see *exactly* how we configure our test machines on Github's test integration platform, Github Actions.

If logged into any github account on github.com, you can access the [Actions](https://github.com/nasa/trick/actions) tab on the Trick repo page. Go to [Trick-CI](https://github.com/nasa/trick/actions?query=workflow%3A%22Trick+CI%22), and click the latest passing run. Here you can access a log of our shell commands to configure each OS with dependencies and also the commands we use to install Trick. In fact, that is exactly where I go when I want to update the install guide! @spfennell

The configuration for these tests can be found in the [trick/.github/workflow/test.yml](https://github.com/nasa/trick/blob/master/.github/workflows/test.yml) file.


---
<a name="redhat8"></a>

### RedHat Enterprise Linux (RHEL) 8, CentOS 8
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



Trick makes use of several optional packages if they are present on the system.  These include using the HDF5 package for logging, the GSL packages for random number generation, and google test (gtest) for Trick's unit testing.  These are available from the EPEL repository. In order to access gtest-devel in the epel repository you need to enable the dnf option PowerTools

```bash
yum install -y 'dnf-command(config-manager)'
yum config-manager --enable PowerTools
yum install hdf5-devel gsl-devel gtest-devel
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

<a name="macos"></a>
---
### MacOS Catelina/Mojave

1. Install XCode from the App Store.

2. Download and install Command Line Tools for MacOSX by opening a terminal and running the following command.

```bash
xcode-select --install
```

3. Install system header files into /usr/include
```bash
### Catalina Only
sudo installer -pkg /Library/Developer/CommandLineTools/Packages/macOS_SDK_headers_for_macOS_10.15.pkg -target /

### Mojave Only
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

5. Install the remaining dependencies. Currently llvm 12 with homebrew is not supported, so we need to treat it specially by installing v11 and linking it.

```bash
brew install java xquartz llvm@11 swig maven udunits openmotif 
brew link llvm llvm@11
```
If you have issues installing java with homebrew, you can install java and the jdk from oracle's website:

[Java](https://www.java.com/en/download/)

[JDK](https://www.oracle.com/java/technologies/javase-downloads.html) (click JDK Download)

Openmotif may install dependent packages that conflict with other installations, fontconfig and freetype.  Use the following command to skip installing these packages if you encounter conflicts.
```bash
brew install --ignore-dependencies openmotif
```

proceed to [Install Trick](#install) section of the install guide

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

The *configure* script will generate makefiles and locate project dependencies automatically. It may be necessary to specify dependency paths manually. Run the following command to see the possible options *configure* will accept.

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
