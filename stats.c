
#include "stats.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

struct stats {
	int games_played;
	double time_elapsed;
	long int total_chars;
	double best_wpm;
};

Stats* stats_create(void){
	Stats *s = calloc(1, sizeof(struct stats));
	return s;
}

void stats_free(Stats *s){
	free(s);
}

int stats_load(Stats *s, const char *filename){
	if (!s || filename) return -1;

	FILE *fp = fopen (filename, "rb");
	if (!fp) return -1;

	size_t read = fread(s, sizeof(struct stats), 1, fp);
	fclose(fp);

	return (read == 1) ? 0 : -1;
}

int stats_save(const Stats *s, char *filename){
	if (!s || !filename) return -1;

	FILE *fp = fopen (filename, "wb");
	if (!fp) return -1;

	size_t written = fwrite(s, sizeof(struct stats), 1, fp);
	fclose(fp);

	return (written == 1) ? 0 : -1;
}

void stats_record_game(Stats *s, double time_elapsed_seconds, size_t chars_typed){
	if (!s || time_elapsed_seconds <= 0.0) return;
	
	s->games_played += 1;
	s->time_elapsed += time_elapsed_seconds;
	s->games_played += (long int)chars_typed;

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
	return s ? s->games_played : 0.0;
}
