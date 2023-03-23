#include "./Interpreter.h"
#include "../RunWidget/RunWidget.h"

int token;
int line = 0;
char* src, * old_src;
int pool_size = 1024 * 256;


// 堆栈信息
intptr_t *text,*old_text;
intptr_t *stack;
char *data;


// 寄存器信息
intptr_t *pc,*bp,*sp,cycle;
intptr_t ax;

// 指令集
enum {
    // MOV,0-4
    IMM ,LC ,LI ,SC ,SI ,
    // Stack,5
    PUSH ,
    // JMP,6-8
    JMP ,JZ ,JNZ ,
    // CALL,9-13
    CALL ,ENT ,ADJ ,LEV ,LEA ,
    // Math,14-29
    OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
    // Fun,30-37
    OPEN ,READ ,CLOS ,PRTF ,MALC ,MSET ,MCMP ,EXIT
};
// 词法分析器
enum {
    Num = 128 ,Fun ,Sys ,Glo ,Loc ,Id ,
    Char ,Else ,Enum ,If ,Int ,Return ,Sizeof ,While ,
    Assign ,Cond ,Lor ,Lan ,Or ,Xor ,And ,Eq ,Ne ,Lt ,Gt ,Le ,Ge ,Shl ,Shr ,Add ,Sub ,Mul ,Div ,Mod ,Inc ,Dec ,Brak
};

// 标识符
struct Identifier {
    int token;
    int hash;
    char *name;
    int _class;     // 数字、全局、局部
    int type;       // int、char、...
    int value;
    // 局部变量和全局变量冲突时，以下三个变量用来保存全局变量的信息
    int Bclass;
    int Btype;
    int Bvalue;

};


int token_val;
intptr_t *current_id, *symbols;
enum {
    Token, Hash, Name, Type, Class, Value, BType, BClass, BValue, IdSize
};

enum {
    CHAR, INT, PTR
};
intptr_t *idmain;    // main 函数







