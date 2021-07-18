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