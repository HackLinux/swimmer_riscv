TARGET_DIR = ../
TARGET = $(TARGET_DIR)swimmer_riscv
TARGET_LIB = sim_main.a
OBJ_DIR = ./objs/

LIB_SRCS = env.c \
	inst_call.c \
	inst_decoder.c \
	inst_riscv.c \
	simulation.c \
	trace.c

SRCS = swimmer_main.c

REVISION=$(shell git rev-parse --short HEAD)
VERSION=$(shell date '+%Y%m%d')

libobjsrc = $(addprefix $(OBJ_DIR), $(LIB_SRCS))
LIB_OBJS = $(libobjsrc:.c=.o)

objsrc = $(addprefix $(OBJ_DIR), $(SRCS))
OBJS = $(objsrc:.c=.o)

CFLAGS = -Wall -O3 -I../include -g

CC = gcc
AR = ar

all: inst_call.c $(TARGET)

$(TARGET): $(OBJS) $(TARGET_LIB)
	gcc $(CFLAGS) -o $@ $^ -DREVISION=\"$(REVISION)\" -DVERSION=\"$(VERSION)\"

inst_call.c:
	./gen_arch_table.rb

$(OBJ_DIR)%.o :: %.c
	gcc $(CFLAGS) -o $@ -c $< -DREVISION=\"$(REVISION)\" -DVERSION=\"$(VERSION)\"

$(TARGET_LIB) : $(OBJ_DIR) $(LIB_OBJS)
	$(AR) rcs sim_main.a $(LIB_OBJS)

$(OBJ_DIR) :
	mkdir -p $(OBJ_DIR)

clean :
	rm -f $(TARGET_LIB) $(TARGET) $(LIB_OBJS) inst_call.c inst_decoder.c inst_decoder.h inst_list.h inst_riscv.h
