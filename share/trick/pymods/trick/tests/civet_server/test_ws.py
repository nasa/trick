import logging
import json
import pytest
import websockets
import asyncio
from time import sleep
import datetime

import pathlib
import ssl



from parameters import Params
params = Params()

if params.get_ssl_enable():
	ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
	# localhost_pem = pathlib.Path(__file__).with_name(params.get_ssl_cert_path())
	localhost_pem = params.get_ssl_cert_path()
	ssl_context.load_verify_locations(localhost_pem)
else:
	ssl_context = None

@pytest.fixture(autouse=True)
def variable_server_path():
	return params.get_ws_url("api/ws/VariableServer")

@pytest.fixture(autouse=True)
def time_path():
	return params.get_ws_url("api/ws/Time")

@pytest.mark.asyncio
async def test_time(time_path):
	if params.get_test_time():
		async with websockets.connect(time_path, ssl=ssl_context) as websocket:
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
async def test_variable_server_vars(variable_server_path):
	msg1 = '{"cmd":"var_add","var_name":"dyn.cannon.pos[0]"}'
	msg2 = '{ "cmd" : "var_send" }'
	async with websockets.connect(variable_server_path, ssl=ssl_context) as websocket:
		await websocket.send(msg1)
		await websocket.send(msg2)
		message = await websocket.recv()
		vars = json.loads(message)
		assert vars["msg_type"] == "values"
		assert "time" in vars
		assert len(vars["values"]) == 1

@pytest.mark.asyncio
async def test_variable_server_sie(variable_server_path):
	async with websockets.connect(variable_server_path, ssl=ssl_context) as websocket:
		await websocket.send('{ "cmd" : "sie" }')
		response = await websocket.recv()
		assert response == '{ "msg_type": "sie", "data": '

@pytest.mark.asyncio
async def test_variable_server_units(variable_server_path):
	msg1 = '{"cmd":"var_add","var_name":"dyn.cannon.pos[0]"}'
	msg2 = '{ "cmd" : "units", "var_name" : "dyn.cannon.pos[0]" }'
	async with websockets.connect(variable_server_path, ssl=ssl_context) as websocket:
		await websocket.send(msg1)
		await websocket.send(msg2)
		response = await websocket.recv()
		assert response == '{ "msg_type": "units", "var_name": "dyn.cannon.pos[0]", "data": "m"}'

if __name__ == "__main__":
	asyncio.get_event_loop().run_until_complete(test_variable_server_send())