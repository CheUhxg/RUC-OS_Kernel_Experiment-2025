# Syzkaller Fuzz 实验手册

请下载[syzkaller-lab.tar.gz](https://pan.ruc.edu.cn/link/AA64BE34BF3172483484C0BBE0CC7AEB3E)，提取码1234，并在当前目录下解压实验环境包：

```sh
tar -xzvf syzkaller-lab.tar.gz
```

> 目标：基于提供的内核源码，配置 syzkaller 对植入的 OOB/UAF/Double Free 漏洞进行 fuzz，在不同漏洞点触发内核崩溃。
> 
> 提示：本目录不包含 syzkaller 可执行文件，需自行下载/编译；本目录提供内核、配置与分发材料的准备说明。

## 实验任务概览

- 以提供的漏洞驱动（`drivers/lab/vuln.c`，提供 `/dev/vuln` 设备）为目标，运行 syzkaller，获取至少一次崩溃。
- 覆盖三类漏洞触发面：栈 OOB、堆 OOB、UAF、Double Free。可多次运行或调节输入使不同漏洞崩溃。
- 产出：syzkaller 崩溃报告 / repro（若能生成）。

## 漏洞触发面说明（/dev/vuln ioctl）

驱动注册 misc 设备名为 `vuln`，设备节点 `/dev/vuln`。IOCTL 命令如下（均使用 `int` 参数指针，除 UAF 外）：
- `VULN_IOCTL_OOB_STACK` (`_IOW('v', 0x01, int)`)：下标写栈数组，`idx > 7` 触发越界。
- `VULN_IOCTL_OOB_HEAP` (`_IOW('v', 0x02, int)`)：下标写堆数组，`idx >= 分配元素数` 触发越界。
- `VULN_IOCTL_UAF` (`_IO('v', 0x03)`)：无参，直接触发 UAF。
- `VULN_IOCTL_DF` (`_IOW('v', 0x04, int)`)：`flag != 0` 时双重释放。

> 如果驱动被编译为模块，请在 VM 内 `insmod vuln.ko` 后确认 `ls /dev/vuln` 存在；若内建则开机即有。

## 实验产物与目录

- `artifacts/`：放置提供给同学的二进制材料（请按下文步骤生成并填充）
  - `bzImage`：开启 KCOV/KASAN 等选项编译后的内核镜像（x86_64）
  - `vmlinux`：未压缩内核（便于符号化崩溃栈）
  - `rootfs.ext4`：含 sshd、基本工具的 rootfs（可以用 BusyBox 构建）
  - `initramfs.cpio.gz`（可选）：若使用 initramfs 启动
  - `id_rsa` / `id_rsa.pub`：供 syz-manager SSH 登录 VM 的密钥
- `kernel-config-fragment`：需要开启的关键内核配置片段
- `syz-manager.cfg`：示例 syzkaller 配置（需按实际路径调整）

## 环境需求

- 主机：Linux x86_64，具备 `gcc`/`clang`、`make`、`qemu-system-x86_64`、`rsync`、`ssh` 等
- Go 1.20+（用于编译 syzkaller，本仓库未包含 syzkaller 源码）
- 推荐开启 KVM 以提升 fuzz 性能；若无 KVM，可将 syz-manager 配置中的 `type` 改为 `qemu` 并去掉 `vm_flags: ["-enable-kvm"]`

## 第 1 步：为 syzkaller 打内核配置

在本目录下的内核源码 `linux-6.6.8/` 执行：
```sh
cd ~/Projects/Linux-Kernel-Note/Tutor/exp/Security/syzkaller/linux-6.6.8
# 基于当前 .config 追加关键选项
../apply-config.sh
# 或手动：
# ./scripts/config --enable CONFIG_KCOV \
#                  --enable CONFIG_KCOV_ENABLE_COMPARISONS \
#                  --enable CONFIG_KASAN \
#                  --enable CONFIG_KASAN_INLINE \
#                  --enable CONFIG_KASAN_SW_TAGS \
#                  --enable CONFIG_DEBUG_FS \
#                  --enable CONFIG_KUNIT \
#                  --disable CONFIG_RANDOMIZE_BASE
make olddefconfig
```
> 如果 `scripts/config` 不存在，请先 `make defconfig`；也可以直接 `cat ../kernel-config-fragment >> .config && make olddefconfig`。

关键开关（已列于 `kernel-config-fragment`）：
- 覆盖采样：`CONFIG_KCOV=y`，`CONFIG_KCOV_ENABLE_COMPARISONS=y`
- 内存检测：`CONFIG_KASAN=y`（可选 inline/sw_tags）
- 调试与符号：`CONFIG_DEBUG_FS=y`，`CONFIG_DEBUG_INFO=y`
- 关闭 KASLR：`CONFIG_RANDOMIZE_BASE=n`，便于符号化

## 第 2 步：编译内核
```sh
cd ~/Projects/Linux-Kernel-Note/Tutor/exp/Security/syzkaller/linux-6.6.8
make -j$(nproc) bzImage
make -j$(nproc) modules
# 可选安装模块到 staging rootfs（若使用 initramfs）：
## 第 0 步：获取并编译 syzkaller
> syzkaller 源码不在本目录，需要自行下载并编译得到 `syz-manager`、`syz-fuzzer` 等二进制。

```sh
cd ~/Projects/Linux-Kernel-Note/Tutor/exp/Security/syzkaller
# 1) 获取源码（可放在当前目录下的 syz-src/）
git clone https://github.com/google/syzkaller.git syz-src

# 2) 准备 Go 环境（确保已安装 Go 1.20+）
export PATH="$(go env GOPATH)/bin:$PATH"

# 3) 编译 syzkaller 常用组件
cd syz-src
make all           # 生成 bin/syz-manager bin/syz-fuzzer bin/syz-execprog 等

# 4) 方便起见可将 bin 加入 PATH
export PATH="$PWD/bin:$PATH"

# 5) 返回实验目录，后续使用 ./syz-manager.cfg 运行
cd ..
```

将生成的：
- `arch/x86/boot/bzImage` 复制到 `../artifacts/bzImage`
- `vmlinux` 复制到 `../artifacts/vmlinux`
- 如驱动为模块：将 `drivers/lab/vuln.ko` 拷贝到 rootfs，对应启动脚本中 `insmod /vuln.ko` 或放入 `/lib/modules/...` 后 `depmod -a && modprobe vuln`。

## 第 3 步：构建最小 rootfs
```sh
cd ~/Projects/Linux-Kernel-Note/Tutor/exp/Security/syzkaller
mkdir -p rootfs && cd rootfs
# 1) 获取 busybox（静态编译）
wget https://busybox.net/downloads/binaries/1.36.1-i686-uclibc/busybox -O busybox
chmod +x busybox
# 2) 目录结构
tree # 可选，观察
mkdir -p bin sbin etc proc sys dev tmp root
cp busybox bin/
ln -s busybox bin/sh
# 3) 基础配置
echo "root::0:0:root:/root:/bin/sh" > etc/passwd
mount -t sysfs none /sys
mount -t tmpfs none /tmp
exec /bin/sh
EOF
chmod +x init
# 4) 制作镜像
find . | cpio -o --format=newc | gzip -c > ../artifacts/initramfs.cpio.gz
# 或使用 ext4：
cd ..
mkdir -p mnt
dd if=/dev/zero of=artifacts/rootfs.ext4 bs=1M count=256
mkfs.ext4 artifacts/rootfs.ext4
sudo mount artifacts/rootfs.ext4 mnt
sudo cp -a rootfs/* mnt/
sudo umount mnt
```
> 也可使用现成的 Debian/Ubuntu cloud image；确保镜像内存在 `/root` 下放入 `id_rsa.pub` 以便 SSH。
> 若使用模块方式，请把 `vuln.ko` 放入 rootfs 并在 `/init` 中 `insmod /vuln.ko`，确保 `/dev/vuln` 可见。

## 第 4 步：准备 VM SSH 密钥
```sh
cd ~/Projects/Linux-Kernel-Note/Tutor/exp/Security/syzkaller/artifacts
ssh-keygen -t rsa -N "" -f id_rsa
```
将 `id_rsa.pub` 添加到 rootfs 的 `/root/.ssh/authorized_keys`。

## 第 5 步：填写 syz-manager 配置
编辑 `syz-manager.cfg` 中的路径与参数，关键字段：
- `target`：`linux/amd64`
- `http`: `127.0.0.1:56700`（Web 状态）
- `workdir`: `./workdir`（自动创建）
- `kernel_obj`: `linux-6.6.8` 或 `artifacts/vmlinux` 所在路径
- `image`: `artifacts/rootfs.ext4`（或 `initramfs.cpio.gz`）
- `sshkey`: `artifacts/id_rsa`
- `kernel`: `artifacts/bzImage`
- `vmlinux`: `artifacts/vmlinux`
- `type`: `qemu`
- `vm`: {`count`:1, `cpu`:2, `mem`:2048, `kernel_flags`: ["nokaslr"], `vm_flags`: ["-enable-kvm"]}

## 第 6 步：运行 syzkaller
```sh
cd ~/Projects/Linux-Kernel-Note/Tutor/exp/Security/syzkaller
# 假设 syzkaller 已编译好 syz-manager 可执行文件并在 PATH 内
syz-manager -config syz-manager.cfg
```
观察 Web 界面或终端输出，等待 syzkaller 生成触发植入漏洞的崩溃（OOB/UAF/Double Free）。

### 可选：手写最小 repro
在 VM 内可用最小 C 程序手动触发，协助 syzkaller 发现路径：
```c
int main() {
  int fd = open("/dev/vuln", O_RDWR);
  int idx = 16; // 栈/堆越界示例
  ioctl(fd, _IOW('v', 0x01, int), &idx); // 栈 OOB
  ioctl(fd, _IOW('v', 0x02, int), &idx); // 堆 OOB
  ioctl(fd, _IO('v', 0x03));             // UAF
  ioctl(fd, _IOW('v', 0x04, int), &idx); // Double Free
  return 0;
}
```
确认这些路径可触发崩溃后，再让 syzkaller 长时间 fuzz 提升命中率。

### 预期结果
1. 能成功启动 syz-manager，VM 正常上线并报告覆盖率。
2. 至少捕获一类植入漏洞的崩溃（栈 OOB / 堆 OOB / UAF / Double Free 中任意一类），最好能多次触发不同类型。
3. 如生成 repro（C/syz 程序）更佳；至少能在终端或 Web 界面看到崩溃报告（含调用栈符号）。
4. 提交物：崩溃日志或 repro、运行截图/说明，必要时附上使用的配置和触发参数。

```
2025/12/19 10:32:41 instance 0: booting VM
2025/12/19 10:32:58 instance 0: executing programs...
2025/12/19 10:33:12 crash: KASAN: slab-out-of-bounds in vuln_oob_heap+0x3a/0x90 [vuln]
2025/12/19 10:33:12   kernel: 6.6.8-syz
2025/12/19 10:33:12   title: KASAN: slab-out-of-bounds Read in vuln_oob_heap
2025/12/19 10:33:12   description: |
    BUG: KASAN: slab-out-of-bounds in vuln_oob_heap+0x3a/0x90 [vuln]
    Read of size 4 at addr ffff88800bcd1234 by task syz-executor.0/1234

    CPU: 0 PID: 1234 Comm: syz-executor.0 Not tainted 6.6.8-syz #1
    Call Trace:
     dump_stack_lvl+0x8b/0xd0
     print_report.cold+0x54/0x4c0
     kasan_report+0xda/0x110
     vuln_oob_heap+0x3a/0x90 [vuln]
     vuln_ioctl+0xe5/0x150 [vuln]
     __x64_sys_ioctl+0x87/0xc0
     do_syscall_64+0x35/0x80
     entry_SYSCALL_64_after_hwframe+0x6e/0x76

2025/12/19 10:33:12 repro: C
2025/12/19 10:33:12 saved repro to: workdir/crashes/32e1.../repro.c
2025/12/19 10:33:12 dashboard client: uploading crash
```