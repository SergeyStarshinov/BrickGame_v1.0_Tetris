/**
 * @file test_frontend_mode.c
 * @brief Тест последовательного переключения режимов для Frontend:
 * START_MODE, GAME_MODE, PAUSE_MODE, EXIT MODE
 */

#include "tests_main.h"

/**
 * @brief Переходы без падения фигуры
 */
START_TEST(test_frontend1) {
  // Старт игры, переход через START_MODE в GAME_MODE
  userInput(Start, true);
  GameInfo_t game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, START_MODE);
  userInput(Start, false);
  game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, GAME_MODE);
  // Остаемся в GAME_MODE при нажатии влево, вправо, ротации,
  // движении вниз по таймеру
  userInput(Left, false);
  game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, GAME_MODE);
  userInput(Right, false);
  game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, GAME_MODE);
  userInput(Action, false);
  game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, GAME_MODE);
  userInput(Down, false);
  game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, GAME_MODE);
  // Переход в PAUSE_MODE и обратно
  userInput(Pause, false);
  game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, PAUSE_MODE);
  userInput(Pause, false);
  game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, GAME_MODE);
  // Переход в GAMEOVER_MODE через PAUSE_MODE
  userInput(Pause, false);
  userInput(Terminate, false);
  game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, GAMEOVER_MODE);
  // Переход из GAMEOVER_MODE в START_MODE
  userInput(Terminate, false);
  game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, START_MODE);
  // Переход в GAMEOVER_MODE из GAME_MODE по Esc
  userInput(Start, false);
  userInput(Terminate, false);
  game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, GAMEOVER_MODE);
  // Через START_MODE выход на EXIT_MODE
  userInput(Action, false);
  userInput(Terminate, false);
  game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, EXIT_MODE);
  userInput(Terminate, true);
}
END_TEST;

/**
 * @brief Переходы с падением фигуры
 */
START_TEST(test_frontend2) {
  // Старт игры, переход через START_MODE в GAME_MODE
  userInput(Start, true);
  GameInfo_t game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, START_MODE);
  userInput(Start, false);
  game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, GAME_MODE);
  // Падение фигуры, остаемся в GAME_MODE
  userInput(Down, true);
  game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, GAME_MODE);
  // Переход в GAMEOVER_MODE из GAME_MODE по Esc
  userInput(Terminate, false);
  game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, GAMEOVER_MODE);
  // Через START_MODE выход на EXIT_MODE
  userInput(Left, false);
  userInput(Terminate, false);
  game_info = updateCurrentState();
  ck_assert_int_eq(game_info.pause, EXIT_MODE);
  userInput(Terminate, true);
}
END_TEST;

Suite *test_frontend_mode(void) {
  Suite *s;
  TCase *tc;
  s = suite_create("test_frontend");
  tc = tcase_create("frontend");
  tcase_add_test(tc, test_frontend1);
  tcase_add_test(tc, test_frontend2);
  suite_add_tcase(s, tc);
  return s;
}
