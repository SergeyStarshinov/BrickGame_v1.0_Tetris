/**
 * @file tests_main.c
 * @brief Основной файл для запуска тестов
 */
#include "tests_main.h"

int main() {
  int number_failed;
  Suite *s = test_init();
  SRunner *sr = srunner_create(s);
  srunner_add_suite(sr, test_frontend_mode());
  srunner_add_suite(sr, test_fsm_mode());
  srunner_add_suite(sr, test_backend_utils());
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  printf("%s\n", (number_failed == 0) ? "SUCCESS" : "FAIL");
}

/**
 * @brief Функция сравнения двух матриц одинакового размера
 * @return 0 - если матрицы равны, 1 - если не равны
 */
int compareMatrix(int rows, int cols, int **matrix_1, int **matrix_2) {
  int res = SUCCESSFUL_EXIT;
  for (int i = 0; i < rows && !res; i++)
    for (int j = 0; j < cols && !res; j++)
      if (matrix_1[i][j] != matrix_2[i][j]) res = FAILURE_EXIT;
  return res;
}

/**
 * @brief Заполняет все поле нулями
 */
void emptyField(int **field) {
  for (int i = 0; i < FIELD_ROWS; i++)
    for (int j = 0; j < FIELD_COLUMNS; j++) field[i][j] = 0;
}