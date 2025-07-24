#ifndef TETRIS_FRONT_H
#define TETRIS_FRONT_H

#include <ncurses.h>

void printGameScreen(GameInfo_t *game_info);
void printWelcome();
void printBorders(int height, int width);
void printTLine(int height, int col);
void printAddInfo();
void printGlass(GameInfo_t *game_info);
void printNext(GameInfo_t *game_info);
UserAction_t getAction(int key);
UserAction_t defineAction(bool *hold, float *timer, int speed);
void printGameStat(GameInfo_t *game_info);
void printGameover(int score);

#endif  // TETRIS_FRONT_H