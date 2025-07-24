/**
 * @file test_backend_utils.c
 * @brief Тестирование backend функций
 */

#include "tests_main.h"

/**
 * @brief Сдвиг влево и вправо, ротация: не выходит за границы или препятствия
 */
START_TEST(test_move) {
  // Создание матриц, начальное состояние, старт игры
  tetris_state state = START;
  GameInfo_t game_info = {NULL, NULL, 0, 0, 0, 0, 0};
  addinfo_t fsm_addinfo = {NULL, 0, 0, 0, 0, 0, 0};
  signal_t signal;
  signal.signal = ACT_SIG;
  signal.action = Start;
  tetrisCreate(&game_info, &fsm_addinfo);
  fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  // Следующую фигуру меняю на Т, для предсказуемости
  getPiece(game_info.next, 6, 0);
  fsm_addinfo.next_id = 6;
  fsm_addinfo.next_rot_id = 0;
  fsmOnSpawnMode(&game_info, &fsm_addinfo);
  ck_assert_int_eq(fsm_addinfo.col_pos, 3);
  // Сдвиг влево возможен 4 раза, дальше остается на месте
  for (int i = 0; i < 4; i++) shiftPiece(&game_info, &fsm_addinfo, -1);
  ck_assert_int_eq(fsm_addinfo.col_pos, -1);
  shiftPiece(&game_info, &fsm_addinfo, -1);
  ck_assert_int_eq(fsm_addinfo.col_pos, -1);
  // Сдвиг вправо возможен 7 раз, дальше остается на месте
  for (int i = 0; i < 7; i++) shiftPiece(&game_info, &fsm_addinfo, 1);
  ck_assert_int_eq(fsm_addinfo.col_pos, 6);
  shiftPiece(&game_info, &fsm_addinfo, 1);
  ck_assert_int_eq(fsm_addinfo.col_pos, 6);
  // Но после ротации сдвиг возможен еще на 1 позицию вправо
  rotatePiece(&game_info, &fsm_addinfo);
  shiftPiece(&game_info, &fsm_addinfo, 1);
  ck_assert_int_eq(fsm_addinfo.col_pos, 7);
  // Проверка, что ротация невозможна, по game_info.field[0][9]
  ck_assert_int_eq(game_info.field[0][9], 7);
  rotatePiece(&game_info, &fsm_addinfo);
  ck_assert_int_eq(game_info.field[0][9], 7);
  // Но после сдвига влево ротация доступна
  shiftPiece(&game_info, &fsm_addinfo, -1);
  ck_assert_int_eq(game_info.field[1][9], 0);
  rotatePiece(&game_info, &fsm_addinfo);
  ck_assert_int_eq(game_info.field[1][9], 7);
  // Проверка на препятствие на поле при сдвиге
  game_info.field[1][6] = 1;
  ck_assert_int_eq(fsm_addinfo.col_pos, 6);
  shiftPiece(&game_info, &fsm_addinfo, -1);
  ck_assert_int_eq(fsm_addinfo.col_pos, 6);
  // Проверка на препятствие снизу и переход в ATTACHING
  game_info.field[2][8] = 1;
  state = movePieceDown(&game_info, &fsm_addinfo);
  ck_assert_int_eq(fsm_addinfo.row_pos, 0);
  ck_assert_int_eq(state, ATTACHING);
  tetrisDestroy(&game_info, &fsm_addinfo);
}
END_TEST;

/**
 * @brief Удаление 1 линии без повышения уровня и изменения скорости
 */
