exec(open("Modified_data/my_waterclock.dr").read())

trick.frame_log_on()
trick.real_time_enable()
trick.exec_set_software_frame(0.1)
trick.itimer_enable()
trick.exec_set_enable_freeze(True)
trick.exec_set_freeze_command(True)
trick.sim_control_panel_set_enabled(True)

dyn.waterclock.input_flow                         = 1.0

dyn.waterclock.intake_bucket_depth                = 5.0
dyn.waterclock.intake_bucket_diam                 = 5.0

dyn.waterclock.intake_overflow_height             = 4.0
dyn.waterclock.intake_overflow_diameter           = 2.5

dyn.waterclock.intake_clock_spout_height          = 1.25
dyn.waterclock.intake_clock_spout_diameter        = 0.05

dyn.waterclock.timer_bucket_depth                 = 5.0
dyn.waterclock.timer_bucket_diam                  = 0.5

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
