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
    int _token = 0;
    intptr_t _hash = 0;
    std::string _name = "";   // 标识符本身的字符串
    int _type = 0;   // 标识符类型，CHAR/INT/PTR
    int _class = 0;  // 标识符类型，局部/全局变量
    intptr_t _value = 0;  // 标识符的值
    int _backup_type = 0;
    int _backup_class = 0;
    intptr_t _backup_value = 0;
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
    void SetMod(int mod) {
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
    int _token = 0;
    int _line;
    int _basetype;   // 声明类型
    int _expr_type;  // 表达式类型
    int _index_of_bp;

    char *_data, *_src, *_old_src;
    intptr_t *_text, *_stack;
    intptr_t *_rip, *_rbp, *_rsp, _rax;


    intptr_t token_val;
    Symbol *current_id;
    std::vector<Symbol> symbols;



    char *_delete_data,*_delete_src;
    intptr_t *_delete_text,*_delete_stack;

    void Next();
    void Match(int tk);
    void Expression(int level);
    void Statement();
    void EnumDeclaration();
    void FunctionParameter();
    void FunctionBody();
    void FunctionDeclaration();
    void GlobalDeclaration();
    void Program();
    int Eval();



};



#endif
