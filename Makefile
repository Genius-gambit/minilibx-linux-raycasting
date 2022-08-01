NAME = exec

SRCS = updated.c

OBJS = ${SRCS:.c=.o}

CC = gcc -ggdb

${NAME}: ${OBJS}
		${MAKE} all -C ./minilibx/
		cp ./minilibx/libmlx.a ./
		$(CC) $(OBJS) libmlx.a -Lmlx_linux -L/usr/lib -Imlx_linux -lXext -lX11 -lm -lz -o $(NAME)

all: ${NAME}

clean:
		rm -rf ${OBJS}
		rm -rf ${NAME}
		rm -rf libmlx.a
		${MAKE} clean -C ./minilibx/

re: clean all