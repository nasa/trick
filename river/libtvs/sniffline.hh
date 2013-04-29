#include <string>

/** \ingroup libtvs
    Deconstructs a sniffer line
 */
class SniffLine {
  public:
    SniffLine(const std::string&);
    std::string serialize();  ///< Put the line back together
    std::string getSniffSpec() const { return _sniffspec; } ///< Getter
    std::string getHost() const { return _host; } ///< Getter
    std::string getPort() const { return _port; } ///< Getter
    std::string getUser() const { return _user; } ///< Getter
    std::string getProcess() const { return _process; } ///< Getter
    bool isValid() { return _isvalid; } ///< Did the spec pass parsing?

  private:
    bool _isRemote(const std::string& line) { return line[0] == '['; }
    std::string _sniffspec;
    std::string _host;
    std::string _port;
    std::string _user;
    std::string _process;
    std::string _version;
    bool _isvalid;
};
