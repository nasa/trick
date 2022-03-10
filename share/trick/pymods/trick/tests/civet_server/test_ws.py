import logging
import json
import pytest
import websockets
import asyncio
from time import sleep
import datetime
import sys
import os
import pathlib
import ssl
import platform

sys.path.append("../..")
from utils import is_web_server_started, params

@pytest.mark.webserver
class TestWebserverWs:
	if params.get_ssl_enable():
		ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
		# localhost_pem = pathlib.Path(__file__).with_name(params.get_ssl_cert_path())
		localhost_pem = params.get_ssl_cert_path()
		ssl_context.load_verify_locations(localhost_pem)
	else:
		ssl_context = None

	@pytest.fixture(autouse=True, scope="session")
	def variable_server_path(self):
		return params.get_ws_url("api/ws/VariableServer")

	@pytest.fixture(autouse=True, scope="session")
	def time_path(self):
		return params.get_ws_url("api/ws/Time")
	
	@pytest.mark.asyncio
	async def test_time(self, time_path):
		if params.get_test_time():
			async with websockets.connect(time_path, ssl=TestWebserverWs.ssl_context) as websocket:
				await websocket.send("LOCAL")
				count = 0
				while count < 2:
					message = await websocket.recv()
					test_format = "Time: %H:%M Date: %m/%d/%Y\n" #Not checking seconds.
					time =  datetime.datetime.strftime(datetime.datetime.strptime(message, "Time: %H:%M:%S Date: %m/%d/%Y\n"), test_format)
					test_time = datetime.datetime.now().strftime(test_format)
					print("Checking:", time, "=", test_time)
					assert time == test_time
					count += 1
		else:
			raise RuntimeError("Parameter test_time is disabled.")

	@pytest.mark.asyncio
	async def test_variable_server_vars(self, variable_server_path):
		msg1 = '{"cmd":"var_add","var_name":"dyn.cannon.pos[0]"}'
		msg2 = '{ "cmd" : "var_send" }'
		async with websockets.connect(variable_server_path, ssl=TestWebserverWs.ssl_context) as websocket:
			await websocket.send(msg1)
			await websocket.send(msg2)
			message = await websocket.recv()
			vars = json.loads(message)
			assert vars["msg_type"] == "values"
			assert "time" in vars
			assert len(vars["values"]) == 1

	@pytest.mark.asyncio
	async def test_variable_server_sie(self, variable_server_path):
		async with websockets.connect(variable_server_path, ssl=TestWebserverWs.ssl_context) as websocket:
			await websocket.send('{ "cmd" : "sie" }')
			response = await websocket.recv()
			assert response == '{ "msg_type": "sie", "data": '

	@pytest.mark.asyncio
	async def test_variable_server_units(self, variable_server_path):
		msg1 = '{"cmd":"var_add","var_name":"dyn.cannon.pos[0]"}'
		msg2 = '{ "cmd" : "units", "var_name" : "dyn.cannon.pos[0]" }'
		async with websockets.connect(variable_server_path, ssl=TestWebserverWs.ssl_context) as websocket:
			await websocket.send(msg1)
			await websocket.send(msg2)
			response = await websocket.recv()
			assert response == '{ "msg_type": "units", "var_name": "dyn.cannon.pos[0]", "data": "m"}'

	@pytest.mark.asyncio
	async def test_variable_server_shell_access(self, variable_server_path):
		async with websockets.connect(variable_server_path, ssl=TestWebserverWs.ssl_context) as websocket:
			file_to_create = "tmp_a.txt"
			await websocket.send('{ "cmd" : "python", "pycode" : "print \'Hello World!---------------------\'" }')
			await websocket.send('{ "cmd" : "python", "pycode" : "import os" }')
			await websocket.send('{ "cmd" : "python", "pycode" : "os.system(\'touch ' + file_to_create + '\')" }')
		path = os.path.join(params.get_path_to_sim(), file_to_create)
		if os.path.exists(path):
			os.remove(path)
			warning = "This test proves that we have shell access through the websocket api.  Is this a security concern?  Should this test fail if shell access is available?"
			print(warning)
			assert 1
			# raise RuntimeError(warning)
		else:
			assert 0

if __name__ == "__main__":
	pass