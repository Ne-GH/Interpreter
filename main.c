#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUL '\0'


int token;                  // 当前token
char *src,*old_src;         // 指向源文件中字符串
int poolsize;               // 读取源文件所需的内存
int line;

int *text;                  // 文本段
int *old_text;              // 用于转储文本段
int *stack;                 // 栈

char *data;                 // 数据段


// 虚拟寄存器
int *pc,                    // 程序计数器,存放下一条需要执行的指令
    *bp,                    // 基址寄存器,用来指向栈顶,在函数调用时使用
    *sp,                    // 指针寄存器,永远指向当前栈顶,栈的增长是由高地址向低地址增长的,因此栈顶增长时应该sp--
    ax,                     // 通用寄存器,用来存放指令执行后的结果
    cycle;

enum{
    LEA,
    IMM,            // IMM <num> ,将num中的指放入ax
    LI,             // 将地址中的整数放入ax,ax中存放地址
    LC,             // 将地址中的字符放入ax.ax中存放地址
    SI,             // 将ax中的数据作为整数放入地址,栈顶存放地址
    SC,             // 将ax中的数据作为整数放入地址,栈顶存放地址
    JMP,            // 将ax中的值作为地址跳转
    CALL,           // 函数调用
    JZ,             // 判断ax中的值是否为0,为1时跳转到指定地址
    JNZ,            // 判断ax中的是为否为0,为0时跳转到指定地址
    ENT,            
    ADJ,
    LEV,
    PUSH,
    OR,
    XOR,
    AND,
    EQ,
    NE,
    LT,
    GT,
    LE,
    GE,
    SHL,
    SHR,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,


    OPEN,
    READ,
    CLOS,
    PRTF,
    MALC,
    MSET,
    MCMP,
    EXIT
};

void next(){
    token = *src++;
    return;
}

void expression(int level){
    // do thing
}

void program(){

    next();
    while (token > 0) {
        printf("token is :%c\n",token);
        next();
    }

}

int evel(){
    
    int op,*tmp;

    while(1){
        op = *pc ++;

        if(op == IMM)     { ax = *pc++; }        // 将下一条指令的地址放入ax
        
        // LI/LC 指令中,地址存放在ax中
        else if(op == LC) { ax = *(char *)ax; }  // 取出ax指向的地址上的字符,并将该字符放入ax
        else if(op == LI) { ax = *(int  *)ax; }  // 取出ax指向的地址上的整数,并将该整数放入ax

        // ax 中的值赋给**sp
        // 之后将**sp中的值赋值给ax
        // 最后sp++
        // ax中的值在传递的过程中,尤其是在sp解引用时,使用char *解引用,使得ax中的值被截断为char
        // SI/SC 指令中,地址存放在栈中
        //默认情况下,计算结果存放在ax中,而地址通常需要额外计算,所以执行LI/LC指令时直接从ax中取值会更高效,另一点是因为push指令只能把ax的值放到栈上,而不能作以值作为参数
        // ax = *(char *)*sp = ax,sp ++ ;
        else if(op == SC) { ax = *(char *)*sp ++ = ax; } 
        else if(op == SI) { *(int *)*sp++ = ax; }

        // (--sp), *sp = ax;
        // 将ax中的值压入stack中
        else if(op == PUSH) { *--sp = ax; }

        // 无条件跳转到pc指定的地址
        // pc 指向的是下一条指令,所以此时指向JMP指令的参数,即目的地址
        else if(op == JMP) { pc = (int *)*pc; }

        // if/else
        // 判断ax是否为0,如果ax为0,表示false,不跳转,因此依次执行之后的指令,即pc = pc + 1
        // 如果ax为1,即true,表示跳转,pc = (int *)*pc;
        // 应该可以改为 
        // if(ax == 0){ pc = pc + 1 } else {pc = (int *)*pc }
        else if(op == JZ) { pc = ax ? pc + 1 : (int *)*pc; }
        // 与JZ相反
        else if(op == JNZ) { pc = ax ? (int *)*pc : pc + 1; }

        // 函数调用,保存特定信息入栈
        // --sp     栈增长
        // *sp = (int)(pc + 1)  // pc + 1 是小一条指令的地址,此时我们将他压入栈中
        // pc = (int *)*pc      // 修改pc,实现跳转
        else if(op == CALL) { *--sp = (int)(pc + 1);pc = (int *)*pc; }
        // pc = (int *)*sp;     // 得到栈顶的元素,即,函数调用时压入的元素
        // sp ++;               // 栈缩小
        // 之后用LEV指令代替RET
        // else if(op == RET) { pc = (int *)*sp++; }

        // --sp;                // sp 栈增长
        // *sp = (int)bp;       // 把bp的值压入栈
        // bp = sp;             // bp 用来指向栈顶
        // sp = sp - *pc;       // sp -= *pc ,sp 栈增长 *pc大小,用来存放局部变量
        // pc ++;               // pc 增长,指向下一条指令
        else if(op == ENT) { *--sp = (int)bp; bp = sp; sp = sp - *pc++; }

        // 将ENT创造的栈清除
        // sp += *pc;           // 
        // pc ++;
        else if(op == ADJ) { sp = sp + *pc ++; }

        // sp = bp              // bp指向函数调用前的栈顶,此时相当于栈顶回退到函数调用之前的样子
        // bp = (int *)sp       // 栈顶指针bp 指向
        // sp ++                // 栈缩小
        // pc = (int *)*sp      // 从栈中取出下一条指令的地址
        // sp ++                // 栈缩小,彻底将函数调用压入栈中的内容弹出
        // LEV 包含了RET的功能,因此我们不再使用RET指令
        else if(op == LEV) { sp = bp;bp = (int *)*sp++; pc = (int *)*sp++; }

        // 为了解决在子函数中获得传入的参数
        // ax = (int)(bp + *pc)
        // pc ++
        else if (op == LEA) { ax = (int)(bp + *pc++); }


        // 运算符
        // ax = *sp op ax,sp++
        // 第一个操作数在栈顶,第二个操作数在ax中,结果存在ax中
        else if(op == OR)   ax = *sp ++     |       ax;
        else if(op == XOR)  ax = *sp ++     ^       ax;
        else if(op == AND)  ax = *sp ++     &       ax;
        else if(op == EQ)   ax = *sp ++     ==      ax;
        else if(op == NE)   ax = *sp ++     !=      ax;
        else if(op == LT)   ax = *sp ++     <       ax;
        else if(op == LE)   ax = *sp ++     <=      ax;
        else if(op == GT)   ax = *sp ++     >       ax;
        else if(op == GE)   ax = *sp ++     >=      ax;
        else if(op == SHL)  ax = *sp ++     <<      ax;
        else if(op == SHR)  ax = *sp ++     >>      ax;
        else if(op == ADD)  ax = *sp ++     +       ax;
        else if(op == SUB)  ax = *sp ++     -       ax;
        else if(op == MUL)  ax = *sp ++     *       ax;
        else if(op == DIV)  ax = *sp ++     /       ax;
        else if(op == MOD)  ax = *sp ++     %       ax;


        // 实现自己的函数
        else if(op == EXIT){ printf("exit(%d)",*sp) ; return (*sp); }
        else if(op == OPEN){ ax = open((char *)sp[1],sp[0]); }
        else if(op == CLOS){ ax = close(*sp); }
        else if(op == READ){ ax = read(sp[2],(char *)sp[1],*sp); }
        else if(op == PRTF){ tmp = sp + pc[1],ax = printf((char *)tmp[-1],tmp[-2],tmp[-3],tmp[-4],tmp[-5],tmp[-6]); }
        else if(op == MALC){ ax = (int)malloc(*sp); }
        else if(op == MSET){ ax = (int)memset((char *)sp[2], sp[1], *sp); }
        else if(op == MCMP){ ax = memcmp((char *)sp[2], (char *)sp[1], *sp); }

        else {
            printf("unknow instruction:%d\n",op);
        }

    }

    return 0;

}