void next() {

    char *last_pos;
    int hash;

    while (token = *src) {
        src ++;

        switch (token) {
            case '\n':
                line ++;
                break;
            case '#':
                while (*src != 0 && *src != '\n') {
                    src ++;
                }
                break;
			// 运算符
            case '=':
                if (*src == '=') {
                    src++;
                    token = Eq;
                }
                break;
            case '+':
                if (*src == '+') {
                    src++;
                    token = Inc;
                }
                else {
                    token = Add;
                }
                break;
            case '-':
                if (*src == '-') {
                    src++;
                    token = Dec;
                }
                else {
                    token = Sub;
                }
                break;
            case '!':
                if (*src == '=') {
                    src++;
                    token = Ne;
                }
                break;
            case '<':
                if (*src == '=') {
                    src++;
                    token = Le;
                }
                else if (*src == '<') {
                    src++;
                    token = Shl;
                }
                else {
                    token = Lt;
                }
                break;
            case '>':
                if (*src == '=') {
                    src++;
                    token = Ge;
                }
                else if (*src == '>') {
                    src++;
                    token = Shr;
                }
                else {
                    token = Gt;
                }
                break;
            case '|':
                if (*src == '|') {
                    src++;
                    token = Lor;
                }
                else {
                    token = Or;
                }
                break;
            case '&':
                if (*src == '&') {
                    src++;
                    token = Lan;
                }
                else {
                    token = And;
                }
                break;
            case '^':
                token = Xor;
                break;
            case '%':
                token = Mod;
                break;
            case '*':
                token = Mul;
                break;
            case '[':
                token = Brak;
                break;
            case '?':
                token = Cond;
                break;
            case '~':
            case ';':
            case '{':
            case '}':
            case '(':
            case ')':
            case ']':
            case ',':
            case ':':
                return;
                break;

			// 字符串
            case '\"':
            case '\'':
                last_pos = data;
                while (*src != 0 && *src != token) {
                    token_val = *src;
                    src++;
                    if (token_val == '\\') {
                        token_val = *src;
                        src++;
                        if (token_val == 'n') {
                            token_val = '\n';
                        }
                    }

                    if (token == '\"') {
                        *data = token_val;
                        data++;
                    }
                }
                src++;
                if (token == '\"') {
                    token_val = (intptr_t)last_pos;
                }
                else {
                    token = Num;
                }
                return;
                break;
            case '/':
                if (*src == '/') {
                    while (*src != 0 && *src != '\n') {
                        src++;
                    }
                }
                else {
                    token = Div;
                    return;
                }
                break;

            default: {
                // 如果是标识符
                if (std::isalpha(token) || token == '_') {
                    // last_pos 是当前标识符的末尾字符的位置
                    last_pos = src - 1;
                    hash = token;
                    // Hash
                    while (std::isalpha(*src) || std::isalnum(*src) || *src == '_') {
                        hash = hash*147 + *src;
                        src ++;
                    }
                    current_id = symbols;
                    // 线性遍历符号表
                    while (current_id[Token]) {
                        if (current_id[Hash] == hash && !memcmp((char *)current_id[Name],last_pos,src-last_pos)) {
                            token = current_id[Token];
                            return;
                        }
                        current_id = current_id + IdSize;
                    }
                    current_id[Name] = (intptr_t)last_pos;
                    current_id[Hash] = hash;
                    token = current_id[Token] = Id;

                }   // if 标识符 end

                // 数字字面量
                else if (std::isalnum(token)) {
                    token_val = token - '0';
                    if(token_val > 0) {
                        while (std::isalnum(*src)) {
                            token_val = token_val*10 + *src - '0';
                            src ++;
                        }
                    }
                    else {
                        if (*src == 'x' || *src == 'X') {
                            src ++;
                            token = *src;
                            while (std::isalnum(token)
                                || token >= 'a' && token <= 'f'
                                || token >= 'A' && token <= 'F') {
                                token_val = token_val * 16
									+ std::stoi(std::string(1, token), nullptr, 16);
                                src++;
                                token = *src;
                            }
                        }
                        else {
                            while (*src >= '0' && *src <= '7') {
                                token_val = token_val * 8 + *src - '0';
                                src++;
                            }
                        }

                    }
                    token = Num;
                    return;
                }   // 数字字面值常量

                break;
            }   // default end
        }   // switch end

    }   // while end

}   // fun end




