#include "Interpreter.h"
#include "../RunWidget/RunWidget.h"
#define COUTASM(message) \
    if (_assembly == true) { \
        RUNRESULT.Output(std::string(message) + "\n"); \
    }

/*******************************************************************************
 * 词法分析
 * 每次处理一个标记token
*******************************************************************************/
void Interpreter::next() {
    char *last_pos;
    intptr_t hash = 0;

    while (token = *src) {
        src++;

        // 变量
        if (std::isalpha(token) || (token == '_')) {
            last_pos = src - 1;
            hash = token;
            while (std::isalnum(*src) || (*src == '_')) {
                hash = hash * 147 + *src;
                src++;
            }

            current_id = symbols;
            while (current_id[Token]) {
                // 该标记在符号表中不存在，即hash和name均不同
                if (current_id[Hash] == hash && !memcmp((char *)current_id[Name], last_pos, src - last_pos)) {
                    token = current_id[Token];
                    return;
                }
                current_id = current_id + IdSize;
            }

            current_id[Name] = (intptr_t)last_pos;
            current_id[Hash] = hash;
            token = current_id[Token] = Id;
            return;
        }
        // 数字字面量
        else if (std::isdigit(token) == true) {
            token_val = token - '0';
            // 十进制
            if (token_val > 0) {
                while (std::isdigit(*src)) {
                    token_val = token_val*10 + *src++ - '0';
                }
            }
            else {
                // 16进制
                if (*src == 'x' || *src == 'X') {
                    token = *++src;
                    while (std::isdigit(token) || (token >= 'a' && token <= 'f') || (token >= 'A' && token <= 'F')) {
                        // 取token的低4位，后根据是大写还是小写进行+9 或 +0的修正
                        token_val = token_val * 16 + (token & 15) + (token >= 'A' ? 9 : 0);
                        token = *++src;
                    }
                }
                // 8进制
                else {
                    while (*src >= '0' && *src <= '7') {
                        token_val = token_val*8 + *src++ - '0';
                    }
                }
            }
            token = Num;
            return;
        }
        else switch (token) {
            case '\n':
                ++line;
                break;
            case '#':
                while (*src != 0 && *src != '\n') {
                    src ++;
                }
                break;
            case '/':   // //
                if (*src == '/') {
                    while (*src != 0 && *src != '\n') {
                        ++src;
                    }
                }
                else {  // /
                    token = Div;
                    return;
                }
                break;
            case '"':
            case '\'':
                last_pos = data;
                // 字符串是否结束
                while (*src != 0 && *src != token) {
                    // 如果是单个字符，仅会保留最后一个字符
                    token_val = *src++;
                    // 支持转义'\n'
                    if (token_val == '\\') {
                        token_val = *src++;
                        if (token_val == 'n') {
                            token_val = '\n';
                        }
                    }
                    // 如果是字符串，则存储在data中
                    if (token == '"') {
                        *data++ = token_val;
                    }
                }
                src++;
                if (token == '"') {
                    token_val = (intptr_t)last_pos;
                }
                // 单个字符被认为是数字
                else {
                    token = Num;
                }
                return;
                break;
            case '=':
                if (*src == '=') {  // ==
                    src ++;
                    token = Eq;
                }
                else {              // =
                    token = Assign;
                }
                return;
                break;
            case '+':
                if (*src == '+') {  // ++
                    src ++;
                    token = Inc;
                }
                else {              // +
                    token = Add;
                }
                return;
                break;
            case '-':
                if (*src == '-') {  // --
                    src ++;
                    token = Dec;
                }
                else {              // -
                    token = Sub;
                }
                return;
                break;
            case '!':
                if (*src == '=') {  // !=
                    src++;
                    token = Ne;
                }
                return;
                break;
            case '<':
                if (*src == '=') {      // <=
                    src ++;
                    token = Le;
                }
                else if (*src == '<') { // <<
                    src ++;
                    token = Shl;
                }
                else {                  // <
                    token = Lt;
                }
                return;
                break;
            case '>':
                if (*src == '=') {      // >=
                    src ++;
                    token = Ge;
                }
                else if (*src == '>') { // >>
                    src ++;
                    token = Shr;
                }
                else {                  // >
                    token = Gt;
                }
                return;
                break;
            case '|':
                if (*src == '|') {      // ||
                    src ++;
                    token = Lor;
                }
                else {                  // |
                    token = Or;
                }
                return;
                break;
            case '&':
                if (*src == '&') {      // &&
                    src ++;
                    token = Lan;
                }
                else {                  // &
                    token = And;
                }
                return;
                break;
            case '^':
                token = Xor;
                return;
                break;
            case '%':
                token = Mod;
                return;
                break;
            case '*':
                token = Mul;
                return;
                break;
            case '[':
                token = Brak;
                return;
                break;
            case '?':
                token = Cond;
                return;
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
                // 无需进一步处理，直接将字符本身作为token返回
                return;
                break;
        }
    }
}
/*******************************************************************************
 * 查看当前token是否是我们的期望
 * 如果与期望的token一致则获取下一个token
 * 否则exit -1
*******************************************************************************/
void Interpreter::match(intptr_t tk) {
    if (token == tk) {
        next();
    }
    else {
        LOG.AddErrorLog(
                "在第" + std::to_string(line) + "行,"
                + "期望的token为:" + std::to_string(tk)
                + "实际的token为:" + std::to_string(token));
        exit(-1);
    }
}


