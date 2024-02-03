#define JSMN_HEADER
#include "snow_data_parser.h"

static jsmn_parser jsmnParser;

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) 
{
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

static int get_int_from_str_token(char *json, jsmntok_t tok) 
{
    int i = 0;
    char *value = json + tok.start;

    if (tok.type == JSMN_STRING) {
        char *restOfStr;
        i = strtol(value, &restOfStr, 10);
    }

    return i;
}

struct snowData parse_depth_and_fresh_from_JSON(char *json) 
{
    printf("snow data parser: received - \n\n%s\n\n", json);
    jsmntok_t jsmnTokens[32];
    jsmn_init(&jsmnParser);
    int r = jsmn_parse(&jsmnParser, json, strlen(json), jsmnTokens, sizeof(jsmnTokens) / sizeof(jsmnTokens[0]));

    struct snowData snowData;

    if (r < 0) 
    {
        printf("snow data parser: Failed to parse JSON: %d\n", r);
        snowData.depth = -1;
        snowData.fresh = -1;
        return snowData;
    }

    // Assume the top-level element is an object
    if (r < 1 || jsmnTokens[0].type != JSMN_OBJECT) 
    {
        printf("snow data parser: Failed to parse JSON: Object expected\n");
        snowData.depth = -1;
        snowData.fresh = -1;
        return snowData;
    }

    // Loop over all keys of the root object, collect depth and fresh
    for (int i = 1; i < r; i++) 
    {
        if (jsoneq(json, &jsmnTokens[i], "topSnowDepth") == 0) 
        {
            snowData.depth = get_int_from_str_token(json, jsmnTokens[i + 1]);
            i++;
        } 
        else if (jsoneq(json, &jsmnTokens[i], "freshSnowfall") == 0) 
        {
            snowData.fresh = get_int_from_str_token(json, jsmnTokens[i + 1]);
            i++;
        }
    }
    
    return snowData;
}