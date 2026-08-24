#ifndef STAT_H
#define STAT_H

#include <stddef.h>
typedef struct stats Stats;

Stats* stats_create(void);
void stats_free(Stats *s);

int stats_load(Stats *s, const char *filename);
int stats_save(const Stats *s, char *filename);

void stats_record_game(Stats *s, double time_elapsed, size_t chars_typed);

double stats_calculate_wpm(double time_elapsed_seconds, size_t chars_typed);
double stats_get_overall_wpm(const Stats *s);
double stats_get_best_wpm(const Stats *s);
int stats_get_games_played(const Stats *s);

#endif
