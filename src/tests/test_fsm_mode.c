/**
 * @file test_fsm_mode.c
 * @brief Тест переключения режимов FSM
 */
#include "tests_main.h"

/**
 * @brief Последовательный перебор состояний FSM
 */
START_TEST(test_fsm_1) {
  // Создание матриц, начальное состояние START
  tetris_state state = START;
  GameInfo_t game_info = {NULL, NULL, 0, 0, 0, 0, 0};
  addinfo_t fsm_addinfo = {NULL, 0, 0, 0, 0, 0, 0};
  signal_t signal;
  // Переход в SPAWN
  signal.signal = ACT_SIG;
  signal.action = Start;
  tetrisCreate(&game_info, &fsm_addinfo);
  state = fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, SPAWN);
  // Переход в MOVING
  state = fsmOnSpawnMode(&game_info, &fsm_addinfo);
  ck_assert_int_eq(state, MOVING);
  // Переход в PAUSE и обратно
  signal.action = Pause;
  state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, PAUSE);
  state = fsmOnPauseMode(&signal, &game_info);
  ck_assert_int_eq(state, MOVING);
  // Остаемся в MOVING при влево, вправо, вниз по таймеру
  signal.action = Left;
  state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, MOVING);
  signal.action = Right;
  state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, MOVING);
  signal.action = Down;
  state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, MOVING);
  // Переход в ATTACHING при падении фигуры
  signal.signal = DROP_SIG;
  state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, ATTACHING);
  // Переход в SPAWN после ATTACHING
  state = fsmOnAttachingMode(&game_info);
  ck_assert_int_eq(state, SPAWN);
  // Выход через несколько Esc
  state = fsmOnSpawnMode(&game_info, &fsm_addinfo);
  ck_assert_int_eq(state, MOVING);
  signal.signal = ACT_SIG;
  signal.action = Terminate;
  state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, GAMEOVER);
  state = fsmOnGameoverMode(&signal, &game_info);
  ck_assert_int_eq(state, START);
  state = fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, EXIT_STATE);
  tetrisDestroy(&game_info, &fsm_addinfo);
}
END_TEST;

/**
 * @brief Переход в ATTACHING через движение фигуры по таймеру
 */
START_TEST(test_fsm_2) {
  // Создание матриц, начальное состояние
  tetris_state state = START;
  GameInfo_t game_info = {NULL, NULL, 0, 0, 0, 0, 0};
  addinfo_t fsm_addinfo = {NULL, 0, 0, 0, 0, 0, 0};
  signal_t signal;
  // Переход в SPAWN
  signal.signal = ACT_SIG;
  signal.action = Start;
  tetrisCreate(&game_info, &fsm_addinfo);
  state = fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  // Следующую фигуру меняю на квадрат, для предсказуемости
  getPiece(game_info.next, 0, 0);
  state = fsmOnSpawnMode(&game_info, &fsm_addinfo);
  // Переход в ATTACHING должен произойти на 19 сдвиге
  signal.action = Down;
  for (int i = 0; i < 18; i++) {
    state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  }
  ck_assert_int_eq(state, MOVING);
  state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, ATTACHING);
  tetrisDestroy(&game_info, &fsm_addinfo);
}
END_TEST;

/**
 * @brief Переход в GAMEOVER через PAUSE и Esc
 */
START_TEST(test_fsm_3) {
  // Создание матриц, начальное состояние
  tetris_state state = START;
  GameInfo_t game_info = {NULL, NULL, 0, 0, 0, 0, 0};
  addinfo_t fsm_addinfo = {NULL, 0, 0, 0, 0, 0, 0};
  signal_t signal;
  signal.signal = ACT_SIG;
  signal.action = Start;
  tetrisCreate(&game_info, &fsm_addinfo);
  state = fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  state = fsmOnSpawnMode(&game_info, &fsm_addinfo);
  signal.action = Pause;
  state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, PAUSE);
  signal.action = Terminate;
  state = fsmOnPauseMode(&signal, &game_info);
  ck_assert_int_eq(state, GAMEOVER);
  tetrisDestroy(&game_info, &fsm_addinfo);
}
END_TEST;

/**
 * @brief Обработка сигналов в режиме START
 */
START_TEST(test_fsm_start) {
  // Создание матриц, начальное состояние
  tetris_state state = START;
  GameInfo_t game_info = {NULL, NULL, 0, 0, 0, 0, 0};
  addinfo_t fsm_addinfo = {NULL, 0, 0, 0, 0, 0, 0};
  tetrisCreate(&game_info, &fsm_addinfo);
  // Сигнал Start - переход в SPAWN
  signal_t signal;
  signal.signal = ACT_SIG;
  signal.action = Start;
  state = fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, SPAWN);
  // Сигнал Pause - остается START
  state = START;
  signal.action = Pause;
  state = fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, START);
  // Сигнал Terminate - переход на EXIT_STATE
  state = START;
  signal.action = Terminate;
  state = fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, EXIT_STATE);
  // Сигнал Left - остается START
  state = START;
  signal.action = Left;
  state = fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, START);
  // Сигнал Right - остается START
  state = START;
  signal.action = Right;
  state = fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, START);
  // Сигнал Up - остается START
  state = START;
  signal.action = Up;
  state = fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, START);
  // Сигнал Down - остается START
  state = START;
  signal.action = Down;
  state = fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, START);
  // Сигнал Action - остается START
  state = START;
  signal.action = Action;
  state = fsmOnStartMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, START);

  tetrisDestroy(&game_info, &fsm_addinfo);
}
END_TEST;

