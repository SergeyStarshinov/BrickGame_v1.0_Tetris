/**
 * @file s21_tetris_frontend.c
 * @brief Функции для отрисовки интерфейса
 */

#include "s21_tetris.h"

/**
 * @brief Отрисовка окна игры в зависимости от режима
 * @param game_info Инфо о текущем состоянии игры
 */
void printGameScreen(GameInfo_t *game_info) {
  if (game_info->pause == START_MODE) {
    clear();
    printWelcome();
  } else if (game_info->pause == PAUSE_MODE) {
    mvprintw(SCORE_ROW + 19, SCORE_COL + 6, "%s", "PAUSE");
  } else if (game_info->pause == GAME_MODE) {
    printGlass(game_info);
    printGameStat(game_info);
  } else if (game_info->pause == GAMEOVER_MODE) {
    printGlass(game_info);
    printGameover(game_info->score);
  }
}

/**
 * @brief Отрисовка стартового окна
 */
void printWelcome() {
  printBorders(FIELD_ROWS + 1, FIELD_COLUMNS * 2 + 21);
  printTLine(FIELD_ROWS + 1, FIELD_COLUMNS * 2 + 1);
  printAddInfo();
  mvprintw(3, 6, "Welcome to");
  mvprintw(5, 6, "s21_Tetris");
  mvprintw(7, 8, "Press:");
  mvprintw(9, 1, "\"Enter\" - Start game");
  mvprintw(11, 5, "\"Esc\" - Exit");
}

/**
 * @brief Отрисовка внешней рамки
 */
void printBorders(int height, int width) {
  for (int i = 1; i < width; i++) mvaddch(0, i, ACS_HLINE);
  for (int i = 1; i < width; i++) mvaddch(height, i, ACS_HLINE);
  for (int i = 1; i < height; i++) {
    mvaddch(i, 0, ACS_VLINE);
    mvaddch(i, width, ACS_VLINE);
  }
  mvaddch(0, 0, ACS_ULCORNER);
  mvaddch(0, width, ACS_URCORNER);
  mvaddch(height, 0, ACS_LLCORNER);
  mvaddch(height, width, ACS_LRCORNER);
}

/**
 * @brief Отрисовка вертикальной линии, между полем и инфо
 */
void printTLine(int height, int col) {
  for (int i = 1; i < height; i++) {
    mvaddch(i, col, ACS_VLINE);
  }
  mvaddch(0, col, ACS_TTEE);
  mvaddch(height, col, ACS_BTEE);
}

/**
 * @brief Печать заголовков доп.информации
 */
void printAddInfo() {
  mvprintw(SCORE_ROW, SCORE_COL, "%16s", "LEVEL     SCORE");
  mvprintw(SCORE_ROW + 3, SCORE_COL, "%16s", "HI-SCORE");
  mvprintw(SCORE_ROW + 6, SCORE_COL, "%16s", "NEXT");
  mvprintw(SCORE_ROW + 13, SCORE_COL, "%16s", "Esc - Exit");
  mvprintw(SCORE_ROW + 14, SCORE_COL, "%16s", "P - Pause");
  mvprintw(SCORE_ROW + 15, SCORE_COL, "%16s", "ARROWS - Move");
  mvprintw(SCORE_ROW + 16, SCORE_COL, "%16s", "and Drop Piece");
  mvprintw(SCORE_ROW + 17, SCORE_COL, "%16s", "Space - Rotate");
}

/**
 * @brief Отрисовка игрового поля
 * @param game_info Инфо о текущем состоянии игры
 */
void printGlass(GameInfo_t *game_info) {
  for (int i = 0; i < FIELD_ROWS; i++) {
    for (int j = 0; j < FIELD_COLUMNS; j++) {
      if (game_info->field[i][j]) {
        mvaddch(i + 1, j * 2 + 1,
                LEFT_CHAR | COLOR_PAIR(game_info->field[i][j]));
        mvaddch(i + 1, j * 2 + 2,
                RIGHT_CHAR | COLOR_PAIR(game_info->field[i][j]));
      } else {
        mvaddch(i + 1, j * 2 + 1, ' ');
        mvaddch(i + 1, j * 2 + 2, '.');
      }
    }
  }
}

