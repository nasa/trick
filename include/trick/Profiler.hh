/*
    PURPOSE:
        (Trick runtime simulation profiler.)
*/
#ifndef PROFILER
#define PROFILER


#include "SimObject.hh"

namespace Trick {

    class Profiler {
        public:
            bool enabled;

            std::vector <Trick::SimObject *> *sim_objects ;        /**< trick_io(*io) trick_units(--) */

            Profiler();

            ~Profiler();

            void shutdown();
    };
}
#endif