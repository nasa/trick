from time import sleep
import subprocess
import os

def pause(my_str = "no message."):
	print("Type exit to continue:" + my_str)
	# os.system("/bin/bash")
	# input()

#This file contains variables for the civet_server tests
class Params:
	#Change the following to change the default parameters
	def __init__(self) -> None:
		self.__port = 9000
		self.__var_server_port = 9001
		self.__host = "localhost"
		self.__enable_ssl = False
		self.__test_time = True
		# self.__ssl_cert_path = "server.pem"
		# self.__ssl_cert_path = "/home/cherpin/git/trick_fork/trick_sims/Cannon/SIM_cannon_numeric/server.pem"
		self.__ssl_cert_path = "/home/cherpin/.ssl/server.pem"
		self.__build_sim = True
		self.__start_sim = True
		self.__trick_home = os.environ.get("TRICK_HOME", None)
		if not self.__trick_home:
			print("ERROR:", "TRICK_HOME not found")
		# self.__trick_home = None
		self.__input_folder = "RUN_test"
		self.__test_input_file = f"tmp_input_for_test.py"
	
	def get_trick_home(self):
		return self.__trick_home
	def get_path_to_sim(self):
		return os.path.join(self.get_trick_home(), "trick_sims", "Cannon", "SIM_cannon_numeric")
	def get_input_folder(self):
		return self.__input_folder
	def get_test_input_file(self):
		return self.__test_input_file

	def get_start_sim(self):
		return self.__start_sim

	def get_build_sim(self):
		return self.__build_sim

	def get_ssl_cert_path(self):
		return self.__ssl_cert_path

	def get_port(self):
		return self.__port

	def get_host(self):
		if self.get_ssl_enable():
			return self.__host + ".ssl"
		else:
			return self.__host

	def get_ssl_enable(self):
		return self.__enable_ssl

	def get_var_server_port(self):
		return self.__var_server_port
	
	def get_test_time(self):
		return self.__test_time

	def get_url(self, endpoint):
		server_port = self.get_port()
		server_host = self.get_host()
		ssl_enable = self.get_ssl_enable()
		base_url = f"http{ 's' if ssl_enable else '' }://{server_host}:{server_port}"
		return f"{base_url}/{endpoint}"

	def get_ws_url(self, endpoint):
		return f"ws{ 's' if self.get_ssl_enable() else '' }://{self.get_host()}:{self.get_port()}/{endpoint}"

params = Params()

def is_web_server_started():
	for _ in range(20): #Wait 2 seconds i.e 20 * .1 seconds, must wait for service to get to listening state.
		cmd = f"echo \"netstat -tulpan | grep {params.get_port()}\" | /bin/bash"
		p = subprocess.run(cmd, capture_output=True, shell=True)
		print("runing........", cmd)
		print(f"Checking for port output: {p.stdout}")
		print(f"Error is: {p.stderr}")
		sleep(.1) #We sleep to use less recourses
		if "LISTEN" in p.stdout.decode():
			return True
	return False
