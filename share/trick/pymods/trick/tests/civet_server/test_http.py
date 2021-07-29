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
import socket

path.append(os.path.join(os.environ.get("TRICK_HOME", "../../../.."), "trick_source/trick_gsetup"))
from requests.api import get, request

# TODO: Get rid of this and use automatic discovery when Trick requires Python 2.7
path.append("../..")
from utils import is_web_server_started, params

def open_connections(numConnections):
    sockets = []
    for _ in range(numConnections):
        sockets.append(socket.socket(socket.AF_INET, socket.SOCK_STREAM))
        sockets[-1].connect(("127.0.0.1", params.get_var_server_port()))
    sleep(1) #Wait for the connection to persist.
    return sockets

def close_sockets(sockets):
    for s in sockets:
        port = s.getsockname()[1]
        s.close()
        # is_web_server_started(port, "")
    sleep(1)

def get_vs_open_connections():
    endpoint = "api/http/vs_connections"
    url = params.get_url(endpoint)
    res = requests.get(url, verify=False)
    print("Array:",res.json()["variable_server_connections"])
    return res

@pytest.mark.webserver
class TestWebserverHttp:    
    def test_404_URL_not_found(self):
        url = params.get_url("api/http/invalid")
        res = requests.get(url, verify=False) #verify=False says to not verify https for self-signed cert
        assert res.status_code == 404, f"Requested URL should not exist.  Status code 404 was not returned. Response text:\n#######\n{res.text}\n#######"

    def test_alloc_info(self):
        url = params.get_url("api/http/alloc_info")
        res = requests.get(url, verify=False) #verify=False says to not verify https for self-signed cert
        print(f"Response: {res.text}")
        assert res.status_code == 200, "Requested URL does not exist."
        data = res.json()
        assert len(data["alloc_list"]) == 10, "Expecting default &count to be 10."
        assert data["chunk_size"] == 10, "Expecting default &count to be 10."
        assert data["chunk_start"] == 0, "expecting default &start to be 0."
        assert data["alloc_total"] == 48, "Expecting 48 memory allocations."
    
    def test_alloc_info_2(self):
        start = 2
        count = 12
        endpoint = f"api/http/alloc_info?start={start}&count={count}"
        url = params.get_url(endpoint)
        res = requests.get(url, verify=False)
        assert len(res.json()["alloc_list"]) == count
        assert res.json()["chunk_start"] == start
        assert res.json()["alloc_total"] == 48, "Expecting 48 memory allocations."

    def test_vs_connections(self):
        sockets = open_connections(1)
        webResponse=get_vs_open_connections()
        close_sockets(sockets)
        assert webResponse.json()["variable_server_connections"][0]["connection"]["client_IP_address"] == "127.0.0.1"
        assert len(webResponse.json()["variable_server_connections"]) == 1

        sockets = open_connections(50)
        webResponse=get_vs_open_connections()
        close_sockets(sockets)
        assert webResponse.json()["variable_server_connections"][0]["connection"]["client_IP_address"] == "127.0.0.1"
        assert len(webResponse.json()["variable_server_connections"]) == 50, "Should be able to open more than 1 connection." #Todo: determine appropriate number of simultaneous connections to test

        assert len(get_vs_open_connections().json()["variable_server_connections"]) == 0

    def test_index(self):
        url = params.get_url("index.html")
        res = requests.get(url)
        assert res.status_code == 200, "No index.html file served"
    
    def test_post(self):
        url = params.get_url("api/http/alloc_info")
        res = requests.post(url)
        assert res.status_code == 405, "Did not receive a method not allowed 405 error"

    def test_delete(self):
        url = params.get_url("api/http/alloc_info")
        res = requests.delete(url)
        assert res.status_code == 405, "Did not receive a method not allowed 405 error"
    
    def test_http_headers(self):
        url = params.get_url("")
        res = requests.get(url)
        assert res.headers["Content-Type"] == 'text/html'


if __name__ == "__main__":
    pass