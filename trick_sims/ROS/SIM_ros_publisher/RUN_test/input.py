
def main():
    trick.real_time_enable()
    trick.exec_set_software_frame(0.5)

    trick.var_allow_connections()
    trick.var_resolve_hostname()

if __name__ == "__main__":
    main()


