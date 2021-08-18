## Extending the WebSocket-API

## When You Create a WebSocket Connection

Consider the following Javascript, that creates a web socket connection:

 ```var ws = new WebSocket('ws://localhost:8888/api/ws/VariableServer');```

In the URL: ```ws://localhost:8888/api/ws/VariableServer```

* ```ws://``` specifies the **protocol** (web-socket in this case.)
* ```localhost``` specifies the **domain**,
* ```:8888``` specifies the **port**, and
* ```/api/ws/VariableServer``` specifies the **path**.

In the Trick web server, the path associated with a websocket must begin with
```/api/ws/```. The remaining part of the path, i.e., ```VariableServer``` is the **key** that specifies the **sub-protocol**, prescribing what messages will be passed between client and server, and what those messages mean.

When a web-socket connection is established, the **key** will determine what type (sub-class) of ```WebSocketSession``` object to create, to manage the connection.

## WebSocketSession
A ```WebSocketSession``` is a pure virtual base class meant to represent the state of one of potentially many websocket connections. It provides methods to:

 1. Synchronously marshall Trick simulation data for out-going messages
 2. Send messages to the websocket client, and
 3. Receive and process messages from the websocket client.

To implement a new websocket sub-protocol, one needs to derive a new class from this base class, and implement the required methods. ```WebSocketSession.hh``` can be found in ```${TRICK_HOME}/include/trick/```.

### WebSocketSession.hh
```c
/*************************************************************************
PURPOSE: (Represent Websocket connection.)
**************************************************************************/
#ifndef WEB_SOCKET_SESSION_HH
#define WEB_SOCKET_SESSION_HH

#include <string>
#ifndef SWIG
#include "civet/CivetServer.h"
#endif

class WebSocketSession {
    public:
        WebSocketSession(struct mg_connection *nc):connection(nc){};
        virtual ~WebSocketSession() {};

        /**
           When HTTP_Server::time_homogeneous is set, WebSocketSession::marshallData() is called from the main
           sim thread in a "top_of_frame" job, so that all of the data can be staged at
           the same sim-time, in other words it's time-homogeneous.
        */
        virtual void marshallData()=0;
        virtual void sendMessage()=0;
        virtual int  handleMessage(std::string)=0;

        struct mg_connection* connection;
};
#endif
```

### Adding Your New WebSocketSession Type to the WebServer

To install your new websocket protocol, you'll need to create a function that
creates an instance of your new WebSocketSession type. Then you'll need to call 
```HTTP_Server::installWebSocketSessionMaker``` to install the function, with a
label.

The function you'll create will take ```struct mg_connection *``` as an argument
and return ```WebSocketSession*```.

## Example

Let's create a new web socket protocol that sends the time in GMT or local time.

First we'll derive a new type called ```TimeSession ``` from ```WebSocketSession```.

### TimeSession.hh

```c
/*************************************************************************
PURPOSE: (Represent the state of a variable server websocket connection.)
**************************************************************************/
#ifndef TIMESESSION_HH
#define TIMESESSION_HH
#include <vector>
#include <string>
#include "time.h"
#include "trick/WebSocketSession.hh"

class TimeSession : public WebSocketSession {
    public:
        enum Zone { GMT, LOCAL};
        TimeSession(struct mg_connection *nc);
        ~TimeSession();
        void marshallData();
        void sendMessage();
        int  handleMessage(std::string);
    private:
        time_t now;
        Zone zone;
};

WebSocketSession* makeTimeSession( struct mg_connection *nc );
#endif
```

Below is our implementation. Notice the function ```makeTimeSession``` at the bottom.


### TimeSession.cpp

