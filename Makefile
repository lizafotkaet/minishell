NAME    = minishell

CC      = cc
# CFLAGS  = -Wall -Wextra -Werror
CFLAGS = -Wall -Wextra -Werror -I include -I libft
LDFLAGS = -lreadline
INCLUDES	= -I include -I libft

# ── libft ──────────────────────────────────────────────
LIBFT_DIR	= libft
LIBFT		= $(LIBFT_DIR)/libft.a

#https://github.com/lizafotkaet/libft
# ── sources ────────────────────────────────────────────
SRC_DIR		= src
OBJ_DIR		= obj

SRCS    = src/exec_files/signals.c      \
          src/exec_files/pipes.c        \
          src/exec_files/exec_path.c    \
          src/exec_files/builtins.c     \
          src/exec_files/builtins_2.c    \
          src/exec_files/heredoc.c      \
          src/exec_files/redirect.c     \
          src/exec_files/exec.c         \
          src/exec_files/pipeline.c     \
          src/exec_files/redirect_builtin.c \
          src/main.c	\
		  src/parser_files/quotes.c \
		  src/parser_files/whitespace.c \
		  src/parser_files/tokenize.c \
		  src/parser_files/tokenize_build.c \
		  src/parser_files/env.c \
		  src/parser_files/env_pair.c \
		  src/parser_files/env_parse.c \
		  src/parser_files/substitute_env.c \
		  src/parser_files/expand_env.c \
		  src/parser_files/buffer.c \
		  src/parser_files/token.c \
		  src/parser_files/pipeline.c \
		  src/parser_files/pipeline_utils.c \
		  src/parser_files/pipeline_print.c \
		  src/parser_files/command.c \
		  src/parser_files/command_print.c \
		  src/parser_files/string_vector.c \
		  src/parser_files/heredoc.c \
		  src/parser_files/debug_alloc.c

OBJS    = $(SRCS:.c=.o)
#OBJS		= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(LIBFT) $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) $(LIBFT) $(LDFLAGS) -o $(NAME)

# %.o: %.c minishell.h
# 	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.c minishell.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
# 	@mkdir -p $(OBJ_DIR)
# 	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# clean:
# 	rm -f $(OBJS)
# clean:
# 	$(MAKE) -C $(LIBFT_DIR) clean
# 	rm -rf $(OBJ_DIR)
clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -f $(OBJS)

# fclean: clean
fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

.PHONY: all clean fclean re


# # ── tests ──────────────────────────────────────────────
# # ── unit tests ─────────────────────────────────────────
# # UT_DIR		= tests/ut
# # UT_SRCS		= $(UT_DIR)/test_quotes.c \
# # 		  $(UT_DIR)/test_skip_word.c \
# # 		  $(UT_DIR)/test_tokenize.c \
# # 		  $(UT_DIR)/test_substitute_env.c \
# # 		  $(UT_DIR)/test_buffer.c \
# # 		  $(UT_DIR)/test_pipeline.c \
# # 		  $(UT_DIR)/test_pipeline_print.c \
# # 		  $(UT_DIR)/test_env.c
# TEST_CFLAGS	= $(CFLAGS) -DDEBUG -g
# TEST_INCLUDES	= $(INCLUDES) -I testlib

# # test: $(LIBFT)
# # 	@mkdir -p $(OBJ_DIR)
# #	@for t in $(UT_SRCS); do \
# 	@for t in $(INT_SRCS); do \
# 		name=$$(basename $$t .c); \
# 		$(CC) $(TEST_CFLAGS) $(TEST_INCLUDES) $$t $(SRCS) $(LIBFT) \
# 			-o $(OBJ_DIR)/$$name && \
# 		echo "──── $$name ────" && \
# 		./$(OBJ_DIR)/$$name; \
# 	done

# # test_asan: $(LIBFT)
# # 	@mkdir -p $(OBJ_DIR)
# #	@for t in $(UT_SRCS); do \
# 	@for t in $(INT_SRCS); do \
# 		name=$$(basename $$t .c)_asan; \
# 		$(CC) $(TEST_CFLAGS) -fsanitize=address $(TEST_INCLUDES) \
# 			$$t $(SRCS) $(LIBFT) -o $(OBJ_DIR)/$$name && \
# 		echo "──── $$name (ASan) ────" && \
# 		./$(OBJ_DIR)/$$name; \
# 	done

# # test_ubsan: $(LIBFT)
# # 	@mkdir -p $(OBJ_DIR)
# #	@for t in $(UT_SRCS); do \
# 	@for t in $(INT_SRCS); do \
# 		name=$$(basename $$t .c)_ubsan; \
# 		$(CC) $(TEST_CFLAGS) -fsanitize=undefined $(TEST_INCLUDES) \
# 			$$t $(SRCS) $(LIBFT) -o $(OBJ_DIR)/$$name && \
# 		echo "──── $$name (UBSan) ────" && \
# 		./$(OBJ_DIR)/$$name; \
# 	done

# # test_valgrind: $(LIBFT)
# # 	@mkdir -p $(OBJ_DIR)
# #	@for t in $(UT_SRCS); do \
# 	@for t in $(INT_SRCS); do \
# 		name=$$(basename $$t .c); \
# 		$(CC) $(TEST_CFLAGS) $(TEST_INCLUDES) $$t $(SRCS) $(LIBFT) \
# 			-o $(OBJ_DIR)/$$name && \
# 		echo "──── $$name (valgrind) ────" && \
# 		valgrind --leak-check=full --error-exitcode=1 \
# 			./$(OBJ_DIR)/$$name; \
# 	done

# # ── integration tests ──────────────────────────────────

# INT_DIR		= tests/integration
# INT_SRCS	= $(INT_DIR)/parser_visualizer.c

# integration: $(LIBFT)
# 	@mkdir -p $(OBJ_DIR)
# 	@for t in $(INT_SRCS); do \
# 		name=$$(basename $$t .c); \
# 		$(CC) $(TEST_CFLAGS) $(TEST_INCLUDES) $$t $(SRCS) $(LIBFT) \
# 			-o $(OBJ_DIR)/$$name && \
# 		echo "built $(OBJ_DIR)/$$name"; \
# 	done

# # .PHONY: all clean fclean re test test_asan test_ubsan test_valgrind integration