// 虚拟机
int eval() {
    int op ;
    // int *tmp;
    int i = 0;
    while(true) {
        op = *pc;
        pc ++;
        switch (op) {
            case IMM :
                ax = *pc;
                pc ++;
                break;
            case LC:
                ax = *(char *)ax;
                break;
            case LI:
                ax = *(int *)ax;
                break;
            case SC:
                ax = *(char *)*sp = ax;
                sp ++;
                break;
            case SI:
                ax = *(int *)*sp = ax;
                sp ++;
                break;
            case PUSH:
                sp --;
                *sp = ax;
                break;
            case JMP:
                pc = (intptr_t *)*pc;
                break;
            case JZ:
                if (ax != 0) {
                    pc = pc + 1;
                }
                else {
                    pc = (intptr_t *)*pc;
                }
                break;
            case JNZ:
                if(ax != 0) {
                    pc = (intptr_t *)*pc;
                }
                else {
                    pc = pc + 1;
                }
                break;

            case CALL:
                *--sp = (intptr_t)(pc + 1);
                pc = (intptr_t *)*pc;
                break;
            case ENT:
                *--sp = (intptr_t)bp;
                bp = sp;
                sp = sp - *pc;
                pc ++;
                break;
            case ADJ:
                sp = sp + *pc;
                pc ++;
                break;
            case LEV:
                sp = bp;
                bp = (intptr_t *)*sp ++;
                pc = (intptr_t *)*sp ++;
                break;
            case LEA:
                ax = (intptr_t)(bp + *pc);
                pc ++;
                break;
            case OR:
                ax = *sp | ax;
                sp ++;
                break;
            case XOR:
                ax = *sp ^ ax;
                sp ++;
                break;
            case AND:
                ax = *sp & ax;
                sp ++;
                break;
            case EQ:
                ax = *sp == ax;
                sp ++;
                break;
            case NE:
                ax = *sp != ax;
                sp ++;
                break;
            case LT:
                ax = *sp < ax;
                sp ++;
                break;
            case LE:
                ax = *sp <= ax;
                sp ++;
                break;
            case GT:
                ax = *sp > ax;
                sp ++;
                break;
            case GE:
                ax = *sp >= ax;
                sp ++;
                break;
            case SHL:
                ax = *sp << ax;
                sp ++;
                break;
            case SHR:
                ax = *sp >> ax;
                sp ++;
                break;
            case ADD:
                ax = *sp + ax;
                sp ++;
                break;
            case SUB:
                ax = *sp - ax;
                sp ++;
                break;
            case MUL:
                ax = *sp * ax;
                sp ++;
                break;
            case DIV:
                ax = *sp / ax;
                sp ++;
                break;
            case MOD:
                ax = *sp % ax;
                sp ++;
                break;
            case EXIT:
                RUNRESULT.Output("返回结果为:" + std::to_string(*sp));
                return *sp;
            case OPEN:
                break;
            case CLOS:
                break;
            case READ:
                break;
            case PRTF:{
                intptr_t *tmp = sp + pc[1];
                ax = printf((char*)tmp[-1],tmp[-2],tmp[-3],tmp[-4],tmp[-5],tmp[-6]);
                break;
            }
            case MALC:
                ax = (intptr_t)new char[*sp];
                break;
            case MSET:
                ax = (intptr_t)memset((char*)sp[2],sp[1],*sp);
                break;
            case MCMP:
                ax = (intptr_t) memcpy((char*)sp[2],(char*)sp[1],*sp);
                break;
            default:
                LOG.AddWringLog("未知的指令"+std::to_string(op));
                return -1;
        }

    }
    return 0;
}

// 递归下降
/*******************************************************************************
    消除左递归后的文法为
    <expr> ::= <term> <expr_tail>
    <expr_tail> ::= + <term> <expr_tail>
              | - <term> <expr_tail>
              | <empty>

    <term> ::= <factor> <term_tail>
    <term_tail> ::= * <factor> <term_tail>
                  | / <factor> <term_tail>
                  | <empty>

    <factor> ::= ( <expr> )
               | Num
*******************************************************************************/
void match(int tk) {
    if (token != tk) {
        LOG.AddErrorLog("期望的token为:" + std::to_string(tk)
                        + "实际的token为:" + std::to_string(token));
        exit(-1);
    }
    next();
}
int expr();

int factor() {
    int value = 0;
    if (token == '(') {
        match('(');
        value = expr();
        match(')');
    }
    else {
        value = token_val;
        match(Num);
    }
    return value;
}


int term_tail(int lvalue) {
    if (token == '*') {
        match('*');
        int value = lvalue * factor();
        return term_tail(value);
    }
    else if (token == '/') {
        match('/');
        int value = lvalue / factor();
        return term_tail(value);
    }
    else {
        return lvalue;
    }
}

int term() {
    int lvalue = factor();
    return term_tail(lvalue);
}

int expr_tail(int lvalue) {
    if (token == '+') {
        match ('+');
        int value = lvalue + term();
        return expr_tail(value);
    }
    else if (token == '-') {
        match('-');
        int value = lvalue - term();
        return expr_tail(value);
    }
    else {
        return lvalue;
    }
}

int expr() {
    int lvalue = term();
    return expr_tail(lvalue);
}


