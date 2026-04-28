
def main():
    trick.real_time_enable()
    trick.exec_set_software_frame(0.5)

    trick.var_allow_all_connections()

if __name__ == "__main__":
    main()


