
"""
Create shortcuts or aliases for difficult to remember Input File commands

Then use "trick.shortcut_name()" in the input file to make your calls
"""

import top
import sys
import trick

# from the executive
if hasattr(top.cvar, 'trick_sys'):
    stop = top.cvar.trick_sys.sched.stop
    run  = top.cvar.trick_sys.sched.run
    freeze = top.cvar.trick_sys.sched.freeze
    if hasattr(top.cvar.trick_sys.sched, 'add_range'):
    # from monte carlo
        mc_add_range = top.cvar.trick_sys.sched.add_range

# from Monte Carlo
if hasattr(top.cvar, 'trick_mc'):
    mc_add_variable = top.cvar.trick_mc.mc.add_variable

# from the memory manager wrapper
if hasattr(top.cvar, 'trick_mm'):
    read_checkpoint_from_string = top.cvar.trick_mm.mm.read_checkpoint_from_string
    mm_declare_ext_var = top.cvar.trick_mm.mm.declare_extern_var

# from message
if hasattr(top.cvar, 'trick_message'):
    message_cpu_set = top.cvar.trick_message.mdevice.get_listen_thread().cpu_set

# from checkpoint restart
if hasattr(top.cvar, 'trick_cpr'):
    checkpoint = top.cvar.trick_cpr.cpr.checkpoint
    load_checkpoint = top.cvar.trick_cpr.cpr.load_checkpoint

# from the input processor
if hasattr(top.cvar, 'trick_ip'):
    #set_event_info_msg_on = top.cvar.trick_ip.ip.set_event_info_msg_on
    #set_event_info_msg_off = top.cvar.trick_ip.ip.set_event_info_msg_off
    pass

set_event_info_msg_on = trick.IPPythonEvent.set_event_info_msg_on
set_event_info_msg_off = trick.IPPythonEvent.set_event_info_msg_off
terminate_on_event_parse_error = trick.IPPythonEvent.terminate_on_event_parse_error

# bind pyton input_processor event routines to shortcut names.
new_event = trick.ippython_new_event
add_event = trick.ippython_add_event
add_event_before = trick.ippython_add_event_before
add_event_after = trick.ippython_add_event_after
add_read = trick.ippython_add_read
delete_event = trick.ippython_delete_event
manual_on = trick.ippython_manual_on
manual_fire = trick.ippython_manual_fire
manual_off = trick.ippython_manual_off
manual_done = trick.ippython_manual_done

# bind event manager name to shortcut names.
get_event = trick.event_manager_get_event
remove_event = trick.event_manager_remove_event

if hasattr(top.cvar, 'trick_em'):
    activate_event = top.cvar.trick_em.em.activate_event
    deactivate_event = top.cvar.trick_em.em.deactivate_event

# from real time
if hasattr(top.cvar, 'trick_real_time'):
    itimer_enable = top.cvar.trick_real_time.itimer.enable
    itimer_disable = top.cvar.trick_real_time.itimer.disable

# from variable server / sim_control panel
if hasattr(top.cvar, 'trick_vs'):
    set_var_server_enabled = top.cvar.trick_vs.vs.set_enabled
    get_var_server_enabled = top.cvar.trick_vs.vs.get_enabled
    set_var_server_info_msg_on = top.cvar.trick_vs.vs.set_var_server_info_msg_on
    set_var_server_info_msg_off = top.cvar.trick_vs.vs.set_var_server_info_msg_off
    set_var_server_log_on = top.cvar.trick_vs.vs.set_var_server_log_on
    set_var_server_log_off = top.cvar.trick_vs.vs.set_var_server_log_off
    var_server_cpu_set = top.cvar.trick_vs.vs.get_listen_thread().cpu_set

    # Send all output to stdout/stderr through var_write_stdio.
    class VarWriteStdio:
        def __init__(self, in_stream):
            self.stream = in_stream
        def write(self, text):
            trick.var_write_stdio(self.stream.fileno() , text)
        def flush(self):
            self.stream.flush()
    sys.stdout = VarWriteStdio(sys.stdout)
    sys.stderr = VarWriteStdio(sys.stderr)

# from real time injector
if hasattr(top.cvar, 'trick_inject'):
    rti_set_debug = top.cvar.trick_inject.rtis.SetDebug
    rti_set_frame_multiple = top.cvar.trick_inject.rtis.SetFrameMultiple
    rti_set_frame_offset = top.cvar.trick_inject.rtis.SetFrameOffset
    rti_add = top.cvar.trick_inject.rtis.Add
    rti_fire = top.cvar.trick_inject.rtis.Fire
    rti_list = top.cvar.trick_inject.rtis.List

# from data_recording
if hasattr(top.cvar, 'trick_data_record'):
    drd_writer_thread = top.cvar.trick_data_record.drd.drd_writer_thread
    data_record_cpu_set = top.cvar.trick_data_record.drd.drd_writer_thread.cpu_set

# This routine will look up the reference name through ref_attributes.
# ref_to_value retrieves the value and places it into a V_DATA structure
# The return statement is an anonymous dictionary lookup that returns
# the value based on the type in val.  The 6 argument is a default
# type to use
def var_get(name):
    ref = trick.ref_attributes(name)
    ref.thisown = True
    val = trick.V_DATA()
    trick.ref_to_value(ref, val)
    return {
        '1': trick.vval_char(val),
        '2': trick.vval_char(val),
        '3': trick.vval_string(val),
        '4': trick.vval_short(val),
        '5': trick.vval_short(val),
        '6': trick.vval_int(val),
        '7': trick.vval_int(val),
        '8': trick.vval_long(val),
        '9': trick.vval_long(val),
        '10': trick.vval_float(val),
        '11': trick.vval_double(val),
        '14': trick.vval_longlong(val),
        '15': trick.vval_longlong(val),
        '16': trick.vval_filep(val),
        '17': trick.vval_char(val),
        '20': trick.vval_voidp(val),
        '21': trick.vval_int(val)
    }.get(str(val.type),6)

