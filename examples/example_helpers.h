#include "ozone.h"

void asHTMLDocument(OzoneAppEvent* event) {
  OzoneTemplate* html_shell;

  ozoneAppContextLock(event);
  ozoneAppContextCacheGetOrCreate(event, OzoneTemplate, "middlewares:html_shell", html_shell, {
    html_shell = ozoneTemplateFromFile(event->context->allocator, &ozoneString("./examples/html/shell.html"));
  });
  ozoneAppContextUnlock(event);

  OzoneStringMap arguments = (OzoneStringMap) { 0 };
  OzoneStringMapInsert(event->allocator, &arguments, &ozoneString("title"), &ozoneString("Ozone Examples"));
  OzoneStringMapInsert(event->allocator, &arguments, &ozoneString("body"), &event->response->body);

  OzoneString* new_body = ozoneStringAllocate(event->allocator, "");
  ozoneTemplateWrite(event->allocator, new_body, html_shell, &arguments);
  event->response->body = *new_body;

  OzoneStringMapInsert(
      event->allocator, &event->response->headers, &ozoneString("Content-Type"), &ozoneString("text/html"));
}
