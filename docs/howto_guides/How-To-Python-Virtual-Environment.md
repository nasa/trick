# HOWTO Setup a Python Virtual Environment


- [Using the Built-in venu Module in Python3](#using-the-built-in-venv-module-in-python-3)
  * [Creating a Virtual Environment](#creating-a-virtual-environment)
  * [Activating the Virtual Environment](#activating-the-virtual-environment)
  * [Installing Python Modules Into Your Virtual Environment](#installing-python-modules-into-your-virtual-environment)
  * [Deactivating the venv Shell](#deactivating-the-venv-shell)
- [Using Conda](#using-conda)
  * [Creating a Conda Environment with Commands](#creating-a-conda-environment-with-commands)
  * [Creating a Conda Environment From a YAML File](#creating-a-conda-environment-from-a-yaml-file)
  * [Activating the Conda Environment](#activating-the-conda-environment)
  * [Installing Packages Into a Conda Environment](#installing-packages-into-a-conda-environment)
  * [Deactivating an Active Conda Environment](#deactivating-an-active-conda-environment)

- [References](#references)


<!-- toc -->

## [Using the Built-in venv Module in Python 3](#using-the-built-in-venv-module-in-python-3)

### [Creating a Virtual Environment](#creating-a-virtual-environment)

The following command creates a virtual Python environment:

```% python3 -m venv <path-of-virtual-environment>```

This command runs the ```venv``` module, to create a virtual environment. The 
directory specified by ```<path-of-virtual-environment>``` is created to store
the resources of the environment. It contains scripts to activate, deactivate,
and otherwise configure the environment. It also provides a place to install Python
modules for that particular environment. One can create multiple virtual environments,
each with different resources.
	
For example, the following will create a Python virtual environment called ```myVenv```
in your home directory.
	
```% python3 -m venv ~/myVenv```

### [Activating the Virtual Environment](#activating-the-virtual-environment)

To activate the virtual environment, execute the following:

```
% source myVenv/bin/activate
```

Note that the name of virtual environment is added to the command prompt.

### [Installing Python Modules Into Your Virtual Environment](#installing-python-modules-into-your-virtual-environment)

Use the following command to install Python modules into the virtual environment:

```
(myVenv) % python3 -m pip install <package-name>
```

For example, the Trick test suite, which uses TrickOps which requires PyYAML.
This Python module would be installed as follows:

```
(myVenv) % python3 -m pip install PyYAML
```

Every time ```myVenv``` is activated, the PyYAML module will be available.


### [Deactivating the venv Shell](#deactivating-the-venv-shell)
To deactivate the venv shell, execute the following:

```(myVenv) % deactivate```


The above should get you going. If you need more details, the following tutorial is pretty good.
[RealPython Tutorial](https://realpython.com/python-virtual-environments-a-primer/).



## [Using Conda](#using-conda)

Conda is a powerful package manager and environment manager that you use with command line commands at the Anaconda Prompt for Windows, or in a terminal window for macOS or Linux.

You can obtain conda by installing [Miniconda](https://docs.conda.io/projects/miniconda/en/latest/) or [Anaconda](https://docs.anaconda.com/free/anacondaorg/).

Miniconda a small bootstrap version of Anaconda that includes only conda, Python, the packages they both depend on, and a small number of other useful packages (like pip, zlib, and a few others).

Anaconda is a downloadable, free, open-source, high-performance, and optimized Python and R distribution. It includes conda, conda-build, Python, and 250+ automatically installed, open-source scientific packages and their dependencies that have been tested to work well together, including SciPy, NumPy, and many others.



### [Creating a Conda Environment with Commands](#creating-a-conda-environment-with-commands)

#### Create a conda virtial environment with Python by running one of following conda commands from a terminal:

```
# A specific version of Python
% conda create --name trick python=3.9.18
or
% conda create -n trick python=3.9.18

# The latest version of Python 3.9
% conda create -n trick python=3.9

# The lastest version of Python
% conda create -n trick python

# The latest version of Python 3.9 and packages
% conda create -n trick python=3.9 PyYAML scipy
```


### [Creating a Conda Environment From a YAML File](#creating-a-conda-environment-from-a-yaml-file)

#### Create the file ```myenv.yml``` with following contents: 

```
name: trick
channels:
    - conda-forge
    - defaults
dependencies:
    - python
    - PyYAML
```
In this example, the environment is named ```trick``` and includes two packages: python and PyYAML.

#### Run conda command to create the new environment: 

Once you have your YAML file ready, you can create your conda environment using the following command in your terminal:

```% conda env create -f myenv.yml```


### [Activating the Conda Environment](#activating-the-conda-environment)

After creating the environment, you can activate it using the following command:

```% conda activate trick```


### [Installing Packages Into a Conda Environment](#installing-packages-into-a-conda-environment)

If you're in your conda environment, you can install package(s) using the following command:

``` (trick) % conda install numpy scipy```


If you're NOT in your conda environment, you can install package(s) into a specified environment using the following command:

``` % conda install -n trick numpy scipy```


### [Deactivating an Active Conda Environment](#deactivating-an-active-conda-environment)

If you're in your conda environment, you can deactivate it using the following command:

```(trick) % conda deactivate```



# [References](#references)

* [RealPython Tutorial - Python Virtual Environment: A Primer](https://realpython.com/python-virtual-environments-a-primer/)
* [Conda Document - Managing environments](https://conda.io/projects/conda/en/latest/user-guide/tasks/manage-environments.html)
* [Creating an environment in Anaconda through a yml file](https://sachinjose31.medium.com/creating-an-environment-in-anaconda-through-a-yml-file-7e5deeb7676d)

