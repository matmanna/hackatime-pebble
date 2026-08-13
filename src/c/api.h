#pragma once
#include <pebble.h>

typedef void (*StatsFetchCallback)(bool success);

void api_init(void);
void api_fetch_stats(StatsFetchCallback callback);
void api_deinit(void);
