/**
 * @file s21_api.c
 * @brief Функции API между GUI и бекэндом (по ТЗ).
 */

#include "s21_tetris_fsm.h"

/**
 * @brief Функция приема пользовательского ввода
 *
 * Принимает ввод действий пользователя, а также действия начала и конца игры.
 * Передает в FSM сформированный сигнал. Получает состояние FSM по результату.
 * Если состояние FSM не требует действий пользователя, то снова запускается FSM
 * @param action Действие пользователя (или GUI)
 * @param hold Уточнение действия (падение фигуры, старт / завершение программы)
 */
void userInput(UserAction_t action, bool hold) {
  tetris_state state = 0;
  signal_t signal;
  signal.action = action;
  signal.signal = ACT_SIG;
  // Создание массивов при запуске программы
  if (signal.action == Start && hold) signal.signal = INIT_SIG;
  // Нажатие стрелки вниз (падение фигуры) отличается от сдвига вниз по таймеру
  if (signal.action == Down && hold) signal.signal = DROP_SIG;
  // Очистка памяти перед выходом из программы
  if (signal.action == Terminate && hold) signal.signal = DESTR_SIG;

  // Для SPAWN и ATTACHING не требуется новых сигналов,
  // сразу передаем обратно в FSM
  bool return_flag = false;
  while (!return_flag) {
    fsm(&signal, &state);
    if (state != SPAWN && state != ATTACHING) {
      return_flag = true;
    }
  }
}

/**
 * @brief Передача в GUI данных о состоянии игры
 *
 * Передает специальный сигнал в FSM, только для получения информации об
 * игре, без выполнения действия. Результат возвращает в GUI.
 */
GameInfo_t updateCurrentState() {
  tetris_state state;  // Нужен только из-за унификации интерфейса fsm
  signal_t signal;
  signal.action = Up;
  signal.signal = GET_SIG;
  GameInfo_t res = fsm(&signal, &state);
  return res;
}
