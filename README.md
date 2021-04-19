# Paper Scheme

简单的 scheme 实现, 基于 minischeme (https://github.com/catseye/minischeme), 重构代码并修改了以下实现:

特征:

- 支持标记-整理 (Mark-Compact) 垃圾回收算法
    - 这意味着所有 C 函数都要对栈上的对象参数谨慎使用, 因为垃圾回收会移动对象
- 支持 int64_t, char, boolean, '(), EOF 类型的标记指针
    - 这些都不是真正的对象, 而是带有特殊标记的指针立即数，可以节省内存使用

已知问题:
    
- 内存不足时会直接退出;
- ungetc 只有在 FILE * 缓存正确设置时生效

TODO:

- [x] 完成基本类型内存布局的基本定义
- [x] 完成标记指针实现
- [x] 实现 `OBJ_CHAR`, `OBJ_BOOLEAN`, `OBJ_UNIT`, `OBJ_EOF` 立即数
- [x] 初步完成基本类型系统的定义
- [x] 完成 `int64` 与 `double number` 的标记指针与实际对象的定义和测试
- [x] 完成 `pair` 类型定义
- [x] 构造上下文结构体
- [x] 实现自动增长的堆结构
- [x] 实现运行时分配对象的内存对齐
- [x] 初步构造 `gc_collect()` 流程
- [x] 实现 `标记-压缩 (Mark-Compact)` 垃圾回收算法
- [x] 实现 C 函数栈上的托管对象保护链和相关 API, 因为 GC 需要追踪 C 调用栈上的对象保证正确追踪对象引用图的根和修正引用
- [x] 正确实现 GC 移动对象后调整对象引用
- [x] GC 应当从 `context_t` 结构体中获取类型信息, 更方便扩充新类型
- [x] 向上下文结构体添加类型信息与相关信息注册
- [x] 实现获取对象类型相关的 API
- [x] 实现大量基础类型: `bytes`, `string`, `string_buffer`, `symbol`, `vector`, `stack`, `port`, `weak_ref`
- [x] 完善基础类型操作 API
- [x] 实现各种类型的 hash 算法 和 equals 函数, 并注册到全局类型信息表
- [x] 实现 `hashmap` 和 `hashset`
- [x] 进行各种基础类型反射, `hash` 和 `equals` 函数的测试
- [x] 完成 `hashset` / `hashmap` 的测试
- [x] 实现 `weak_ref_hashmap`
- [x] 实现支持 `延续` 的 scheme stack
- [x] 实现 environment 链
- [x] 实现 repl 需要的相关框架
- [x] 开始实现元循环
- [x] 实现 proc syntax 相关
- [x] 实现 opcode/proc 类型检查
- [x] 实现初始过程和特殊符号表赋值
- [x] 实现运行时类型与解释器结构
- [x] 实现 `stdio_port` / `string_port` 相关操作
- [x] 初步实现 `opcodes.h`
- [x] 初步实现元循环
- [x] 实现词法分析
- [x] 实现 promise
- [x] 完善元循环
- [x] 完善内部过程
- [x] 完善类型系统
- [x] 完善解释器
- [x] 实现标准库
- [x] 完成内部函数
- [ ] 完成最终测试
- [ ] 性能测试
- [ ] 尝试优化性能