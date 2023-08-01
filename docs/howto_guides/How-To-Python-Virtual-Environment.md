## HOWTO Setup a Python Virtual Environment


### Creating a Virtual Environment

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

### Activating the Virtual Environment

To activate the virtual environment, execute the following:

```
% source myVenv/bin/activate
```

Note that the name of virtual environment is added to the command prompt.

### Installing Python Modules Into Your Virtual Environment

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


### Deactivating the venv Shell
To deactivate the venv shell, execute the following:

```(myVenv) % deactivate```


The above should get you going. If you need more details, the following tutorial is pretty good.
[RealPython Tutorial](https://realpython.com/python-virtual-environments-a-primer/).

