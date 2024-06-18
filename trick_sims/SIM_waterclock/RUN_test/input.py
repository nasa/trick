exec(open("Modified_data/my_waterclock.dr").read())

trick.frame_log_on()
trick.real_time_enable()
trick.exec_set_software_frame(0.1)
trick.itimer_enable()
trick.exec_set_enable_freeze(True)
trick.exec_set_freeze_command(True)
trick.sim_control_panel_set_enabled(True)

dyn.waterclock.input_flow                         = 1000.0

dyn.waterclock.intake_bucket_depth                = 70.0
dyn.waterclock.intake_bucket_diam                 = 40.0

dyn.waterclock.intake_overflow_height             = 60.0
dyn.waterclock.intake_overflow_diameter           = 25.0

dyn.waterclock.intake_clock_spout_height          = 15.0
dyn.waterclock.intake_clock_spout_diameter        = 20.0

dyn.waterclock.timer_bucket_depth                 = 100.0
dyn.waterclock.timer_bucket_diam                  = 45.0

dyn.waterclock.total_ticks                        = 60

# ==========================================
# Start the Satellite Graphics Client
# ==========================================
varServerPort = trick.var_server_get_port();
WaterClockDisplay_path = "models/graphics/build/WaterClockDisplay.jar"

if (os.path.isfile(WaterClockDisplay_path)) :
    WaterClockDisplay_cmd = "java -jar " \
                   + WaterClockDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(WaterClockDisplay_cmd)
    os.system( WaterClockDisplay_cmd);
else :
    print('==================================================================================')
    print('Display needs to be built. Please \"cd\" into ../models/graphics and type \"mvn package\".')
    print('==================================================================================')