START_TEST(test_score1) {
  tetris_state state = START;
  GameInfo_t game_info = {NULL, NULL, 0, 0, 0, 0, 0};
  addinfo_t fsm_addinfo = {NULL, 0, 0, 0, 0, 0, 0};
  signal_t signal;
  signal.signal = ACT_SIG;
  signal.action = Start;
  tetrisCreate(&game_info, &fsm_addinfo);
  fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  game_info.high_score = 0;
  // Следующую фигуру меняю на I, для предсказуемости
  getPiece(game_info.next, 1, 1);
  fsm_addinfo.next_id = 1;
  fsm_addinfo.next_rot_id = 1;
  fsmOnSpawnMode(&game_info, &fsm_addinfo);
  // Поле для теста под заполнение 1 линии
  emptyField(game_info.field);
  for (int i = 0; i < FIELD_COLUMNS; i++) {
    if (i != 5) game_info.field[19][i] = 1;
  }
  // Референсное поле после падения
  int **ref = createMatrix(FIELD_ROWS, FIELD_COLUMNS);
  emptyField(ref);
  ref[17][5] = 2;
  ref[18][5] = 2;
  ref[19][5] = 2;
  int old_speed = game_info.speed;
  // Падение и отработка ATTACHING
  signal.signal = DROP_SIG;
  signal.action = Down;
  fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  state = fsmOnAttachingMode(&game_info);
  // Сверка результата с ожиданием
  ck_assert_int_eq(state, SPAWN);
  ck_assert_int_eq(game_info.score, 100);
  ck_assert_int_eq(game_info.high_score, 100);
  ck_assert_int_eq(game_info.level, 1);
  ck_assert_int_eq(game_info.speed, old_speed);
  int compare = compareMatrix(FIELD_ROWS, FIELD_COLUMNS, game_info.field, ref);
  ck_assert_int_eq(compare, SUCCESSFUL_EXIT);
  tetrisDestroy(&game_info, &fsm_addinfo);
  free(ref);
}
END_TEST;

/**
 * @brief Удаление 2 линий без повышения уровня и изменения скорости
 */
START_TEST(test_score2) {
  tetris_state state = START;
  GameInfo_t game_info = {NULL, NULL, 0, 0, 0, 0, 0};
  addinfo_t fsm_addinfo = {NULL, 0, 0, 0, 0, 0, 0};
  signal_t signal;
  signal.signal = ACT_SIG;
  signal.action = Start;
  tetrisCreate(&game_info, &fsm_addinfo);
  fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  game_info.high_score = 0;
  // Следующую фигуру меняю на I, для предсказуемости
  getPiece(game_info.next, 1, 1);
  fsm_addinfo.next_id = 1;
  fsm_addinfo.next_rot_id = 1;
  fsmOnSpawnMode(&game_info, &fsm_addinfo);
  // Поле для теста под заполнение 2 линий
  emptyField(game_info.field);
  for (int i = 0; i < FIELD_COLUMNS; i++) {
    if (i != 5) game_info.field[18][i] = 1;
    if (i != 5) game_info.field[19][i] = 1;
  }
  // Референсное поле после падения
  int **ref = createMatrix(FIELD_ROWS, FIELD_COLUMNS);
  emptyField(ref);
  ref[18][5] = 2;
  ref[19][5] = 2;
  int old_speed = game_info.speed;
  // Падение и отработка ATTACHING
  signal.signal = DROP_SIG;
  signal.action = Down;
  fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  state = fsmOnAttachingMode(&game_info);
  // Сверка результата с ожиданием
  ck_assert_int_eq(state, SPAWN);
  ck_assert_int_eq(game_info.score, 300);
  ck_assert_int_eq(game_info.high_score, 300);
  ck_assert_int_eq(game_info.level, 1);
  ck_assert_int_eq(game_info.speed, old_speed);
  int compare = compareMatrix(FIELD_ROWS, FIELD_COLUMNS, game_info.field, ref);
  ck_assert_int_eq(compare, SUCCESSFUL_EXIT);
  tetrisDestroy(&game_info, &fsm_addinfo);
  free(ref);
}
END_TEST;

/**
 * @brief Удаление 3 линий с повышением уровня на 1 и изменением скорости
 */
START_TEST(test_score3) {
  tetris_state state = START;
  GameInfo_t game_info = {NULL, NULL, 0, 0, 0, 0, 0};
  addinfo_t fsm_addinfo = {NULL, 0, 0, 0, 0, 0, 0};
  signal_t signal;
  signal.signal = ACT_SIG;
  signal.action = Start;
  tetrisCreate(&game_info, &fsm_addinfo);
  fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  game_info.high_score = 0;
  // Следующую фигуру меняю на I, для предсказуемости
  getPiece(game_info.next, 1, 1);
  fsm_addinfo.next_id = 1;
  fsm_addinfo.next_rot_id = 1;
  fsmOnSpawnMode(&game_info, &fsm_addinfo);
  // Поле для теста под заполнение 3 линий
  emptyField(game_info.field);
  for (int i = 0; i < FIELD_COLUMNS; i++) {
    if (i != 5) game_info.field[17][i] = 1;
    if (i != 5) game_info.field[18][i] = 1;
    if (i != 5) game_info.field[19][i] = 1;
  }
  // Референсное поле после падения
  int **ref = createMatrix(FIELD_ROWS, FIELD_COLUMNS);
  emptyField(ref);
  ref[19][5] = 2;
  int old_speed = game_info.speed;
  // Падение и отработка ATTACHING
  signal.signal = DROP_SIG;
  signal.action = Down;
  fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  state = fsmOnAttachingMode(&game_info);
  // Сверка результата с ожиданием
  ck_assert_int_eq(state, SPAWN);
  ck_assert_int_eq(game_info.score, 700);
  ck_assert_int_eq(game_info.high_score, 700);
  ck_assert_int_eq(game_info.level, 2);
  ck_assert_int_lt(game_info.speed, old_speed);
  int compare = compareMatrix(FIELD_ROWS, FIELD_COLUMNS, game_info.field, ref);
  ck_assert_int_eq(compare, SUCCESSFUL_EXIT);
  tetrisDestroy(&game_info, &fsm_addinfo);
  free(ref);
}
END_TEST;

