NAME := codexion
ASAN_NAME := codexion_asan

CC := cc
CFLAGS := -Wall -Wextra -Werror
CPPFLAGS :=
LDFLAGS :=
LDLIBS := -pthread

ASAN_FLAGS := -fsanitize=address -g -O1 -fno-omit-frame-pointer

SRC_DIR := coders
SRCS := \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/args.c \
	$(SRC_DIR)/sim.c \
	$(SRC_DIR)/sim_stop.c \
	$(SRC_DIR)/time.c \
	$(SRC_DIR)/log.c \
	$(SRC_DIR)/heap.c \
	$(SRC_DIR)/dongle.c \
	$(SRC_DIR)/coder.c \
	$(SRC_DIR)/monitor.c

OBJS := $(SRCS:.c=.o)
ASAN_OBJS := $(SRCS:.c=.asan.o)

SMOKE_ARGS ?= 5 1200 200 200 200 2 10 fifo
ASAN_OPTIONS ?= detect_leaks=0
VALGRIND ?= valgrind
VALGRIND_FLAGS ?= --leak-check=full --show-leak-kinds=all --track-origins=yes --errors-for-leak-kinds=all --error-exitcode=42

.PHONY: all clean fclean re asan asan-run run valgrind-run

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

asan: $(ASAN_NAME)

$(ASAN_NAME): $(ASAN_OBJS)
	$(CC) $(LDFLAGS) $(ASAN_FLAGS) $(ASAN_OBJS) $(LDLIBS) -o $@

$(SRC_DIR)/%.asan.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(ASAN_FLAGS) -c $< -o $@

run: $(NAME)
	./$(NAME) $(SMOKE_ARGS)

asan-run: $(ASAN_NAME)
	ASAN_OPTIONS=$(ASAN_OPTIONS) ./$(ASAN_NAME) $(SMOKE_ARGS)

valgrind-run: $(NAME)
	@if ! command -v $(VALGRIND) >/dev/null 2>&1; then \
		echo "valgrind not found on this system (expected on evaluator Linux machines)."; \
		exit 1; \
	fi
	$(VALGRIND) $(VALGRIND_FLAGS) ./$(NAME) $(SMOKE_ARGS)

clean:
	rm -f $(OBJS) $(ASAN_OBJS)

fclean: clean
	rm -f $(NAME) $(ASAN_NAME)

re: fclean all
