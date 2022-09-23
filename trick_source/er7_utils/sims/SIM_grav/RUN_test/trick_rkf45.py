# Configure the simulation control parameters.
trick.sim_services.exec_set_trap_sigbus(1)
trick.sim_services.exec_set_trap_sigfpe(1)
trick.sim_services.exec_set_trap_sigsegv(1)

# Turn off unneeded Trick objects
trick_sys.sched.set_sim_object_onoff("trick_vs", 0)
trick_sys.sched.set_sim_object_onoff("instruments", 0)
trick_sys.sched.set_sim_object_onoff("utest", 0)
trick.set_var_server_enabled(0)
trick.message_device_set_enabled(0)


# Configure the time model.
jeod_time.time_manager_init.initializer = "UTC"
jeod_time.time_manager_init.sim_start_format = trick.TimeEnum.calendar

jeod_time.time_utc.calendar_year   = 1968
jeod_time.time_utc.calendar_month  =   12  
jeod_time.time_utc.calendar_day    =   23  
jeod_time.time_utc.calendar_hour   =   19  
jeod_time.time_utc.calendar_minute =   38  
jeod_time.time_utc.calendar_second =   0.0 

jeod_time.time_tai.initialize_from_name = "UTC"
jeod_time.time_ut1.initialize_from_name = "TAI"
jeod_time.time_tt.initialize_from_name  = "TAI"

jeod_time.time_gmst.update_from_name = "UT1"
jeod_time.time_gmst.initialize_from_name = "UT1"

jeod_time.time_tai.update_from_name = "Dyn"
jeod_time.time_ut1.update_from_name = "TAI"
jeod_time.time_utc.update_from_name = "TAI"
jeod_time.time_tt.update_from_name  = "TAI"


# Configure the JEOD manager.
dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager.deriv_ephem_update = True


# Configure the vehicle.
vehicle.dyn_body.name = "vehicle"
vehicle.dyn_body.integ_frame_name = "Earth.inertial"
vehicle.dyn_body.translational_dynamics = True
vehicle.dyn_body.rotational_dynamics = False

# Specify the vehicle's orbit.
vehicle.trans_init.subject = vehicle.dyn_body
vehicle.trans_init.action_name = "TransState_pfix_body"
vehicle.trans_init.reference_ref_frame_name = "Earth.pfix"
vehicle.trans_init.body_frame_id = "composite_body"

vehicle.trans_init.position = trick.attach_units ("M",
   [ 42164000.0, 0.0, 0.0 ])
vehicle.trans_init.velocity = trick.attach_units ("M/s",
   [ 0.0, 0.0, 0.0 ])

dynamics.dyn_manager.add_body_action (vehicle.trans_init)

# Specify the vehicle's mass properties.
vehicle.mass_init.subject = vehicle.dyn_body
vehicle.mass_init.action_name = "Mass"

vehicle.mass_init.properties.mass  = trick.attach_units ("kg", 10000.0)
vehicle.mass_init.properties.position  = trick.attach_units ("M",
   [ 0.0, 0.0, 0.0 ])

vehicle.mass_init.properties.inertia_spec = trick.MassPropertiesInit.Body
vehicle.mass_init.properties.inertia[0]  = trick.attach_units ("kg*M2",
   [  8e11, 0.0,   0.0])
vehicle.mass_init.properties.inertia[1]  = trick.attach_units ("kg*M2",
   [  0.0,  8e11,  0.0])
vehicle.mass_init.properties.inertia[2]  = trick.attach_units ("kg*M2",
   [  0.0,  0.0,  8e11])

vehicle.mass_init.properties.pt_orientation.data_source = \
   trick.Orientation.InputMatrix

dynamics.dyn_manager.add_body_action (vehicle.mass_init)

# Specify the objects toward which the vehicle gravitates.
vehicle.sun_grav_ctrl.planet_name   = "Sun"
vehicle.sun_grav_ctrl.active        = 1
vehicle.sun_grav_ctrl.spherical     = 1

vehicle.dyn_body.grav_interaction.add_control(vehicle.sun_grav_ctrl);

vehicle.earth_grav_ctrl.planet_name = "Earth"
vehicle.earth_grav_ctrl.active      = 1
vehicle.earth_grav_ctrl.spherical   = 0
vehicle.earth_grav_ctrl.degree      = 8 
vehicle.earth_grav_ctrl.order       = 8 
vehicle.earth_grav_ctrl.gradient    = 0 


vehicle.dyn_body.grav_interaction.add_control(vehicle.earth_grav_ctrl);

vehicle.moon_grav_ctrl.planet_name   = "Moon"
vehicle.moon_grav_ctrl.active        = 1
vehicle.moon_grav_ctrl.spherical     = 1

vehicle.dyn_body.grav_interaction.add_control(vehicle.moon_grav_ctrl);


# Configure integration
vehicle.trick_technique = trick.Runge_Kutta_Fehlberg_45
vehicle.er7_technique = trick.Integration.Unspecified
vehicle.use_trick_integ = True
end_time = 3600.0


trick.exec_set_terminate_time (end_time)
