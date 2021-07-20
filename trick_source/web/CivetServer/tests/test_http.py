import requests
from pprint import pprint
import logging
import os
import subprocess

host = "localhost:8888"

def test_alloc_info():
    endpoint = "api/http/alloc_info"
    url = f"http://{host}/{endpoint}"
    print("url:", url)
    res = requests.get(f"http://{host}/{endpoint}")
    assert res.json()["alloc_list"] == []

def test_alloc_info_2():
    endpoint = "api/http/alloc_info?start=0&count=10"
    url = f"http://{host}/{endpoint}"
    print("url:", url)
    res = requests.get(f"http://{host}/{endpoint}")
    assert len(res.json()["alloc_list"]) == 10

def test_vs_connections():
    subprocess.Popen("nc localhost 5001".split())
    endpoint = "api/http/vs_connections"
    url = f"http://{host}/{endpoint}"
    res = requests.get(url)
    assert res.json()["variable_server_connections"][0]["connection"]["client_IP_address"] == "127.0.0.1"



if __name__ == "__main__":
    logging.getLogger().setLevel(logging.DEBUG)
    test_alloc_info()
    test_vs_connections()