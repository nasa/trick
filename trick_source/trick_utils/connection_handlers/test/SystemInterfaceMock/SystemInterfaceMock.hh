
#ifndef __SYSTEM_INTERFACE_MOCK__
#define __SYSTEM_INTERFACE_MOCK__


#include "trick/SystemInterface.hh"



typedef std::function<int (int, int, int)> socket_func_type;

typedef std::function<int (int, int, int, const void *, socklen_t)> setsockopt_func_type;

typedef std::function<int (int, const struct sockaddr *, socklen_t)> bind_func_type;

typedef std::function<int (int, struct sockaddr *, socklen_t *)> getsockname_func_type;

typedef std::function<int (int, int)> listen_func_type;

typedef std::function<int (int, fd_set *, fd_set *, fd_set *, struct timeval *)> select_func_type;

typedef std::function<int (int)> close_func_type;

typedef std::function<int (const char *, const char *, const struct addrinfo *, struct addrinfo **)> getaddrinfo_func_type;

typedef std::function<int (int, int, int)> fcntl_func_type;

typedef std::function<int (int, int)> shutdown_func_type;

typedef std::function<int (int, struct sockaddr *, socklen_t *)> accept_func_type;

typedef std::function<ssize_t (int, const void *, size_t, int)> send_func_type;

typedef std::function<ssize_t (int, const void *, size_t, int, const struct sockaddr *, socklen_t)> sendto_func_type;

typedef std::function<ssize_t (int, void *, size_t, int)> recv_func_type;

typedef std::function<ssize_t (int, void *, size_t, int, struct sockaddr *, socklen_t *)> recvfrom_func_type;

typedef std::function<in_addr_t (const char *)> inet_addr_func_type;


class SystemInferfaceMock : public SystemInterface {
    public:
        SystemInferfaceMock () {
            
            real_socket_impl();
            real_setsockopt_impl();
            real_bind_impl();
            real_getsockname_impl();
            real_listen_impl();
            real_select_impl();
            real_close_impl();
            real_getaddrinfo_impl();
            real_fcntl_impl();
            real_shutdown_impl();
            real_accept_impl();
            real_send_impl();
            real_sendto_impl();
            real_recv_impl();
            real_recvfrom_impl();
            real_inet_addr_impl();
        }
        
        void set_all_real () {
            
            real_socket_impl();
            real_setsockopt_impl();
            real_bind_impl();
            real_getsockname_impl();
            real_listen_impl();
            real_select_impl();
            real_close_impl();
            real_getaddrinfo_impl();
            real_fcntl_impl();
            real_shutdown_impl();
            real_accept_impl();
            real_send_impl();
            real_sendto_impl();
            real_recv_impl();
            real_recvfrom_impl();
            real_inet_addr_impl();
        }

        void set_all_noop() {
            
            noop_socket_impl();
            noop_setsockopt_impl();
            noop_bind_impl();
            noop_getsockname_impl();
            noop_listen_impl();
            noop_select_impl();
            noop_close_impl();
            noop_getaddrinfo_impl();
            noop_fcntl_impl();
            noop_shutdown_impl();
            noop_accept_impl();
            noop_send_impl();
            noop_sendto_impl();
            noop_recv_impl();
            noop_recvfrom_impl();
            noop_inet_addr_impl();
        }

   
   // socket implementation
   public:
       virtual int  socket (int domain, int type, int protocol) override { return socket_impl( domain,  type,  protocol); }
       void register_socket_impl (socket_func_type impl) { socket_impl = impl; }
       void real_socket_impl () { socket_impl = [](int domain, int type, int protocol) -> int  { return ::socket( domain,  type,  protocol); }; }
       void noop_socket_impl () { socket_impl = [](int domain, int type, int protocol) -> int  { return 0; }; }
   private:
       socket_func_type socket_impl;
   
   // setsockopt implementation
   public:
       virtual int  setsockopt (int socket, int level, int option_name, const void * option_value, socklen_t option_len) override { return setsockopt_impl( socket,  level,  option_name,  option_value,  option_len); }
       void register_setsockopt_impl (setsockopt_func_type impl) { setsockopt_impl = impl; }
       void real_setsockopt_impl () { setsockopt_impl = [](int socket, int level, int option_name, const void * option_value, socklen_t option_len) -> int  { return ::setsockopt( socket,  level,  option_name,  option_value,  option_len); }; }
       void noop_setsockopt_impl () { setsockopt_impl = [](int socket, int level, int option_name, const void * option_value, socklen_t option_len) -> int  { return 0; }; }
   private:
       setsockopt_func_type setsockopt_impl;
   
