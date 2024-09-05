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

## Redhat 8+

```sh
sudo dnf install -y qt5-qtbase-devel bison flex make gcc gcc-c++
```

## Ubuntu 22.04
```sh
sudo apt-get install qtbase5-dev make flex bison g++ 
```

## Mac OS

```zsh
brew install qt@5
brew link qt5 --force
```

# Build

## Redhat 7 & 8

```sh
qmake-qt5
make
```
## Ubuntu

This is only required if you want to run the binary directly instead of using `build-and-run-koviz.sh`.

```sh
qmake
make
```

## Mac OS

```zsh
qmake
make
```

# Run

```sh
bin/koviz -h                  # for usage
bin/koviz /path/to/RUN_dir    # View trick run
bin/koviz /path/to/MONTE_dir  # View trick MONTE dir (set of runs)
```

# Docker
See docker directory's README.
