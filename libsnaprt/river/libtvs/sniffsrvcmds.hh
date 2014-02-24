#ifndef sniffsrvcmds_hh
#define sniffsrvcmds_hh
#include <string>

/** \ingroup libtvs
   A factory "warehouse" class for sniff server command strings
 */
class SniffSrvCmds {
  public:
    static const std::string CMDDELIM;    ///< Indicates a command
    static const std::string FAILUREDELIM;///< Indicates a failure

    /// Route to another sniffsrv
    static std::string route();
    /// Route to another sniffsrv
    static std::string route(const std::string& host, const int port);

    /// Add multiple commands
    static std::string addmultiple();
    /// Check if a parameter exists
    static std::string exists();

    /// Detach and kill connection
    static std::string detach();

    /// Connect to a sim
    static std::string connect();
    /// Connect to a sim
    static std::string connect(const std::string& host, const int port);
    /// Disconnect from a sim
    static std::string disconnect();

    /// Request one-shot list of sims
    static std::string listonce();

    /// Request periodically-updated list of sims
    static std::string listperiodic();

    /// Unsubscribe from periodically-updated list of sims
    static std::string listremove();

    /// Successful execution
    static std::string success();

    /// Error status
    static std::string failure(const std::string& msg);

    /// Whether buffer contains a command (based on CMDDELIM)
    static bool isCmd(const std::string& buffer);
};

// TODO: Write classes that encapsulate sniff cmds following the command pattern
// and do proper validation and execution
/*
class SniffCmd {
};
*/

#endif
