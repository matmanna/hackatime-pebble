#pragma once
#include <pebble.h>

#define SETTINGS_KEY 1
#define API_BASE_URL "https://hackatime.hackclub.com/api/v1/users/"
#define REFRESH_INTERVAL_MS 600000  // 10 minutes
#define MAX_USERNAME_LENGTH 64
#define MAX_LANGUAGES 10
#define MAX_PROJECTS 10
#define DEFAULT_USERNAME "mat"

#define MSG_REQUEST_FETCH 10000
#define MSG_TOTAL_SECONDS 10001
#define MSG_STREAK 10002
#define MSG_LANGUAGES 10003
#define MSG_PROJECTS 10100
#define MSG_ERROR 10200

typedef struct {
  char username[MAX_USERNAME_LENGTH];
  bool show_languages;
  bool show_projects;
  int max_languages;
  int max_projects;
} AppSettings;

extern AppSettings settings;

void config_init(void);
void config_load(void);
void config_save(void);
void config_default(void);
