#include "stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct stats {
	int games_played;
	double time_elapsed;
	long int total_chars;
	double best_wpm;
};

static int get_cache_path(char *dest, size_t max_len){
	const char *home = getenv("HOME");
	if (!home) return -1;

	char dir_path[512];
	char cache_root[512];

	snprintf(cache_root, sizeof(cache_root), "%s/.cache", home);
	snprintf(dir_path, sizeof(dir_path), "%s/.cache/CTypeRacer", home);

	mkdir(cache_root, 0755);
	mkdir(dir_path, 0755);

	int written = snprintf(dest, max_len, "%s/.cache/CTypeRacer/stats.bin", home);
	if (written < 0 || (size_t)written >= max_len) {
	    return -1;
	}

	return 0;

}

Stats* stats_create(void){
	Stats *s = calloc(1, sizeof(struct stats));
	return s;
}

void stats_free(Stats *s){
	free(s);
}

int stats_load_cache(Stats *s){
	if (!s) return -1;

	char filepath[512];
	if (get_cache_path(filepath, sizeof(filepath)) != 0) return -1;

	FILE *fp = fopen (filepath, "rb");
	if (!fp) return -1;

	size_t read_bytes = fread(s, sizeof(struct stats), 1, fp);
	fclose(fp);

	return (read_bytes == 1) ? 0 : -1;
}

int stats_save_cache(const Stats *s){
	if (!s) return -1;

	char filepath[512];
	if (get_cache_path(filepath, sizeof(filepath)) != 0) return -1;

	FILE *fp = fopen (filepath, "wb");
	if (!fp) return -1;

	size_t written = fwrite(s, sizeof(struct stats), 1, fp);
	fclose(fp);

	return (written == 1) ? 0 : -1;
}

void stats_record_game(Stats *s, double time_elapsed_seconds, size_t chars_typed){
	if (!s || time_elapsed_seconds <= 0.0) return;
	
	s->games_played += 1;
	s->time_elapsed += time_elapsed_seconds;
	s->total_chars += (long int)chars_typed;

	double session_wpm = stats_calculate_wpm(time_elapsed_seconds, chars_typed);
	if (session_wpm > s->best_wpm){
		s->best_wpm = session_wpm;
	}
}

double stats_calculate_wpm(double time_elapsed_seconds, size_t chars_typed){
	if (time_elapsed_seconds <= 0.0) { return 0.0; }
	return ((double)chars_typed / 5.0 ) / (time_elapsed_seconds / 60.0);
}

double stats_get_overall_wpm(const Stats *s){
	if(!s || s->time_elapsed <= 0.0 ) return 0.0;
	return stats_calculate_wpm(s->time_elapsed,(size_t)s->total_chars);

}

double stats_get_best_wpm(const Stats *s){
	return s ? s->best_wpm : 0.0;
}

int stats_get_games_played(const Stats *s){
	return s ? s->games_played : 0;
}

long int stats_get_total_chars_typed(const Stats *s){
	return s ? s->total_chars : 0;
}

double stats_get_total_time_elapsed(const Stats *s){
	return s ? s->time_elapsed : 0;
}
