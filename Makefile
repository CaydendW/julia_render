LD = gcc
CC = gcc

CFLAGS := -Ofast \
	-std=gnu99 \
	-pipe \
	-Wall \
	-Wextra \
	-Werror \
	-ffast-math \
	-march=native \
	-flto \
	-Iinclude \
	-Wno-unused-parameter \
	-Wno-implicit-fallthrough\
	-static \
	-I/usr/include/SDL2 -D_REENTRANT \

LDFLAGS := -Ofast \
	-ffast-math \
	-march=native \
	-flto \
	-L/usr/lib \
	-lSDL2 \
	-lSDL2_image \
	-lm \
	-lpthread \

CFILES := $(shell find src/ -name '*.c')
OFILES := $(patsubst src/%.c, build/%.c.o, $(CFILES))

TARGET = julia

all: compile

compile: ld
	@ echo "Done!"
	
ld: $(OFILES)
	@ echo "[LD] $^"
	@ $(LD) $(LDFLAGS) $^ -o $(TARGET)

build/%.c.o: src/%.c
	@ echo "[CC] $<"
	@ mkdir -p $(@D)
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	@ rm -rf build
	@ rm $(TARGET)

renice:
	@ sudo renice -n -19 $(pgrep julia)

nvrender:
	@ ffmpeg -framerate 120 -pattern_type glob -i "pics/*.png" -vcodec h264_nvenc -crf 0 -preset slow -qp 0 "render.mp4"

nvsmall:
	@ ffmpeg -i render.mp4 -filter:v scale=1920:-1 -r 30 -vcodec h264_nvenc -crf 17 -preset slow render_small.mp4

render:
	@ ffmpeg -framerate 120 -pattern_type glob -i "pics/*.png" -crf 0 -preset veryslow -qp 0 "render.mp4"

small:
	@ ffmpeg -i render.mp4 -filter:v scale=1920:-1 -r 30 -crf 17 -preset slow render_small.mp4