/**
 * @brief Удаление 4 линий с повышением уровня на 2 и изменением скорости
 */
START_TEST(test_score4) {
  tetris_state state = START;
  GameInfo_t game_info = {NULL, NULL, 0, 0, 0, 0, 0};
  addinfo_t fsm_addinfo = {NULL, 0, 0, 0, 0, 0, 0};
  signal_t signal;
  signal.signal = ACT_SIG;
  signal.action = Start;
  tetrisCreate(&game_info, &fsm_addinfo);
  fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  game_info.high_score = 0;
  // Следующую фигуру меняю на I, для предсказуемости
  getPiece(game_info.next, 1, 1);
  fsm_addinfo.next_id = 1;
  fsm_addinfo.next_rot_id = 1;
  fsmOnSpawnMode(&game_info, &fsm_addinfo);
  // Поле для теста под заполнение 4 линий
  emptyField(game_info.field);
  for (int i = 0; i < FIELD_COLUMNS; i++) {
    if (i != 5) game_info.field[16][i] = 1;
    if (i != 5) game_info.field[17][i] = 1;
    if (i != 5) game_info.field[18][i] = 1;
    if (i != 5) game_info.field[19][i] = 1;
  }
  // Референсное поле после падения
  int **ref = createMatrix(FIELD_ROWS, FIELD_COLUMNS);
  emptyField(ref);
  int old_speed = game_info.speed;
  // Падение и отработка ATTACHING
  signal.signal = DROP_SIG;
  signal.action = Down;
  fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  state = fsmOnAttachingMode(&game_info);
  // Сверка результата с ожиданием
  ck_assert_int_eq(state, SPAWN);
  ck_assert_int_eq(game_info.score, 1500);
  ck_assert_int_eq(game_info.high_score, 1500);
  ck_assert_int_eq(game_info.level, 3);
  ck_assert_int_lt(game_info.speed, old_speed);
  int compare = compareMatrix(FIELD_ROWS, FIELD_COLUMNS, game_info.field, ref);
  ck_assert_int_eq(compare, SUCCESSFUL_EXIT);
  tetrisDestroy(&game_info, &fsm_addinfo);
  free(ref);
}
END_TEST;

/**
 * @brief Запись рекорда в файл.
 */
START_TEST(test_highscore) {
  GameInfo_t game_info = {NULL, NULL, 0, 0, 0, 0, 0};
  FILE *file = fopen("highscore.txt", "w");
  fprintf(file, "%d", 0);
  fclose(file);
  int ref = 0;
  // Если score < high_score, то не записывается
  game_info.high_score = 1000;
  game_info.score = 500;
  saveHighScore(&game_info);
  file = fopen("highscore.txt", "r");
  fscanf(file, "%d", &ref);
  fclose(file);
  ck_assert_int_eq(ref, 0);
  // Если score = high_score, то записывается
  game_info.high_score = 1000;
  game_info.score = 1000;
  saveHighScore(&game_info);
  file = fopen("highscore.txt", "r");
  fscanf(file, "%d", &ref);
  fclose(file);
  ck_assert_int_eq(ref, 1000);
}
END_TEST;

Suite *test_backend_utils(void) {
  Suite *s;
  TCase *tc;
  s = suite_create("test_backend_utils");
  tc = tcase_create("back_utils");
  tcase_add_test(tc, test_move);
  tcase_add_test(tc, test_score1);
  tcase_add_test(tc, test_score2);
  tcase_add_test(tc, test_score3);
  tcase_add_test(tc, test_score4);
  tcase_add_test(tc, test_highscore);
  suite_add_tcase(s, tc);
  return s;
}