obj-m += hook_syscall.o

# 当前模块的路径
CURRENT_PATH := $(shell pwd)

# 目标内核的根目录（确认是否存在对应目录）
LINUX_KERNEL_PATH := /home/user/Kernel/v6.6/x86_64/

all: hook_syscall.ko

malware: malware.c
	gcc -o malware malware.c

hook_syscall.ko: hook_syscall.c
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules

clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean
	rm -f malware passwd

run:
	./malware &

load:
	sudo insmod hook_syscall.ko

unload:
	sudo rmmod hook_syscall