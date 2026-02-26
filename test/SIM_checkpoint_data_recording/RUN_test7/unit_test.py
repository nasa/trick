import trick

def main():
    exec(open("Modified_data/fooChange.dr").read())

    trick.add_read(5.0, 'trick.load_checkpoint("RUN_test7/chkpnt_5.000000")') # contains data recording, starts at t=5

    trick.stop(20.0)

if __name__ == "__main__":
    main()