
import inspect

curr_file = inspect.getfile(inspect.currentframe())
sys.path.append(os.getcwd() + "/" + os.path.dirname(curr_file))

import input

def main():

    test = struct.unpack("!d", binascii.unhexlify("C040000000000000"))[0]
    print test

    print trick.command_line_args_get_default_dir()
    print trick.get_trick_env("TRICK_CFLAGS")

if __name__ == "__main__":
    input.main()
    main()

