#ifndef API_BACK_H
#define API_BACK_H

#include <stdbool.h>

#include "../../gui/cli/s21_define.h"

void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState();

#endif  // API_BACK_H