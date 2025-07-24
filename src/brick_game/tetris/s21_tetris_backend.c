/**
 * @file s21_tetris_backend.c
 * @brief Вспомогательные функции backend
 */
#include "s21_tetris_backend.h"

/**
 * @brief Создание массивов для игры и инициализация. При ошибке выделения
 * памяти game_info.pause устанавливается в EXIT_MODE, для выхода из программы.
 *
 * @param game_info Информация о состоянии игры для GUI. Устанавливаются
 * начальные значения.
 * @param fsm_addinfo Доп. инфо FSM. Устанавливаются начальные значения.
 */
void tetrisCreate(GameInfo_t *game_info, addinfo_t *fsm_addinfo) {
  game_info->field = createMatrix(FIELD_ROWS, FIELD_COLUMNS);
  game_info->next = createMatrix(PIECE_ROWS, PIECE_COLUMNS);
  fsm_addinfo->piece = createMatrix(PIECE_ROWS, PIECE_COLUMNS);
  if (game_info->field == NULL || game_info->next == NULL ||
      fsm_addinfo->piece == NULL) {
    game_info->pause = EXIT_MODE;
  } else {
    tetrisInit(game_info, fsm_addinfo);
  }
}

/**
 * @brief Очистка памяти в конце работы программы.
 *
 * @param game_info Информация о состоянии игры. Очистка полей field, next.
 * @param fsm_addinfo Доп. инфо FSM. Очистка поля piece.
 */
void tetrisDestroy(GameInfo_t *game_info, addinfo_t *fsm_addinfo) {
  if (game_info->field != NULL) {
    free(game_info->field[0]);
    free(game_info->field);
    game_info->field = NULL;
  }
  if (game_info->next != NULL) {
    free(game_info->next[0]);
    free(game_info->next);
    game_info->next = NULL;
  }
  if (fsm_addinfo->piece != NULL) {
    free(fsm_addinfo->piece[0]);
    free(fsm_addinfo->piece);
    fsm_addinfo->piece = NULL;
  }
}

/**
 * @brief Сброс начальных значений при старте каждой игры.
 */
void tetrisInit(GameInfo_t *game_info, addinfo_t *fsm_addinfo) {
  for (int i = 0; i < FIELD_ROWS; i++)
    for (int j = 0; j < FIELD_COLUMNS; j++) game_info->field[i][j] = 0;
  game_info->score = 0;
  game_info->level = 1;
  game_info->speed = START_SPEED;
  game_info->pause = START_MODE;
  game_info->high_score = getHighScore();

  fsm_addinfo->col_pos = 0;
  fsm_addinfo->row_pos = 0;
  fsm_addinfo->piece_id = 0;
  fsm_addinfo->piece_rot_id = 0;
  fsm_addinfo->next_id = 0;
  fsm_addinfo->next_rot_id = 0;
  genNextPiece(game_info, fsm_addinfo);
}

/**
 * @brief Создание двумерного массива размера rows x cols.
 * При ошибке выделения памяти возвращает NULL.
 */
int **createMatrix(int rows, int cols) {
  int **res = (int **)calloc(rows, sizeof(int *));
  int *matrix = (int *)calloc(rows * cols, sizeof(int));
  if (res == NULL || matrix == NULL) {
    if (res) free(res);
    if (matrix) free(matrix);
    res = NULL;
  } else {
    for (int i = 0; i < rows; i++) res[i] = matrix + cols * i;
  }
  return res;
}

/**
 * @brief Генерация новой следующей фигуры. Фигура определяется по двум
 * случайным числам - id фигуры и id вращения.
 * @param game_info Информация о состоянии игры. Заполняется next.
 * @param fsm_addinfo Доп. инфо FSM. Заполняются поля с id для next.
 */
void genNextPiece(GameInfo_t *game_info, addinfo_t *fsm_addinfo) {
  fsm_addinfo->next_id = rand() % 7;
  fsm_addinfo->next_rot_id = rand() % 4;
  getPiece(game_info->next, fsm_addinfo->next_id, fsm_addinfo->next_rot_id);
}

