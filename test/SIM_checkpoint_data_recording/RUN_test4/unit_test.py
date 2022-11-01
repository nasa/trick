import trick

def main():
    
    exec(open("Modified_data/foo.dr").read())

    trick.add_read(5.0, 'trick.load_checkpoint("RUN_test4/chkpnt_test4")') # contains data recording, starts at t=2

    trick.stop(10.0)

if __name__ == "__main__":
    main()