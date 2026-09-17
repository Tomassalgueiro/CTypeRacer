#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <time.h>

#include "stats.h"

struct timespec start, end;
struct termios orig_termios;

struct quote {
	int id;
	char* text;
	char* author;
}; 

/* 
 * ptr <- data curl received
 * size <- size of each item 
 * nmemb <- number of items 
 * usedata <- your own data
 */
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata){
	char **response = (char **)userdata;
	size_t bytes = size * nmemb;
	size_t curr_size = (*response) ? strlen(*response) : 0;

	char *temp = realloc(*response, curr_size + bytes + 1);
	if (!temp) return 0; // no memory avail

	*response = temp;
	memcpy(*response + curr_size, ptr, bytes);
	(*response)[curr_size + bytes] = '\0';

	return bytes;
}


char* get_quote(){
	CURL *curl;
	CURLcode res;
	char *response = malloc(1);
	response[0] = '\0';
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (curl){
		curl_easy_setopt(curl, CURLOPT_URL, "https://dummyjson.com/quotes/random");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		res = curl_easy_perform(curl);
		if (res!=CURLE_OK){
			fprintf(stderr,
				"curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		}
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return response;
}

int parse_json(char *message, struct quote* level){

	if(!message) return 0;
	cJSON *json = cJSON_Parse(message);
	if(!json) return 0;

	cJSON *id = cJSON_GetObjectItem(json, "id");
	cJSON *quote = cJSON_GetObjectItem(json, "quote");
	cJSON *author = cJSON_GetObjectItem(json, "author");

	if(id && quote && author && cJSON_IsString(quote)){
		level->id = id->valueint;
		level->text = strdup(quote->valuestring);
		level->author = strdup(author->valuestring);
		cJSON_Delete(json);
		return 1;
	}

	cJSON_Delete(json);
	return 0;

}

void disable_raw_mode(){
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
	printf("\033[?25h\033[0m\n");
}

void enable_raw_mode(){
	tcgetattr(STDIN_FILENO, &orig_termios);
	struct termios raw = orig_termios;
	raw.c_lflag &= ~ (ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void clean_screen(){
	printf("\033[H\033[J");
}

void render(const char *target, const char *typed, int len){
	printf("\r\033[K");

	for(int i = 0; target[i] != '\0'; i++){
		if (i < len){
			if(typed[i] == target[i]){
				printf("\033[32m%c\033[0m", target[i]);
			}
			else {
				printf("\033[31;4m%c\033[0m", target[i]);
			}
		}
		else {
			printf("\033[2m%c\033[0m", target[i]);
		}
	}
	printf("\033[%dG", len+1);
	fflush(stdout);
}

void tr_game_loop(){

	// change for web request
	char *raw_json = get_quote();
	struct quote level = {0};

	if (!parse_json(raw_json, &level)){
		free(raw_json);
		fprintf(stderr, "Failed to load quote!\n");
	}
	free(raw_json);

	char* target = level.text; 
	int target_len = strlen(target);
	char typed[1024] = {0};
	int current_idx = 0;
	

	enable_raw_mode();
	render(target, typed, current_idx);
	clock_gettime(CLOCK_MONOTONIC, &start);
	printf("started timer");
	while (current_idx < target_len){
		char c;
		if (read(STDIN_FILENO, &c, 1) <= 0) continue;

		if (c==3) break;
		if (c == 127 || c == 8) {
			if (current_idx > 0){
				current_idx--;
				typed[current_idx] = '\0';
			}

		}
		else if (c >= 32 && c <= 126 && current_idx < (int)sizeof(typed) - 1){
			typed[current_idx] = c;
			current_idx++;
		}
		render(target, typed, current_idx);
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	disable_raw_mode();
	free(level.author);
	free(level.text);
}

void tr_main_menu() {
    enable_raw_mode();
    
    while (1) {
        clean_screen();
        printf("=== Welcome to CTypeRacer ===\n\n");
        printf(" [1] Start Game\n");
        printf(" [2] Stats\n");
        printf(" [3] Exit\n\n");
        printf("Select an option: ");
        fflush(stdout);

        char c;
        if (read(STDIN_FILENO, &c, 1) <= 0) continue;

        if ( c == '3') {
            clean_screen();
            printf("Thanks for playing!\n");
            break;
        }

	// go to stats screen wip
	if (c == '2') {
            clean_screen();
            printf("Stats screen WIP\n");
            break;
        }

        if (c == '1') {
            clean_screen();
	    disable_raw_mode();
            tr_game_loop();
            
            enable_raw_mode();
            printf("\n\nPress any key to return to menu...");
            fflush(stdout);
            read(STDIN_FILENO, &c, 1);
        }
    }

    disable_raw_mode();
}

int main(){
	tr_main_menu();
	return 0;
}