/**
 * @brief Передает "шаблон" фигуры по ее id. Цифры заполнения соответствуют
 * цветовой схеме для этой фигуры.
 *
 * @param dst массив, куда переносится "шаблон" фигуры.
 * @param id Номер, определяющий тип фигуры (от 0 до 6).
 * @param rot_id Номер вращения, определяющий поворот фигуры (от 0 до 4).
 */
void getPiece(int **dst, int id, int rot_id) {
  // static чтобы не пересоздавать массив при каждом вызове
  static int src[7][4][PIECE_ROWS][PIECE_COLUMNS] = {
      {{{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}},
      {{{2, 2, 2, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 0, 2, 0}, {0, 0, 2, 0}, {0, 0, 2, 0}, {0, 0, 2, 0}},
       {{2, 2, 2, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 0, 2, 0}, {0, 0, 2, 0}, {0, 0, 2, 0}, {0, 0, 2, 0}}},
      {{{0, 3, 3, 0}, {0, 0, 3, 3}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 0, 3, 0}, {0, 3, 3, 0}, {0, 3, 0, 0}, {0, 0, 0, 0}},
       {{0, 3, 3, 0}, {0, 0, 3, 3}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 0, 3, 0}, {0, 3, 3, 0}, {0, 3, 0, 0}, {0, 0, 0, 0}}},
      {{{0, 0, 4, 4}, {0, 4, 4, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 4, 0, 0}, {0, 4, 4, 0}, {0, 0, 4, 0}, {0, 0, 0, 0}},
       {{0, 0, 4, 4}, {0, 4, 4, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 4, 0, 0}, {0, 4, 4, 0}, {0, 0, 4, 0}, {0, 0, 0, 0}}},
      {{{0, 5, 5, 5}, {0, 0, 0, 5}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 0, 5, 0}, {0, 0, 5, 0}, {0, 5, 5, 0}, {0, 0, 0, 0}},
       {{0, 5, 0, 0}, {0, 5, 5, 5}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 5, 5, 0}, {0, 5, 0, 0}, {0, 5, 0, 0}, {0, 0, 0, 0}}},
      {{{0, 6, 6, 6}, {0, 6, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 6, 6, 0}, {0, 0, 6, 0}, {0, 0, 6, 0}, {0, 0, 0, 0}},
       {{0, 0, 0, 6}, {0, 6, 6, 6}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 6, 0, 0}, {0, 6, 0, 0}, {0, 6, 6, 0}, {0, 0, 0, 0}}},
      {{{0, 7, 7, 7}, {0, 0, 7, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 0, 7, 0}, {0, 7, 7, 0}, {0, 0, 7, 0}, {0, 0, 0, 0}},
       {{0, 0, 7, 0}, {0, 7, 7, 7}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 7, 0, 0}, {0, 7, 7, 0}, {0, 7, 0, 0}, {0, 0, 0, 0}}}};
  for (int i = 0; i < PIECE_ROWS; i++)
    for (int j = 0; j < PIECE_COLUMNS; j++) dst[i][j] = src[id][rot_id][i][j];
}

/**
 * @brief Перенос фигуры (шаблона и id) из next в текущую. Сброс
 * координат фигуры на поле на стартовые.
 * @param game_info Информация о состоянии игры. Не изменяется.
 * @param fsm_addinfo Доп. инфо FSM. Заполняется шаблон из game_info->next,
 * id текущей фигуры, сбрасываются координаты положения фигуры на начальные.
 */
void fromNextIntoCurrent(GameInfo_t *game_info, addinfo_t *fsm_addinfo) {
  fsm_addinfo->piece_id = fsm_addinfo->next_id;
  fsm_addinfo->piece_rot_id = fsm_addinfo->next_rot_id;
  fsm_addinfo->row_pos = 0;
  fsm_addinfo->col_pos = 3;
  for (int i = 0; i < PIECE_ROWS; i++)
    for (int j = 0; j < PIECE_COLUMNS; j++) {
      fsm_addinfo->piece[i][j] = game_info->next[i][j];
    }
}