/**
 * @brief Отрисовка игровой статистики
 * @param game_info Инфо о текущем состоянии игры
 */
void printGameStat(GameInfo_t *game_info) {
  mvprintw(SCORE_ROW + 1, FIELD_COLUMNS * 2 + 2, "%6.5d", game_info->level);
  mvprintw(SCORE_ROW + 1, FIELD_COLUMNS * 2 + 12, "%6.5d", game_info->score);
  mvprintw(SCORE_ROW + 4, FIELD_COLUMNS * 2 + 2, "%16.5d",
           game_info->high_score);
  printNext(game_info);
  // Затирание пробелами места, где пишется PAUSE
  mvprintw(SCORE_ROW + 19, SCORE_COL + 6, "%s", "     ");
}

/**
 * @brief Отрисовка следующей фигуры
 * @param game_info Инфо о текущем состоянии игры
 */
void printNext(GameInfo_t *game_info) {
  for (int i = 0; i < PIECE_ROWS; i++) {
    for (int j = 0; j < PIECE_COLUMNS; j++) {
      if (game_info->next[i][j]) {
        mvaddch(i + SCORE_ROW + 8, j * 2 + SCORE_COL + 6,
                LEFT_CHAR | COLOR_PAIR(game_info->next[i][j]));
        mvaddch(i + SCORE_ROW + 8, j * 2 + SCORE_COL + 7,
                RIGHT_CHAR | COLOR_PAIR(game_info->next[i][j]));
      } else {
        mvaddch(i + SCORE_ROW + 8, j * 2 + SCORE_COL + 6, ' ');
        mvaddch(i + SCORE_ROW + 8, j * 2 + SCORE_COL + 7, ' ');
      }
    }
  }
}

/**
 * @brief Определение действия по таймингу (в приоритете) или нажатой клавише
 * @param hold Устанавливается в true только для падения фигуры.
 * @param timer Последнее срабатываание таймера. Для проверки прошедшего периода
 * времени с прошлого сдвига по таймеру, обновляется при срабатывании таймера.
 * @param speed Скорость игры. От нее зависит период срабатывания таймера.
 * @return Вариант действий пользователя в формате для передачи в FSM.
 */
UserAction_t defineAction(bool *hold, float *timer, int speed) {
  UserAction_t action;
  float curr_time = (unsigned)clock() * 1000. / CLOCKS_PER_SEC;
  int key = 0;
  *hold = false;
  if (curr_time - *timer > speed / 1000 || curr_time < *timer) {
    action = Down;
    *timer = curr_time;
  } else {
    key = getch();
    action = getAction(key);
    if (action == Down) *hold = true;
  }
  return action;
}

/**
 * @brief Определение действия по нажатой клавише
 * @param key Нажатая клавиша. Для всех неиспользуемых клавиш - Up.
 * @return Вариант действий пользователя в формате для передачи в FSM.
 */
UserAction_t getAction(int key) {
  UserAction_t res = Up;
  if (key == KEY_DOWN || key == '2')
    res = Down;
  else if (key == KEY_LEFT || key == '4')
    res = Left;
  else if (key == KEY_RIGHT || key == '6')
    res = Right;
  else if (key == ESCAPE_KEY)
    res = Terminate;
  else if (key == ENTER_KEY)
    res = Start;
  else if (key == 'p' || key == 'P')
    res = Pause;
  else if (key == ' ' || key == '5')
    res = Action;
  return res;
}

/**
 * @brief Печать сообщения об окончании игры.
 */
void printGameover(int score) {
  mvprintw(6, 6, "GAME  OVER");
  mvprintw(7, 6, "Your final");
  mvprintw(8, 6, " Score is ");
  mvprintw(9, 6, "%8.6d  ", score);
}