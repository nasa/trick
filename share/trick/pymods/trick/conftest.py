import pytest
import sys
import os
from typing import Dict, Tuple
import subprocess
import inspect

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0))), '../..')))
from trick.utils import is_web_server_started, params

# store history of failures per test class name and per index in parametrize (if parametrize used)
web_server_status = {}

def pytest_runtest_setup(item):
    if "webserver" in item.keywords:
        #retrieve the class name of the test
        cls_name = str(item.cls)
        status = web_server_status.get(cls_name, None)
        if status == None:
            print(f"Building and starting sim for class {cls_name}")
            build_sim()
            status = is_web_server_started()
            web_server_status[cls_name] = status
            print(f"Web server status for {cls_name} = {status}")
        
        if not web_server_status[cls_name]:
            pytest.fail("web server is not started.")

# @pytest.fixture(scope="session", autouse=True)

def build_sim():
    with open(os.path.join(params.get_path_to_sim(), params.get_input_folder(), params.get_test_input_file()), "w") as f:
        f.write( \
            f"""web.server.enable = True
web.server.debug = False
web.server.ssl_enable = {params.get_ssl_enable()}
web.server.path_to_ssl_cert = '{params.get_ssl_cert_path()}'
web.server.port = {params.get_port()}

trick.var_server_set_port({params.get_var_server_port()})

trick.frame_log_on()
trick.real_time_enable()
trick.exec_set_software_frame(0.1)
trick.itimer_enable()

trick.exec_set_enable_freeze(True)
trick.exec_set_freeze_command(True)""")
    
    if params.get_build_sim():
        build_cmd = f"echo \"cd {params.get_path_to_sim()} && make -C {params.get_trick_home()}/trick_source/web/CivetServer && make clean && {params.get_trick_home()}/bin/trick-CP\" | /bin/bash"
        print("....................Running:", build_cmd)
        subprocess.run(build_cmd, shell=True)
    
    if params.get_start_sim():
        cmd = f'echo "cd {params.get_path_to_sim()} && ./S_main_Linux_9.3_x86_64.exe {os.path.join(params.get_input_folder(), params.get_test_input_file())} &" | /bin/bash'
        print("....................Running:", cmd)
        subprocess.run(cmd, shell=True)
        os.system("/bin/bash")

@pytest.fixture(scope="session", autouse=True)
def close_sim():
    yield
    if params.get_start_sim():
        os.system("pgrep S_ | xargs kill -9")
    os.remove(os.path.join(params.get_path_to_sim(), params.get_input_folder(), params.get_test_input_file()))