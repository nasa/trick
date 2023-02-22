#include "trick/ClientConnection.hh"

std::string Trick::ClientConnection::getClientTag () {
    return _client_tag;
}

int Trick::ClientConnection::setClientTag (std::string tag) {
    _client_tag = tag;
    return 0;
}