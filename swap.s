.text
.p2align 4,,15
.global swap

# 该函数用于保存和回复rsp/rbp
swap:
    # 把当前栈寄存器保存到swap的第二个参数
    # 保存栈寄存器
    movq %rsp, 0(%rsi)
    movq %rbp, 8(%rsi)

    # 把swap的第一个参数的值放入到各个寄存器中
    movq 0(%rdi), %rsp
    movq 8(%rdi), %rbp

    # ret执行时会pop %eip,将栈顶处存储的数据存放到eip中用作ret后执行的代码
    # coroutine_init已经将coroutine function存入栈顶
    ret
