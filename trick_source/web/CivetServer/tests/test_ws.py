import websockets
import asyncio
import datetime
import json

port = 8888

def send_receive(url, data=""):
    async def inner():
        async with websockets.connect(url) as websocket:
            await websocket.send(data)
            response = await websocket.recv()
            return response
    return asyncio.get_event_loop().run_until_complete(inner())

def send(url, data=""):
    async def inner():
        async with websockets.connect(url) as websocket:
            await websocket.send(data)
    return asyncio.get_event_loop().run_until_complete(inner())
    

def test_time():
    time = send_receive(f"ws://localhost:{port}/api/ws/Time", "LOCAL")
    test_time = datetime.datetime.strftime(datetime.datetime.now(), "Time: %H:%M:%S Date: %m/%d/%Y\n")
    print(time)
    print()
    print(test_time)
    # assert time == test_time

def test_variable_server():
    def my_send(data="", receive=True):
        if receive:
            return send_receive(f"ws://localhost:{port}/api/ws/VariableServer", data=data)
        else:
            send(f"ws://localhost:{port}/api/ws/VariableServer", data=data)
            return None


    d = '{"cmd":"var_add","var_name":"dyn.cannon.pos[0]"}'
    my_send(str(d), receive = False)

    d = '{"cmd":"var_unpause"}'
    r = my_send(str(d))
    data = json.loads(r)
    print(data)
    assert data["msg_type"] == "values"
    assert "time" in data
    assert len(data["values"]) == 1


def test_variable_server_sie():
    def my_send(data="", receive=True):
        if receive:
            return send_receive(f"ws://localhost:{port}/api/ws/VariableServer", data=data)
        else:
            send(f"ws://localhost:{port}/api/ws/VariableServer", data=data)
            return None

    assert my_send('{ "cmd" : "sie" }') == '{ "msg_type" : "sie", "data" : string}'

if __name__ == "__main__":
    test_time()