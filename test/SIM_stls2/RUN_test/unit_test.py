
def main():

    trick.checkpoint_post_init(True)
    trick.checkpoint_end(True)

    the_object.stlc2.name = "Daisy"

    # Data recording HDF5 test
    trick.exec_set_freeze_frame(0.10)
    trick.stop(5.0)

if __name__ == "__main__":
    main()

