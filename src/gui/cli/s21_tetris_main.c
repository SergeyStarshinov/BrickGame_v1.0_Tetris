/**
 * @file s21_tetris_main.c
 * @brief Стандартный Тетрис в поле 10 х 20 с 7 фигурами.
 *
 * Управление: Esc -выход, Enter -старт, P - пауза.
 * Движение фигуры - стрелками влево, вправо, вниз, пробел (вращение).
 * Дублировано на NumPad - 4, 6, 2 и 5 соответственно.
 * Подсчет очков: 100, 300, 700 и 1500 за 1, 2, 3 и 4 линии.
 * Повышение уровня с изменением скорости за каждые 600 набранных очков.
 */

#include "s21_tetris.h"

/**
 * @brief Запуск программы
 */
int main() {
  srand(time(0));
  ncursesInitialisation();
  // Выделение памяти под массивы для игры
  userInput(Start, true);

  tetrisGame();

  // Очистка памяти
  userInput(Terminate, true);

  endwin();
  return 0;
}

/**
 * @brief Игровой цикл от Старта до Завершения игры
 *
 * 4 режима интерфейса, связаны с состоянием FSM (через game_info.pause):
 * 1. Стартовый. Показывается перед началом каждой игры.
 * 2. Основной режим игры, управление фигурами.
 * 3. Пауза.
 * 4. Итоговый результат. С него переходит на Стартовый.
 * Дополнительно, EXIT_MODE - выход из игры
 *
 * В бэкэнд передаются либо данные о нажатой клавише либо
 * сигнал по таймеру о сдвиге фигуры вниз (периодичность зависит от скорости)
 */
void tetrisGame() {
  float timer = (unsigned)clock() * 1000. / CLOCKS_PER_SEC;
  clear();
  printWelcome();
  refresh();
  GameInfo_t game_info = updateCurrentState();
  while (game_info.pause != EXIT_MODE) {
    UserAction_t action;
    bool hold;
    action = defineAction(&hold, &timer, game_info.speed);
    // Up - нажата любая кнопка, кроме управляющих. Игнорируется.
    if (action != Up) {
      userInput(action, hold);
      game_info = updateCurrentState();
      printGameScreen(&game_info);
      refresh();
    }
  }
}

/**
 * @brief Запуск и начальная настройка функций ncurses
 */
void ncursesInitialisation() {
  initscr();
  noecho();
  curs_set(0);
  timeout(10);
  // Настройка цветов для отображения фигур
  start_color();
  init_pair(1, COLOR_MAGENTA, 0);
  init_pair(2, COLOR_RED, 0);
  init_pair(3, COLOR_CYAN, 0);
  init_pair(4, COLOR_GREEN, 0);
  init_pair(5, COLOR_WHITE, 0);
  init_pair(6, COLOR_YELLOW, 0);
  init_pair(7, COLOR_BLUE, 0);
  // Для чтения функциональных клавиш (стрелки)
  keypad(stdscr, TRUE);
}
