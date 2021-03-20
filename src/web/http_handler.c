#include "http_handler.h"

#include <fs/fs.h>
#include <logging/log.h>
LOG_MODULE_DECLARE(webserver, LOG_LEVEL_DBG);

#include "mime.h"
#include "sdcard.h"

#define WEBSERVER_ROOT "web"
#define STREAM_BUFFER_SIZE 4096

#define TX_CHUNK_SIZE_BYTES CONFIG_NET_TX_STACK_SIZE

#define URL_MAIN "/$"
#define URL_INDEX_HTML "/index.html"

void register_handlers(struct mg_context *ctx);

int send_buffer_chunked(struct mg_connection *conn, const char *mime_type,
                             const char *path);

int redirect_to_index_html(struct mg_connection *conn, void *cbdata);
int file_handler(struct mg_connection *conn, void *cbdata);

void init_http_server_handlers(struct mg_context *ctx) {
  register_handlers(ctx);
}

void register_handlers(struct mg_context *ctx) {
  mg_set_request_handler(ctx, URL_MAIN, redirect_to_index_html, NULL);
  mg_set_request_handler(ctx, "/**", file_handler, NULL);
}

int file_handler(struct mg_connection *conn, void *cbdata) {
  const struct mg_request_info *request = mg_get_request_info(conn);
  const char* mime_type = get_mime_type(request->request_uri);
  LOG_DBG("Processing request for file: %s Mime Type: %s", request->request_uri, mime_type);
  return send_buffer_chunked(conn, mime_type, request->request_uri);
}

int send_buffer_chunked(struct mg_connection *conn, const char *mime_type,
                             const char *path) {
  int ret = 200;

  static char uri_path[4096];
  static struct fs_dirent entry;
  static struct fs_file_t zfp;
  strcpy(uri_path, get_disk_mount());
  strcat(uri_path, "/");
  strcat(uri_path, WEBSERVER_ROOT);
  strcat(uri_path, path);

  ret = fs_stat(uri_path, &entry);
  if (ret) {
    LOG_ERR("Unable to retrieve file information: %s. Reason: %d", uri_path,
            ret);
    return 404;
  }
  ret = fs_open(&zfp, uri_path, FS_O_READ);
  if (ret) {
    LOG_ERR("Failed to open file for read: %s. Reason: %d", uri_path, ret);
    return 500;
  }

  ret = mg_send_http_ok(conn, mime_type, -1);
  if (ret < 0) {
    LOG_ERR("Failed to send HTTP OK for file: %s. Reason: %d", uri_path, ret);
    return 500;
  }

  static char buffer[TX_CHUNK_SIZE_BYTES];
  long remaining = entry.size;

  while (remaining > 0) {
    ssize_t count =
        remaining > TX_CHUNK_SIZE_BYTES ? TX_CHUNK_SIZE_BYTES : remaining;
    LOG_DBG("Reading %d bytes from %s at position: %d.", count, uri_path, fs_tell(&zfp));
    ret = fs_read(&zfp, buffer, count);
    if (ret < 0) {
      LOG_ERR("Failed to read data from file: %s. Reason. %d", uri_path, ret);
      return 500;
    }
    LOG_DBG("Transferring:");
    LOG_DBG("ret: %d remaining: %ld chunk_size: %zd B", ret, remaining,
            count);
    ret = mg_send_chunk(conn, buffer, ret);
    if (ret > 0) {
      remaining -= count;
    } else if (ret == 0) {
      LOG_WRN("Unable to write %d bytes to connection which has already been "
              "closed.",
              count);
      return 500;
    } else {
      LOG_ERR("Error writing %d bytes to connection.", count);
      return 500;
    }

    LOG_DBG("Transferred:");
    LOG_DBG("ret: %d remaining: %ld chunk_size: %zd B", ret, remaining,
            count);
  }
  // Close the file
  fs_close(&zfp);

  /* Must be sent at the end of the chunked sequence */
  ret = mg_send_chunk(conn, "", 0);

  return ret;
}

int redirect_to_index_html(struct mg_connection *conn, void *cbdata) {
  mg_send_http_redirect(conn, URL_INDEX_HTML, 303);
  return 202;
}