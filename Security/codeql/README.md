# CodeQL漏洞挖掘

请下载[codeql-lab.tar.gz](https://pan.ruc.edu.cn/link/AAD1F796FD0E30487B94E70ED0A947DB33)，提取码1234，并在当前目录下解压实验环境包：

```sh
tar -xzvf codeql-lab.tar.gz
```

已在 drivers/lab/vuln.c 手工植入 3 类漏洞：栈/堆 OOB、UAF、Double Free。请基于编译好的内核源码创建 CodeQL 数据库，并用自定义查询找出这些漏洞。

## 漏洞详情

```c
#include <linux/module.h>
#include <linux/slab.h>

static void vuln_oob_stack(int idx)
{
    int arr[8];
    if (idx < 16) {      // Stack OOB
        arr[idx] = 1;
    }
}

static void vuln_oob_heap(int idx)
{
    int *arr = kmalloc(idx * sizeof(int), GFP_KERNEL);
    arr[idx] = 1;        // Heap OOB
    kfree(arr);
}

static void vuln_uaf(void)
{
    int *p = kmalloc(sizeof(int), GFP_KERNEL);
    if (!p)
        return;

    kfree(p);
    *p = 42;             // UAF
}

static void vuln_df(int flag)
{
    char *p = kmalloc(32, GFP_KERNEL);
    if (!p)
        return;

    if (flag)
        kfree(p);

    kfree(p);            // Double Free
}

static int __init vuln_init(void)
{
    vuln_oob_stack(10);
    vuln_oob_heap(64);
    vuln_uaf();
    vuln_df(1);
    return 0;
}

static void __exit vuln_exit(void) {}

module_init(vuln_init);
module_exit(vuln_exit);

MODULE_LICENSE("GPL");
```

## 前置环境

- CodeQL CLI 已解压于 `codeql-bin/`，Linux shell 为 zsh。
- 数据库目录：`linux-db/`
- 查询目录：`queries/ql/`（包含 `oob_stack.ql`, `oob_heap.ql`, `uaf.ql`, `double_free.ql`）。

## 实验要求

实现`oob_heap.ql`, `uaf.ql`, `double_free.ql`，使得CodeQL可以查找到对应漏洞。

## 路径准备

```sh
cd Security
export PATH="$PWD/codeql-bin:$PATH"
```

## 运行查询并输出

```sh
export PATH="$PWD/codeql-bin:$PATH"
codeql database analyze linux-db queries/ql --format=sarif-latest --output lab-findings.sarif
```

## CodeQL 基础操作

- `codeql database create ...`：从源码构建数据库，`--command` 指定真实构建命令。
- `codeql database analyze <db> <ql or dir> --format=sarif-latest --output <file>`：执行查询，输出结果。
- QL 文件基本结构：
  - 头部元数据：`@name`, `@description`, `@kind problem`, `@id`, `@problem.severity`。
  - `import cpp` 引入 C/C++ 库。
  - `from ... where ... select ...` 描述要匹配的元素和输出。
- 缓存：重复运行同一查询会命中缓存，速度更快。

## CodeQL 基础语法

下面按“从零写一个查询”讲解，配合本实验的几个漏洞场景。

### 1. 文件头注释与元数据

每个查询都需要描述信息，便于结果展示与管理：
```ql
/**
 * @name Heap off-by-one write after kmalloc
 * @description Flags writes that index the element count returned by kmalloc
 * @kind problem
 * @problem.severity warning
 * @id lab/heap-oob
 */
```
`@kind problem` 表示这是问题类查询；`@problem.severity` 决定告警等级。

### 2. 引入标准库

对 C/C++ 代码，始终 `import cpp`：
```ql
import cpp
```
如果要用到别的语法元素，仍从 `cpp` 库提供的类里取，避免引用缺失的扩展库。

### 3. 绑定语法元素

`from` 子句里声明要遍历的元素，例如数组访问、函数调用、变量：
```ql
from ArrayExpr access, VariableAccess arrUse, LocalVariable arr
```
`ArrayExpr` 是下标表达式，`VariableAccess` 表示对某个变量的使用，`LocalVariable` 是局部变量声明。

### 4. 编写过滤条件（where）

在 `where` 子句里添加约束，逐步缩小到你要的模式。例如“数组是谁”“下标是谁”“是否和 kmalloc 绑定”：
```ql
where
  arrUse = access.getArrayBase().(VariableAccess) and
  arr = arrUse.getTarget() and
  access.getArrayOffset() instanceof VariableAccess idxUse and
  kmallocWithIndex(arr, idxUse.getTarget())
```
这里用到自定义谓词 `kmallocWithIndex`（下一节）来描述“这个数组来自 kmalloc，大小表达式里用到了同一个下标变量”。

### 5. 写自定义谓词封装复杂条件

把复杂匹配拆成谓词，便于复用和阅读：
```ql
predicate kmallocWithIndex(LocalVariable arr, Variable idx) {
  exists(FunctionCall alloc, BinaryOperation mul, VariableAccess idxUse |
    alloc.getTarget().hasName("kmalloc") and
    mul = alloc.getArgument(0).(BinaryOperation) and
    mul.getOperator() = "*" and
    mul.hasOperands(idxUse, _) and idxUse.getTarget() = idx and
    (
      arr.getInitializer().getExpr() = alloc or
      exists(AssignExpr asgn, VariableAccess lhs |
        asgn.getRValue() = alloc and
        lhs = asgn.getLValue().(VariableAccess) and lhs.getTarget() = arr
      )
    )
  )
}
```
要点：
- 先锁定 `kmalloc` 调用，再看第 0 个实参是否是乘法表达式，乘号左/右之一是我们的下标变量。
- 数组指针可能通过初始化或赋值得到，所以两种路径都覆盖。

### 6. select 输出结果

`select` 决定结果定位与消息：
```ql
select access, "Write uses index equal to the allocation count; this is one past the allocated buffer."
```
第一个参数通常是带位置信息的 AST 节点（如表达式、语句、声明），这样 SARIF 能高亮到源码位置。

### 7. 运行与调试

- 最小化查询：先写简单版，只绑定 `ArrayExpr` 并 `select access`，确认能跑通再加条件。
- 逐步加约束：每加一条 `and`，重跑看看是否仍有结果，若为 0 或报错，回退检查。
- 查库 API：在 `codeql-codeql-cli-latest/cpp/ql/lib` 里搜类名，确认版本存在的类与方法。
- 行号顺序近似“可达”：`stmt1.getLocation().getStartLine() < stmt2.getLocation().getStartLine()`，可在缺少控制流库时使用。

### 8. 完整示例回顾：栈 OOB 查询

请参考[queries/ql/oob_stack.ql](queries/ql/oob_stack.ql)。

运行查询后，可以得到lab-findings.sarif输出：

```json
{
    "ruleId": "lab/stack-oob",
    "ruleIndex": 1,
    "rule": {
        "id": "lab/stack-oob",
        "index": 1
    },
    "message": {
        "text": "Array of size 8 may be indexed up to literal 16."
    },
    "locations": [
        {
            "physicalLocation": {
                "artifactLocation": {
                    "uri": "drivers/lab/vuln.c",
                    "uriBaseId": "%SRCROOT%",
                    "index": 95
                },
                "region": {
                    "startLine": 8,
                    "startColumn": 9,
                    "endColumn": 17
                }
            }
        }
    ],
    "partialFingerprints": {
        "primaryLocationLineHash": "54d5ab491d3a0e6c:1",
        "primaryLocationStartColumnFingerprint": "0"
    }
},
```
