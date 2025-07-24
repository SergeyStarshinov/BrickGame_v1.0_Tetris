/**
 * @file test_init.c
 * @brief Тест создания матриц, инициализации поля и next, переноса фигуры на
 * поле при SPAWN, функций API
 */
#include "tests_main.h"

/**
 * @brief Создание и инициализация основных массивов игры
 */
START_TEST(test_create) {
  // Создание массовов игры
  userInput(Start, true);
  // Проверка, что массивы созданы, указатели не NULL
  GameInfo_t game_info = updateCurrentState();
  ck_assert_ptr_ne(game_info.field, NULL);
  ck_assert_ptr_ne(game_info.next, NULL);
  // Проверка, что поле игры на старте пустое
  int **empty = createMatrix(FIELD_ROWS, FIELD_COLUMNS);
  for (int i = 0; i < FIELD_ROWS; i++)
    for (int j = 0; j < FIELD_COLUMNS; j++) empty[i][j] = 0;
  int res = compareMatrix(FIELD_ROWS, FIELD_COLUMNS, game_info.field, empty);
  ck_assert_int_eq(res, SUCCESSFUL_EXIT);
  ck_assert_int_eq(game_info.pause, START_MODE);
  // Проверка, что после старта игры поле не пусто
  userInput(Start, false);
  game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, GAME_MODE);
  res = compareMatrix(FIELD_ROWS, FIELD_COLUMNS, game_info.field, empty);
  ck_assert_int_eq(res, FAILURE_EXIT);
  free(empty);
  // Проверка, что есть информация о следующей фигуре
  empty = createMatrix(PIECE_ROWS, PIECE_COLUMNS);
  for (int i = 0; i < PIECE_ROWS; i++)
    for (int j = 0; j < PIECE_COLUMNS; j++) empty[i][j] = 0;
  res = compareMatrix(PIECE_ROWS, PIECE_COLUMNS, game_info.next, empty);
  ck_assert_int_eq(res, FAILURE_EXIT);
  userInput(Terminate, true);
  free(empty);
}
END_TEST;

/**
 * @brief Проверка различных вариантов чтения данных о рекорде
 */
START_TEST(test_high_score) {
  char filename[] = "highscore.txt";
  // Если файл есть и в нем записано число
  int high_score_ref = 1700;
  FILE *file = fopen(filename, "w");
  fprintf(file, "%d", high_score_ref);
  fclose(file);
  int high_score = getHighScore();
  ck_assert_int_eq(high_score, high_score_ref);
  // Если файл есть, но он пустой
  file = fopen(filename, "w");
  fclose(file);
  high_score = getHighScore();
  ck_assert_int_eq(high_score, 0);
  // Если файл есть, но он некорректный
  file = fopen(filename, "w");
  fprintf(file, "aaa");
  fclose(file);
  high_score = getHighScore();
  ck_assert_int_eq(high_score, 0);
  // Если файла не существует
  remove(filename);
  high_score = getHighScore();
  ck_assert_int_eq(high_score, 0);
}
END_TEST;

Suite *test_init(void) {
  Suite *s;
  TCase *tc;
  s = suite_create("test_create");
  tc = tcase_create("create");
  tcase_add_test(tc, test_create);
  tcase_add_test(tc, test_high_score);
  suite_add_tcase(s, tc);
  return s;
}
