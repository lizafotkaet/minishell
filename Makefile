NAME		= minishell

CC		= cc
CFLAGS		= -Wall -Wextra -Werror -I include -I libft

LIBFT_DIR	= libft
LIBFT		= $(LIBFT_DIR)/libft.a
LDFLAGS		= -lreadline

SRC_DIR		= src
OBJ_DIR		= obj

SRCS		= src/main.c \
		  src/exec_files/builtins.c \
		  src/exec_files/builtins_2.c \
		  src/exec_files/builtins_3.c \
		  src/exec_files/cmd_convert.c \
		  src/exec_files/cmd_convert_utils.c \
		  src/exec_files/exec.c \
		  src/exec_files/exec_path.c \
		  src/exec_files/heredoc.c \
		  src/exec_files/pipeline.c \
		  src/exec_files/pipes.c \
		  src/exec_files/redirect.c \
		  src/exec_files/redirect_builtin.c \
		  src/exec_files/signals.c \
		  src/parser/buffer.c \
		  src/parser/command.c \
		  src/parser/command_print.c \
		  src/parser/debug_alloc.c \
		  src/parser/env.c \
		  src/parser/env_pair.c \
		  src/parser/env_parse.c \
		  src/parser/expand_env.c \
		  src/parser/heredoc.c \
		  src/parser/pipeline.c \
		  src/parser/pipeline_print.c \
		  src/parser/pipeline_utils.c \
		  src/parser/process_env_quotes.c \
		  src/parser/quotes.c \
		  src/parser/string_vector.c \
		  src/parser/substitute_env.c \
		  src/parser/token.c \
		  src/parser/tokenize.c \
		  src/parser/tokenize_build.c \
		  src/parser/tokenize_substitute.c \
		  src/parser/whitespace.c

OBJS		= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(LIBFT) $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(LDFLAGS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -rf $(OBJ_DIR)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
