/**
 * @file s21_tetris_fsm.c
 * @brief Реализация автомата конечных состояний (FSM).
 */
#include "s21_tetris_fsm.h"

/**
 * @brief Автомат конечных состояний (FSM).
 *
 * В зависимости от текущего состояния и пришедшего сигнала выполняются
 * определенные действия и, если нужно, переход в другое состояние.
 * Схема работы FSM описана в файле FSM.pdf
 * @param signal Обрабатываемый сигнал.
 * @param state Состояние FSM после обработки сигнала.
 */
GameInfo_t fsm(signal_t *signal, tetris_state *state) {
  // Текущий режим FSM
  static tetris_state fsm_state = START;
  // Информация о состоянии игры для фронтенд (по ТЗ)
  static GameInfo_t game_info = {NULL, NULL, 0, 0, 0, 0, 0};
  // Доп.инфо: о текущей фигуре, id следующей фигуры
  static addinfo_t fsm_addinfo = {NULL, 0, 0, 0, 0, 0, 0};

  if (signal->signal == INIT_SIG) {
    tetrisCreate(&game_info, &fsm_addinfo);
  } else if (signal->signal == DESTR_SIG) {
    tetrisDestroy(&game_info, &fsm_addinfo);
  } else if (signal->signal != GET_SIG) {
    if (fsm_state == START) {
      fsm_state = fsmOnStartMode(signal, &game_info, &fsm_addinfo);
    } else if (fsm_state == SPAWN) {
      fsm_state = fsmOnSpawnMode(&game_info, &fsm_addinfo);
    } else if (fsm_state == MOVING) {
      fsm_state = fsmOnMovingMode(signal, &game_info, &fsm_addinfo);
    } else if (fsm_state == ATTACHING) {
      fsm_state = fsmOnAttachingMode(&game_info);
    } else if (fsm_state == PAUSE) {
      fsm_state = fsmOnPauseMode(signal, &game_info);
    } else if (fsm_state == GAMEOVER) {
      fsm_state = fsmOnGameoverMode(signal, &game_info);
    }
  }
  *state = fsm_state;
  return game_info;
}

/**
 * @brief Действия из состояния START.
 *
 * Для действия Start - инициализация игры и переход в SPAWN.
 * Для действия Terminate - переход в EXIT_STATE для окончания работы программы.
 * Остальные действия игнорируются.
 * @param signal Обрабатываемый сигнал.
 * @param game_info Информация о состоянии игры для GUI. Изменяется при
 * инициализации, а также поле pause меняется при смене режима игры
 * @param fsm_addinfo Доп. инфо FSM. Изменяется при инициализации.
 * @return Обновленное состояние FSM
 */
tetris_state fsmOnStartMode(signal_t *signal, GameInfo_t *game_info,
                            addinfo_t *fsm_addinfo) {
  tetris_state state = START;
  if (signal->action == Start) {
    tetrisInit(game_info, fsm_addinfo);
    state = SPAWN;
    game_info->pause = GAME_MODE;
  } else if (signal->action == Terminate) {
    state = EXIT_STATE;
    game_info->pause = EXIT_MODE;
  }
  return state;
}

/**
 * @brief Действия из состояния SPAWN.
 *
 * Не требует действий пользователя.
 * Следующая фигура переносится на поле, с переходом в состояние MOVING.
 * При невозможности перемещения фигуры игра заканчивается - переход в GAMEOVER.
 * @param game_info Информация о состоянии игры для GUI. Изменяется помещением
 * новой фигуры на поле.
 * @param fsm_addinfo Доп. инфо FSM. Изменяется информация о текущей фигуре, id
 * следующей фигуры.
 * @return Обновленное состояние FSM
 */
tetris_state fsmOnSpawnMode(GameInfo_t *game_info, addinfo_t *fsm_addinfo) {
  tetris_state state = SPAWN;
  fromNextIntoCurrent(game_info, fsm_addinfo);
  genNextPiece(game_info, fsm_addinfo);
  if (checkPlacePiece(game_info, fsm_addinfo)) {
    state = GAMEOVER;
    game_info->pause = GAMEOVER_MODE;
  } else {
    state = MOVING;
  }
  // Фигура рисуется в любом случае, чтобы показать заполненность стакана
  placePieceOnField(game_info, fsm_addinfo);
  return state;
}