/*******************************************************************************
 * 递归下降 BNF
 * 语法分析，递归生成语法树
*******************************************************************************/
void Interpreter::expression(intptr_t level) {
    intptr_t *id;
    intptr_t *addr;
    if (!token) {
        LOG.AddErrorLog(std::to_string(line) + ":遇到EOF,意外结束");
        exit(-1);
    }
    switch (token) {
        case Num: {
            match(Num);

            *++text = IMM;
            *++text = token_val;
            expr_type = INT;
            break;

        }
        case '"': {
            *++text = IMM;
            *++text = token_val;

            // 多行字符串支持
            match('"');
            while (token == '"') {
                match('"');
            }

            // 内存对齐，并添加NUL
            data = (char *)(((intptr_t)data + sizeof(intptr_t)) & (-sizeof(intptr_t)));
            expr_type = PTR;        // 字符串表达式的结果是指针
            break;

        }
        case Sizeof: {

            match(Sizeof);
            match('(');
            expr_type = INT;

            if (token == Int) {
                match(Int);
            }
            else if (token == Char) {
                match(Char);
                expr_type = CHAR;
            }
            // 多重指针
            while (token == Mul) {
                match(Mul);
                expr_type = expr_type + PTR;
            }

            match(')');

            *++text = IMM;
            *++text = (expr_type == CHAR) ? sizeof(char) : sizeof(intptr_t);

            expr_type = INT;            // sizeof表达式的结果是int
            break;
        }
        case Id: {
            match(Id);
            id = current_id;

            // 函数调用
            if (token == '(') {
                match('(');

                intptr_t arg_num = 0;
                while (token != ')') {
                    expression(Assign);
                    *++text = PUSH;
                    arg_num ++;

                    if (token == ',') {
                        match(',');
                    }

                }
                match(')');

                // 系统调用,直接给出系统函数地址，无需call
                if (id[Class] == Sys) {
                    *++text = id[Value];
                }
                    // 函数调用
                else if (id[Class] == Fun) {
                    *++text = CALL;
                    *++text = id[Value];
                }
                else {
                    LOG.AddErrorLog(std::to_string(line) + "错误的函数调用");
                    exit(-1);
                }

                // 清除栈中的参数
                if (arg_num > 0) {
                    *++text = ADJ;
                    *++text = arg_num;
                }
                expr_type = id[Type];
            }
                // 枚举
            else if (id[Class] == Num) {
                *++text = IMM;
                *++text = id[Value];
                expr_type = INT;
            }
                // 变量
            else {
                // 局部变量
                if (id[Class] == Loc) {
                    *++text = LEA;
                    *++text = index_of_bp - id[Value];
                }
                    // 全局变量
                else if (id[Class] == Glo) {
                    *++text = IMM;
                    *++text = id[Value];
                }
                else {
                    LOG.AddErrorLog(std::to_string(line) + "未定义的变量");
                    exit(-1);
                }

                expr_type = id[Type];
                *++text = (expr_type == CHAR) ? LC : LI;
            }
            break;

        }
        case '(': { // 类型转换
            match('(');
            if (token == Int || token == Char) {
                int tmp_type = (token == Char) ? CHAR : INT; // 类型转换
                match(token);
                while (token == Mul) {
                    match(Mul);
                    tmp_type = tmp_type + PTR;
                }

                match(')');

                expression(Inc); // 处理优先级

                expr_type  = tmp_type;
            }
            // 如果不是类型转换就按普通的表达式处理
            else {
                expression(Assign);
                match(')');
            }
            break;
        }
        case Mul: {
            match(Mul);
            expression(Inc); // 处理优先级
            // 是指针的解引用
            if (expr_type >= PTR) {
                expr_type = expr_type - PTR;
            }
            else {
                LOG.AddErrorLog(std::to_string(line) + "错误的解引用");
                exit(-1);
            }

            *++text = (expr_type == CHAR) ? LC : LI;
            break;
        }
        case And: {
            // 取地址
            match(And);
            expression(Inc);
            // 避免递归解析时产生的指令影响指令运行
            if (*text == LC || *text == LI) {
                text --;
            }
            else {
                LOG.AddErrorLog(std::to_string(line) + "错误的取地址");
                exit(-1);
            }

            expr_type = expr_type + PTR;
            break;
        }
        case '!': {
            match('!');
            expression(Inc);

            *++text = PUSH;
            *++text = IMM;
            *++text = 0;
            *++text = EQ;

            expr_type = INT;
            break;
        }
        case '~': {
            // num ^ 0xfff
            match('~');
            expression(Inc);
            *++text = PUSH;
            *++text = IMM;
            *++text = -1;
            *++text = XOR;

            expr_type = INT;
            break;
        }
        case Add: {
            // 正
            match(Add);
            expression(Inc);

            expr_type = INT;
            break;
        }
        case Sub: {
            // 负
            match(Sub);
            // 数字直接取负
            if (token == Num) {
                *++text = IMM;
                *++text = -token_val;
                match(Num);
            }
            // 表达式递归求值 * -1
            else {
                *++text = IMM;
                *++text = -1;
                *++text = PUSH;
                expression(Inc);
                *++text = MUL;
            }

            expr_type = INT;
            break;
        }
        case Inc:
        case Dec: {
            // 前置 ++ --
            intptr_t old_token = token;
            match(token);
            expression(Inc);
            // LC和LI的出现表示当前操作的是数组或指针
            if (*text == LC) {
                *text = PUSH;
                *++text = LC;
            }
            else if (*text == LI) {
                *text = PUSH;
                *++text = LI;
            }
            else {
                LOG.AddErrorLog(std::to_string(line) + "不可前置操作的左值");
                exit(-1);
            }
            *++text = PUSH;
            *++text = IMM;
            *++text = (expr_type > PTR) ? sizeof(intptr_t) : sizeof(char);
            *++text = (old_token == Inc) ? ADD : SUB;
            *++text = (expr_type == CHAR) ? SC : SI;
            break;
        }
        default:
            LOG.AddErrorLog(std::to_string(line) + "编译错误");
            exit(-1);

            break;
    }
    // binary operator and postfix operators.
    while (token >= level) {
        // handle according to current operator's precedence
        intptr_t tmp = expr_type;
        if (token == Assign) {
            // var = expr;
            match(Assign);
            if (*text == LC || *text == LI) {
                *text = PUSH; // save the lvalue's pointer
            } else {
                LOG.AddErrorLog(std::to_string(line)
                                + "%d: bad lvalue in assignment");
                exit(-1);
            }
            expression(Assign);

            expr_type = tmp;
            *++text = (expr_type == CHAR) ? SC : SI;
        }
        else if (token == Cond) {
            // expr ? a : b;
            match(Cond);
            *++text = JZ;
            addr = ++text;
            expression(Assign);
            if (token == ':') {
                match(':');
            } else {
                LOG.AddErrorLog(std::to_string(line)
                                + "%d: missing colon in conditional");
                exit(-1);
            }
            *addr = (intptr_t)(text + 3);
            *++text = JMP;
            addr = ++text;
            expression(Cond);
            *addr = (intptr_t)(text + 1);
        }
        else if (token == Lor) {
            // logic or
            match(Lor);
            *++text = JNZ;
            addr = ++text;
            expression(Lan);
            *addr = (intptr_t)(text + 1);
            expr_type = INT;
        }
        else if (token == Lan) {
            // logic and
            match(Lan);
            *++text = JZ;
            addr = ++text;
            expression(Or);
            *addr = (intptr_t)(text + 1);
            expr_type = INT;
        }
        else if (token == Or) {
            // bitwise or
            match(Or);
            *++text = PUSH;
            expression(Xor);
            *++text = OR;
            expr_type = INT;
        }
        else if (token == Xor) {
            // bitwise xor
            match(Xor);
            *++text = PUSH;
            expression(And);
            *++text = XOR;
            expr_type = INT;
        }
        else if (token == And) {
            // bitwise and
            match(And);
            *++text = PUSH;
            expression(Eq);
            *++text = AND;
            expr_type = INT;
        }
        else if (token == Eq) {
            // equal ==
            match(Eq);
            *++text = PUSH;
            expression(Ne);
            *++text = EQ;
            expr_type = INT;
        }
        else if (token == Ne) {
            // not equal !=
            match(Ne);
            *++text = PUSH;
            expression(Lt);
            *++text = NE;
            expr_type = INT;
        }
        else if (token == Lt) {
            // less than
            match(Lt);
            *++text = PUSH;
            expression(Shl);
            *++text = LT;
            expr_type = INT;
        }
        else if (token == Gt) {
            // greater than
            match(Gt);
            *++text = PUSH;
            expression(Shl);
            *++text = GT;
            expr_type = INT;
        }
        else if (token == Le) {
            // less than or equal to
            match(Le);
            *++text = PUSH;
            expression(Shl);
            *++text = LE;
            expr_type = INT;
        }
        else if (token == Ge) {
            // greater than or equal to
            match(Ge);
            *++text = PUSH;
            expression(Shl);
            *++text = GE;
            expr_type = INT;
        }
        else if (token == Shl) {
            // shift left
            match(Shl);
            *++text = PUSH;
            expression(Add);
            *++text = SHL;
            expr_type = INT;
        }
        else if (token == Shr) {
            // shift right
            match(Shr);
            *++text = PUSH;
            expression(Add);
            *++text = SHR;
            expr_type = INT;
        }
        else if (token == Add) {
            // add
            match(Add);
            *++text = PUSH;
            expression(Mul);

            expr_type = tmp;
            if (expr_type > PTR) {
                // pointer type, and not `char *`
                *++text = PUSH;
                *++text = IMM;
                *++text = sizeof(intptr_t);
                *++text = MUL;
            }
            *++text = ADD;
        }
        else if (token == Sub) {
            // sub
            match(Sub);
            *++text = PUSH;
            expression(Mul);
            if (tmp > PTR && tmp == expr_type) {
                // pointer subtraction
                *++text = SUB;
                *++text = PUSH;
                *++text = IMM;
                *++text = sizeof(intptr_t);
                *++text = DIV;
                expr_type = INT;
            } else if (tmp > PTR) {
                // pointer movement
                *++text = PUSH;
                *++text = IMM;
                *++text = sizeof(intptr_t);
                *++text = MUL;
                *++text = SUB;
                expr_type = tmp;
            } else {
                // numeral subtraction
                *++text = SUB;
                expr_type = tmp;
            }
        }
        else if (token == Mul) {
            // multiply
            match(Mul);
            *++text = PUSH;
            expression(Inc);
            *++text = MUL;
            expr_type = tmp;
        }
        else if (token == Div) {
            // divide
            match(Div);
            *++text = PUSH;
            expression(Inc);
            *++text = DIV;
            expr_type = tmp;
        }
        else if (token == Mod) {
            // Modulo
            match(Mod);
            *++text = PUSH;
            expression(Inc);
            *++text = MOD;
            expr_type = tmp;
        }
        else if (token == Inc || token == Dec) {
            // postfix inc(++) and dec(--)
            // we will increase the value to the variable and decrease it
            // on `rax` to get its original value.
            if (*text == LI) {
                *text = PUSH;
                *++text = LI;
            }
            else if (*text == LC) {
                *text = PUSH;
                *++text = LC;
            }
            else {
                LOG.AddErrorLog(std::to_string(line)
                                + "%d: bad value in increment");
                exit(-1);
            }

            *++text = PUSH;
            *++text = IMM;
            *++text = (expr_type > PTR) ? sizeof(intptr_t) : sizeof(char);
            *++text = (token == Inc) ? ADD : SUB;
            *++text = (expr_type == CHAR) ? SC : SI;
            *++text = PUSH;
            *++text = IMM;
            *++text = (expr_type > PTR) ? sizeof(intptr_t) : sizeof(char);
            *++text = (token == Inc) ? SUB : ADD;
            match(token);
        }
        else if (token == Brak) {
            // array access var[xx]
            match(Brak);
            *++text = PUSH;
            expression(Assign);
            match(']');

            if (tmp > PTR) {
                // pointer, `not char *`
                *++text = PUSH;
                *++text = IMM;
                *++text = sizeof(intptr_t);
                *++text = MUL;
            }
            else if (tmp < PTR) {
                LOG.AddErrorLog(std::to_string(line)
                                + "%d: pointer type expected");
                exit(-1);
            }
            expr_type = tmp - PTR;
            *++text = ADD;
            *++text = (expr_type == CHAR) ? LC : LI;
        }
        else {
            LOG.AddErrorLog(std::to_string(line)
                            + ": compiler error, token = "
                            + std::to_string(token));
            exit(-1);
        }
    }
}

