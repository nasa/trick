# Adding a Web Server to Your Sim

To add a web server to your simulation, simply include the CivetServer sim module into your **S_define** file:

```
#include "sim_objects/CivetServer.sm"
```

## Configuration of the Web Server

The following (input.py) parameters are available to configure your web server:   

|Parameter Name             | Default Value             | Description                                                     |
|---------------------------|---------------------------|-----------------------------------------------------------------|
|web.server.enable          | False                     |Must be explicitly enabled                                       |
|web.server.port            | "8888"                    |Web servers “listen” port                                        |
|web.server.document_root   | "www"                     |Web servers document root                                        |
|web.server.debug           | False                     |Print Client/Server Communication.                               |
|web.server.ssl_enable      | False                     |Encrypt traffic. Uses https instead of http.                     |
|web.server.path_to_ssl_cert|"~/.ssl/server.pem"        |Path to your certificate.  This is only used if ssl_enable = True|
|web.server.error_log_file  | "civet_server_error.log"  |CivetWeb error log file.                                         |

For your web server to be active, you must at least specify the following :   

```python
web.server.enable = True

```

To have your web server listen on port 8890, rather than 8888, you would specify:   

```python
web.server.port = "8890"
```

To serve files from a directory called ```my_document_root```, rather than ```www```:

```python
web.server.document_root = "my_document_root"
```

To see client/server communication:

```python
web.server.debug = True
```

## When the Web Server Starts
The web server, if enabled, will start during sim initialization. When it does, it will look for the specified document root directory. By default that’s “www”. If root directory doesn’t exist, one will be created with a simple index.html file , a style sheet, and a couple of directories. 


## Connecting to Your Web Server
Assuming that you accepted the default port, connect to ```http://localhost:8888/``` (```https://localhost:8888/``` if ssl_enable=True) from your web browser. This will display the index.html file in your root directory.


## The Default Document Root Directory

The default document root directory that was initially created for you is minimal.

```
www/
    index.html
    style.css
    apps/
    images/
```

**index.html** is the file that’s displayed when you connect to http://localhost:8888/.

**style.css** is a CSS style-sheet that’s included by index.html to give it some pizzazz. 

The **apps** directory contains links to some example html/javascript applications
 in ```$TRICK_HOME/trick_source/web/apps```.

The **images** directory contains trick_icon.png.

**You are encouraged to add to, modify, and/or delete these files and directories to best suite the needs of your project.**

