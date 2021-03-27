# Paper Scheme

简单的 scheme 实现, 基于 minischeme (https://github.com/catseye/minischeme), 重构代码并修改了以下实现:

- 支持标记压缩垃圾回收算法
    - 这意味着所有 C 函数都要对栈上的对象参数谨慎使用, 因为垃圾回收会移动对象
- 支持 int64_t, boolean, Unit 类型的标记指针
- 

下一阶段: 运行时类型; 