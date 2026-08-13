#include "parse.h"
#include "config.h"

StatsData stats;

void parse_init(void) {
  memset(&stats, 0, sizeof(StatsData));
}

bool parse_stats_from_js(DictionaryIterator *iter) {
  memset(&stats, 0, sizeof(StatsData));

  Tuple *total_t = dict_find(iter, MSG_TOTAL_SECONDS);
  if (total_t) {
    stats.total_seconds = total_t->value->int32;
  }

  Tuple *streak_t = dict_find(iter, MSG_STREAK);
  if (streak_t) {
    stats.streak = streak_t->value->int32;
  }

  Tuple *lang_count_t = dict_find(iter, MSG_LANGUAGES);
  if (lang_count_t) {
    stats.language_count = lang_count_t->value->int32;
    if (stats.language_count > settings.max_languages) {
      stats.language_count = settings.max_languages;
    }
  }

  for (int i = 0; i < stats.language_count; i++) {
    Tuple *name_t = dict_find(iter, MSG_LANGUAGES + 1 + i * 3);
    Tuple *seconds_t = dict_find(iter, MSG_LANGUAGES + 2 + i * 3);
    Tuple *percent_t = dict_find(iter, MSG_LANGUAGES + 3 + i * 3);

    if (name_t) {
      strncpy(stats.languages[i].name, name_t->value->cstring, MAX_NAME_LENGTH - 1);
    }
    if (seconds_t) {
      stats.languages[i].total_seconds = seconds_t->value->int32;
    }
    if (percent_t) {
      stats.languages[i].percent = (float)percent_t->value->int32 / 100.0f;
    }
  }

  Tuple *proj_count_t = dict_find(iter, MSG_PROJECTS);
  if (proj_count_t) {
    stats.project_count = proj_count_t->value->int32;
    if (stats.project_count > settings.max_projects) {
      stats.project_count = settings.max_projects;
    }
  }

  for (int i = 0; i < stats.project_count; i++) {
    Tuple *name_t = dict_find(iter, MSG_PROJECTS + 1 + i * 3);
    Tuple *seconds_t = dict_find(iter, MSG_PROJECTS + 2 + i * 3);
    Tuple *percent_t = dict_find(iter, MSG_PROJECTS + 3 + i * 3);

    if (name_t) {
      strncpy(stats.projects[i].name, name_t->value->cstring, MAX_NAME_LENGTH - 1);
    }
    if (seconds_t) {
      stats.projects[i].total_seconds = seconds_t->value->int32;
    }
    if (percent_t) {
      stats.projects[i].percent = (float)percent_t->value->int32 / 100.0f;
    }
  }

  return true;
}
