
%module sim_services

#include "trick_swig/trick_swig.i"
#include "trick_swig/units_attach.i"
#include "trick_swig/extra_functions.i"
#include "trick_swig/swig_class_typedef.i"
#include "trick_swig/cast_as.i"

// Special typemap for collectee in add_collect and delete_collect.
%typemap(in) void * collectee {
    // VOID *
    void * argp2 ;

    if ( SWIG_IsOK(SWIG_ConvertPtr($input, &argp2,SWIG_TypeQuery("void *"), 0)) ) {
        // We have an address coming in, we don't have to do any translation
        $1 = reinterpret_cast< $1_basetype * >(argp2) ;
    } else if ( SWIG_IsOK(SWIG_ConvertPtr($input, &argp2,SWIG_TypeQuery("swig_ref *"), 0)) ) {
        // Array to pointer assignment
        swig_ref * temp_swig_ref = reinterpret_cast< swig_ref * >(argp2);
        if ( temp_swig_ref != NULL ) {
            $1 = ($1_basetype *)temp_swig_ref->ref.address ;
        }
        // package the array address as a void
    }
}

%inline %{
#include "trick_swig/swig_global_vars.hh"

#include "trick_utils/math/include/reference_frame.h"
#include "trick_utils/math/include/rand_generator.h"
#include "trick_utils/math/include/wave_form.h"
#include "sim_services/Integrator/include/integrator_c_intf.h"

#include "sim_services/Clock/include/GetTimeOfDayClock.hh"
#include "sim_services/Clock/include/BC635Clock.hh"
#include "sim_services/Clock/include/TPROCTEClock.hh"
#include "sim_services/Clock/include/clock_proto.h"
#include "sim_services/CommandLineArguments/include/CommandLineArguments.hh"
#include "sim_services/CommandLineArguments/include/command_line_protos.h"
#include "sim_services/DataRecord/include/DRAscii.hh"
#include "sim_services/DataRecord/include/DRBinary.hh"
#ifdef HDF5
#include "sim_services/DataRecord/include/DRHDF5.hh"
#endif
#ifdef DMTCP
#include "sim_services/DMTCP/include/DMTCP.hh"
#include "sim_services/DMTCP/include/dmtcp_checkpoint_c_intf.hh"
#endif
#include "sim_services/DataRecord/include/DataRecordDispatcher.hh"
#include "sim_services/DataRecord/include/data_record_proto.h"
#include "sim_services/DebugPause/include/DebugPause.hh"
#include "sim_services/DebugPause/include/debug_pause_proto.h"
#include "sim_services/EchoJobs/include/EchoJobs.hh"
#include "sim_services/EchoJobs/include/echojobs_proto.h"
#include "sim_services/Environment/include/Environment.hh"
#include "sim_services/Environment/include/env_proto.h"
#include "sim_services/Executive/include/Executive.hh"
#include "sim_services/Executive/include/exec_proto.h"
#include "sim_services/Executive/include/exec_proto.hh"
#include "sim_services/ExternalApplications/include/MalfunctionsTrickView.hh"
#include "sim_services/ExternalApplications/include/MonteMonitor.hh"
#include "sim_services/ExternalApplications/include/SimControlPanel.hh"
#include "sim_services/ExternalApplications/include/StripChart.hh"
#include "sim_services/ExternalApplications/include/TrickView.hh"
#include "sim_services/ExternalApplications/include/ExternalApplicationManager.hh"
#include "sim_services/ExternalApplications/include/external_application_c_intf.h"
#include "sim_services/FrameLog/include/FrameDataRecordGroup.hh"
#include "sim_services/FrameLog/include/FrameLog.hh"
#include "sim_services/FrameLog/include/framelog_proto.h"
#include "sim_services/InputProcessor/include/IPPython.hh"
#include "sim_services/InputProcessor/include/input_processor_proto.h"
#include "sim_services/InputProcessor/include/MTV.hh"
#include "sim_services/EventManager/include/EventManager.hh"
#include "sim_services/EventManager/include/EventProcessor.hh"
#include "sim_services/EventManager/include/EventManager_c_intf.hh"
#include "sim_services/JITInputFile/include/JITInputFile.hh"
#include "sim_services/JITInputFile/include/jit_input_file_proto.hh"
#include "sim_services/JSONVariableServer/include/JSONVariableServer.hh"
#include "sim_services/Integrator/include/IntegLoopScheduler.hh"
#include "sim_services/Integrator/include/IntegLoopManager.hh"
#include "sim_services/Integrator/include/IntegLoopSimObject.hh"
#include "sim_services/Integrator/include/Integrator.hh"
#include "sim_services/Integrator/include/regula_falsi.h"

#ifdef USE_ER7_UTILS_INTEGRATORS
#  include "sim_services/Integrator/er7_algorithms/ABM_Integrator.hh"
#  include "sim_services/Integrator/er7_algorithms/Euler_Cromer_Integrator.hh"
#  include "sim_services/Integrator/er7_algorithms/Euler_Integrator.hh"
#  include "sim_services/Integrator/er7_algorithms/MM4_Integrator.hh"
#  include "sim_services/Integrator/er7_algorithms/NL2_Integrator.hh"
#  include "sim_services/Integrator/er7_algorithms/RK2_Integrator.hh"
#  include "sim_services/Integrator/er7_algorithms/RK4_Integrator.hh"
#  include "sim_services/Integrator/er7_algorithms/RKF45_Integrator.hh"
#  include "sim_services/Integrator/er7_algorithms/RKF78_Integrator.hh"
#  include "sim_services/Integrator/er7_algorithms/RKG4_Integrator.hh"
#else
#  include "sim_services/Integrator/trick_algorithms/ABM_Integrator.hh"
#  include "sim_services/Integrator/trick_algorithms/Euler_Cromer_Integrator.hh"
#  include "sim_services/Integrator/trick_algorithms/Euler_Integrator.hh"
#  include "sim_services/Integrator/trick_algorithms/MM4_Integrator.hh"
#  include "sim_services/Integrator/trick_algorithms/NL2_Integrator.hh"
#  include "sim_services/Integrator/trick_algorithms/RK2_Integrator.hh"
#  include "sim_services/Integrator/trick_algorithms/RK4_Integrator.hh"
#  include "sim_services/Integrator/trick_algorithms/RKF45_Integrator.hh"
#  include "sim_services/Integrator/trick_algorithms/RKF78_Integrator.hh"
#  include "sim_services/Integrator/trick_algorithms/RKG4_Integrator.hh"
#endif

#include "sim_services/MasterSlave/include/MSSocket.hh"
#include "sim_services/MasterSlave/include/MSSharedMem.hh"
#include "sim_services/MasterSlave/include/Master.hh"
#include "sim_services/MasterSlave/include/Slave.hh"
#include "sim_services/MasterSlave/include/master_proto.h"
#include "sim_services/MemoryManager/include/MemoryManager.hh"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"
#include "sim_services/CheckPointRestart/include/CheckPointRestart.hh"
#include "sim_services/CheckPointRestart/include/CheckPointRestart_c_intf.hh"
#include "sim_services/Collect/include/collect_proto.hh"
#include "sim_services/Sie/include/AttributesMap.hh"
#include "sim_services/Sie/include/sie_c_intf.h"
#include "sim_services/Message/include/MessageCout.hh"
#include "sim_services/Message/include/MessageFile.hh"
#include "sim_services/Message/include/MessageLCout.hh"
#include "sim_services/Message/include/MessagePublisher.hh"
#include "sim_services/Message/include/MessageSubscriber.hh"
#include "sim_services/Message/include/MessageTCDevice.hh"
#include "sim_services/Message/include/message_proto.h"
#include "sim_services/MonteCarlo/include/MonteCarlo.hh"
#include "sim_services/MonteCarlo/include/montecarlo_c_intf.h"
#include "sim_services/MonteCarlo/include/MonteVarCalculated.hh"
#include "sim_services/MonteCarlo/include/MonteVarFile.hh"
#include "sim_services/MonteCarlo/include/MonteVarFixed.hh"
#include "sim_services/MonteCarlo/include/MonteVarRandom.hh"
#include "sim_services/RealtimeSync/include/RealtimeSync.hh"
#include "sim_services/RealtimeSync/include/realtimesync_proto.h"
#include "sim_services/RealtimeInjector/include/RtiExec.hh"
#include "sim_services/RealtimeInjector/include/RtiStager.hh"
#include "sim_services/Timer/include/ITimer.hh"
#include "sim_services/UnitTest/include/UnitTest.hh"
#include "sim_services/UnitTest/include/trick_tests.h"
#include "sim_services/VariableServer/include/VariableServer.hh"
#include "sim_services/VariableServer/include/variable_server_proto.h"
#include "sim_services/include/Flag.h"
#include "trick_utils/units/include/Unit.hh"
#include "sim_services/SimTime/include/SimTime.hh"
#include "sim_services/SimTime/include/simtime_proto.h"


#ifdef USE_ER7_UTILS_INTEGRATORS
#include "er7_utils/integration/core/include/integrator_constructor_factory.hh"
#include "er7_utils/integration/core/include/integrable_object.hh"
#include "er7_utils/integration/core/include/base_integration_group.hh"
#include "er7_utils/trick/integration/include/first_order_ode_integrator.hh"
#include "er7_utils/trick/integration/include/second_order_ode_integrator.hh"
#endif

%}

%pythoncode %{
import os
import linecache
import re

curr_dir = os.getcwd()
exclude_dir = os.getcwd() + "/trick"

def traceit(frame, event, arg):
    if event == "line":
        lineno = frame.f_lineno
        filename = frame.f_code.co_filename
        if ( not filename.startswith(exclude_dir) and not filename.startswith("/usr")):
            if (filename.endswith(".pyc") or
                filename.endswith(".pyo")):
                filename = filename[:-1]
            line = linecache.getline(filename, lineno)
            print "%s:%s: %s" % (filename, lineno, line.rstrip())
    return traceit
%}

