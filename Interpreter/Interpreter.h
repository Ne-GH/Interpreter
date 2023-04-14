#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <vector>
#include "../Logs/Log.h"
// 指令集
enum {
    // MOV,0-4
    IMM, LC, LI, SC, SI,
    // Stack,5
    PUSH,// IMM
    // JMP,6-8
    JMP, JZ, JNZ,
    // CALL,9-13
    CALL, ENT, ADJ, LEV, LEA,
    // Math,14-29
    OR, XOR, AND, EQ, NE, LT, GT, LE, GE, SHL, SHR, ADD, SUB, MUL, DIV, MOD,
    // 内置命令,30-37
    OPEN, READ, CLOS, PRTF, MALC, MSET, MCMP, EXIT
};

// 词法分析器/语法分析，定义的位置隐含了优先级，Assign的优先级最低，Brak的优先级最高
enum {
    Num = 128, Fun, Sys, Glo, Loc, Id,
    Char, Else, Enum, If, Int, Return, Sizeof, While,
    Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};

// 符号表
struct Symbol {
    intptr_t Token;
    intptr_t Hash;
    std::string Name;   // 标识符本身的字符串
    intptr_t Type;   // 标识符类型，CHAR/INT/PTR
    intptr_t Class;  // 标识符类型，局部/全局变量
    intptr_t Value;  // 标识符的值
    intptr_t BType;
    intptr_t BClass;
    intptr_t BValue;
    intptr_t IdSize;
};



// 数据类型
enum { CHAR, INT, PTR };

class Interpreter {
public:
	Interpreter();
	~Interpreter();
	
	int Run(std::string &file_content);
    enum {
        ASM,RUN
    };
    void SetMod(bool mod) {
        if (mod == ASM) {
            _assembly = true;
        }
        else {
            _assembly = false;
        }
    }
private:
    bool _assembly = false;
    int _pool_size = 1024*256;
    int token;



    intptr_t *text, *stack;
    char *data;


    char *src, *old_src;

    intptr_t *pc, *bp, *sp, rax, cycle;

    Symbol *idmain,*current_id;
    Symbol* symbols;
    // std::vector<Symbols> symbols;

    intptr_t line,        // line number of source code
        token_val;   // value of current token (mainly for number)

    int basetype;    // the type of a declaration, make it global for convenience
    int expr_type;   // 表达式的类型

    intptr_t index_of_bp; // 局部的栈底指针


    intptr_t *delete_text,*delete_stack;
    char *delete_data,*delete_src;

    void next();
    void match(intptr_t tk);
    void expression(intptr_t level);
    void statement();
    void enum_declaration();
    void function_parameter();
    void function_body();
    void function_declaration();
    void global_declaration();
    void program();
    int eval();



};



#endif
