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
    int token_ = 0;
    std::string name = "";   // 标识符本身的字符串
    int type = 0;   // 标识符类型，CHAR/INT/PTR
    int symbol_class = 0;  // 标识符类型，局部/全局变量
    intptr_t value = 0;  // 标识符的值
    int backup_type = 0;
    int backup_class = 0;
    intptr_t backup_value = 0;
};



// 数据类型
enum { CHAR, INT, PTR };

class Interpreter {
    bool CLI = false;
public:
	Interpreter(bool cli = false);
	~Interpreter();
	
	int Run(std::string &file_content);
    enum {
        ASM,RUN
    };
    void SetMod(int mod) {
        if (mod == ASM) {
            assembly_ = true;
        }
        else {
            assembly_ = false;
        }
    }
private:
    bool assembly_ = false;
    int pool_size_ = 1024*256;
    int token_ = 0;
    int line_;
    int basetype_;   // 声明类型
    int exprtype_;  // 表达式类型
    int index_of_bp_;

    char *data_, *src_, *old_src_;
    intptr_t *text_, *stack_;
    intptr_t *rip_, *rbp_, *rsp_, rax_;


    intptr_t token_val_;
    Symbol *current_id_;
    // std::vector<Symbol> symbols_;
    std::unordered_map<std::string,Symbol> symbols_map_;



    char *delete_data_,*delete_src_;
    intptr_t *delete_text_,*delete_stack_;

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
