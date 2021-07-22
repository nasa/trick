class Params:
    #Change the following to change the default parameters
    def __init__(self) -> None:
        self.__port = 9000
        self.__var_server_port = 9001
        self.__host = "localhost"
        self.__enable_ssl = False
        self.__test_time = True
        # self.__ssl_cert_path = "server.pem"
        # self.__ssl_cert_path = "/home/cherpin/git/trick_fork/trick_sims/Cannon/SIM_cannon_numeric/server.pem"
        self.__ssl_cert_path = "/home/cherpin/.ssl/server.pem"
    
    def get_ssl_cert_path(self):
        return self.__ssl_cert_path

    def get_port(self):
        return self.__port

    def get_host(self):
        return self.__host

    def get_ssl_enable(self):
        return self.__enable_ssl

    def get_var_server_port(self):
        return self.__var_server_port
    
    def get_test_time(self):
        return self.__test_time

    def get_url(self, endpoint):
        server_port = self.get_port()
        server_host = self.get_host()
        ssl_enable = self.get_ssl_enable()
        base_url = f"http{ 's' if ssl_enable else '' }://{server_host}:{server_port}"
        return f"{base_url}/{endpoint}"

    def get_ws_url(self, endpoint):
        return f"ws{ 's' if self.get_ssl_enable() else '' }://{self.get_host()}:{self.get_port()}/{endpoint}"
