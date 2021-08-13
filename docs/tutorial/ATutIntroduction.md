| [Home](/trick) → [Tutorial Home](Tutorial) → Introduction |
|---------------------------------------------------------|

<!-- Section -->
<a id=introduction></a>
# 🏁 Introduction

**Contents**

* [What Trick Is](#what-trick-is)<br>
* [The Goal of Trick](#the-goal-of-trick)<br>
* [The Objective of This Tutorial](#the-objective-of-this-tutorial)<br>
* [Installing Trick](#installing-trick)<br>

***

<a id=what-trick-is></a>
## What Trick Is

Trick is a NASA Open Source simulation framework for running time-based physics
models. It builds executable simulations with a common architecture, using user
supplied simulation definition files and user supplied "model code". A Trick
based simulation provides many capabilities such as realtime
synchronization, job scheduling, simulation state save and restore (checkpoint),
data-recording, interactive variable manipulation (variable server), numeric
state integration and an input processor. It also includes tools for plotting
recorded data, and many other supporting utilities and libraries.

Trick is written in C/C++ and Java and supports both Linux and MacOSX computer
operating systems.

The name "Trick" is handed down from motocross. It is not an acronym. It is
slang for "really cool"... or something of that nature.

---

<a id=the-goal-of-trick></a>
## The Goal of Trick

The goal of Trick is to help simulation developers to focus on the algorithms
and equations of their problem domain rather than repeatedly solving the same,
time-consuming simulation architecture problems and reimplementing the same
required capabilities. Trick seeks to reliably accomplish this by automating
the implementation of commonly needed simulation capabilities, and by automating
the process of building the executable.

---

<a id=the-objective-of-this-tutorial></a>
## The Objective of This Tutorial

This tutorial covers the basic concepts for developing and operating Trick
simulations. It assumes absolutely no previous knowledge of Trick, and
should be self-contained, with no need for outside references other than maybe a
C book, and/or a UNIX book. By the end of the tutorial you should know how to
build a Trick simulation of your own from the ground up. The approach of this
tutorial is hands-on.

The intent here is not to teach about simulation math models, nor to teach about
C programming, nor UNIX, but rather to teach about the Trick architecture and
its capabilities.

---

<a id=installing-trick></a>
## Installing Trick

If Trick is not already installed on your machine, then you will need to do that
first, by following the directions at: [Install Guide](/trick/documentation/install_guide/Install-Guide).

Once Trick is installed on your machine, you will need add the Trick **bin**
directory to your execution path. For the sake of example, let us assume that
you installed Trick in your home directory, and you used the default name for
the repository, which is **trick**. If you named it something different, then 
use that name instead in the scripts below.

If you are using **bash**, then add the following lines to your **.profile**
file.

```bash
export TRICK_HOME="${HOME}/trick"
export PATH=${PATH}:${TRICK_HOME}/bin
```

If you are using **csh** or **tcsh**, then add the following lines to your **.cshrc** file.

```csh
setenv TRICK_HOME ${HOME}/trick
setenv PATH ${PATH}:${TRICK_HOME}/bin
```

Close and then re-open your terminal window.

---
[Next Page](ATutASimpleSim)