void Interpreter::statement() {
    // there are 8 kinds of statements here:
    // 1. if (...) <statement> [else <statement>]
    // 2. while (...) <statement>
    // 3. { <statement> }
    // 4. return xxx;
    // 5. <empty statement>;
    // 6. expression; (expression end with semicolon)

    intptr_t *a = nullptr, *b = nullptr; // bess for branch control

    if (token == If) {
        // if (...) <statement> [else <statement>]
        //
        //   if (...)           <cond>
        //                      JZ a
        //     <statement>      <statement>
        //   else:              JMP b
        // a:
        //     <statement>      <statement>
        // b:                   b:
        //
        //
        match(If);
        match('(');
        expression(Assign);  // parse condition
        match(')');

        // emit code for if
        *++text = JZ;
        b = ++text;

        statement();         // parse statement
        if (token == Else) { // parse else
            match(Else);

            // emit code for JMP B
            *b = (intptr_t)(text + 3);
            *++text = JMP;
            b = ++text;

            statement();
        }

        *b = (intptr_t)(text + 1);
    }
    else if (token == While) {
        //
        // a:                     a:
        //    while (<cond>)        <cond>
        //                          JZ b
        //     <statement>          <statement>
        //                          JMP a
        // b:                     b:
        match(While);

        a = text + 1;

        match('(');
        expression(Assign);
        match(')');

        *++text = JZ;
        b = ++text;

        statement();

        *++text = JMP;
        *++text = (intptr_t)a;
        *b = (intptr_t)(text + 1);
    }
    else if (token == '{') {
        // { <statement> ... }
        match('{');

        while (token != '}') {
            statement();
        }

        match('}');
    }
    else if (token == Return) {
        // return [expression];
        match(Return);

        if (token != ';') {
            expression(Assign);
        }

        match(';');

        // emit code for return
        *++text = LEV;
    }
    else if (token == ';') {
        // empty statement
        match(';');
    }
    else {
        // a = b; or function_call();
        expression(Assign);
        match(';');
    }
}

