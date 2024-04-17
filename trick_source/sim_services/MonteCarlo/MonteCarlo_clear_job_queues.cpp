
#include "trick/MonteCarlo.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"

void Trick::MonteCarlo::clear_job_queues() {

    master_init_queue.clear();
    master_pre_queue.clear();
    master_post_queue.clear();
    master_shutdown_queue.clear();
    slave_init_queue.clear();
    slave_pre_queue.clear();
    slave_post_queue.clear();
    slave_shutdown_queue.clear();

}
