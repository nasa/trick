# The ThreadBase Class

A trick sim is a multi-threaded process, and all of the threads that are created by Trick or Trick generated code inherit from the abstract class ThreadBase. ThreadBase classes have the capability to change the priority and cpu affinity of the thread. ThreadBase classes include trick_sys threads as well as user defined threads from the S_define:

| Class                       | Access                                          |
| --------------------------- | ----------------------------------------------- |
| Threads (user defined)      | See [[Executive-Scheduler#thread-control]]      |
| VariableServerListenThread  | `trick_vs.vs.get_listen_thread()`               |
| MessageTCDeviceListenThread | `trick_message.mdevice.get_listen_thread()`     |
| MessageThreadedCout         | `trick_message.mtcout`                          |
| DRDWriterThread             | `trick_data_record.drd.drd_writer_thread`       |
| VariableServerThread        | `trick_vs.vs.get_vst(pthread_t thread_id)`      |


## ThreadBase Methods

Get the name of the thread:
```cpp
std::string get_name()
```
***

Set the name of the thread:
```cpp
void set_name(std::string in_name)
```
***

Get the thread pthread ID:
```cpp
pthread_t get_pthread_id();
```
***


Get the thread process ID (Linux only):
```c++
pid_t get_pid()
```
***

`cpu_set` adds an affinity for a single CPU to the current thread. The thread will use this CPU if it is available. To add affinity for multiple cpus you can call cpu_set multiple times. For a better understanding of how this may affect your sim, see the linux man pages for CPU_SET_S and sched_setaffinity. (Linux only)
```cpp 
int cpu_set(unsigned int cpu)
```
***


Clear an affinity for a single CPU from the current thread (Linux only):
```cpp
int cpu_clr(unsigned int cpu )
```
***

`get_cpus` returns the cpu set affinity bitmask for the current thread. If your sim requires very finely tuned CPU affinities for your threads, you can use functions from GNU sched.h to do more complex operations on this set. Refer to the man page for CPU_SET_S for a list of other functions that can be used with this bitmask. Use `CPU_ALLOC_SIZE(sysconf(_SC_NPROCESSORS_ONLN))` as your size argument to the `CPU_*_S` functions (Linux only).

```cpp
cpu_set_t * get_cpus()
```
***


Overwrite the thread cpu affinity bitmask with in_cpus (Linux only):
```cpp
void copy_cpus(cpu_set_t * in_cpus)
```
***

Set the priority for the thread. See the man page for sched(7) for more details.
```cpp
int set_priority(unsigned int req_priority)
```
[Continue to Simulation Utilities](/trick/documentation/simulation_utilities/Simulation-Utilities)
