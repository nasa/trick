import trick

def main():
    
    exec(open("Modified_data/foo.dr").read())

    trick.checkpoint(5.5)
    trick.stop(10.0)

if __name__ == "__main__":
    main()