/**
 * @brief Проверка, возможно ли размещение текущей фигуры по текущим
 * координатам.
 *
 * Для непустых клеток шаблона фигуры проверяется выход за границы поля и
 * совмещение с текущим заполнением поля.
 * @param game_info Информация о состоянии игры (текущего поля). Не изменяется.
 * @param fsm_addinfo Доп. инфо FSM (текущая фигура). Не изменяется.
 * @return 0 - если фигуру можно разместить, 1 - если нельзя.
 */
int checkPlacePiece(GameInfo_t *game_info, addinfo_t *fsm_addinfo) {
  int res = SUCCESSFUL_EXIT;
  for (int i = 0; i < PIECE_ROWS && !res; i++) {
    for (int j = 0; j < PIECE_COLUMNS && !res; j++) {
      if (fsm_addinfo->piece[i][j] &&
          (i + fsm_addinfo->row_pos > 19 || j + fsm_addinfo->col_pos < 0 ||
           j + fsm_addinfo->col_pos > 9 ||
           game_info
               ->field[i + fsm_addinfo->row_pos][j + fsm_addinfo->col_pos])) {
        res = FAILURE_EXIT;
      }
    }
  }
  return res;
}

/**
 * @brief Размещение фигуры на поле по текущим координатам.
 *
 * @param game_info Информация о состоянии игры. Изменяется field.
 * @param fsm_addinfo Доп. инфо FSM (текущая фигура). Не изменяется.
 */
void placePieceOnField(GameInfo_t *game_info, addinfo_t *fsm_addinfo) {
  for (int i = 0; i < PIECE_ROWS; i++) {
    for (int j = 0; j < PIECE_COLUMNS; j++) {
      if (fsm_addinfo->piece[i][j]) {
        game_info->field[i + fsm_addinfo->row_pos][j + fsm_addinfo->col_pos] =
            fsm_addinfo->piece[i][j];
      }
    }
  }
}

/**
 * @brief Удаление текущей фигуры с поля перед ее перемещением.
 * @param game_info Информация о состоянии игры. Изменяется field.
 * @param fsm_addinfo Доп. инфо FSM (текущая фигура). Не изменяется.
 */
void removePieceFromField(GameInfo_t *game_info, addinfo_t *fsm_addinfo) {
  for (int i = 0; i < PIECE_ROWS; i++) {
    for (int j = 0; j < PIECE_COLUMNS; j++) {
      if (fsm_addinfo->piece[i][j]) {
        game_info->field[i + fsm_addinfo->row_pos][j + fsm_addinfo->col_pos] =
            0;
      }
    }
  }
}

/**
 * @brief Сдвиг фигуры на поле вправо или влево на 1. При невозможности
 * перемещения фигура остается на месте.
 * @param game_info Информация о состоянии игры. Изменяется field.
 * @param fsm_addinfo Доп. инфо FSM (текущая фигура). Изменяются координаты.
 * @param shift Смещение фигуры. 1 - для смещения вправо, -1 - влево.
 */
void shiftPiece(GameInfo_t *game_info, addinfo_t *fsm_addinfo, int shift) {
  removePieceFromField(game_info, fsm_addinfo);
  fsm_addinfo->col_pos += shift;
  if (checkPlacePiece(game_info, fsm_addinfo)) {
    fsm_addinfo->col_pos -= shift;
  }
  placePieceOnField(game_info, fsm_addinfo);
}

/**
 * @brief Вращение фигуры. При невозможности вращения фигура остается на месте.
 *
 * Текущая фигура заменяется на фигуру со следующим id вращения, проверяется
 * возможность ее помещения на поле. Если невозможно, то возвращается старая
 * фигура. Итоговая фигура помещается на поле по тем же координатам.
 * @param game_info Информация о состоянии игры. Изменяется field.
 * @param fsm_addinfo Доп. инфо FSM (текущая фигура). Изменяются данные о
 * фигуре.
 */
