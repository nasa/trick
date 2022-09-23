SHELL = /bin/sh

all: testing

testing:
	@echo "===== Making DPC_TEST ====="
	$(MAKE) -C DPC_TEST 
	@echo "===== Making DPM_TEST ====="
	$(MAKE) -C DPM_TEST 
	@echo "===== Making DS_exercisor ====="
	$(MAKE) -C DS_exercisor 
	@echo "===== Making DS ====="
	$(MAKE) -C DS
	@echo "===== Making TCDS ====="
	$(MAKE) -C TCDS 
	@echo "===== Making UDS ====="
	$(MAKE) -C UDS 

clean:
	$(MAKE) -C DPC_TEST clean
	$(MAKE) -C DPM_TEST clean
	$(MAKE) -C DS_exercisor clean
	$(MAKE) -C DS clean
	$(MAKE) -C TCDS clean
	$(MAKE) -C UDS clean

spotless:
	$(MAKE) -C DPC_TEST spotless
	$(MAKE) -C DPM_TEST spotless
	$(MAKE) -C DS_exercisor spotless
	$(MAKE) -C DS spotless
	$(MAKE) -C TCDS spotless
	$(MAKE) -C UDS spotless
