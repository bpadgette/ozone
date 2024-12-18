#include "ozone.h"

ozoneHTTPHandler(handler, { res->body = ozoneCharArray("Hello, world!"); });

int main() { return ozoneHTTPServe(8080, handler); }
