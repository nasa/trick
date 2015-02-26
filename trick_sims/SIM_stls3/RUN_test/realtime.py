
def main():
    trick.real_time_enable()
    trick.exec_set_software_frame(0.1)
    trick.sim_control_panel_set_enabled(True)

if __name__ == "__main__":
    main()

