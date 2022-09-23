web.server.enable = True
web.server.debug = True
web.server.ssl_enable = False
web.server.path_to_ssl_cert = "server.pem"
web.server.error_log_file = "log.error"

web.server.port = 8888
trick.var_server_set_port(9001)

exec(open("Modified_data/realtime.py").read())
exec(open("Modified_data/cannon.dr").read())