   // bind implementation
   public:
       virtual int  bind (int socket, const struct sockaddr * address, socklen_t address_len) override { return bind_impl( socket,  address,  address_len); }
       void register_bind_impl (bind_func_type impl) { bind_impl = impl; }
       void real_bind_impl () { bind_impl = [](int socket, const struct sockaddr * address, socklen_t address_len) -> int  { return ::bind( socket,  address,  address_len); }; }
       void noop_bind_impl () { bind_impl = [](int socket, const struct sockaddr * address, socklen_t address_len) -> int  { return 0; }; }
   private:
       bind_func_type bind_impl;
   
   // getsockname implementation
   public:
       virtual int  getsockname (int socket, struct sockaddr * address, socklen_t * address_len) override { return getsockname_impl( socket,  address,  address_len); }
       void register_getsockname_impl (getsockname_func_type impl) { getsockname_impl = impl; }
       void real_getsockname_impl () { getsockname_impl = [](int socket, struct sockaddr * address, socklen_t * address_len) -> int  { return ::getsockname( socket,  address,  address_len); }; }
       void noop_getsockname_impl () { getsockname_impl = [](int socket, struct sockaddr * address, socklen_t * address_len) -> int  { return 0; }; }
   private:
       getsockname_func_type getsockname_impl;
   
   // listen implementation
   public:
       virtual int  listen (int socket, int backlog) override { return listen_impl( socket,  backlog); }
       void register_listen_impl (listen_func_type impl) { listen_impl = impl; }
       void real_listen_impl () { listen_impl = [](int socket, int backlog) -> int  { return ::listen( socket,  backlog); }; }
       void noop_listen_impl () { listen_impl = [](int socket, int backlog) -> int  { return 0; }; }
   private:
       listen_func_type listen_impl;
   
   // select implementation
   public:
       virtual int  select (int nfds, fd_set * readfds, fd_set * writefds, fd_set * errorfds, struct timeval * timeout) override { return select_impl( nfds,  readfds,  writefds,  errorfds,  timeout); }
       void register_select_impl (select_func_type impl) { select_impl = impl; }
       void real_select_impl () { select_impl = [](int nfds, fd_set * readfds, fd_set * writefds, fd_set * errorfds, struct timeval * timeout) -> int  { return ::select( nfds,  readfds,  writefds,  errorfds,  timeout); }; }
       void noop_select_impl () { select_impl = [](int nfds, fd_set * readfds, fd_set * writefds, fd_set * errorfds, struct timeval * timeout) -> int  { return 0; }; }
   private:
       select_func_type select_impl;
   
   // close implementation
   public:
       virtual int  close (int fildes) override { return close_impl( fildes); }
       void register_close_impl (close_func_type impl) { close_impl = impl; }
       void real_close_impl () { close_impl = [](int fildes) -> int  { return ::close( fildes); }; }
       void noop_close_impl () { close_impl = [](int fildes) -> int  { return 0; }; }
   private:
       close_func_type close_impl;
   
   // getaddrinfo implementation
   public:
       virtual int  getaddrinfo (const char * hostname, const char * servname, const struct addrinfo * hints, struct addrinfo ** res) override { return getaddrinfo_impl( hostname,  servname,  hints,  res); }
       void register_getaddrinfo_impl (getaddrinfo_func_type impl) { getaddrinfo_impl = impl; }
       void real_getaddrinfo_impl () { getaddrinfo_impl = [](const char * hostname, const char * servname, const struct addrinfo * hints, struct addrinfo ** res) -> int  { return ::getaddrinfo( hostname,  servname,  hints,  res); }; }
       void noop_getaddrinfo_impl () { getaddrinfo_impl = [](const char * hostname, const char * servname, const struct addrinfo * hints, struct addrinfo ** res) -> int  { return 0; }; }
   private:
       getaddrinfo_func_type getaddrinfo_impl;
   
   // fcntl implementation
   public:
       virtual int  fcntl (int fildes, int cmd, int arg) override { return fcntl_impl( fildes,  cmd,  arg); }
       void register_fcntl_impl (fcntl_func_type impl) { fcntl_impl = impl; }
       void real_fcntl_impl () { fcntl_impl = [](int fildes, int cmd, int arg) -> int  { return ::fcntl( fildes,  cmd,  arg); }; }
       void noop_fcntl_impl () { fcntl_impl = [](int fildes, int cmd, int arg) -> int  { return 0; }; }
   private:
       fcntl_func_type fcntl_impl;
   
