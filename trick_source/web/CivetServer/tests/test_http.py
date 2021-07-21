from sys import path
import pytest
import requests
from pprint import pprint
import logging
import os
import subprocess

from requests.api import get

from parameters import Params
params = Params()

@pytest.fixture(scope="session", autouse=True)
def build_sim():
    path_to_sim = os.path.join(os.environ.get("TRICK_HOME", None), "trick_sims", "Cannon", "SIM_cannon_numeric")
    cmd = f'echo "cd {path_to_sim} && ./S_main_Linux_9.3_x86_64.exe RUN_test/input.py &" | /bin/bash'
    print("....................Running:", cmd)
    p = subprocess.run(cmd, shell=True)
    yield
    os.system("pgrep S_ | xargs kill -9")



def test_alloc_info():
    url = params.get_url("api/http/alloc_info")
    res = requests.get(url, verify=False)
    data = res.json()
    assert len(data["alloc_list"]) == 10
    assert data["chunk_size"] == 10
    assert data["chunk_start"] == 0
    assert data["alloc_total"] == 49

def test_alloc_info_2():
    endpoint = "api/http/alloc_info?start=0&count=10"
    url = params.get_url(endpoint)
    res = requests.get(url, verify=False)
    assert len(res.json()["alloc_list"]) == 10

def test_vs_connections():
    subprocess.Popen(f"nc localhost {params.get_var_server_port()}".split())
    endpoint = "api/http/vs_connections"
    url = params.get_url(endpoint)
    res = requests.get(url, verify=False)
    assert res.json()["variable_server_connections"][0]["connection"]["client_IP_address"] == "127.0.0.1"



if __name__ == "__main__":
    logging.getLogger().setLevel(logging.DEBUG)
    test_alloc_info()
    test_vs_connections()