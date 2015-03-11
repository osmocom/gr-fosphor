UNAME=$(shell uname)
CC=gcc
CFLAGS=-Wall -Werror -O2 `pkg-config freetype2 glfw3 libpng --cflags` -g
LDLIBS=`pkg-config freetype2 glfw3 libpng --libs` -lm
ifneq ($(AMDAPPSDKROOT), )
CFLAGS+=-I$(AMDAPPSDKROOT)/include
endif
ifeq ($(UNAME), Linux)
LDLIBS+=-lOpenCL -lGL -ldl
endif
ifeq ($(UNAME), Darwin)
LDLIBS+=-framework OpenCL -framework OpenGL -framework Cocoa -framework IOKit
endif
LDFLAGS=-g

RESOURCE_FILES=fft.cl display.cl cmap_simple.glsl cmap_bicubic.glsl cmap_fallback.glsl DroidSansMonoDotted.ttf

all: main

resource_data.c: $(RESOURCE_FILES) mkresources.py
	./mkresources.py $(RESOURCE_FILES) > resource_data.c

main: resource.o resource_data.o axis.o cl.o cl_compat.o fosphor.o gl.o gl_cmap.o gl_cmap_gen.o gl_font.o main.o

clean:
	rm -f main *.o resource_data.c
