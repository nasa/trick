def main():

    exec(open("Modified_data/realtime.py").read())

    trick.exec_set_software_frame(0.10)
    trick.exec_set_freeze_frame(0.10)
    trick.stop(5.0)

if __name__ == "__main__":
    main()