void rotatePiece(GameInfo_t *game_info, addinfo_t *fsm_addinfo) {
  removePieceFromField(game_info, fsm_addinfo);
  int tmp_rot_id = fsm_addinfo->piece_rot_id;
  fsm_addinfo->piece_rot_id = (fsm_addinfo->piece_rot_id + 1) % 4;
  getPiece(fsm_addinfo->piece, fsm_addinfo->piece_id,
           fsm_addinfo->piece_rot_id);
  if (checkPlacePiece(game_info, fsm_addinfo)) {
    fsm_addinfo->piece_rot_id = tmp_rot_id;
    getPiece(fsm_addinfo->piece, fsm_addinfo->piece_id,
             fsm_addinfo->piece_rot_id);
  }
  placePieceOnField(game_info, fsm_addinfo);
}

/**
 * @brief Падение фигуры до препятствия или края поля.
 * @param game_info Информация о состоянии игры. Изменяется field.
 * @param fsm_addinfo Доп. инфо FSM (текущая фигура). Изменяются координаты.
 */
void dropPiece(GameInfo_t *game_info, addinfo_t *fsm_addinfo) {
  removePieceFromField(game_info, fsm_addinfo);
  do {
    fsm_addinfo->row_pos++;
  } while (!checkPlacePiece(game_info, fsm_addinfo));
  fsm_addinfo->row_pos--;
  placePieceOnField(game_info, fsm_addinfo);
}

/**
 * @brief Смещение фигуры на 1 позицию вниз. При невозможности - переход FSM в
 * режим ATTACHING.
 * @param game_info Информация о состоянии игры. Изменяется field.
 * @param fsm_addinfo Доп. инфо FSM (текущая фигура). Изменяются координаты.
 * @return Обновленное состояние FSM.
 */
tetris_state movePieceDown(GameInfo_t *game_info, addinfo_t *fsm_addinfo) {
  tetris_state state = MOVING;
  removePieceFromField(game_info, fsm_addinfo);
  fsm_addinfo->row_pos++;
  if (checkPlacePiece(game_info, fsm_addinfo)) {
    fsm_addinfo->row_pos--;
    state = ATTACHING;
  }
  placePieceOnField(game_info, fsm_addinfo);
  return state;
}

/**
 * @brief Проверка строки на заполненность.
 * @param row Проверяемая строка.
 * @return 1 - если строка заполнена (нет нолей), 0 - иначе.
 */
int isRowFilled(const int *row) {
  int res = 1;
  for (int i = 0; i < FIELD_COLUMNS && res; i++) {
    if (!row[i]) res = 0;
  }
  return res;
}

/**
 * @brief Удаление строки путем сдвига вниз всех строк "выше".
 * @param field Массив поля, в котором происходит удаление строки.
 * @param row Номер удаляемой строки.
 */
void shiftField(int **field, int row) {
  for (int i = row; i > 0; i--) {
    for (int j = 0; j < FIELD_COLUMNS; j++) {
      field[i][j] = field[i - 1][j];
    }
  }
  for (int j = 0; j < FIELD_COLUMNS; j++) field[0][j] = 0;
}

/**
 * @brief Запись рекорда в файл.
 *
 * В случае, если установлен новый рекорд, он записывается в файл.
 * Информация о предыдущем рекорде уничтожается.
 * @param game_info Информация о состоянии игры. Не изменяется.
 */
void saveHighScore(GameInfo_t *game_info) {
  if (game_info->score >= game_info->high_score) {
    FILE *file = fopen("highscore.txt", "w");
    fprintf(file, "%d", game_info->high_score);
    fclose(file);
  }
}

/**
 * @brief Считывание рекорда из файла.
 * @return Значение рекорда из файла. Если файл не найден или пустой
 * (некорректный), то возвращается 0.
 */
int getHighScore() {
  int high_score = 0;
  int tmp;
  FILE *file = fopen("highscore.txt", "r");
  if (file != NULL) {
    if (fscanf(file, "%d", &tmp) == 1) {
      high_score = tmp;
    }
    fclose(file);
  }
  return high_score;
}