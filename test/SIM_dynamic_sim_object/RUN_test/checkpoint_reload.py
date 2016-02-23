
def main():
    trick.load_checkpoint("RUN_test/chkpnt_120.000000")
    trick.load_checkpoint_job()

    #trick.echo_jobs_on()
    #trick.stop(121.0)

if __name__ == "__main__":
    main()


