PYTHON = python3

test: venv
	venv/bin/python -m pytest tests/civet_server/test_http.py tests/civet_server/test_ws.py tests/gsetup

venv:
	$(PYTHON) -m pip install virtualenv
	$(PYTHON) -m virtualenv venv
