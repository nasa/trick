#include "civet/CivetServer.h"

void handle_HTTP_GET_hello(struct mg_connection *nc, struct http_message *hm) {
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    const char* json_text =
        "{ \"greeting\" : \"Hello Trick Sim Developer!\" }";
    mg_printf_http_chunk(nc, "%s", json_text);
    mg_send_http_chunk(nc, "", 0);
}