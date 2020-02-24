SRC_FILES = main.c \
						jtime.c \
						sound.c \
						frite.c \
						keys.c

INCLUDE_DIRS = -I /usr/include/alsa

LIBS = -lasound -lm

OBJ_DIR = obj
OBJS += $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))

FLAGS = -g3

$(OBJ_DIR)/%.o: src/%.c
	gcc -c $< ${INCLUDE_DIRS} ${FLAGS} -o $@

app: ${OBJS}
	gcc $^ ${LIBS} ${FLAGS} -o $@

.PHONY: clean
clean:
	rm ${OBJS}
	rm app

.PHONY: debug
debug: app
	gdbgui ./app --no-browser

.PHONY: pcm
pcm:
	gcc examples/pcm.c ${INCLUDE_DIRS} ${LIBS} -o examples/pcm
