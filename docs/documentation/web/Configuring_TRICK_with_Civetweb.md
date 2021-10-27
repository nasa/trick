## Configuring Trick with Civetweb
To configure Trick to support the civetweb web server, you'll need to

1. Download Civetweb from [Github](https://github.com/civetweb/civetweb). Where you put the Civetweb directory will be designated as $(CIVETWEB_HOME).

2. Build the Civetweb library:
3. Configure Trick

### Building the Civetweb Library
```bash
cd $(CIVETWEB_HOME)
make build
make WITH_ALL=1
mv $(CIVETWEB_HOME)/output/build/src/libcivetweb.a $(CIVETWEB_HOME)/lib
```
### Configuring Trick with Civetweb
```bash
cd $(TRICK_HOME)
./configure --with-civetweb=$(CIVETWEB_HOME)
make clean
make
```
