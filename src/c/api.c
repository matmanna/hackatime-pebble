#include "api.h"
#include "config.h"

static StatsFetchCallback s_fetch_callback = NULL;

static void prv_outbox_sent_handler(DictionaryIterator *iter, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Request sent to JS");
}

static void prv_outbox_failed_handler(DictionaryIterator *iter, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to send request: %d", reason);
  if (s_fetch_callback) {
    s_fetch_callback(false);
  }
}

void api_init(void) {
  app_message_register_outbox_sent(prv_outbox_sent_handler);
  app_message_register_outbox_failed(prv_outbox_failed_handler);
}

void api_fetch_stats(StatsFetchCallback callback) {
  s_fetch_callback = callback;

  DictionaryIterator *out_iter;
  AppMessageResult result = app_message_outbox_begin(&out_iter);
  if (result != APP_MSG_OK) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to begin outbox: %d", result);
    if (s_fetch_callback) {
      s_fetch_callback(false);
    }
    return;
  }

  dict_write_int32(out_iter, MSG_REQUEST_FETCH, 1);
  app_message_outbox_send();
}

void api_deinit(void) {
}
