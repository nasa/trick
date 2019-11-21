# WS-API: VariableServer 

```ws://localhost:8888/api/ws/VariableServer```

## Purpose

JSON Variable Server

## Client to Server Command Messages

Add a Trick Variable to the current session.

```json
{ "cmd" : "var_add",
  "var_name" : string
}
```
Stop sending periodic ```var_list``` messages (*see below*) from the server.

```json
{ "cmd" : "var_pause" }
```

Resume sending periodic ```var_list``` response messages from the server.

```json
{ "cmd" : "var_unpause" }

```

Send one ```var_list``` message from the server.

```json
{ "cmd" : "var_send" }
```

Clear all variables from the current session, that is: undo all of the ```var_add``` commands.

```json
{ "cmd" : "var_clear" }
```

Disconnect from the variable server.

```json
{ "cmd" : "var_exit" }
```

Set the period (in milliseconds) at which ```var_list``` messages are sent form the server.

```json
{ "cmd" : "var_cycle",
  "period" : integer
}
```

Execute the given Python code in the host sim. 

```json
{ "cmd" : "python",
  "pycode" : string
}
```

Send the sie structure from the server. Response will be the ```sie``` response message (*below*).

```json
{ "cmd" : "sie" }
```

Send the units for the given variable. Response will be the ```units``` response message (*below*).

```json
{ "cmd" : "units",
  "var_name" : string
}
```

## Server to Client Response Messages

Error Response

```json
{ "msg_type" : "error",
  "error_text" : string
}
```

Periodic response containing the values of variables requested by ```var_add```. 

```json
{ "msg_type" : "var_list"
  "time" : double
  "values" : []
}
```

Response to the ```sie``` command (*above*).

```json
{ "msg_type" : "sie",
  "data" : string
}
```

Response to the ```units``` command (*above*).

```json
{ "msg_type" : "units",
  "var_name" : string,
  "data" : string
}
```


## Example Variable Server Client
```html
<!DOCTYPE html>
<html>
  <head>
    <title>WS Experiments</title>
  </head>
  <body>
      <style>
          table { border-collapse: collapse; width: 100%; }
          th, td { text-align: left; padding: 8px; }
          tr:nth-child(even){background-color: #f2f2f2}
          th { background-color: #562399; color: white; }
      </style>
      <header>
      </header>

      <div class="variableDisplay"></div>
      <table class="variables">
          <tr>
              <th>Variable</th>
              <th>Value</th>
          </tr>
      </table>

      <div id="output"></div>
      <script type="text/javascript">
          function log(s) {
              var p = document.createElement("p");
              p.style.wordWrap = "break-word";
              p.textContent = s;
              output.appendChild(p);
          }
          function sendMessage(msg) {
              ws.send(msg);
          }
          // Interface to Trick WebSocket Variable Server
          function setPeriod(period) {
              sendMessage(`{"cmd":"var_cycle","period":${period}}`);
          }
          function addVarTableRow(name, value) {
              // create a row in the table that contains two <td>s, one for the var_name and one for its value.
              let tr = document.createElement('tr');
              let td1 = document.createElement('td');
              td1.textContent = `${name}`;
              let td2 = document.createElement('td');
              td2.textContent = `${value}`;
              td2.className = "values";
              tr.appendChild(td1);
              tr.appendChild(td2);
              varTable.appendChild(tr);
          }
          function addVariable(name, value) {
              sendMessage(`{"cmd":"var_add","var_name": "${name}"}`);
              addVarTableRow(name, value);
          }
          var varTable = document.querySelector('table.variables');


          var ws = new WebSocket('ws://localhost:8888/api/ws/VariableServer');
          ws.onopen = function(e) {
              setPeriod(100);
              addVarTableRow("Time", 0.0);
              addVariable("dyn.cannon.pos[0]", 0.0);
              addVariable("dyn.cannon.pos[1]", 0.0);
              addVariable("dyn.cannon.vel[0]", 0.0);
              addVariable("dyn.cannon.vel[1]", 0.0);
              addVariable("dyn.cannon.time", 0.0);
              addVariable("dyn.cannon.timeRate", 0.0);
              addVariable("dyn.cannon.impact", 0.0);
              addVariable("I.dont.exist", 0.0);
              sendMessage("{\"cmd\":\"var_unpause\"}");
          };
          ws.onmessage = function(e) {
             let msg = JSON.parse(e.data);
             if (msg.msg_type == "values") {
                 let valueNodes = varTable.getElementsByClassName("values");
                 valueNodes[0].textContent = msg.time;
                 for (let i = 0; i < msg.values.length; i++ ) {
                     valueNodes[i+1].textContent = msg.values[i];
                 }
             }
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
