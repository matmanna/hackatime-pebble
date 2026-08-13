#pragma once
#include <pebble.h>

void ui_init(void);
void ui_push_window(void);
void ui_set_click_config_provider(ClickConfigProvider provider);
void ui_update_stats(void);
void ui_show_loading(void);
void ui_show_error(const char *message);
void ui_scroll_up(void);
void ui_scroll_down(void);
void ui_deinit(void);
