## Extending the HTTP-API

The HTTP-API is implemented as a collection of ```httpMethodHandlers```. An ```httpMethodHandler``` is a pointer to a function that is expected to respond to an HTTP GET request, using the **CivetWeb** framework. An ```httpMethodHandler``` is defined (in ```trick/CivetWeb.hh```) as follows:





```c
typedef void (*httpMethodHandler)(struct mg_connection *, void* cbdata);
```

Documentation for the **CivetWeb Networking Library** can be found at:
[https://cesanta.com/docs/overview/intro.html](http://civetweb.github.io/civetweb/)

## Example HTTP-API Extension

Suppose you want your web server to send you a JSON message: 

```json
{ "greeting" : "Hello Trick Sim Developer!" }
```

when you invoke the URL: ```http://localhost:8888/api/http/hello```.

### Creating an ```httpMethodHandler```.

The following two files will be our implementation of an ```httpMethodHandler```. We'll put these in some models directory  ```httpMethods/```.

**```handle_HTTP_GET_hello.h```**

```c
#ifndef HANDLE_HTTP_GET_HELLO
#define HANDLE_HTTP_GET_HELLO

#ifndef SWIG
void handle_HTTP_GET_hello(struct mg_connection *nc, void *hm);
#endif

#endif
```

**```handle_HTTP_GET_hello.c```**

```c
#include "civet/CivetServer.h"
#include "civet/civetweb.h"
#include <string.h>

void handle_HTTP_GET_hello(struct mg_connection *nc, void *hm) {
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    const char* json_text =
        "{ \"greeting\" : \"Hello Trick Sim Developer!\" }";
    mg_send_chunk(nc, json_text, strlen(json_text));
    mg_send_chunk(nc, "", 0);
}
```

### Installing our ```httpMethodHandler```.

We'll do this from our **S_define** file:

* Add  ```(httpMethods/handle_HTTP_GET_hello.c)``` to the ```LIBRARY DEPENDENCIES```.

* Include our header file:

   ```##include "httpMethods/handle_HTTP_GET_hello.h"```
   
* In ```create_connections()``` add :

```c
web.server.installHTTPGEThandler( "hello", &handle_HTTP_GET_hello );
```
### A Complete S_define

```c++
/***********************TRICK HEADER*************************
PURPOSE:
    (Cannon Numeric)
LIBRARY DEPENDENCIES:
    (
     (cannon/gravity/src/cannon_init.c)
     (cannon/gravity/src/cannon_numeric.c)
     (httpMethods/handle_HTTP_GET_hello.c)
    )
*************************************************************/

#include "sim_objects/default_trick_sys.sm"
#include "sim_objects/CivetServer.sm"
##include "cannon/gravity/include/cannon_numeric.h"
##include "httpMethods/handle_HTTP_GET_hello.h"

class CannonSimObject : public Trick::SimObject {

    public:
        CANNON      cannon ;
        int foo;
        CannonSimObject() {
            ("default_data") cannon_default_data( &cannon ) ;
            ("initialization") cannon_init( &cannon ) ;
            ("derivative") cannon_deriv( &cannon ) ;
            ("integration") trick_ret = cannon_integ( &cannon ) ;
            ("dynamic_event") cannon_impact( &cannon) ;
        }
} ;
CannonSimObject dyn ;

IntegLoop dyn_integloop (0.01) dyn;

void create_connections() {
    dyn_integloop.getIntegrator(Runge_Kutta_4, 5);
    web.server.installHTTPGEThandler( "hello", &handle_HTTP_GET_hello );
}

```