// 语句
int index_of_bp;
int basetype;
int expr_type;
// 表达式
void expression(int level) {

    if (token == 0) {
        LOG.AddErrorLog(std::to_string(line) +
                        "unexpected token EOF of expression");
    }

    if(token == Num) {
        match(Num);
        *++text = IMM;
        *++text = token_val;
        expr_type = INT;
    }


}

void statement() {
    intptr_t *a,*b;
    if (token == If) {
        match(If);
        match('(');
        expression(Assign);
        match(')');

        text ++;
        *text = JZ;
        b = ++ text;
        statement();
        if (token == Else) {
            match(Else);
            *b = (intptr_t)(text + 3);
            text ++;
            *text = JMP;
            statement();
        }
        *b = (intptr_t)(text + 1);
    }
    else if (token == While) {
        match(While);
        a = text + 1;
        match('(');
        expression(Assign);
        match(')');
        text ++;
        *text = JZ;
        b = ++text;
        statement();
        *++text = JMP;
        *++text = (intptr_t)a;
        *b = (intptr_t)(text + 1);
    }
    else if (token == Return) {
        match(Return);

        if (token != ';') {
            expression(Assign);
        }
        match(';');
        *++text = LEV;
    }

    else if (token == '{'){
        match('{');
        while (token != '}') {
            statement();
        }
        match('}');
    }
    else if (token == ';') {
        match(';');
    }
    else {
        expression(Assign);
        match(';');
    }

}
void function_parameter() {
    int type;
    int params = 0;

    while (token != ')') {
        type = INT;

        if (token == Int) {
            match(Int);
        }
        else if (token == Char) {
            type = CHAR;
            match(Char);
        }

        while (token == Mul) {
            match(Mul);
            type = type + PTR;
        }

        if (token != Id) {
            LOG.AddErrorLog(std::to_string(line) +
                "bad parameter declaration");
            exit(-1);
        }
        if (current_id[Class] == Loc) {
            LOG.AddErrorLog(std::to_string(line) +
                "duplicate parameter declaration");
            exit(-1);
        }
        match(Id);

        current_id[BClass] = current_id[Class];
        current_id[Class] = Loc;

        current_id[BType] = current_id[Type];
        current_id[Type] = type;

        current_id[BValue] = current_id[Value];
        current_id[Value] = params;

        if (token == ',') {
            match(',');
        }
    }
    index_of_bp = params + 1;
}
void function_body() {
    int pos_local; // position of local variables on the stack.
    int type;
    pos_local = index_of_bp;

    // ①
    while (token == Int || token == Char) {
        // local variable declaration, just like global ones.
        basetype = (token == Int) ? INT : CHAR;
        match(token);

        while (token != ';') {
            type = basetype;
            while (token == Mul) {
                match(Mul);
                type = type + PTR;
            }

            if (token != Id) {
                // invalid declaration
                printf("%d: bad local declaration\n", line);
                exit(-1);
            }
            if (current_id[Class] == Loc) {
                // identifier exists
                printf("%d: duplicate local declaration\n", line);
                exit(-1);
            }
            match(Id);

            // store the local variable
            current_id[BClass] = current_id[Class]; current_id[Class]  = Loc;
            current_id[BType]  = current_id[Type];  current_id[Type]   = type;
            current_id[BValue] = current_id[Value]; current_id[Value]  = ++pos_local;   // index of current parameter

            if (token == ',') {
                match(',');
            }
        }
        match(';');
    }

    // ②
    // save the stack size for local variables
    *++text = ENT;
    *++text = pos_local - index_of_bp;

    // statements
    while (token != '}') {
        statement();
    }

    // emit code for leaving the sub function
    *++text = LEV;
}
void function_declaration(){
    // type func_name (...) {...}
    //               | this part

    match('(');
    function_parameter();
    match(')');
    match('{');
    function_body();
    //match('}');                 //  ①

    // ②
    // unwind local variable declarations for all local variables.
    current_id = symbols;
    while (current_id[Token]) {
        if (current_id[Class] == Loc) {
            current_id[Class] = current_id[BClass];
            current_id[Type]  = current_id[BType];
            current_id[Value] = current_id[BValue];
        }
        current_id = current_id + IdSize;
    }
}

