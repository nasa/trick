from sys import path
import pytest
import requests
from pprint import pprint
import logging
import os
import subprocess
from time import sleep
import shutil
import datetime

path.append(os.path.join(os.environ.get("TRICK_HOME", "../../../.."), "trick_source/trick_gsetup"))
from requests.api import get

# TODO: Get rid of this and use automatic discovery when Trick requires Python 2.7
path.append("../..")
from utils import params, is_web_server_started

@pytest.mark.webserver
class TestWebserverHttp:    
    def test_alloc_info(self):
        url = params.get_url("api/http/alloc_info")
        res = requests.get(url, verify=False)
        data = res.json()
        assert len(data["alloc_list"]) == 10
        assert data["chunk_size"] == 10
        assert data["chunk_start"] == 0
        assert data["alloc_total"] == 48

    def test_alloc_info_2(self):
        endpoint = "api/http/alloc_info?start=0&count=10"
        url = params.get_url(endpoint)
        res = requests.get(url, verify=False)
        assert len(res.json()["alloc_list"]) == 10

    def test_vs_connections(self):
        subprocess.Popen(f"nc localhost {params.get_var_server_port()}".split())
        sleep(.2) #Wait for the connection to persist.
        endpoint = "api/http/vs_connections"
        url = params.get_url(endpoint)
        res = requests.get(url, verify=False)
        assert res.json()["variable_server_connections"][0]["connection"]["client_IP_address"] == "127.0.0.1"



if __name__ == "__main__":
    logging.getLogger().setLevel(logging.DEBUG)
    test_alloc_info()
    test_vs_connections()