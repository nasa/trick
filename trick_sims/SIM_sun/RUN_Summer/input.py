trick_sys.sched.set_enable_freeze(True)

""" Sim Control Panel """
simControlPanel = trick.SimControlPanel()
trick.add_external_application(simControlPanel)

RECORD_DATA = True
if RECORD_DATA:
    execfile("Modified_data/helios.dr")

REALTIME = False
if REALTIME:
    execfile("Modified_data/realtime.py")

JAPANESE = False
if JAPANESE:
    execfile("Modified_data/Japanese_labels_alt.py")

STRIPCHART = False
if STRIPCHART:
    trickView = trick.TrickView()
    trickView.set_auto_open_file("sun.tv")
    trickView.set_strip_charts_only(True)
    trick.add_external_application(trickView)

""" ======================================== """
"""                 LOCATION                 """
""" ======================================== """
""" JSC MAIN Gate """
sun_predictor.sun.observer_latitude  = trick.sim_services.attach_units("d" , 29.55298)
sun_predictor.sun.observer_longitude = trick.sim_services.attach_units("d" , 95.09379)


""" ======================================== """
"""                TIME ZONE                 """
""" Pick your time zone by uncommenting the  """
""" appropriate line below.                  """
""" ======================================== """
#sun_predictor.sun.observer_offset_from_UTC = -4
""" Eastern Standard Time """
#sun_predictor.sun.observer_offset_from_UTC = -5
""" Central Daylight Time """
#sun_predictor.sun.observer_offset_from_UTC = -5

""" Central Standard Time """
sun_predictor.sun.observer_offset_from_UTC = -6

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
sun_predictor.sun.local_time.year  = 2006
sun_predictor.sun.local_time.month = 6
sun_predictor.sun.local_time.day   = 21
sun_predictor.sun.local_time.hour  = 0
sun_predictor.sun.local_time.min   = 0
sun_predictor.sun.local_time.sec   = 0.0

trick.stop(86400.0)


