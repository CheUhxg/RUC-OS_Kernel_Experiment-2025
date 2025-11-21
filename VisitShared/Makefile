obj-m += visit_shared.o

TEST_PROG = test
CC = gcc
CFLAGS = -Wall -O2 -pthread  # 使用线程演示竞态需要 -pthread

# 内核构建路径（根据实际情况修改）
LINUX_KERNEL_PATH := /home/user/Kernel/v6.6/x86_64/

# 当前工作目录
CURRENT_PATH := $(shell pwd)

all: module

module:
	@echo "Building kernel module..."
	@make -s -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules

$(TEST_PROG): test.c
	@echo "Building userspace test program (race demo)..."
	@$(CC) $(CFLAGS) -o $@ $^

clean:
	@make -s -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean
	@rm -f $(TEST_PROG)

load:
	@sudo insmod visit_shared.ko

unload:
	@sudo rmmod visit_shared

.PHONY: all module clean load unload run