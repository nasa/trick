
import trick
import socket

def main():

    trick.var_server_set_port(50000)
    #trick.var_server_set_source_address(None)
    #trick.var_server_set_source_address('')
    #trick.var_server_set_source_address('192.168.1.23')
    #trick.var_server_set_source_address('galvatron')
    #trick.var_server_set_source_address('localhost')
    #trick.var_server_set_source_address('jfdlkjfklsda')

    #trick.var_server_create_tcp_socket('192.168.1.23', 49000)
    #trick.var_server_create_udp_socket('', 48000)
    trick.var_server_create_multicast_socket('224.10.10.10','', 47000)

    trick.sim_control_panel_set_enabled(True)
    trick.real_time_enable()
    trick.itimer_enable()

    # set the software frame and freeze_frame
    trick.exec_set_software_frame(0.1)
    trick.exec_set_freeze_frame(0.1)
    trick.exec_set_terminate_time(3000.0)

if __name__ == "__main__":
    main()

