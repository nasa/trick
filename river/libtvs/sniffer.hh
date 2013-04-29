#include <string>
#include <list>
#include <vector>
#ifdef WIN32
#include <windows.h>
#endif
#include "vsclientconnector.hh"
#include "../libutils/mutex.hh"
#include "../libutils/thread.hh"

/** \ingroup libtvs
    Constructs a list of active sims.
    Sniffer will listen on port 9265, multicast IP 224.3.14.15 for any sims
    broadcasting. It can also connect to remote sniff servers on other networks
    via user-specified hostspec, or tunnelled via local ports. Because of 
    tunneling, the idea of "local" versus "remote is less clear." The
    distinction adopted by the sniffer is that a sim we would connect to 
    directly via a local or tunnelled connection is "direct," and a sim we 
    would connect to via a sniff server is "remote."

    Sniffer has two constructors. The default constructor has no parameters
    and will listen for direct sims on the default port only. The secondary 
    constructor allows a list of hosts and ports to be used in addition to the
    default.

    The list of active sims is checked at regular (5s) intervals. It could be
    that between checks an active sim terminates. Sniffer must therefore be
    able to judge when a sim is no longer active. It does this by removing
    from the list any sim that has not appears for 2 polls (10s).
 */

class Sniffer {
  public:
    Sniffer(std::list<std::string>& hostnports /**< list of hostspecs in the
                                                    form host:port */
           );
    Sniffer();                                 ///< listen on default port only
    ~Sniffer(); 

    /// Add sniffer port in form "host:port"
    void add(const std::string& hostnport);

    /// Add sniffer port as a HostSpec
    void add(const HostSpec& hostspec);

    /// Add a list of sniffer port(s) in form "host:port"
    void add(std::list<std::string>& hostnports);

    std::vector<std::string> get_list(); ///< Get list of active sims
    std::string get_serialized_list();   /**< Get list of active sims as a
                                              single newline-delimited string */

    /// Get a connector to a sim specified by line
    VSClientConnector* getConnector(const std::string& line);

    /// Get the number of active sims
    unsigned int size() { return _list.size(); }

    /// Time in seconds before a previously added sim is considered "dead"
    static const int TOOOLD = 10; 

  private:
    /// Encapsulates a list item
    struct Item {
        Item(const std::string& thecontent, const double theage): 
                      content(thecontent), age(theage) {}
        bool operator==(Item& item) { return item.content == content; }

        std::string content;  ///< Sim information
        double age;           ///< How long since the sim was last heard from
    };

    struct ThreadInit {
        Sniffer* sniffer;
        HostSpec spec;
    };

    std::list<Item> _list;    ///< list of sims
    Thread _direct_thread; ///< listener thread for default processing
    std::list<Thread> _remote_threads; ///< threads for remote hosts/ports
    Mutex _mutex;   ///< list mutex

    void _init(void); ///< Initialize mutex and direct thread
    static void* _static_sniffdirect(void*);  ///< Thread entry function
    void _sniffdirect(void);  ///< Thread entry function
    static void* _static_sniffremote(void*);  ///< Thread entry function
    void _sniffremote(HostSpec& spec);  ///< Thread entry function
    void _update_list(const std::string& item); ///< Update list of sims
    bool _keep_running;     ///< Semaphore used to sync threads at termination
    bool _too_old(Item& item, double time); /**< Test whether it's too long 
                                                 since a sim was heard from */
    HostSpec _currentSpec;  /**< Working space to store the current spec
                                 being spawned */
};
