#ifndef FSM_BACK_H
#define FSM_BACK_H

#include <stdbool.h>

#include "../../gui/cli/s21_define.h"
#include "s21_tetris_backend.h"

tetris_state fsmOnStartMode(signal_t *signal, GameInfo_t *game_info,
                            addinfo_t *fsm_addinfo);
tetris_state fsmOnSpawnMode(GameInfo_t *game_info, addinfo_t *fsm_addinfo);
tetris_state fsmOnMovingMode(signal_t *signal, GameInfo_t *game_info,
                             addinfo_t *fsm_addinfo);
tetris_state fsmOnAttachingMode(GameInfo_t *game_info);
tetris_state fsmOnGameoverMode(signal_t *signal, GameInfo_t *game_info);
tetris_state fsmOnPauseMode(signal_t *signal, GameInfo_t *game_info);
GameInfo_t fsm(signal_t *signal, tetris_state *state);

#endif  // FSM_BACK_H
