import trick


def main():
    trick.add_read(5.0, 'trick.load_checkpoint("RUN_test1/chkpnt_test1")') # This checkpoint has data recording

    trick.stop(10.0)

if __name__ == "__main__":
    main()