## Configuring Trick with Civetweb
To configure Trick to support the civetweb web server, you'll need to

1. Download Civetweb from [Github](https://github.com/civetweb/civetweb). Where you put the Civetweb directory will be designated as $(CIVETWEB_HOME).

2. Build the Civetweb library.
3. Configure Trick.

### Building the Civetweb Library
```bash
cd $(CIVETWEB_HOME)
make lib
mkdir lib
mv $libcivetweb.a lib
```
### Configuring Trick with Civetweb
```bash
cd $(TRICK_HOME)
./configure --with-civetweb=$(CIVETWEB_HOME)
make clean
make
```
