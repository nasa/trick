import trick

def main():
    
    exec(open("Modified_data/foo.dr").read())

    # redirect data recording after checkpoint reload to new directory to preserve original data logs
    trick.add_read(0.0, 'trick.load_checkpoint("RUN_test11/chkpnt_5.500000","RUN_test11_redirect")')
    trick.stop(10.0)

if __name__ == "__main__":
    main()
