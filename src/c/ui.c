#include "ui.h"
#include "config.h"
#include "parse.h"

static Window *s_window;
static TextLayer *s_rows[30];
static TextLayer *s_pct_layers[MAX_LANGUAGES];
static Layer *s_bars[MAX_LANGUAGES];
static int s_row_count = 0;
static int s_scroll_offset = 0;
static int s_total_height = 0;
static int s_row_y[30];
static int s_lang_y[MAX_LANGUAGES];
static char s_row_texts[30][40];
static char s_pct_texts[MAX_LANGUAGES][8];
#define SCREEN_H 228

static void prv_int_to_str(int num, char *buf, int buf_size) {
  char tmp[12];
  int i = 0;
  unsigned int unum;
  if (num < 0) { unum = (unsigned int)(-num); } else { unum = (unsigned int)num; }
  if (unum == 0) { tmp[i++] = '0'; }
  else { while (unum > 0 && i < 11) { tmp[i++] = '0' + (char)(unum % 10); unum /= 10; } }
  int j = 0;
  while (i > 0 && j < buf_size - 1) { buf[j++] = tmp[--i]; }
  buf[j] = '\0';
}

static void prv_format_time(int total_seconds, char *buf, int buf_size) {
  int h = total_seconds / 3600;
  int m = (total_seconds % 3600) / 60;
  char hs[8], ms[8];
  prv_int_to_str(h, hs, 8);
  prv_int_to_str(m, ms, 8);
  int p = 0, k = 0;
  while (hs[k] && p < buf_size - 1) buf[p++] = hs[k++];
  if (p < buf_size - 1) buf[p++] = 'h';
  if (p < buf_size - 1) buf[p++] = ' ';
  k = 0;
  while (ms[k] && p < buf_size - 1) buf[p++] = ms[k++];
  if (p < buf_size - 1) buf[p++] = 'm';
  buf[p] = '\0';
}

static void prv_format_streak(int days, char *buf, int buf_size) {
  char ds[8];
  prv_int_to_str(days, ds, 8);
  int p = 0, k = 0;
  while (ds[k] && p < buf_size - 1) buf[p++] = ds[k++];
  const char *suf = " days";
  k = 0;
  while (suf[k] && p < buf_size - 1) buf[p++] = suf[k++];
  buf[p] = '\0';
}

static void prv_format_pct(int pct, char *buf, int buf_size) {
  char ps[8];
  prv_int_to_str(pct, ps, 8);
  int p = 0, k = 0;
  while (ps[k] && p < buf_size - 1) buf[p++] = ps[k++];
  if (p < buf_size - 1) buf[p++] = '%';
  buf[p] = '\0';
}

static void prv_draw_bar(Layer *layer, GContext *ctx) {
  for (int i = 0; i < stats.language_count; i++) {
    if (layer == s_bars[i]) {
      graphics_context_set_fill_color(ctx, GColorDarkGray);
      graphics_fill_rect(ctx, GRect(0, 0, 65, 8), 0, GCornerNone);
      int w = (int)(stats.languages[i].percent / 100.0f * 65);
      graphics_context_set_fill_color(ctx, GColorChromeYellow);
      graphics_fill_rect(ctx, GRect(0, 0, w, 8), 0, GCornerNone);
      break;
    }
  }
}

static TextLayer *prv_add_row(int x, int y, int w, int h, GColor color, GFont font) {
  TextLayer *tl = text_layer_create(GRect(x, y, w, h));
  text_layer_set_text_color(tl, color);
  text_layer_set_background_color(tl, GColorClear);
  text_layer_set_font(tl, font);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(tl));
  s_rows[s_row_count] = tl;
  s_row_y[s_row_count] = y;
  s_row_texts[s_row_count][0] = '\0';
  s_row_count++;
  return tl;
}

static void prv_set_row_text(int idx, const char *text) {
  int i = 0;
  while (text[i] && i < 39) {
    s_row_texts[idx][i] = text[i];
    i++;
  }
  s_row_texts[idx][i] = '\0';
  text_layer_set_text(s_rows[idx], s_row_texts[idx]);
}

