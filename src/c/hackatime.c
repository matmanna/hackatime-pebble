#include <pebble.h>
#include "config.h"
#include "api.h"
#include "parse.h"
#include "ui.h"

AppSettings settings;
static AppTimer *s_refresh_timer = NULL;

void config_init(void) {
  config_load();
}

void config_load(void) {
  config_default();
  if (persist_exists(SETTINGS_KEY)) {
    persist_read_data(SETTINGS_KEY, &settings, sizeof(AppSettings));
  }
}

void config_save(void) {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(AppSettings));
}

void config_default(void) {
  strncpy(settings.username, DEFAULT_USERNAME, MAX_USERNAME_LENGTH - 1);
  settings.show_languages = true;
  settings.show_projects = true;
  settings.max_languages = 5;
  settings.max_projects = 3;
}

static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *error_t = dict_find(iter, MSG_ERROR);
  if (error_t) {
    ui_show_error(error_t->value->cstring);
    return;
  }

  Tuple *total_t = dict_find(iter, MSG_TOTAL_SECONDS);
  if (total_t) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Got data, total=%d", (int)total_t->value->int32);
    parse_stats_from_js(iter);
    ui_update_stats();
    return;
  }
}

static void prv_stats_fetched(bool success) {
  if (!success) {
    ui_show_error("Failed to load");
  }
}

static void prv_refresh_timer_callback(void *data) {
  ui_show_loading();
  api_fetch_stats(prv_stats_fetched);
  s_refresh_timer = app_timer_register(REFRESH_INTERVAL_MS, prv_refresh_timer_callback, NULL);
}

static void prv_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  ui_scroll_up();
}

static void prv_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  ui_scroll_down();
}

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  ui_show_loading();
  api_fetch_stats(prv_stats_fetched);
}

static void prv_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, prv_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
}

static void init(void) {
  config_init();
  parse_init();
  api_init();
  ui_init();

  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  ui_set_click_config_provider(prv_click_config_provider);
  ui_push_window();
  ui_show_loading();
  api_fetch_stats(prv_stats_fetched);

  s_refresh_timer = app_timer_register(REFRESH_INTERVAL_MS, prv_refresh_timer_callback, NULL);
}

static void deinit(void) {
  if (s_refresh_timer) {
    app_timer_cancel(s_refresh_timer);
  }
  api_deinit();
  ui_deinit();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
