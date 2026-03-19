NAME      = minishell
CC        = cc
CFLAGS    = -Wall -Wextra -Werror
RM        = rm -f

OBJ_DIR   = obj
LIBFT_DIR = libft
LIBFT     = $(LIBFT_DIR)/libft.a

INCLUDES  = -I include -I libft

PARSER_SRCS = $(wildcard src/parser/*.c)
EXEC_SRCS   = $(wildcard src/exec_files/*.c)
MAIN_SRC    = src/main.c

PARSER_OBJS = $(patsubst src/parser/%.c,    $(OBJ_DIR)/parser_%.o, $(PARSER_SRCS))
EXEC_OBJS   = $(patsubst src/exec_files/%.c,$(OBJ_DIR)/exec_%.o,   $(EXEC_SRCS))
MAIN_OBJ    = $(OBJ_DIR)/main.o

OBJS = $(PARSER_OBJS) $(EXEC_OBJS) $(MAIN_OBJ)

all: $(LIBFT) $(NAME)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) -L$(LIBFT_DIR) -lft -lreadline -o $(NAME)

$(OBJ_DIR)/parser_%.o: src/parser/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/exec_%.o: src/exec_files/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/main.o: src/main.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

clean:
	$(RM) -r $(OBJ_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	$(RM) $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re
