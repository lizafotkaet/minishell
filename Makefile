NAME    = minishell

CC      = cc
CFLAGS  = -Wall -Wextra -Werror
LDFLAGS = -lreadline

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
          src/main.c

OBJS    = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(NAME)

%.o: %.c minishell.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
