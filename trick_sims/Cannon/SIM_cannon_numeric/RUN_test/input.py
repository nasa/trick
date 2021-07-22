web.server.enable = True
web.server.debug = True
# web.server.ssl_enable = True
web.server.path_to_ssl_cert = '/home/cherpin/.ssl/server.pem'
web.server.path_to_ssl_cert = "server.pem"
web.server.port = 5000

trick.var_server_set_port(5001)

exec(open("Modified_data/realtime.py").read())
exec(open("Modified_data/cannon.dr").read())
