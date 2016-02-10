/*******************************************************************************

 PURPOSE:
 (Jobs to call python routine to checkpoint/restart input file allocations.)

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (March 2014) (--)))

 *******************************************************************************/

#ifndef CHECKPOINTHELPER_INPUTALLOCSCHKPTRESTART_HH_
#define CHECKPOINTHELPER_INPUTALLOCSCHKPTRESTART_HH_

#include <sstream>

#include "sim_services/InputProcessor/include/InputProcessor.hh"
#include "sim_services/CheckPointRestart/include/CheckPointRestart.hh"

extern Trick::InputProcessor * the_ip;
extern Trick::CheckPointRestart * the_cpr;

namespace CheckpointHelper
{

class InputAllocsChkptRestart
{
public:
   InputAllocsChkptRestart()
   {
   }
   virtual ~InputAllocsChkptRestart()
   {
   }

   void writeChkptFile()
   {
      std::string cFile = the_cpr->get_output_file();
      cFile += "_inputAllocs.py";
      std::stringstream ss;
      ss << "if \"externAllocs\" in globals():" << std::endl;
      ss << "  externAllocs.writeChkptFile(\"" << cFile << "\")" << std::endl;
      ss << "  trick.message_publish(1, \"Dumped input allocations checkpoint " << cFile << ".\\n\")" << std::endl;
      ss << "else:" << std::endl;
      ss << "  trick.message_publish(1, \"externAllocs not defined: skipping input allocations checkpoint\\n\")"
            << std::endl;
      the_ip->parse(ss.str().c_str());
   }

   void loadChkptFile()
   {
      std::string cFile = the_cpr->get_load_file();
      cFile += "_inputAllocs.py";
      std::stringstream ss;
      ss << "try:" << std::endl;
      ss << "  execfile(\"" << cFile << "\")" << std::endl;
      ss << "  trick.message_publish(1, \"Loaded " << cFile << " input allocations checkpoint.\\n\")" << std::endl;
      ss << "except IOError:" << std::endl;
      ss << "  trick.message_publish(1, \"" << cFile << " was not found. Skipping input allocations restoration.\\n\")"
            << std::endl;
      the_ip->parse(ss.str().c_str());
   }
};

} /* namespace STLHelper */
#endif /* STLHELPER_INPUTALLOCSCHKPTRESTART_HH_ */
