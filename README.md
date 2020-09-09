# Get the Code
```sh
git clone https://github.com/nasa/koviz.git
```
# Qt Prerequisites

`koviz` is built upon the Qt framework.

## Redhat 7+

```sh
sudo yum install qt5-qtbase-devel gcc gcc-c++ make flex bison
```

## Ubuntu latest
```sh
sudo apt-get install qtbase5-dev qt5-default
```

# Build

## Redhat

```sh
qmake-qt5
make
```
## Ubuntu

```sh
qmake
make
```

# Run

```sh
bin/koviz -h                  # for usage
bin/koviz /path/to/RUN_dir    # View trick run
bin/koviz /path/to/MONTE_dir  # View trick MONTE dir (set of runs)
```
