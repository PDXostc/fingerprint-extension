#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>
#include <libfprint/fprint.h>
#include <jansson.h>
#include "XW_Extension.h"
#include "uthash.h"
#include "fingerprint.h"

typedef void (*api_func)(json_t *);

typedef struct {
  char *message;
  api_func func;
  UT_hash_handle hh;
} api_t;

static api_t *interface = NULL;
static XW_Extension g_extension = 0;
static const XW_CoreInterface *g_core = 0;
static const XW_MessagingInterface *g_messaging = 0;

static void log(int type, const char *format, ...) {
  va_list arglist;

  va_start(arglist, format);
  vsyslog(LOG_MAKEPRI(LOG_USER, type), format, arglist);
  va_end(arglist);
}

static void add_to_api(const char *message, api_func func) {
  api_t *method;
  size_t message_len;

  method = malloc(sizeof(api_t));
  method->message = strdup(message);
  method->func = func;

  message_len = strlen(method->message);

  HASH_ADD_KEYPTR(hh, interface, method->message, message_len, method);
}

static json_t *dispatch_message(api_t *interface, const char *message) {
  api_t *match;
  json_t *data;

  data = json_object();

  HASH_FIND_STR(interface, message, match);
  if (match) {
    match->func(data);
  } else {
    log(LOG_ERR, "dispatch error: no message '%s'", message);
  }

  return data;
}

static void scanFinger(json_t *response) {
  log(LOG_DEBUG, "entering scanFinger");
  json_object_set_new(response, "result", json_string("success"));
}

static void instance_created(XW_Instance instance) {
  log(LOG_DEBUG, "instance %d created", instance);
}

static void instance_destroyed(XW_Instance instance) {
  log(LOG_DEBUG, "instance %d destroyed", instance);
}

static void handle_message(XW_Instance instance, const char *json) {
  json_t *request, *response, *data;
  json_error_t error;
  char *dump, *message;
  int id;
  api_t *i, *tmp;

  response = json_object();
  json_object_set_new(response, "err", json_null());
  json_object_set_new(response, "id", json_null());

  request = json_loads(json, 0, &error);
  if (!request) {
    log(LOG_ERR, "JSON error on line %d: %s", error.line, error.text);
    json_object_set_new(response, "err", json_string("JSON parsing error"));
    goto done;
  }

  dump = json_dumps(request, JSON_INDENT(2));
  log(LOG_DEBUG, "JSON request: %s", dump);
  free(dump);

  json_unpack(request, "{siss}", "id", &id, "message", &message);
  json_object_set_new(response, "id", json_integer(id));

  json_object_set_new(response, "response",
                      dispatch_message(interface, message));

 done:
  dump = json_dumps(response, JSON_INDENT(2));
  log(LOG_DEBUG, "JSON response: %s", dump);
  g_messaging->PostMessage(instance, dump);
  free(dump);
}

static void shutdown(XW_Extension extension) {
  fp_exit();
  log(LOG_DEBUG, "shutting down");
  closelog();
}

int32_t XW_Initialize(XW_Extension extension, XW_GetInterface get_interface) {
  struct fp_dscv_dev **device_list;
  struct fp_dscv_dev *device;
  struct fp_driver *driver;
  char kSource_fingerprint_api[src_fingerprint_js_len+1];

  openlog("fingerprint", LOG_CONS, LOG_USER);
  log(LOG_DEBUG, "initialized");

  fp_init();

  device_list = fp_discover_devs();
  device = device_list[0];
  if (device) {
    driver = fp_dscv_dev_get_driver(device);
    log(LOG_DEBUG, "found device %s", fp_driver_get_full_name(driver));
  }
  fp_dscv_devs_free(device_list);

  memcpy(kSource_fingerprint_api, src_fingerprint_js, src_fingerprint_js_len);
  kSource_fingerprint_api[src_fingerprint_js_len] = '\0';

  add_to_api("scanFinger", scanFinger);

  g_extension = extension;
  g_core = get_interface(XW_CORE_INTERFACE);
  g_core->SetExtensionName(extension, PACKAGE);
  g_core->SetJavaScriptAPI(extension, kSource_fingerprint_api);
  g_core->
    RegisterInstanceCallbacks(extension, instance_created, instance_destroyed);
  g_core->RegisterShutdownCallback(extension, shutdown);
  g_messaging = get_interface(XW_MESSAGING_INTERFACE);
  g_messaging->Register(extension, handle_message);

  return XW_OK;
}
