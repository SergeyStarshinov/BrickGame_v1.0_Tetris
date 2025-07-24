#ifndef TETRIS_DEFINE_H
#define TETRIS_DEFINE_H

#define START_SPEED 8400
#define STEP_SPEED 800
#define FIELD_ROWS 20
#define FIELD_COLUMNS 10
#define PIECE_ROWS 4
#define PIECE_COLUMNS 4

// Значение game_info.pause для режимов GUI
#define GAME_MODE 0
#define PAUSE_MODE 1
#define START_MODE 2
#define EXIT_MODE 3
#define GAMEOVER_MODE 4

// Коды некоторых клавиш
#define ESCAPE_KEY 27
#define ENTER_KEY 10

// Начальные координаты для печати доп.инфо игры
#define SCORE_ROW 1
#define SCORE_COL FIELD_COLUMNS * 2 + 2

// Символы для печати фигур, каждый "пиксель" из двух символов
#define LEFT_CHAR '['
#define RIGHT_CHAR ']'

// состояния для FSM
typedef enum {
  START = 0,
  SPAWN,
  MOVING,
  ATTACHING,
  PAUSE,
  GAMEOVER,
  EXIT_STATE
} tetris_state;

// Варианты действий пользователя (по ТЗ)
typedef enum {
  Start = 0,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

/// @brief Структура данных для отрисовки в интерфейсе (по ТЗ)
typedef struct {
  /// Игровое поле
  int **field;
  /// Следующая фигура
  int **next;
  /// Количество набранных очков
  int score;
  /// Текущий рекорд
  int high_score;
  /// Текущий уровень
  int level;
  /// Скорость игры
  int speed;
  /// Режим Паузы и другие режимы GUI
  int pause;
} GameInfo_t;

#define SUCCESSFUL_EXIT 0
#define FAILURE_EXIT 1

#endif  // TETRIS_DEFINE_H