   // shutdown implementation
   public:
       virtual int  shutdown (int socket, int how) override { return shutdown_impl( socket,  how); }
       void register_shutdown_impl (shutdown_func_type impl) { shutdown_impl = impl; }
       void real_shutdown_impl () { shutdown_impl = [](int socket, int how) -> int  { return ::shutdown( socket,  how); }; }
       void noop_shutdown_impl () { shutdown_impl = [](int socket, int how) -> int  { return 0; }; }
   private:
       shutdown_func_type shutdown_impl;
   
   // accept implementation
   public:
       virtual int  accept (int socket, struct sockaddr * address, socklen_t * address_len) override { return accept_impl( socket,  address,  address_len); }
       void register_accept_impl (accept_func_type impl) { accept_impl = impl; }
       void real_accept_impl () { accept_impl = [](int socket, struct sockaddr * address, socklen_t * address_len) -> int  { return ::accept( socket,  address,  address_len); }; }
       void noop_accept_impl () { accept_impl = [](int socket, struct sockaddr * address, socklen_t * address_len) -> int  { return 0; }; }
   private:
       accept_func_type accept_impl;
   
   // send implementation
   public:
       virtual ssize_t  send (int socket, const void * buffer, size_t length, int flags) override { return send_impl( socket,  buffer,  length,  flags); }
       void register_send_impl (send_func_type impl) { send_impl = impl; }
       void real_send_impl () { send_impl = [](int socket, const void * buffer, size_t length, int flags) -> ssize_t  { return ::send( socket,  buffer,  length,  flags); }; }
       void noop_send_impl () { send_impl = [](int socket, const void * buffer, size_t length, int flags) -> ssize_t  { return 0; }; }
   private:
       send_func_type send_impl;
   
   // sendto implementation
   public:
       virtual ssize_t  sendto (int socket, const void * buffer, size_t length, int flags, const struct sockaddr * dest_addr, socklen_t dest_len) override { return sendto_impl( socket,  buffer,  length,  flags,  dest_addr,  dest_len); }
       void register_sendto_impl (sendto_func_type impl) { sendto_impl = impl; }
       void real_sendto_impl () { sendto_impl = [](int socket, const void * buffer, size_t length, int flags, const struct sockaddr * dest_addr, socklen_t dest_len) -> ssize_t  { return ::sendto( socket,  buffer,  length,  flags,  dest_addr,  dest_len); }; }
       void noop_sendto_impl () { sendto_impl = [](int socket, const void * buffer, size_t length, int flags, const struct sockaddr * dest_addr, socklen_t dest_len) -> ssize_t  { return 0; }; }
   private:
       sendto_func_type sendto_impl;
   
   // recv implementation
   public:
       virtual ssize_t  recv (int socket, void * buffer, size_t length, int flags) override { return recv_impl( socket,  buffer,  length,  flags); }
       void register_recv_impl (recv_func_type impl) { recv_impl = impl; }
       void real_recv_impl () { recv_impl = [](int socket, void * buffer, size_t length, int flags) -> ssize_t  { return ::recv( socket,  buffer,  length,  flags); }; }
       void noop_recv_impl () { recv_impl = [](int socket, void * buffer, size_t length, int flags) -> ssize_t  { return 0; }; }
   private:
       recv_func_type recv_impl;
   
   // recvfrom implementation
   public:
       virtual ssize_t  recvfrom (int socket, void * buffer, size_t length, int flags, struct sockaddr * address, socklen_t * address_len) override { return recvfrom_impl( socket,  buffer,  length,  flags,  address,  address_len); }
       void register_recvfrom_impl (recvfrom_func_type impl) { recvfrom_impl = impl; }
       void real_recvfrom_impl () { recvfrom_impl = [](int socket, void * buffer, size_t length, int flags, struct sockaddr * address, socklen_t * address_len) -> ssize_t  { return ::recvfrom( socket,  buffer,  length,  flags,  address,  address_len); }; }
       void noop_recvfrom_impl () { recvfrom_impl = [](int socket, void * buffer, size_t length, int flags, struct sockaddr * address, socklen_t * address_len) -> ssize_t  { return 0; }; }
   private:
       recvfrom_func_type recvfrom_impl;
   
   // inet_addr implementation
   public:
       virtual in_addr_t  inet_addr (const char * cp) override { return inet_addr_impl( cp); }
       void register_inet_addr_impl (inet_addr_func_type impl) { inet_addr_impl = impl; }
       void real_inet_addr_impl () { inet_addr_impl = [](const char * cp) -> in_addr_t  { return ::inet_addr( cp); }; }
       void noop_inet_addr_impl () { inet_addr_impl = [](const char * cp) -> in_addr_t  { return 0; }; }
   private:
       inet_addr_func_type inet_addr_impl;
   
};

#endif