```c
#include <stdio.h>
#include <time.h>
#include <iostream>
#include "TimeSession.hh"
#include <cstring>

// CONSTRUCTOR
TimeSession::TimeSession( struct mg_connection *nc ) : WebSocketSession(nc) {
    time(&now);
}

// DESTRUCTOR
TimeSession::~TimeSession() {}

void TimeSession::marshallData() {
    time(&now);
}

void TimeSession::sendMessage() {

    char message[1024];
    struct tm *theTime;
    if (zone == TimeSession::LOCAL) {
        theTime = localtime(&now);
    } else {
        theTime = gmtime(&now);
    }
    int hours   = theTime->tm_hour;
    int minutes = theTime->tm_min;
    int seconds = theTime->tm_sec;
    int day     = theTime->tm_mday;
    int month   = theTime->tm_mon + 1;
    int year    = theTime->tm_year + 1900;

    sprintf(message, "Time: %02d:%02d:%02d Date: %02d/%02d/%d\n", hours, minutes, seconds, month, day, year);
    mg_websocket_write(connection, MG_WEBSOCKET_OPCODE_TEXT, message, strlen(message));
}

int TimeSession::handleMessage(std::string client_msg) {

   if (client_msg.compare("GMT") == 0) {
       zone = TimeSession::GMT;
   } else if (client_msg.compare("LOCAL") == 0) {
       zone = TimeSession::LOCAL;
   } else {
      std::cerr << "ERROR: Unknown command \"" << client_msg << "\"." << std::endl;
   }
   return 0;
}

// WebSocketSessionMaker function for a TimeSession.
WebSocketSession* makeTimeSession( struct mg_connection *nc ) {
    std::cerr << "DEBUG: Creating new TimeSession." << std::endl;
    return new TimeSession(nc);
}
```

We put ```TimeSession.cpp``` and ```TimeSession.cpp``` into a models directory called ```httpMethods/```.


### S_define Modifications

1. Specify the dependency on the ```httpMethods/TimeSession.cpp``` compilation unit.
2. We should already be including the WebServer sim object, otherwise we don't even have a webserver.
3. We need to include our new header file: ```##include "httpMethods/TimeSession.hh"```
4. Finally, install our WebSocketSession type: ```web.server.installWebSocketSessionMaker("Time", &makeTimeSession);```
The label we use for our protocol here is "Time", but it can be whatever name you choose.

```c++
/***********************TRICK HEADER*************************
PURPOSE:
    (Cannon Numeric)
LIBRARY DEPENDENCIES:
    (
     (cannon/gravity/src/cannon_init.c)
     (cannon/gravity/src/cannon_numeric.c)
     (httpMethods/handle_HTTP_GET_hello.c)
     (httpMethods/TimeSession.cpp)
    )
*************************************************************/

#include "sim_objects/default_trick_sys.sm"
#include "sim_objects/CivetServer.sm"
##include "cannon/gravity/include/cannon_numeric.h"
##include "httpMethods/TimeSession.hh"

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
    web.server.installWebSocketSessionMaker( "Time", &makeTimeSession );
}
```


## Testing The New WebSocket Interface

To test your new web socket interface, put the following ```time.html``` file in  ```$YOUR_SIM_DIRECTORY/www/apps```. Then request ```http://localhost:8888/apps/time.html``` from your browser. You should see the time messages from your sim.

### time.html

```html
<!DOCTYPE html>
<html>
  <head>
    <title>WS Example</title>
  </head>
  <body>
      <div id="output"></div>
      <script type="text/javascript">

          function log(s) {
              var p = document.createElement("p");
              p.style.wordWrap = "break-word";
              p.textContent = s;
              output.appendChild(p);
          }

          var ws = new WebSocket('ws://localhost:8888/api/ws/Time');

          // WebSocket Event Handlers
          ws.onopen = function(e) {
              ws.send("GMT");
          };
          ws.onmessage = function(e) {
             log(e.data);
          };
          ws.onerror = function(e) {
             console.log("WebSocket Error: " , e);
             handleErrors(e);
          };
          ws.onclose = function(e) {
             console.log("Connection closed", e);
          };

      </script>
  </body>
</html>
```