static void prv_create_ui(void) {
  s_row_count = 0;
  int y = 6;

  prv_add_row(10, y, 180, 30, GColorChromeYellow,
              fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  prv_set_row_text(0, "HACKATIME");
  y += 44;

  prv_add_row(10, y, 180, 24, GColorChromeYellow,
              fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  y += 30;

  prv_add_row(10, y, 180, 24, GColorWhite,
              fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  y += 30;

  prv_add_row(10, y, 180, 24, GColorWhite,
              fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  prv_set_row_text(3, "<> LANGUAGES");
  y += 32;

  for (int i = 0; i < settings.max_languages; i++) {
    s_lang_y[i] = y;
    prv_add_row(10, y, 80, 20, GColorLightGray,
                fonts_get_system_font(FONT_KEY_GOTHIC_18));

    s_pct_layers[i] = text_layer_create(GRect(155, y - 4, 30, 20));
    text_layer_set_text_color(s_pct_layers[i], GColorLightGray);
    text_layer_set_background_color(s_pct_layers[i], GColorClear);
    text_layer_set_font(s_pct_layers[i], fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_text_alignment(s_pct_layers[i], GTextAlignmentRight);
    layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_pct_layers[i]));
    s_pct_texts[i][0] = '\0';

    s_bars[i] = layer_create(GRect(85, y + 6, 65, 8));
    layer_set_update_proc(s_bars[i], prv_draw_bar);
    layer_add_child(window_get_root_layer(s_window), s_bars[i]);

    y += 24;
  }
  y += 10;

  prv_add_row(10, y, 180, 24, GColorWhite,
              fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  prv_set_row_text(s_row_count - 1, "[] PROJECTS");
  y += 32;

  for (int i = 0; i < settings.max_projects; i++) {
    prv_add_row(10, y, 180, 20, GColorLightGray,
                fonts_get_system_font(FONT_KEY_GOTHIC_18));
    y += 24;
  }

  s_total_height = y + 10;
}

static void prv_update_layout(void) {
  for (int i = 0; i < s_row_count; i++) {
    GRect f = layer_get_frame(text_layer_get_layer(s_rows[i]));
    f.origin.y = s_row_y[i] - s_scroll_offset;
    layer_set_frame(text_layer_get_layer(s_rows[i]), f);
  }

  for (int i = 0; i < settings.max_languages; i++) {
    GRect f = layer_get_frame(s_bars[i]);
    f.origin.y = s_lang_y[i] - s_scroll_offset;
    layer_set_frame(s_bars[i], f);

    GRect pf = layer_get_frame(text_layer_get_layer(s_pct_layers[i]));
    pf.origin.y = s_lang_y[i] - s_scroll_offset;
    layer_set_frame(text_layer_get_layer(s_pct_layers[i]), pf);
  }

  layer_mark_dirty(window_get_root_layer(s_window));
}

void ui_init(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  prv_create_ui();
}

void ui_push_window(void) {
  window_stack_push(s_window, true);
}

void ui_set_click_config_provider(ClickConfigProvider provider) {
  window_set_click_config_provider(s_window, provider);
}

void ui_update_stats(void) {
  char buf[32];
  char row_buf[40];

  prv_format_time(stats.total_seconds, buf, 32);
  int p = 0, k = 0;
  const char *today = "* TODAY  ";
  while (today[k] && p < 39) { row_buf[p++] = today[k++]; }
  k = 0;
  while (buf[k] && p < 39) { row_buf[p++] = buf[k++]; }
  row_buf[p] = '\0';
  prv_set_row_text(1, row_buf);

  prv_format_streak(stats.streak, buf, 32);
  p = 0; k = 0;
  const char *streak = "* STREAK  ";
  while (streak[k] && p < 39) { row_buf[p++] = streak[k++]; }
  k = 0;
  while (buf[k] && p < 39) { row_buf[p++] = buf[k++]; }
  row_buf[p] = '\0';
  prv_set_row_text(2, row_buf);

  int lang_idx = 4;
  for (int i = 0; i < settings.max_languages; i++) {
    if (i < stats.language_count) {
      prv_set_row_text(lang_idx, stats.languages[i].name);

      int pct = (int)(stats.languages[i].percent + 0.5f);
      prv_format_pct(pct, s_pct_texts[i], 8);
      text_layer_set_text(s_pct_layers[i], s_pct_texts[i]);

      layer_mark_dirty(s_bars[i]);
    } else {
      prv_set_row_text(lang_idx, "");
      text_layer_set_text(s_pct_layers[i], "");
    }
    lang_idx++;
  }

  int proj_idx = lang_idx + 1;
  for (int i = 0; i < settings.max_projects; i++) {
    if (i < stats.project_count) {
      p = 0; k = 0;
      while (stats.projects[i].name[k] && p < 39) { row_buf[p++] = stats.projects[i].name[k++]; }
      while (p < 22) { row_buf[p++] = ' '; }
      prv_format_time(stats.projects[i].total_seconds, buf, 32);
      k = 0;
      while (buf[k] && p < 39) { row_buf[p++] = buf[k++]; }
      row_buf[p] = '\0';
      prv_set_row_text(proj_idx, row_buf);
    } else {
      prv_set_row_text(proj_idx, "");
    }
    proj_idx++;
  }

  prv_update_layout();
}

void ui_show_loading(void) {
  prv_set_row_text(1, "* TODAY  ...");
  prv_set_row_text(2, "* STREAK  ...");
}

void ui_show_error(const char *message) {
  prv_set_row_text(1, "* TODAY  ---");
  prv_set_row_text(2, message);
}

void ui_scroll_up(void) {
  if (s_scroll_offset > 0) {
    s_scroll_offset -= 40;
    if (s_scroll_offset < 0) s_scroll_offset = 0;
    prv_update_layout();
  }
}

void ui_scroll_down(void) {
  int max_scroll = s_total_height - SCREEN_H;
  if (max_scroll < 0) max_scroll = 0;
  if (s_scroll_offset < max_scroll) {
    s_scroll_offset += 40;
    if (s_scroll_offset > max_scroll) s_scroll_offset = max_scroll;
    prv_update_layout();
  }
}

void ui_deinit(void) {
  for (int i = 0; i < s_row_count; i++) {
    text_layer_destroy(s_rows[i]);
  }
  for (int i = 0; i < settings.max_languages; i++) {
    text_layer_destroy(s_pct_layers[i]);
    layer_destroy(s_bars[i]);
  }
  window_destroy(s_window);
}
