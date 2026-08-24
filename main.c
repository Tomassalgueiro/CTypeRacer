#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <curl/curl.h>

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

	size_t curr_size = strlen(*response);

	*response = realloc(*response, bytes);

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
	fflush(stdout);
}

void tr_game_loop(){

	// change for web request
	const char *target = "The quick brown fox jumps over the lazy dog";

	int target_len = strlen(target);

	char typed[256] = {0};
	int current_idx = 0;

	enable_raw_mode();
	render(target, typed, current_idx);

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
		else if (c >= 32 && c <= 126){
			typed[current_idx] = c;
			current_idx++;
		}
		render(target, typed, current_idx);
	}

	disable_raw_mode();
	
}

void tr_main_menu(){
	 
}

int main(){

}
