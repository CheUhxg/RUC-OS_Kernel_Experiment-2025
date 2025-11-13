obj-m += cheat_ipc.o

# 当前模块的路径
CURRENT_PATH := $(shell pwd)

# 目标内核的根目录（确认是否存在对应目录）
LINUX_KERNEL_PATH := /home/user/Kernel/v6.6/x86_64/

CC := gcc
CFLAGS := -Wall -Wextra -std=c99 -O2

USER_PROGS := sender receiver

.PHONY: all kernel userspace clean load unload check run

all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules

clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean
	rm -f $(USER_PROGS)

load:
	sudo insmod cheat_ipc.ko qid=$(QID)

unload:
	sudo rmmod cheat_ipc

user: $(USER_PROGS)

sender: sender.c
	$(CC) $(CFLAGS) -o $@ $<

receiver: receiver.c
	$(CC) $(CFLAGS) -o $@ $<

run:
	./receiver > ./rcv &
	./sender &

check:
	@grep "cheat success!" ./rcv >/dev/null && \
		echo "\033[1;32mPASS\033[0m" || \
		echo "\033[1;31mFAIL\033[0m"
