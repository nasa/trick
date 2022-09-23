exec(open("Modified_data/data_record.dr").read())
exec(open("Modified_data/auto_test.dr").read())

ball1.altimeter.input.add_noise = False
ball2.altimeter.input.add_noise = False

trick.stop(300.0)
