NAME	= docker-listen-host
PRELOAD	= libpreload-docker-listen-host.so
CC	= gcc
RM	= rm -f

min_port	:= 81
max_port	:= 82

CFLAGS	= -g -ggdb -fPIC -DHOOK_MIN_PORT=$(min_port) -DHOOK_MAX_PORT=$(max_port)
LIBS	=

SRC	= hook.c

OBJ	= $(SRC:.c=.o)

all	: $(NAME)

re	: fclean all

$(NAME) : $(PRELOAD) main.o
	$(CC) -o $(NAME) main.o $(PRELOAD) -Wl,-rpath .

$(PRELOAD) : $(OBJ)
	$(CC) -o $(PRELOAD) -shared $(OBJ) $(LIBS)

clean	:
	-$(RM) $(OBJ) *~
	-$(RM) main.o

fclean	: clean
	-$(RM) $(PRELOAD) $(NAME)
