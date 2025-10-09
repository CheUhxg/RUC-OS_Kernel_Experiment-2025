obj-m += show_vruntime.o

CURRENT_PATH := $(shell pwd)
LINUX_KERNEL_PATH := /home/user/Kernel/v6.6/x86_64/

# 默认目标：编译内核模块 + 用户程序
all: module

module:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules

workload: workload.c
	gcc -O2 -Wall -o workload workload.c -lpthread

clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean
	rm -f workload

load:
	sudo insmod show_vruntime.ko pid=$(PID)

unload:
	sudo rmmod show_vruntime
