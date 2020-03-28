SRC_FILES = main.c \
						jtime.c \
						sound.c \
						frite.c \
						keys.c \
						screen.c \
						shapes.c \
						input.c \
						box.c \
						button.c \
						ui.c \
						textbox.c

INCLUDE_DIRS = -I /usr/include/alsa \
							 -I/usr/local/include/SDL2

LIBS = -lasound -lm -lGLEW -lGL \
			 -lSDL2 -lSDL2_ttf -lSDL2_image

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

.PHONY: x11
x11:
	gcc examples/x11.c ${INCLUDE_DIRS} ${LIBS} -o examples/x11

.PHONY: wayland
wayland:
	gcc examples/wayland.c ${INCLUDE_DIRS} -lwayland-client -lwayland-egl -lEGL -lGLESv2 -o examples/wayland
