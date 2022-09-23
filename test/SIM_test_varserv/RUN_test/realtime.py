import trick

def main():
	trick.var_server_set_port(50000)

	trick.set_var_server_info_msg_on()
	#trick.sim_control_panel_set_enabled(True)

	trick.real_time_enable()
	trick.itimer_enable()

	#trick.add_read(1.1, """vsx.vst.vs_read()""")
	#trick.add_read(2.1, """vsx.vst.vs_read()""")

	trick.exec_set_terminate_time(3000.0)

	#print trick.var_add("vsx.vst.b")

if __name__ == "__main__":
	main()