void enum_declaration() {
    int i = 0;
    while (token != '}') {
        if (token != Id) {
            LOG.AddErrorLog(std::to_string(line) + ": bad enum identifier " + std::to_string(token));
            exit(-1);
        }
        next();
        if (token == Assign) {
            next();
            if (token != Num) {
                LOG.AddErrorLog(std::to_string(line) + ": bad enum initializer" );
                exit(-1);
            }
            i = token_val;
            next();
        }
        current_id[Class] = Num;
        current_id[Type] = INT;
        current_id[Value] = i ++;

        if (token == ',') {
            next();
        }

    }

}
void global_declaration() {
    int type;
    int i;
    basetype = INT;

    switch(token) {
        // enum [id] { a = 10, b = 20, ... }
        case Enum:
            match(Enum);

            if (token != '{') {
                match(Id);
            }
            if (token == '{') {
                match('{');
                enum_declaration();
                match('}');
            }
            match(';');
            return;
            break;
        case Int:
            match(Int);
            break;
        case Char:
            match(Char);
            basetype = CHAR;
            break;
    }
    while (token != ';' && token != '}') {
        type = basetype;
        // 解析类似于int *********的类型
        while (token == Mul) {
            match(Mul);
            type = type + PTR;
        }

        if (token != Id) {
            LOG.AddErrorLog(std::to_string(line) + ": bad global declaration");
            exit(-1);
        }
        if (current_id[Class]) {
            LOG.AddErrorLog(std::to_string(line) + ": duplicate global declaration");
            exit(-1);
        }
        match(Id);
        current_id[Type] = type;

        if (token == '(') {
            current_id[Class] = Fun;
            current_id[Value] = (intptr_t)(text + 1);
            function_declaration();
        }
        else {
            current_id[Class] = Glo;
            current_id[Value] = (intptr_t)data;
            data = data + sizeof(int);
        }
        if (token == ',') {
            match(',');
        }
    }
    next();
}

void program() {
    next();
    while (token > 0) {
        LOG.AddLog("token is: " + std::to_string(token));
        global_declaration();
    }
}

void Interpreter::Run(std::string& file_content) {
    // 从ui获取文件内容
    // src = &file_content[0];

//    int linecap = 0,linelen = 0;
//    while((linelen = getline(&line,&linecap,stdin)) > 0) {
//        src = line;
//        next();
//        printf("%d\n",expr());
//    }
//    return;



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




    // program();
    eval();
}
Interpreter::Interpreter() {
    text = old_text = (intptr_t *)new char[pool_size]();
    stack = (intptr_t *)new char[pool_size]();
    data = new char[pool_size]();
    symbols = (intptr_t *)new char[pool_size]();

    if(text == nullptr || old_text == nullptr
       || stack == nullptr || data == nullptr
       || symbols == nullptr ) {
        LOG.AddWringLog("为虚拟机分配内存失败");
        return;
    }

    bp = sp = (intptr_t *)((intptr_t)stack + pool_size);
    ax = 0;

    src = "char else enum if int return sizeof while "
          "open read close printf malloc memset memcmp exit void main";
    int i = Char;
    while (i <= While) {
        next();
        current_id[Token] = i++;
    }

    i = OPEN;
    while (i <= EXIT) {
        next();
        current_id[Class] = Sys;
        current_id[Type] = INT;
        current_id[Value] = i ++;
    }

    next();
    current_id[Token] = Char;
    next();
    idmain = current_id;

}
Interpreter::~Interpreter() {
    delete text;
    delete stack;
    delete data;
    text = nullptr;
    stack = nullptr;
    data = nullptr;
}
