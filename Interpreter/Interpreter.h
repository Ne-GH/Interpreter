#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string_view>

#include "../Logs/Log.h"

class Interpreter {
public:
	Interpreter();
	~Interpreter();
	
	int Run(std::string &file_content);

private:
    int _pool_size = 1024*256;
    int token; // current token

// instructions
    enum { LEA ,IMM ,JMP ,CALL,JZ  ,JNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PUSH,
        OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
        OPEN,READ,CLOS,PRTF,MALC,MSET,MCMP,EXIT };

// tokens and classes (operators last and in precedence order)
// copied from c4
    enum {
        Num = 128, Fun, Sys, Glo, Loc, Id,
        Char, Else, Enum, If, Int, Return, Sizeof, While,
        Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
    };

// fields of identifier
    enum {Token, Hash, Name, Type, Class, Value, BType, BClass, BValue, IdSize};


// types of variable/function
    enum { CHAR, INT, PTR };


    intptr_t *text, *stack;
    char *data; // data segment
    intptr_t *idmain;

    char *src, *old_src;  // pointer to source code string;

    intptr_t *pc, *bp, *sp, ax, cycle; // virtual machine registers

    intptr_t *current_id, // current parsed ID
    *symbols,    // symbol table
    line,        // line number of source code
    token_val;   // value of current token (mainly for number)

    int basetype;    // the type of a declaration, make it global for convenience
    int expr_type;   // the type of an expression

// function frame
//
// 0: arg 1
// 1: arg 2
// 2: arg 3
// 3: return address
// 4: old bp pointer  <- index_of_bp
// 5: local var 1
// 6: local var 2
    intptr_t index_of_bp; // index of bp pointer on stack


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
