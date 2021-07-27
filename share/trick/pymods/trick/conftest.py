import pytest
import sys
import os
from typing import Dict, Tuple
import subprocess
import inspect

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0))), '../..')))
from utils import is_web_server_started, params, pause

# store history of failures per test class name and per index in parametrize (if parametrize used)
web_server_status = {}

def pytest_runtest_setup(item):
	pause("start of test")
	if "webserver" in item.keywords:
		#retrieve the class name of the test
		cls_name = str(item.cls)
		status = web_server_status.get(cls_name, None)
		if status == None:
			print(f"Building and starting sim for class {cls_name}")
			pause("before build sim")
			build_sim()
			pause("here 2")
			status = is_web_server_started()
			web_server_status[cls_name] = status
			print(f"Web server status for {cls_name} = {status}")
		pause("here 1")
		
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
		#TODO: Need make file to only rebuild only when necessary, otherwise, test need to rebuild and this is time consuming.
		print("#"*10)
		print("Auto rebuilding sim.  Auto rebuild will build the SIM everytime the test is run, which can take some time.")
		print("To turn auto rebuild off, in utils.py, self.__build_sim = False.  Note: it's important that SIM rebuild is current.")
		print("#"*10)
		build_cmd = f"echo \"cd {params.get_path_to_sim()} && make -C {params.get_trick_home()}/trick_source/web/CivetServer && make clean && {params.get_trick_home()}/bin/trick-CP\" | /bin/bash"
		print("....................Running:", build_cmd)
		subprocess.run(build_cmd, shell=True)
	
	if params.get_start_sim():
		pathToSim=params.get_path_to_sim()
		if not os.path.exists(os.path.join(pathToSim, "S_main_Linux_9.3_x86_64.exe")):
			raise RuntimeError(f"Sim executable does not exist in {pathToSim}.  Buid this sim before running this test.")
		cmd = f'echo "cd {pathToSim} && ./S_main_Linux_9.3_x86_64.exe {os.path.join(params.get_input_folder(), params.get_test_input_file())} &" | /bin/bash'
		print("....................Running:", cmd)
		subprocess.run(cmd, shell=True)

@pytest.fixture(scope="session", autouse=True)
def close_sim():
	yield
	if params.get_start_sim():
		os.system("pgrep S_ | xargs kill -9")
	os.remove(os.path.join(params.get_path_to_sim(), params.get_input_folder(), params.get_test_input_file()))