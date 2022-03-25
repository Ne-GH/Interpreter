#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUL '\0'


char *src,*old_src;      // 指向源文件中字符串
int poolsize;            // 读取源文件所需的内存

int *text;               // 文本段
int *old_text;           // 用于转储文本段
int *stack;              // 栈

char *data;              // 数据段


// 虚拟寄存器
int *pc,                // 程序计数器,存放下一条需要执行的指令
    *bp,                // 基址寄存器,用来指向栈顶,在函数调用时使用
    *sp,                // 指针寄存器,永远指向当前栈顶,栈的增长是由高地址向低地址增长的,因此栈顶增长时应该sp--
    ax,                 // 通用寄存器,用来存放指令执行后的结果
    cycle;

enum{
    LEA,
    IMM,            // IMM <num> ,将num中的指放入ax
    LI,             // 将地址中的整数放入ax,ax中存放地址
    LC,             // 将地址中的字符放入ax.ax中存放地址
    SI,             // 将ax中的数据作为整数放入地址,栈顶存放地址
    SC,             // 将ax中的数据作为整数放入地址,栈顶存放地址
    JMP ,CALL ,JZ ,JNZ ,ENT ,ADJ ,LEV ,
    PUSH ,OR ,XOR ,AND ,EQ ,NE ,LT ,GT ,LE ,GE ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,OPEN ,READ ,CLOS ,PRTF ,MALC ,MSET ,MCMP ,EXIT
};


void evel(){
    
    int op,*tmp;

    while(1){
        if(op == IMM)     { ax = *pc++; }        // 将下一条指令的地址放入ax
        else if(op == LC) { ax = *(char *)ax; }  // 取出ax指向的地址上的字符,并将该字符放入ax
        else if(op == LI) { ax = *(int  *)ax; }  // 取出ax指向的地址上的整数,并将该整数放入ax

        // ax 中的值赋给**sp
        // 之后将**sp中的值赋值给ax
        // 最后sp++
        // ax中的值在传递的过程中,尤其是在sp解引用时,使用char *解引用,使得ax中的值被截断为char
        else if(op == SC) { ax = *(char *)*sp ++ = ax; } 
        else if(op == SI) {  }
    }


}


int main(int argc,char *argv[]){

    argc --;    // 参数数量,第一个参数是程序名,忽略掉
    argv ++;    // 参数列表,第一个参数是程序名,忽略掉

    
    FILE * file;
    poolsize = 256 * 1024;

    if((file = fopen(argv[0], "r")) == NULL){
        printf("Open file %s error",argv[0]);
        exit(1);
    }

    src = old_src = (char *)malloc(sizeof(char) * poolsize);
    if(src == NULL){
        printf("can't malloc %d Byty\n",poolsize);
        exit(1);
    }

    int i = 0;
    if((i = fread(src, sizeof(char), poolsize-1, file)) && i < 0){
        printf("fread is error\n");
        exit(1);
    }

    src[i] = NUL;



    fclose(file);       // 读取文件完成,没file什么事了
    file = NULL;

    
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

    return 0;
}
