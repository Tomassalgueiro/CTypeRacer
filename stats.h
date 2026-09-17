#ifndef STAT_H
#define STAT_H

#include <stddef.h>
typedef struct stats Stats;

Stats* stats_create(void);
void stats_free(Stats *s);

int stats_load_cache(Stats *s);
int stats_save_cache(const Stats *s);

void stats_record_game(Stats *s, double time_elapsed, size_t chars_typed);

double stats_calculate_wpm(double time_elapsed_seconds, size_t chars_typed);
double stats_get_overall_wpm(const Stats *s);
double stats_get_best_wpm(const Stats *s);
int stats_get_games_played(const Stats *s);
long int stats_get_total_chars_typed(const Stats *s);
double stats_get_total_time_elapsed(const Stats *s);

#endif
