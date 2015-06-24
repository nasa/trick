
#include <iomanip>
#include <sstream>
#include <sys/time.h>

#include "trick/MonteCarlo.hh"
#include "trick/tc_proto.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"

void Trick::MonteCarlo::dispatch_run_to_slave(MonteRun *in_run, MonteSlave *in_slave) {
    if (in_slave && in_run) {
        current_run = in_run->id;
        if (prepare_run(in_run) == -1) {
            return;
        }
        in_slave->state = MonteSlave::RUNNING;
        connection_device.hostname = (char*)in_slave->machine_name.c_str();
        connection_device.port = in_slave->port;
        if (tc_connect(&connection_device) == TC_SUCCESS) {
            std::stringstream buffer_stream;
            buffer_stream << run_directory << "/RUN_" << std::setw(5) << std::setfill('0') << in_run->id;
            std::string buffer = "";
            for (std::vector<std::string>::size_type j = 0; j < in_run->variables.size(); ++j) {
                buffer += in_run->variables[j] + "\n";
            }
            buffer += std::string("trick.set_output_dir(\"") + buffer_stream.str() + std::string("\");\n");
            buffer_stream.str("");
            buffer_stream << in_run->id ;
            buffer += std::string("trick.mc_set_current_run(") + buffer_stream.str() + std::string(");\n");

            if (verbosity >= INFORMATIONAL) {
                message_publish(MSG_INFO, "Monte [Master] Dispatching run %d to %s:%d.\n",
		     in_run->id, in_slave->machine_name.c_str(), in_slave->id) ;
            }
            int command = htonl(MonteSlave::PROCESS_RUN);
            tc_write(&connection_device, (char *)&command, (int)sizeof(command));
            int num_bytes = htonl(buffer.length());
            tc_write(&connection_device, (char*)&num_bytes, (int)sizeof(num_bytes));
            tc_write(&connection_device, (char*)buffer.c_str(), (int)buffer.length());
            tc_disconnect(&connection_device);

            ++in_slave->num_dispatches;
            in_slave->current_run = in_run;

            struct timeval time_val;
            gettimeofday(&time_val, NULL);
            in_run->start_time = time_val.tv_sec + (double)time_val.tv_usec / 1000000;
            ++in_run->num_tries;
        } else {
            in_slave->state = Trick::MonteSlave::DISCONNECTED;
            if (verbosity >= ERROR) {
                message_publish(MSG_ERROR, "Monte [Master] Lost connection to %s:%d while dispatching run.\n",
                                in_slave->machine_name.c_str(), in_slave->id) ;
            }
        }
    }
}
