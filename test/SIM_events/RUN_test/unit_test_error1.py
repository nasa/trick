trick.stop(1.0)

# print(dir(trick))
trick.terminate_on_event_parse_error(True)

# Error in add read
trick.add_read(0.1, "a = b")