int main(int argc,char *argv[]){

    argc --;    // 参数数量,第一个参数是程序名,忽略掉
    argv ++;    // 参数列表,第一个参数是程序名,忽略掉

    
    FILE * file;
    poolsize = 256 * 1024;
    line = 1;

    /* if((file = fopen(argv[0], "r")) == NULL){ */
        /* printf("Open file %s error",argv[0]); */
        /* exit(1); */
    /* } */
/*  */
    /* src = old_src = (char *)malloc(sizeof(char) * poolsize); */
    /* if(src == NULL){ */
        /* printf("can't malloc %d Byty\n",poolsize); */
        /* exit(1); */
    /* } */
/*  */
    /* int i = 0; */
    /* if((i = fread(src, sizeof(char), poolsize-1, file)) && i < 0){ */
        /* printf("fread is error\n"); */
        /* exit(1); */
    /* } */
/*  */
    /* src[i] = NUL; */
/*  */
/*  */
/*  */
    /* fclose(file);       // 读取文件完成,没file什么事了 */
    /* file = NULL; */

    
    text = old_text = malloc(poolsize);
    if(text == NULL){
        printf("can't malloc %d to text segment\n",poolsize);
        exit(1);
    }
    if((data = malloc(poolsize)) == NULL){
        printf("can't malloc %d to data segment\n",poolsize);
        exit(1);
    }
    if((stack = malloc(poolsize)) == NULL){
        printf("can't malloc %d to stack segment\n",poolsize);
    }


    // 初始化堆栈
    memset(text, 0, poolsize);
    memset(data, 0, poolsize);
    memset(stack, 0, poolsize);


    ax = 0;
    int i = 0;
    text[i++] = IMM;
    text[i++] = 10;
    text[i++] = PUSH;
    text[i++] = IMM;
    text[i++] = 20;
    text[i++] = ADD;
    text[i++] = PUSH;
    text[i++] = EXIT;
    pc = text;


    /* program(); */
    return evel();
}