/**
 * @brief Обработка сигналов в режиме MOVING
 */
START_TEST(test_fsm_moving) {
  // Создание матриц, начальное состояние
  tetris_state state = MOVING;
  GameInfo_t game_info = {NULL, NULL, 0, 0, 0, 0, 0};
  addinfo_t fsm_addinfo = {NULL, 0, 0, 0, 0, 0, 0};
  tetrisCreate(&game_info, &fsm_addinfo);
  // Сигнал Start - остаемся в MOVING
  signal_t signal;
  signal.signal = ACT_SIG;
  signal.action = Start;
  state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, MOVING);
  // Сигнал Pause - переход в PAUSE
  state = MOVING;
  signal.action = Pause;
  state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, PAUSE);
  // Сигнал Terminate - переход на GAMEOVER
  state = MOVING;
  signal.action = Terminate;
  state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, GAMEOVER);
  // Сигнал Left - остается MOVING
  state = MOVING;
  signal.action = Left;
  state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, MOVING);
  // Сигнал Right - остается MOVING
  state = MOVING;
  signal.action = Right;
  state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, MOVING);
  // Сигнал Up - остается MOVING
  state = MOVING;
  signal.action = Up;
  state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, MOVING);
  // Сигнал Down - остается START
  state = MOVING;
  signal.action = Down;
  state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, MOVING);
  // Сигнал Action - остается MOVING
  state = MOVING;
  signal.action = Action;
  state = fsmOnMovingMode(&signal, &game_info, &fsm_addinfo);
  ck_assert_int_eq(state, MOVING);

  tetrisDestroy(&game_info, &fsm_addinfo);
}
END_TEST;

/**
 * @brief Обработка сигналов в режиме PAUSE
 */
START_TEST(test_fsm_pause) {
  // Создание матриц, начальное состояние
  tetris_state state = PAUSE;
  GameInfo_t game_info = {NULL, NULL, 0, 0, 0, 0, 0};
  addinfo_t fsm_addinfo = {NULL, 0, 0, 0, 0, 0, 0};
  tetrisCreate(&game_info, &fsm_addinfo);
  // Сигнал Start - остаемся в PAUSE
  signal_t signal;
  signal.signal = ACT_SIG;
  signal.action = Start;
  state = fsmOnPauseMode(&signal, &game_info);
  ck_assert_int_eq(state, PAUSE);
  // Сигнал Pause - переход в MOVING
  state = PAUSE;
  signal.action = Pause;
  state = fsmOnPauseMode(&signal, &game_info);
  ck_assert_int_eq(state, MOVING);
  // Сигнал Terminate - переход на GAMEOVER
  state = PAUSE;
  signal.action = Terminate;
  state = fsmOnPauseMode(&signal, &game_info);
  ck_assert_int_eq(state, GAMEOVER);
  // Сигнал Left - остается PAUSE
  state = PAUSE;
  signal.action = Left;
  state = fsmOnPauseMode(&signal, &game_info);
  ck_assert_int_eq(state, PAUSE);
  // Сигнал Right - остается PAUSE
  state = PAUSE;
  signal.action = Right;
  state = fsmOnPauseMode(&signal, &game_info);
  ck_assert_int_eq(state, PAUSE);
  // Сигнал Up - остается PAUSE
  state = PAUSE;
  signal.action = Up;
  state = fsmOnPauseMode(&signal, &game_info);
  ck_assert_int_eq(state, PAUSE);
  // Сигнал Down - остается PAUSE
  state = PAUSE;
  signal.action = Down;
  state = fsmOnPauseMode(&signal, &game_info);
  ck_assert_int_eq(state, PAUSE);
  // Сигнал Action - остается PAUSE
  state = PAUSE;
  signal.action = Action;
  state = fsmOnPauseMode(&signal, &game_info);
  ck_assert_int_eq(state, PAUSE);

  tetrisDestroy(&game_info, &fsm_addinfo);
}
END_TEST;

/**
 * @brief Переход из SPAWN в GAMEOVER.
 */
START_TEST(test_fsm_spawn) {
  // Создание матриц, начальное состояние
  tetris_state state = SPAWN;
  GameInfo_t game_info = {NULL, NULL, 0, 0, 0, 0, 0};
  addinfo_t fsm_addinfo = {NULL, 0, 0, 0, 0, 0, 0};
  tetrisCreate(&game_info, &fsm_addinfo);
  emptyField(game_info.field);
  // Заполнение 0-й строки, чтобы фигура не могла лечь на поле
  game_info.field[0][4] = 1;
  game_info.field[0][5] = 1;

  state = fsmOnSpawnMode(&game_info, &fsm_addinfo);
  ck_assert_int_eq(state, GAMEOVER);
  tetrisDestroy(&game_info, &fsm_addinfo);
}
END_TEST;

Suite *test_fsm_mode(void) {
  Suite *s;
  TCase *tc;
  s = suite_create("test_fsm");
  tc = tcase_create("fsm");
  tcase_add_test(tc, test_fsm_1);
  tcase_add_test(tc, test_fsm_2);
  tcase_add_test(tc, test_fsm_3);
  tcase_add_test(tc, test_fsm_start);
  tcase_add_test(tc, test_fsm_moving);
  tcase_add_test(tc, test_fsm_pause);
  tcase_add_test(tc, test_fsm_spawn);
  suite_add_tcase(s, tc);
  return s;
}