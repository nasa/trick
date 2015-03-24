
execfile("Modified_data/data_record.dr")
execfile("Modified_data/auto_test.dr")

ball.altimeter.input.add_noise = False
ball2.altimeter.input.add_noise = False

my_integ_loop.getIntegrator(trick.Runge_Kutta_Fehlberg_78, 4)

trick.stop(300.0)
