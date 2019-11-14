
trick_sys.sched.set_enable_freeze(True)

""" Sim Control Panel """
simControlPanel = trick.SimControlPanel()
trick.add_external_application(simControlPanel)

RECORD_DATA = True
if RECORD_DATA:
    exec(open("Modified_data/helios.dr").read())

REALTIME = True
if REALTIME:
    exec(open("Modified_data/realtime.py").read())

JAPANESE = False
if JAPANESE:
    exec(open("Modified_data/Japanese_labels_alt.py").read())

STRIPCHART = False
if STRIPCHART:
    trickView = trick.TrickView()
    trickView.set_auto_open_file("sun.tv")
    trickView.set_strip_charts_only(True)
    trick.add_external_application(trickView)

""" ======================================== """
"""                 LOCATION                 """
""" ======================================== """
# JSC MAIN Gate
sun_predictor.sun.observer_latitude  = trick.sim_services.attach_units("degree" , 29.55298)
sun_predictor.sun.observer_longitude = trick.sim_services.attach_units("degree" , 95.09379)

# Mom's House
#sun_predictor.sun.observer_latitude  = trick.sim_services.attach_units("d" , 38.842677)
#sun_predictor.sun.observer_longitude = trick.sim_services.attach_units("d" , 83.19773)


""" ======================================== """
"""                TIME ZONE                 """
""" Pick your time zone by uncommenting the  """
""" appropriate line below.                  """
""" ======================================== """
#sun_predictor.sun.observer_offset_from_UTC = -4

""" Eastern Standard Time """
#sun_predictor.sun.observer_offset_from_UTC = -5


""" Eastern Daylight Time """
#sun_predictor.sun.observer_offset_from_UTC = -4


""" Central Daylight Time """
sun_predictor.sun.observer_offset_from_UTC = -5

""" Central Standard Time """
#sun_predictor.sun.observer_offset_from_UTC = -6

""" Mountain Daylight Time """
#sun_predictor.sun.observer_offset_from_UTC = -6
""" Mountain Standard Time """
#sun_predictor.sun.observer_offset_from_UTC = -7
""" Pacific Daylight Time """
#sun_predictor.sun.observer_offset_from_UTC = -7
""" Pacific Standard Time """
#sun_predictor.sun.observer_offset_from_UTC = -8


""" ======================================== """
"""               LOCAL TIME                 """
""" Set local time here.                     """
""" ======================================== """
#sun_predictor.sun.local_time.year  = 2016
#sun_predictor.sun.local_time.month = 3
#sun_predictor.sun.local_time.day   = 14
#sun_predictor.sun.local_time.hour  = 14
#sun_predictor.sun.local_time.min   = 33
#sun_predictor.sun.local_time.sec   = 0.0

import datetime
current_local_time =  datetime.datetime.now()
sun_predictor.sun.local_time.year   = current_local_time.year
sun_predictor.sun.local_time.month  = current_local_time.month
sun_predictor.sun.local_time.day    = current_local_time.day
sun_predictor.sun.local_time.hour   = current_local_time.hour
sun_predictor.sun.local_time.min    = current_local_time.minute
sun_predictor.sun.local_time.sec    = current_local_time.second

#==========================================
# Start the display VarServer Client
#==========================================
varServerPort = trick.var_server_get_port();
SunDisplay_path = "models/graphics/dist/SunDisplay.jar"

if (os.path.isfile(SunDisplay_path)) :
    SunDisplay_cmd = "java -jar " \
                   + SunDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(SunDisplay_cmd)
    os.system( SunDisplay_cmd);
else :
    print('==================================================================================')
    print('SunDisplay needs to be built. Please \"cd\" into models/graphics and type \"make\".')
    print('==================================================================================')

trick.stop(86400.0)


