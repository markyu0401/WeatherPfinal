#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>

#define API_KEY "1b4a4b9200a67466d2f3dae2bbec327e"
#define API_URL "http://api.weatherstack.com/current?access_key=%s&query=%s"

struct string {
    char *ptr;
    size_t len;
};

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size * nmemb;
    char *new_ptr = realloc(s->ptr, new_len + 1);
    if (new_ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(new_ptr + s->len, ptr, size * nmemb);
    new_ptr[new_len] = '\0';
    s->ptr = new_ptr;
    s->len = new_len;

    return size * nmemb;
}

void cleanup(struct string *s, struct json_object *parsed_json) {
    free(s->ptr);
    json_object_put(parsed_json);
}

int main(void) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Curl initialization failed\n");
        return EXIT_FAILURE;
    }

    struct string s;
    init_string(&s);

    char city[100];
    printf("Enter the city: ");
    scanf("%99s", city);

    char url[256];
    snprintf(url, sizeof(url), API_URL, API_KEY, city);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        cleanup(&s, NULL);
        curl_easy_cleanup(curl);
        return EXIT_FAILURE;
    }

    struct json_object *parsed_json = json_tokener_parse(s.ptr);
    if (parsed_json == NULL) {
        fprintf(stderr, "JSON parsing failed\n");
        cleanup(&s, NULL);
        curl_easy_cleanup(curl);
        return EXIT_FAILURE;
    }

    struct json_object *current, *temperature;
    if (json_object_object_get_ex(parsed_json, "current", &current) &&
        json_object_object_get_ex(current, "temperature", &temperature)) {
        printf("Current Temperature: %d°C\n", json_object_get_int(temperature));
    } else {
        fprintf(stderr, "Error extracting temperature from JSON\n");
    }

    cleanup(&s, parsed_json);
    curl_easy_cleanup(curl);

    return EXIT_SUCCESS;
}