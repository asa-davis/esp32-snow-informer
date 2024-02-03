#ifndef SNOW_DATA_PARSER_H
#define SNOW_DATA_PARSER_H

#include <string.h>
#include <stdio.h>
#include "jsmn.h"
#include "snow_data.h"

struct snowData parse_depth_and_fresh_from_JSON(char *json);

#endif // SNOW_DATA_PARSER_H