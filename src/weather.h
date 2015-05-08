#include <pebble.h>

typedef struct  {
  int conditions;
  time_t time;
  char summary[64];
  uint8_t temperature;
  uint8_t feelslike;
  uint8_t wind;
  uint16_t winddir;
} weather_info;