void Interpreter::enum_declaration() {
    // parse enum [id] { a = 1, b = 3, ...}
    intptr_t i;
    i = 0;
    while (token != '}') {
        if (token != Id) {
            LOG.AddErrorLog(std::to_string(line)
                    + ": bad enum identifier "
                    + std::to_string(token));
            exit(-1);
        }
        next();
        if (token == Assign) {
            // like {a=10}
            next();
            if (token != Num) {
                LOG.AddErrorLog(std::to_string(line)
                        + ": bad enum initializer");
                exit(-1);
            }
            i = token_val;
            next();
        }

        current_id[Class] = Num;
        current_id[Type] = INT;
        current_id[Value] = i++;

        if (token == ',') {
            next();
        }
    }
}

void Interpreter::function_parameter() {
    intptr_t type;
    intptr_t params;
    params = 0;
    while (token != ')') {
        // int name, ...
        type = INT;
        if (token == Int) {
            match(Int);
        } else if (token == Char) {
            type = CHAR;
            match(Char);
        }

        // pointer type
        while (token == Mul) {
            match(Mul);
            type = type + PTR;
        }

        // parameter name
        if (token != Id) {
            LOG.AddErrorLog(std::to_string(line)
                    + ": bad parameter declaration");
            exit(-1);
        }
        if (current_id[Class] == Loc) {
            LOG.AddErrorLog(std::to_string(line)
                    + ": duplicate parameter declaration");
            exit(-1);
        }

        match(Id);
        // store the local variable
        current_id[BClass] = current_id[Class]; current_id[Class]  = Loc;
        current_id[BType]  = current_id[Type];  current_id[Type]   = type;
        current_id[BValue] = current_id[Value]; current_id[Value]  = params++;   // index of current parameter

        if (token == ',') {
            match(',');
        }
    }
    index_of_bp = params+1;
}

