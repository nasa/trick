
#ifndef JIT_INPUT_FILE_PROTO_HH
#define JIT_INPUT_FILE_PROTO_HH

#include <string>

int jit_compile(std::string file_name) ;
int jit_run(std::string library_name , std::string run_function ) ;
int jit_compile_and_run(std::string file_name, std::string run_function = "run_me" ) ;
int jit_add_library(std::string lib_name) ;
void * jit_find_symbol(std::string sym ) ;

int jit_add_read( unsigned int thread_id , double time , std::string func_name ) ;
int jit_add_read( double time , std::string func_name ) ;
int jit_add_event( std::string func_name , std::string name , double cycle ) ;
int jit_add_event_before( std::string func_name , std::string name , std::string target_name , unsigned int target_inst = 1 ) ;
int jit_add_event_after( std::string func_name , std::string name , std::string target_name , unsigned int target_inst = 1 ) ;

#endif
