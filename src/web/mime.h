#ifndef TEKDAQC_ZEPHYR_MIME_H
#define TEKDAQC_ZEPHYR_MIME_H

#define HTTP_TEXT_HTML "text/html"
#define HTTP_TEXT_CSS "text/css"
#define HTTP_TEXT_JS "text/javascript"
#define HTTP_APPLICATION_JSON "application/json"
#define HTTP_APPLICATION_OCTET "application/octet-stream"

const char* get_mime_type(const char* file_name) {
  char *dot = strrchr(file_name, '.');
  if (dot) {
    if (!strcmp(dot, ".html")) {
      return HTTP_TEXT_HTML;
    } else if (!strcmp(dot, ".css")) {
      return HTTP_TEXT_CSS;
    } else if (!strcmp(dot, ".js")) {
      return HTTP_TEXT_JS;
    } else if (!strcmp(dot, ".json")) {
      return HTTP_APPLICATION_JSON;
    } else {
      LOG_WRN("Unknown mime type for file name: %s", file_name);
      return HTTP_APPLICATION_OCTET;
    }
  }
}

#endif // TEKDAQC_ZEPHYR_MIME_H
