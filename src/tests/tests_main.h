#ifndef TESTS_MAIN_H
#define TESTS_MAIN_H

#include <check.h>
#include <stdio.h>

#include "../brick_game/tetris/s21_api.h"
#include "../brick_game/tetris/s21_tetris_fsm.h"
#include "../gui/cli/s21_define.h"

int compareMatrix(int rows, int cols, int **matrix_1, int **matrix_2);
void emptyField(int **field);

Suite *test_init(void);
Suite *test_frontend_mode(void);
Suite *test_fsm_mode(void);
Suite *test_backend_utils(void);

#endif  // TESTS_MAIN_H
