SRC_FILES = main.c \
						jtime.c

INCLUDE_DIRS = -I /usr/include/alsa

LIBS = -lasound

OBJ_DIR = obj
OBJS += $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))

$(OBJ_DIR)/%.o: src/%.c
	gcc -c $< ${INCLUDE_DIRS} -o $@

app: ${OBJS}
	gcc $^ ${LIBS} -o $@

.PHONY: clean
clean:
	rm ${OBJS}
	rm app
