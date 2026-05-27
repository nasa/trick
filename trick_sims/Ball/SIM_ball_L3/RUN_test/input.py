exec(open("Modified_data/data_record.dr").read())
exec(open("Modified_data/auto_test.dr").read())

trick.var_allow_connections()
trick.var_resolve_hostname()

ball1.altimeter.input.add_noise = False
ball2.altimeter.input.add_noise = False

trick.stop(300.0)
