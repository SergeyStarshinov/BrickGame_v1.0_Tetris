#ifndef TETRIS_BACK_H
#define TETRIS_BACK_H

#include <stdio.h>
#include <stdlib.h>

#include "../../gui/cli/s21_define.h"

/// @brief Доп.информация FSM, которая сохраняется на протяжении игры
typedef struct {
  /// Текущая фигура
  int **piece;
  /// Позиция текущей фигуры - строка
  int row_pos;
  /// Позиция текущей фигуры - столбец
  int col_pos;
  /// id текущей фигуры
  int piece_id;
  /// id вращения текущей фигуры
  int piece_rot_id;
  /// id следующей фигуры
  int next_id;
  /// id вращения следующей фигуры
  int next_rot_id;
} addinfo_t;

// Типы сигналов в FSM, дополнительно к Action_t
typedef enum {
  // Только считывание информации, без действий
  GET_SIG = 0,
  // Для создания массивов в начале работы программы
  INIT_SIG,
  // Выполнение действий (кроме падения)
  ACT_SIG,
  // Для удаления массивов в конце работы программы
  DESTR_SIG,
  // Выполнение действия падения фигуры
  DROP_SIG
} sig;

/// @brief Полная информация по действию для FSM
typedef struct {
  /// Информация по действиям пользователя
  UserAction_t action;
  /// Тип сигнала
  sig signal;
} signal_t;

void tetrisCreate(GameInfo_t *game_info, addinfo_t *fsm_addinfo);
void tetrisInit(GameInfo_t *game_info, addinfo_t *fsm_addinfo);
void tetrisDestroy(GameInfo_t *game_info, addinfo_t *fsm_addinfo);
int **createMatrix(int rows, int cols);
void genNextPiece(GameInfo_t *game_info, addinfo_t *fsm_addinfo);
void getPiece(int **dst, int id, int rot_id);
void fromNextIntoCurrent(GameInfo_t *game_info, addinfo_t *fsm_addinfo);
int checkPlacePiece(GameInfo_t *game_info, addinfo_t *fsm_addinfo);
void placePieceOnField(GameInfo_t *game_info, addinfo_t *fsm_addinfo);
void removePieceFromField(GameInfo_t *game_info, addinfo_t *fsm_addinfo);
void shiftPiece(GameInfo_t *game_info, addinfo_t *fsm_addinfo, int shift);
void rotatePiece(GameInfo_t *game_info, addinfo_t *fsm_addinfo);
void dropPiece(GameInfo_t *game_info, addinfo_t *fsm_addinfo);
tetris_state movePieceDown(GameInfo_t *game_info, addinfo_t *fsm_addinfo);
int isRowFilled(const int *row);
void shiftField(int **field, int row);
void saveHighScore(GameInfo_t *game_info);
int getHighScore();

#endif  // TETRIS_BACK_H