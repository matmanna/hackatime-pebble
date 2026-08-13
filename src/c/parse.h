#pragma once
#include <pebble.h>
#include "config.h"

#define MAX_NAME_LENGTH 64

typedef struct {
  char name[MAX_NAME_LENGTH];
  int total_seconds;
  float percent;
} LanguageData;

typedef struct {
  char name[MAX_NAME_LENGTH];
  int total_seconds;
  float percent;
} ProjectData;

typedef struct {
  int total_seconds;
  int streak;
  int language_count;
  int project_count;
  LanguageData languages[MAX_LANGUAGES];
  ProjectData projects[MAX_PROJECTS];
} StatsData;

extern StatsData stats;

void parse_init(void);
bool parse_stats_from_js(DictionaryIterator *iter);
