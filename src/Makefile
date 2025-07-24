CFLAGS_LIB = -Wall -Wextra -Werror -std=c11
CFLAGS_TEST = $(CFLAGS_LIB) -fprofile-arcs -ftest-coverage
LDFLAGS_TEST = -lcheck -lsubunit -lm

BACK_DIR = brick_game/tetris
FRONT_DIR = gui/cli
OBJ_DIR = obj
OBJ_BACK_DIR = $(OBJ_DIR)/back
OBJ_FRONT_DIR = $(OBJ_DIR)/front
OBJ_TEST_DIR = obj_test
TEST_DIR = tests
COMPILED_TESTS = obj_test
HTML_DIR = html_report
DIST_DIR = s21_tetris_v1.0
DIST_NAME = s21_tetris_v1.tar.gz
INSTALL_DIR = build

TEST_EXEC = $(COMPILED_TESTS)/tetris_tests
TETRIS_EXEC = tetris

BACKS = $(wildcard $(BACK_DIR)/*.c)
FRONTS = $(wildcard $(FRONT_DIR)/*.c)
TESTS= $(wildcard $(TEST_DIR)/*.c)
OBJS = $(BACKS:$(BACK_DIR)/%.c=$(OBJ_BACK_DIR)/%.o)
OBJS_FRONT = $(FRONTS:$(FRONT_DIR)/%.c=$(OBJ_FRONT_DIR)/%.o)
TEST_OBJS = $(BACKS:$(BACK_DIR)/%.c=$(OBJ_TEST_DIR)/%.o)
TEST_FILES_OBJS = $(TESTS:$(TEST_DIR)/%.c=$(COMPILED_TESTS)/%.o)

all: $(TETRIS_EXEC)
	./$(TETRIS_EXEC)

install: $(TETRIS_EXEC)
	@mkdir -p $(INSTALL_DIR)
	mv $(TETRIS_EXEC) $(INSTALL_DIR)/$(TETRIS_EXEC)
	@echo "Tetris was istalled in $(INSTALL_DIR)"

$(TETRIS_EXEC): $(OBJS) $(OBJS_FRONT)
	gcc -o $@ $^ -lncurses

$(OBJ_BACK_DIR)/%.o: $(BACK_DIR)/%.c
	@mkdir -p $(OBJ_BACK_DIR)
	gcc $(CFLAGS_LIB) -c $< -o $@

$(OBJ_FRONT_DIR)/%.o: $(FRONT_DIR)/%.c
	@mkdir -p $(OBJ_FRONT_DIR)
	gcc $(CFLAGS_LIB) -c $< -o $@

$(OBJ_TEST_DIR)/%.o: $(BACK_DIR)/%.c
	@mkdir -p $(OBJ_TEST_DIR)
	gcc $(CFLAGS_TEST) -c $< -o $@

$(COMPILED_TESTS)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(COMPILED_TESTS)
	gcc $(CFLAGS_LIB) -c $< -o $@

$(TEST_EXEC): $(TEST_OBJS) $(TEST_FILES_OBJS)
	gcc $(CFLAGS_TEST) -o $@ $^ $(LDFLAGS_TEST)

test: $(TEST_EXEC)
	rm -rf $(COMPILED_TESTS)/*.gcda
	./$(TEST_EXEC)

gcov_report: test
	lcov --capture --directory $(OBJ_TEST_DIR) --output-file coverage.info
	genhtml coverage.info --output-directory $(HTML_DIR)

dist: 
	@mkdir -p $(DIST_DIR)
	cp -a brick_game $(DIST_DIR)/
	cp -a gui $(DIST_DIR)/
	cp -a tests $(DIST_DIR)/
	cp -a Makefile $(DIST_DIR)/
	cp -a FSM.pdf $(DIST_DIR)/
	cp -a Doxyfile $(DIST_DIR)/
	tar -czf $(DIST_NAME) $(DIST_DIR)
	rm -rf $(DIST_DIR)

dvi:
	doxygen

uninstall:
	rm -rf $(INSTALL_DIR)/$(TETRIS_EXEC)
	@echo "Tetris was unistalled from $(INSTALL_DIR)"

clean:
	rm -rf $(OBJ_DIR) $(OBJ_TEST_DIR) $(TEST_EXEC) $(HTML_DIR) $(TETRIS_EXEC) $(DIST_NAME) doxygen coverage.info

