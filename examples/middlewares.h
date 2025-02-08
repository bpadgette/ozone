#include "ozone.h"

void asHTMLDocument(OzoneAppEvent* event) {
  OzoneTemplatesComponent* html_shell;

  ozoneAppContextLock(event);
  ozoneAppContextCachedValue(event, OzoneTemplatesComponent, html_shell, {
    html_shell = ozoneTemplatesComponentFromFile(
        event->context->allocator, &ozoneStringConstant("./examples/html/shell.html"));
  });
  ozoneAppContextUnlock(event);

  OzoneStringMap arguments = (OzoneStringMap) { 0 };
  ozoneMapInsertOzoneString(
      event->allocator, &arguments, &ozoneStringConstant("title"), &ozoneStringConstant("Ozone Examples"));
  ozoneMapInsertOzoneString(event->allocator, &arguments, &ozoneStringConstant("body"), &event->response->body);
  ozoneTemplatesComponentRender(event->allocator, html_shell, &arguments);

  ozoneAppSetResponseHeader(event, &ozoneStringConstant("Content-Type"), &ozoneStringConstant("text/html"));
}