/**
 * @brief Действия из состояния MOVING.
 *
 * Перемещение фигуры по полю, в зависимости от полученного сигнала.
 * При невозможности перемещения вниз (или падении) переход в состояние
 * ATTACHING. Если нажата Пауза - переход в состояние PAUSE. Если нажат Esc -
 * окончание игры и переход в GAMEOVER.
 * @param signal Обрабатываемый сигнал.
 * @param game_info Информация о состоянии игры для GUI. Изменяется при
 * перемещении фигуры по полю.
 * @param fsm_addinfo Доп. инфо FSM. Изменяются координаты текущий фигуры и ее
 * форма и id при вращении.
 * @return Обновленное состояние FSM
 */
tetris_state fsmOnMovingMode(signal_t *signal, GameInfo_t *game_info,
                             addinfo_t *fsm_addinfo) {
  tetris_state state = MOVING;
  if (signal->action == Terminate) {
    state = GAMEOVER;
    game_info->pause = GAMEOVER_MODE;
  } else if (signal->action == Pause) {
    state = PAUSE;
    game_info->pause = PAUSE_MODE;
  } else if (signal->action == Left) {
    shiftPiece(game_info, fsm_addinfo, -1);
  } else if (signal->action == Right) {
    shiftPiece(game_info, fsm_addinfo, 1);
  } else if (signal->action == Action) {
    rotatePiece(game_info, fsm_addinfo);
  } else if (signal->action == Down && signal->signal == DROP_SIG) {
    dropPiece(game_info, fsm_addinfo);
    state = ATTACHING;
  } else if (signal->action == Down) {
    state = movePieceDown(game_info, fsm_addinfo);
  }
  return state;
}

/**
 * @brief Действия из состояния ATTACHING.
 *
 * Не требует действий пользователя.
 * Если есть заполненные строки, то они удаляются, подсчитываются очки, меняется
 * уровень и скорость. После этого переход в состояние SPAWN.
 * @param game_info Информация о состоянии игры для GUI. Изменяется статистика
 * игры.
 * @return Обновленное состояние FSM
 */
tetris_state fsmOnAttachingMode(GameInfo_t *game_info) {
  // Удаление строк с подсчетом количества
  int count = 0;
  for (int i = 0; i < FIELD_ROWS; i++) {
    if (isRowFilled(game_info->field[i])) {
      count++;
      shiftField(game_info->field, i);
    }
  }
  // Подсчет очков за удаленные строки (согласно ТЗ)
  if (count == 1) {
    game_info->score += 100;
  } else if (count == 2) {
    game_info->score += 300;
  } else if (count == 3) {
    game_info->score += 700;
  } else if (count == 4) {
    game_info->score += 1500;
  }
  // Изменение скорости, уровня, рекорда
  if (count > 0) {
    if (game_info->score > game_info->high_score) {
      game_info->high_score = game_info->score;
    }
    game_info->level = 1 + game_info->score / 600;
    if (game_info->level > 10) game_info->level = 10;
    game_info->speed = START_SPEED - (game_info->level - 1) * STEP_SPEED;
  }
  return SPAWN;
}

/**
 * @brief Действия из состояния GAMEOVER.
 *
 * Если нажата кнопка (не запрос на информацию и не таймер), то переход в
 * состояние START и запись информации о рекорде в файл.
 * @param signal Обрабатываемый сигнал.
 * @param game_info Информация о состоянии игры для GUI. Изменяется поле pause,
 * которое хранит режим игры для GUI.
 * @return Обновленное состояние FSM
 */
tetris_state fsmOnGameoverMode(signal_t *signal, GameInfo_t *game_info) {
  tetris_state state = GAMEOVER;
  if (signal->action != Up &&
      !(signal->action == Down && signal->signal == ACT_SIG)) {
    state = START;
    game_info->pause = START_MODE;
    game_info->speed = START_SPEED;
    saveHighScore(game_info);
  }
  return state;
}

/**
 * @brief Действия из состояния PAUSE.
 *
 * Для действия Pause - переход в состояние MOVING.
 * Для действия Terminate - переход в состояние GAMEOVER.
 * Остальные действия игнорируются.
 * @param signal Обрабатываемый сигнал.
 * @param game_info Информация о состоянии игры для GUI. Изменяется поле pause,
 * которое хранит режим игры для GUI.
 * @return Обновленное состояние FSM
 */
tetris_state fsmOnPauseMode(signal_t *signal, GameInfo_t *game_info) {
  tetris_state state = PAUSE;
  if (signal->action == Pause) {
    game_info->pause = GAME_MODE;
    state = MOVING;
  } else if (signal->action == Terminate) {
    game_info->pause = GAMEOVER_MODE;
    state = GAMEOVER;
  }
  return state;
}
