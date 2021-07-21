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

from requests.api import get

from parameters import Params
params = Params()

@pytest.fixture(scope="session", autouse=True)
def build_sim():
    path_to_sim = os.path.join(os.environ.get("TRICK_HOME", None), "trick_sims", "Cannon", "SIM_cannon_numeric")
    input_folder = "RUN_test"
    # test_input_file = f"tmp_input_for_test_{datetime.datetime.now()}.py"
    test_input_file = f"tmp_input_for_test.py"
    with open(os.path.join(path_to_sim, input_folder, test_input_file), "w") as f:
        f.write( \
            f"""web.server.enable = True
web.server.debug = False
web.server.ssl_enable = {params.get_ssl_enable()}
web.server.path_to_ssl_cert = '{params.get_ssl_cert_path()}'
web.server.port = {params.get_port()}

trick.var_server_set_port({params.get_var_server_port()})

exec(open("Modified_data/realtime.py").read())
exec(open("Modified_data/cannon.dr").read())""")
    cmd = f'echo "cd {path_to_sim} && ./S_main_Linux_9.3_x86_64.exe {os.path.join(input_folder, test_input_file)} &" | /bin/bash'
    print("....................Running:", cmd)
    subprocess.run(cmd, shell=True)
    while True:
        p = subprocess.run(f"echo \"netstat -tulpan | grep {params.get_port()}\" | /bin/bash", capture_output=True, shell=True)
        print(f"Checking for port output: {p.stdout}")
        sleep(.1) #We sleep to use less recourses
        if p.stdout != b"":
            break
    yield
    os.system("pgrep S_ | xargs kill -9")
    os.remove(os.path.join(path_to_sim, input_folder, test_input_file))

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