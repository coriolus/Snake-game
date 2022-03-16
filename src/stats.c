#define _CRT_SECURE_NO_WARNINGS
#include "stats.h"
#include "frontend.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>

#define HISCORE_OFFSET 1
#define LIVES_OFFSET 30
#define SCORE_OFFSET 15

#define STANDARD_POINTS 10
#define VALUE_POINTS 50
#define HIDDEN_POINTS 20

void display_stats(void)
{
    char *gameStats[] = {"Hi-score:", "Lives:", "Score:"};

    Border border = get_border_coordinates();

    clear_tab_stops();

    move_cursor(border.left + HISCORE_OFFSET, TOP_BORDER_OFFSET);
    printf("%s", gameStats[hiscore]);
    move_cursor_right(1);
    set_tab_stop();

    move_cursor((border.right - border.left) / 2, TOP_BORDER_OFFSET);
    set_tab_stop();

    move_cursor(border.right - LIVES_OFFSET, TOP_BORDER_OFFSET);
    printf("%s", gameStats[lives]);
    move_cursor_right(1);
    set_tab_stop();

    move_cursor_right(2);
    set_tab_stop();

    move_cursor(border.right - SCORE_OFFSET, TOP_BORDER_OFFSET);
    printf("%s", gameStats[score]);
    move_cursor_right(1);
    set_tab_stop();
}

void set_default_stats(Stats *stats, int lives)
{
    stats->hiscore = get_hiscore();
    stats->lives = lives;

    move_cursor(1, TOP_BORDER_OFFSET);
    printf("\t%d", stats->hiscore);
    printf("\t\t%d", stats->lives);
    printf("\t\t%8d", stats->score);
}

// read hi-score from file, if not available, create new file
int get_hiscore(void)
{
    int hiscore = 0;

    FILE *fp = fopen("hiscore.dat", "ab+");
    if (fp == NULL)
    {
        printf("Error opening/ creating file.");
        exit(EXIT_FAILURE);
    }
    int dataRead = fread(&hiscore, sizeof(int), 1, fp);
    if (!dataRead)
    {
        int dataWritten = fwrite(&hiscore, sizeof(int), 1, fp);
        if (!dataWritten)
        {
            printf("Error writing data.");
            exit(EXIT_FAILURE);
        }
    }
    fclose(fp);

    return hiscore;
}

// save hi-score to file
void save_hiscore(Stats *stats)
{
    FILE *fp = fopen("hiscore.dat", "wb");
    if (fp == NULL)
    {
        printf("Error opening/ creating the file.");
        exit(EXIT_FAILURE);
    }
    int dataWritten = fwrite(&stats->hiscore, sizeof(int), 1, fp);
    if (!dataWritten)
    {
        printf("Error writing data.");
        exit(EXIT_FAILURE);
    }
    fclose(fp);
}

void update_hiscore(Stats *stats)
{
    stats->hiscore = stats->score;

    move_cursor(1, TOP_BORDER_OFFSET);
    printf("\t%d", stats->hiscore);
}

void update_lives(Stats *stats, int lifeMod)
{
    if (lifeMod)
    {
        stats->lives++;
        stats->lifeAdded = 1;
    }
    else
        stats->lives--;

    move_cursor(1, TOP_BORDER_OFFSET);
    
    if (stats->lives == 0)
        set_color(BRIGHT_RED);

    printf("\t\t\t%d%s", stats->lives, RESET_COLOR);
}

void update_score(Stats *stats, Eats *eats)
{
    if (eats->ftype == standard)
        stats->score += STANDARD_POINTS;
    else if (eats->ftype == value)
        stats->score += VALUE_POINTS;
    else if (eats->ftype == hidden)
        stats->score += HIDDEN_POINTS;

    move_cursor(1, TOP_BORDER_OFFSET);
    printf("\t\t\t\t\t%8d", stats->score);
}

// display food modifier
void display_modifier(Eats *eats)
{
    move_cursor(1, TOP_BORDER_OFFSET);

    if (eats->mod == slow)
        printf("\t\t%s%c%s%c%s", RED, '<', BRIGHT_RED, '<', RESET_COLOR);
    else if (eats->mod == fast)
        printf("\t\t%s%c%s%c%s", BRIGHT_GREEN, '>', GREEN, '>', RESET_COLOR);
    else if (eats->mod == life)
        printf("\t\t\t\t%s%s%s", BRIGHT_CYAN, "+1", RESET_COLOR);
}

// hide food modifier
void hide_modifier(void)
{
    move_cursor(1, TOP_BORDER_OFFSET);
    printf("\t\t%s\t\t%s", "  ", "  ");
}
