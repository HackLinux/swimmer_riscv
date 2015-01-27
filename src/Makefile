TARGET_DIR = ../
TARGET = $(TARGET_DIR)sim_main
OBJ_DIR = ./objs/

SRCS = env.c \
	inst_call.c \
	inst_decoder.c \
	inst_riscv.c \
	simulation.c \
	trace.c

REVISION=$(shell git rev-parse --short HEAD)
VERSION=$(shell date '+%Y%m%d')

objsrc = $(addprefix $(OBJ_DIR), $(SRCS))
OBJS = $(objsrc:.c=.o)

CFLAGS = -Wall -O3 -I../include -g

CC = gcc
AR = ar

all: inst_call.c $(TARGET)

inst_call.c:
	./gen_arch_table.rb


$(OBJ_DIR)%.o :: %.c
	gcc $(CFLAGS) -o $@ -c $< -DREVISION=\"$(REVISION)\" -DVERSION=\"$(VERSION)\"

$(TARGET) : $(OBJ_DIR) $(OBJS)
	$(AR) rcs sim_main.a $(OBJS)

$(OBJ_DIR) :
	mkdir -p $(OBJ_DIR)

clean :
	rm -f $(TARGET) $(OBJS) inst_call.c inst_decoder.c inst_decoder.h inst_list.h inst_riscv.h
