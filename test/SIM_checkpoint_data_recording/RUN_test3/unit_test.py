import trick

def main():
    
    exec(open("Modified_data/foo.dr").read())

    trick.add_read(5.0, 'trick.load_checkpoint("RUN_test3/chkpnt_5.000000")') # contains data recording

    trick.stop(10.0)

if __name__ == "__main__":
    main()