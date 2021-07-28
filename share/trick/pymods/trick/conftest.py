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
	
	pathToSim=params.get_path_to_sim()
	if params.get_build_sim():
		#TODO: Need make file to only rebuild only when necessary, otherwise, test need to rebuild and this is time consuming.
		print("#"*10)
		print("Auto rebuilding sim.  Auto rebuild will build the SIM everytime the test is run, which can take some time.")
		print("To turn auto rebuild off, in utils.py, self.__build_sim = False.  Note: it's important that SIM rebuild is current.")
		print("#"*10)

		build_cmd = f"echo \"cd {pathToSim} && make -C {params.get_trick_home()}/trick_source/web/CivetServer\" | /bin/bash" #TODO: Only rebuild if nessary. 
		print("....................Running:", build_cmd)
		subprocess.run(build_cmd, shell=True)

		# clean_cmd = f"echo \"cd {pathToSim} && make clean\" | /bin/bash"
		# print("....................Running:", build_cmd)
		# subprocess.run(clean_cmd, shell=True)
		
		if not os.path.exists(os.path.join(pathToSim, "S_main_Linux_9.3_x86_64.exe")):
			build_cmd = f"echo \"cd {pathToSim} && {params.get_trick_home()}/bin/trick-CP\" | /bin/bash"
			print("....................Running:", build_cmd)
			subprocess.run(build_cmd, shell=True)

	# pause("After build before start")
	if params.get_start_sim():
		if not os.path.exists(os.path.join(pathToSim, "S_main_Linux_9.3_x86_64.exe")):
			raise RuntimeError(f"Sim executable does not exist in {pathToSim}.  Build this sim before running this test.")
		cmd = f'echo "cd {pathToSim} && ./S_main_Linux_9.3_x86_64.exe {os.path.join(params.get_input_folder(), params.get_test_input_file())} &" | /bin/bash'
		print("....................Running:", cmd)
		subprocess.run(cmd, shell=True)

@pytest.fixture(scope="session", autouse=True)
def close_sim():
	yield
	if params.get_start_sim():
		os.system("pgrep S_ | xargs kill -9")
	os.remove(os.path.join(params.get_path_to_sim(), params.get_input_folder(), params.get_test_input_file()))