void Interpreter::function_body() {
    // type func_name (...) {...}
    //                   -->|   |<--

    // ... {
    // 1. local declarations
    // 2. statements
    // }

    intptr_t pos_local; // position of local variables on the stack.
    intptr_t type;
    pos_local = index_of_bp;

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
                LOG.AddErrorLog(std::to_string(line)
                        + ": bad local declaration");
                exit(-1);
            }
            if (current_id[Class] == Loc) {
                // identifier exists
                LOG.AddErrorLog(std::to_string(line)
                        + ": duplicate local declaration");
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

void Interpreter::function_declaration() {
    // type func_name (...) {...}
    //               | this part

    match('(');
    function_parameter();
    match(')');
    match('{');
    function_body();
    //match('}');

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

void Interpreter::global_declaration() {
    // int [*]id [; | (...) {...}]


    int type; // tmp, actual type for variable
    int i; // tmp

    basetype = INT;

    // parse enum, this should be treated alone.
    if (token == Enum) {
        // enum [id] { a = 10, b = 20, ... }
        match(Enum);
        if (token != '{') {
            match(Id); // skip the [id] part
        }
        if (token == '{') {
            // parse the assign part
            match('{');
            enum_declaration();
            match('}');
        }

        match(';');
        return;
    }

    // parse type information
    if (token == Int) {
        match(Int);
    }
    else if (token == Char) {
        match(Char);
        basetype = CHAR;
    }

    // parse the comma seperated variable declaration.
    while (token != ';' && token != '}') {
        type = basetype;
        // parse pointer type, note that there may exist `int ****x;`
        while (token == Mul) {
            match(Mul);
            type = type + PTR;
        }

        if (token != Id) {
            // invalid declaration
            LOG.AddErrorLog(std::to_string(line)
                    + ": bad global declaration");
            exit(-1);
        }
        if (current_id[Class]) {
            // identifier exists
            LOG.AddErrorLog(std::to_string(line)
                    + ": duplicate global declaration");
            exit(-1);
        }
        match(Id);
        current_id[Type] = type;

        if (token == '(') {
            current_id[Class] = Fun;
            current_id[Value] = (intptr_t)(text + 1); // the memory address of function
            function_declaration();
        } else {
            // variable declaration
            current_id[Class] = Glo; // global variable
            current_id[Value] = (intptr_t)data; // assign memory address
            data = data + sizeof(intptr_t);
        }

        if (token == ',') {
            match(',');
        }
    }
    next();
}

void Interpreter::program() {
    // get next token
    next();
    while (token > 0) {
        global_declaration();
    }
}

// 虚拟机模拟汇编运行
int Interpreter::eval() {
    cycle = 0;
    while (1) {
        cycle ++;
        intptr_t op = *pc++;
        switch(op) {
            case IMM:
                rax = *pc ++;
                COUTASM("mov %pc,rax");
                break;
            case LC:
                rax = *(char *)rax;
                COUTASM("LC rax,rax");
                break;
            case LI:
                rax = *(intptr_t *)rax;
                COUTASM("LI rax,rax");
                break;
            case SC:
                rax = *(char *)sp = rax;
                sp ++;
                COUTASM("mov rax,sp");
                break;
            case SI:
                *(intptr_t *)*sp = rax;
                sp ++;
                COUTASM("mov rax,sp");
                break;
            case PUSH:
                *--sp = rax;
                COUTASM("push rax");
                break;
            case JMP:
                pc = (intptr_t *)*pc;
                COUTASM("JMP pc");
                break;
            case JZ:
                if (rax != 0) {
                    pc ++;
                }
                else {
                    pc = (intptr_t *)*pc;
                }
                COUTASM("JZ pc");
                break;
            case JNZ:
                if (rax != 0) {
                    pc = (intptr_t *)*pc;
                }
                else {
                    pc ++;
                }
                COUTASM("JNZ pc");
                break;
            case CALL:
                *--sp = (intptr_t)(pc + 1);
                pc = (intptr_t *)*pc;
                COUTASM("CALL " + std::to_string((intptr_t)pc));
                break;
            case ENT:
                *--sp = (intptr_t)bp;
                bp = sp;
                sp = sp - *pc;
                pc ++;
                COUTASM("ENT");
                break;
            case ADJ:
                sp = sp + *pc;
                pc ++;
                COUTASM("ADJ");
                break;
            case LEV:
                sp = bp;
                bp = (intptr_t *)*sp;
                sp ++;
                pc = (intptr_t *)*sp;
                sp ++;
                COUTASM("LEV");
                break;
            case LEA:
                rax = (intptr_t)(bp + *pc);
                pc ++;
                COUTASM("LEA");
                break;
#define OPERATOR_BREAK(op) rax = *sp++ op rax;break
            case OR:
                OPERATOR_BREAK(|);
                COUTASM("OR *sp,rax");
            case XOR:
                OPERATOR_BREAK(^);
                COUTASM("XOR sp,rax");
            case AND:
                OPERATOR_BREAK(&);
                COUTASM("AND sp,rax");
            case EQ:
                OPERATOR_BREAK(==);
                COUTASM("EQ sp,rax");
            case NE:
                OPERATOR_BREAK(!=);
                COUTASM("NE sp,rax");
            case LT:
                OPERATOR_BREAK(<);
                COUTASM("LT sp,rax");
            case LE:
                OPERATOR_BREAK(<=);
                COUTASM("LE sp,rax");
            case GT:
                OPERATOR_BREAK(>);
                COUTASM("GT sp,rax");
            case GE:
                OPERATOR_BREAK(>=);
                COUTASM("GE sp,rax");
            case SHL:
                OPERATOR_BREAK(<<);
                COUTASM("SHL sp,rax");
            case SHR:
                OPERATOR_BREAK(>>);
                COUTASM("SHR sp,rax");
            case ADD:
                OPERATOR_BREAK(+);
                COUTASM("ADD sp,rax");
            case SUB:
                OPERATOR_BREAK(-);
                COUTASM("SUB sp,rax");
            case MUL:
                OPERATOR_BREAK(*);
                COUTASM("MUL sp,rax");
            case DIV:
                OPERATOR_BREAK(/);
                COUTASM("DIV sp,rax");
            case MOD:
                OPERATOR_BREAK(%);
                COUTASM("MOD sp,rax");
#undef OPERATOR
            case EXIT:
                if (*sp == 0){
                    LOG.AddLog("exit(" + std::to_string(*sp) + ")");
                }
                else {
                    LOG.AddErrorLog("exit(" + std::to_string(*sp) + ")");
                }
                return *sp;
                break;
            case PRTF: {
                intptr_t *tmp = sp + pc[1];
                char buf[1024] = {0};
                rax = sprintf(buf,(char *)tmp[-1], tmp[-2], tmp[-3], tmp[-4], tmp[-5], tmp[-6]);

                if (!_assembly)
                    RUNRESULT.Output(std::string(buf));
                break;
            }
            case MALC:
                rax = (intptr_t)malloc(*sp);
                break;
            case MSET:
                rax = (intptr_t)memset((char*)sp[2],sp[1],*sp);
                break;
            case MCMP:
                rax = memcmp((char*)sp[2],(char*)sp[1],*sp);
                break;
            default:

//        else if (op == OPEN) { rax = open((char *)sp[1], sp[0]); }
//        else if (op == CLOS) { rax = close(*sp);}
//        else if (op == READ) { rax = read(sp[2], (char *)sp[1], *sp); }
                LOG.AddErrorLog("未知的指令:" + std::to_string(op));
                return -1;
                break;
        }
    }
}

int Interpreter::Run(std::string& file_content) {
    line = 1;

    memset(text, 0, _pool_size);
    memset(data, 0, _pool_size);
    memset(stack, 0, _pool_size);
    memset(symbols, 0, _pool_size);


#ifdef _MSC_VER
    std::string temp = "char else enum if int return sizeof while "
          "open read close printf malloc memset memcmp exit void main";
    src = &temp[0];
#else
    src = "char else enum if int return sizeof while "
          "open read close printf malloc memset memcmp exit void main";
#endif

    // add keywords to symbol table
    for(int i = Char;i <= While;) {
        next();
        current_id[Token] = i++;
    }

    // add library to symbol table
    for(int i = OPEN;i <= EXIT;) {
        next();
        current_id[Class] = Sys;
        current_id[Type] = INT;
        current_id[Value] = i++;
    }

    next(); current_id[Token] = Char; // handle void type
    next(); idmain = current_id; // keep track of main


    src = &file_content[0];

    program();

    if (!(pc = (intptr_t *)idmain[Value])) {
        LOG.AddErrorLog("main() not defined\n");
        return -1;
    }


    // setup stack
    intptr_t *tmp;
    sp = (intptr_t *)((intptr_t)stack + _pool_size);
    *--sp = EXIT; // call exit if main returns
    *--sp = PUSH; tmp = sp;
    *--sp = (intptr_t)tmp;

    return eval();
}

Interpreter::Interpreter() {
    delete_text = text = (intptr_t *)new char[_pool_size];
    delete_data = data = (char *)new char[_pool_size];
    delete_stack = stack = (intptr_t *)new char[_pool_size];
    delete_src = src = old_src = (char *)new char[_pool_size];
    symbols = (intptr_t *)new char[_pool_size];
    if (text == nullptr || data == nullptr
        || stack == nullptr || symbols == nullptr
        || src == nullptr || old_src == nullptr) {
        LOG.AddErrorLog("为虚拟机分配内存失败");
    }
}
Interpreter::~Interpreter() {
    delete delete_text;
    delete delete_data;
    delete delete_stack;
    delete delete_src;
    delete symbols;
    text = nullptr;
    data = nullptr;
    stack = nullptr;
    symbols = nullptr;
    src = old_src = nullptr;
}