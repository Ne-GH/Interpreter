#include "Interpreter.h"
#include "../RunWidget/RunWidget.h"
#include <fstream>
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
        LOG.AddLog("next" + std::to_string(token));
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
            while (current_id->Token) {
                // 该标记在符号表中不存在，即hash和name均相同
                if (current_id->Hash == hash && current_id->Name == std::string(last_pos,(int)(src-last_pos))) {
                    token = current_id->Token;
                    return;
                }
                current_id++;
                //current_id = current_id + IdSize;
            }

            current_id->Name = std::string(last_pos,(int)(src-last_pos));
            current_id->Hash = hash;
            token = current_id->Token = Id;
            return;
        }
        // 数字字面量
        else if (std::isdigit(token) != false) {
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
 * 语法分析,解析表达式，递归生成语法树
 * 参数level为当前运算符的优先级
*******************************************************************************/
void Interpreter::expression(intptr_t level) {
    if (!token) {
        LOG.AddErrorLog(std::to_string(line) + ":遇到EOF,意外结束");
        exit(-1);
    }
    LOG.AddLog("expression" + std::to_string(token) + "level" + std::to_string(level));
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
            Symbols *id = current_id;

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
                if (id->Class == Sys) {
                    *++text = id->Value;
                }
                    // 函数调用
                else if (id->Class == Fun) {
                    *++text = CALL;
                    *++text = id->Value;
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
                expr_type = id->Type;
            }
                // 枚举
            else if (id->Class == Num) {
                *++text = IMM;
                *++text = id->Value;
                expr_type = INT;
            }
                // 变量
            else {
                // 局部变量
                if (id->Class == Loc) {
                    *++text = LEA;
                    *++text = index_of_bp - id->Value;
                }
                    // 全局变量
                else if (id->Class == Glo) {
                    *++text = IMM;
                    *++text = id->Value;
                }
                else {
                    LOG.AddErrorLog(std::to_string(line) + "未定义的变量");
                    exit(-1);
                }

                expr_type = id->Type;
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
    // 为了处理右侧所有优先级更高的情况，因此这里用while,不断向右扫描，直到遇到优先级小于当前优先级的运算符
    while (token >= level) {
        // 根据当前优先级处理
        intptr_t tmp_type = expr_type;
        switch (token) {
            // 赋值运算符
            case Assign:
                match(Assign);
                if (*text == LC || *text == LI) {
                    *text = PUSH;
                }
                else {
                    LOG.AddErrorLog(std::to_string(line) + ":赋值时出现错误的左值");
                    exit(-1);
                }
                expression(Assign);

                expr_type = tmp_type;
                *++text = (expr_type == CHAR) ? SC : SI;
                break;
            // 三目运算符(if)
            case Cond: {
                match(Cond);
                *++text = JZ;
                intptr_t *addr = ++text;
                expression(Assign);
                if (token == ':') {
                    match(':');
                }
                else {
                    LOG.AddErrorLog(std::to_string(line) + ":三目运算符缺少':'");
                    exit(-1);
                }
                *addr = (intptr_t)(text + 3);
                *++text = JMP;
                addr = ++text;
                expression(Cond);
                *addr = (intptr_t)(text + 1);
                break;
            }
            // ||
            case Lor: {
                match(Lor);
                *++text = JNZ;
                intptr_t* addr = ++text;
                expression(Lan);
                *addr = (intptr_t)(text + 1);
                expr_type = INT;
                break;
            }
            // &&
            case Lan: {
                match(Lan);
                *++text = JZ;
                intptr_t *addr = ++text;
                expression(Or);
                *addr = (intptr_t)(text + 1);
                expr_type = INT;
                break;
            }
            // |
            case Or:
                match(Or);
                *++text = PUSH;
                expression(Xor);
                *++text = OR;
                expr_type = INT;
                break;
            // ^
            case Xor:
                match(Xor);
                *++text = PUSH;
                expression(And);
                *++text = XOR;
                expr_type = INT;
                break;
            // &
            case And:
                match(And);
                *++text = PUSH;
                expression(Eq);
                *++text = AND;
                expr_type = INT;
                break;
            // ==
            case Eq:
                match(Eq);
                *++text = PUSH;
                expression(Ne);
                *++text = EQ;
                expr_type = INT;
                break;
            // !=
            case Ne:
                match(Ne);
                *++text = PUSH;
                expression(Lt);
                *++text = NE;
                expr_type = INT;
                break;
            // <
            case Lt:
                match(Lt);
                *++text = PUSH;
                expression(Shl);
                *++text = LT;
                expr_type = INT;
                break;
            case Gt:
                match(Gt);
                *++text = PUSH;
                expression(Shl);
                *++text = GT;
                expr_type = INT;
                break;
            // <=
            case Le:
                match(Le);
                *++text = PUSH;
                expression(Shl);
                *++text = LE;
                expr_type = INT;
                break;
            // >=
            case Ge:
                match(Ge);
                *++text = PUSH;
                expression(Shl);
                *++text = GE;
                expr_type = INT;
                break;
            // <<
            case Shl:
                match(Shl);
                *++text = PUSH;
                expression(Add);
                *++text = SHL;
                expr_type = INT;
                break;
            // >>
            case Shr:
                match(Shr);
                *++text = PUSH;
                expression(Add);
                *++text = SHR;
                expr_type = INT;
                break;
            // +
            case Add: {
                match(Add);
                *++text = PUSH;
                expression(Mul);

                expr_type = tmp_type;
                // 指针 + val ==> 值 + sizeof(intptr_t) * val
                if (expr_type > PTR) {
                    *++text = PUSH;
                    *++text = IMM;
                    *++text = sizeof(intptr_t);
                    *++text = MUL;
                }
                *++text = ADD;
                break;
            }
            // -
            case Sub: {
                match(Sub);
                *++text = PUSH;
                expression(Mul);
                // 指针相减
                if (tmp_type > PTR && tmp_type == expr_type) {
                    *++text = SUB;
                    *++text = PUSH;
                    *++text = IMM;
                    *++text = sizeof(intptr_t);
                    *++text = DIV;
                    expr_type = INT;
                }
                // 指针减法
                else if (tmp_type > PTR) {
                    *++text = PUSH;
                    *++text = IMM;
                    *++text = sizeof(intptr_t);
                    *++text = MUL;
                    *++text = SUB;
                    expr_type = tmp_type;
                }
                // 数字减法
                else {
                    *++text = SUB;
                    expr_type = tmp_type;
                }
                break;
            }
            // *
            case Mul:
                match(Mul);
                *++text = PUSH;
                expression(Inc);
                *++text = MUL;
                expr_type = tmp_type;
                break;
            // /
            case Div:
                match(Div);
                *++text = PUSH;
                expression(Inc);
                *++text = DIV;
                expr_type = tmp_type;
                break;
            // %
            case Mod:
                match(Mod);
                *++text = PUSH;
                expression(Inc);
                *++text = MOD;
                expr_type = tmp_type;
                break;
            // 后置++ --
            case Inc:
            case Dec:
                if (*text == LI) {
                    *text = PUSH;
                    *++text = LI;
                }
                else if (*text == LC) {
                    *text = PUSH;
                    *++text = LC;
                }
                else {
                    LOG.AddErrorLog(std::to_string(line) + ":错误的后置自增");
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
                break;
            // [] ==> *(arr + val)
            case Brak:
                match(Brak);
                *++text = PUSH;
                expression(Assign);
                match(']');

                if (tmp_type > PTR) {
                    *++text = PUSH;
                    *++text = IMM;
                    *++text = sizeof(intptr_t);
                    *++text = MUL;
                }
                else if (tmp_type < PTR) {
                    LOG.AddErrorLog(std::to_string(line) + ":期望得到指针类型");
                    exit(-1);
                }
                expr_type = tmp_type - PTR;
                *++text = ADD;
                *++text = (expr_type == CHAR) ? LC : LI;
                break;
            default:
                    LOG.AddErrorLog(std::to_string(line) + ":编译错误,token = " + std::to_string(token));
                    exit(-1);
                break;
        }

    }

}

/*******************************************************************************
 * 解析语句
*******************************************************************************/
void Interpreter::statement() {
    // 支持如下6种语句
    // 1. if (...) <statement> [else <statement>]
    // 2. while (...) <statement>
    // 3. { <statement> }
    // 4. return xxx;
    // 5. <empty statement>;
    // 6. expression; (expression end with semicolon)

    intptr_t *a = nullptr, *b = nullptr;
    LOG.AddLog("statement" + std::to_string(token));
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
        expression(Assign);  // 解析条件
        match(')');

        *++text = JZ;
        b = ++text;

        statement();         // 解析语句
        if (token == Else) { // 解析else
            match(Else);

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

        // 返回
        *++text = LEV;
    }
    else if (token == ';') {
        // 空语句
        match(';');
    }
    else {
        // 表达式
        expression(Assign);
        match(';');
    }
}

/*******************************************************************************
 * 解析enum
*******************************************************************************/
void Interpreter::enum_declaration() {
    // parse enum [id] { a = 1, b = 3, ...}
    intptr_t enum_val = 0;
    while (token != '}') {
        if (token != Id) {
            LOG.AddErrorLog(std::to_string(line) + ":错误enum标识符");
            exit(-1);
        }
        next();
        // {VAL = 10,VAL2 = 20}
        if (token == Assign) {
            next();
            if (token != Num) {
                LOG.AddErrorLog(std::to_string(line) + "错误的enum初始化值");
                exit(-1);
            }
            enum_val = token_val;
            next();
        }

        current_id->Class = Num;
        current_id->Type = INT;
        current_id->Value = enum_val++;

        if (token == ',') {
            next();
        }
    }
}

/*******************************************************************************
 * 解析函数参数
*******************************************************************************/
void Interpreter::function_parameter() {
    intptr_t params = 0;
    while (token != ')') {
        intptr_t type = INT;
        if (token == Int) {
            match(Int);
        }
        else if (token == Char) {
            type = CHAR;
            match(Char);
        }

        // 指针类型
        while (token == Mul) {
            match(Mul);
            type = type + PTR;
        }

        // 参数名
        if (token != Id) {
            LOG.AddErrorLog(std::to_string(line) + ":错误的参数声明");
            exit(-1);
        }
        if (current_id->Class == Loc) {
            LOG.AddErrorLog(std::to_string(line) + ":重复的参数声明");
            exit(-1);
        }

        match(Id);
        // 局部覆盖全局
        current_id->BClass = current_id->Class; current_id->Class  = Loc;
        current_id->BType  = current_id->Type;  current_id->Type   = type;
        current_id->BValue = current_id->Value; current_id->Value  = params++;

        if (token == ',') {
            match(',');
        }
    }
    // params 保存参数数量，并修改index_of_bp来调整函数的栈帧
    index_of_bp = params+1;
}

/*******************************************************************************
 * 解析函数体
*******************************************************************************/
void Interpreter::function_body() {
    intptr_t pos_local; // 本地变量所在栈地址
    intptr_t type;
    pos_local = index_of_bp;

    while (token == Int || token == Char) {
        basetype = (token == Int) ? INT : CHAR;
        match(token);

        while (token != ';') {
            type = basetype;
            while (token == Mul) {
                match(Mul);
                type = type + PTR;
            }

            if (token != Id) {
                LOG.AddErrorLog(std::to_string(line) + ":错误的本地变量声明");
                exit(-1);
            }
            if (current_id->Class == Loc) {
                LOG.AddErrorLog(std::to_string(line) + "本地变量重复声明");
                exit(-1);
            }
            match(Id);

            // 保存本地变量
            current_id->BClass = current_id->Class; current_id->Class  = Loc;
            current_id->BType  = current_id->Type;  current_id->Type   = type;
            current_id->BValue = current_id->Value; current_id->Value  = ++pos_local;

            if (token == ',') {
                match(',');
            }
        }
        match(';');
    }

    // 在函数调用时开辟参数所需大小
    *++text = ENT;
    *++text = pos_local - index_of_bp;

    while (token != '}') {
        statement();
    }

    // 函数返回
    *++text = LEV;
}

/*******************************************************************************
 * 解析函数声明
*******************************************************************************/
void Interpreter::function_declaration() {
    match('(');
    function_parameter();
    match(')');
    match('{');
    function_body();
    //match('}');

    // unwind local variable declarations for all local variables.
    current_id = symbols;
    while (current_id->Token) {
        if (current_id->Class == Loc) {
            current_id->Class = current_id->BClass;
            current_id->Type  = current_id->BType;
            current_id->Value = current_id->BValue;
        }
        current_id++;
        //current_id = current_id + IdSize;
    }
}

/*******************************************************************************
 * 全局的变量定义，类型定义（仅支持enum），函数定义
*******************************************************************************/
void Interpreter::global_declaration() {
    // int [*]id [; | (...) {...}]
    basetype = INT;

    // 解析enum
    // enum [id] { a = 10, b = 20, ... }
    if (token == Enum) {
        match(Enum);
        if (token != '{') {
            match(Id); // 匹配id
        }
        if (token == '{') {
            match('{');
            enum_declaration();
            match('}');
        }

        match(';');
        return;
    }

    // 解析类型信息
    if (token == Int) {
        match(Int);
    }
    else if (token == Char) {
        match(Char);
        basetype = CHAR;
    }

    // 解析','分隔的变量声明
    while (token != ';' && token != '}') {
        int type = basetype;
        // 解析指针类型，为了处理多重指针，使用while
        while (token == Mul) {
            match(Mul);
            type = type + PTR;
        }

        // 是否存在标识符
        if (token != Id) {
            LOG.AddErrorLog(std::to_string(line) + ":错误的全局声明");
            exit(-1);
        }
        if (current_id->Class) {
            LOG.AddErrorLog(std::to_string(line) + ":重复的全局声明");
            exit(-1);
        }
        match(Id);
        current_id->Type = type;

        // 如果是函数
        if (token == '(') {
            current_id->Class = Fun;
            current_id->Value = (intptr_t)(text + 1);
            function_declaration();
        }
        // 变量类型
        else {
            current_id->Class = Glo;
            current_id->Value = (intptr_t)data;
            data = data + sizeof(intptr_t);
        }
        if (token == ',') {
            match(',');
        }
    }

    next();
}

/*******************************************************************************
 * 语法解析入口
*******************************************************************************/
void Interpreter::program() {
    next();
    while (token > 0) {
        global_declaration();
    }
}

/*******************************************************************************
 * 虚拟机，模拟计算机堆栈运行
*******************************************************************************/
int Interpreter::eval() {
    cycle = 0;
    while (true) {
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

    // 添加关键字到符号表
    for(int i = Char;i <= While;) {
        next();
        current_id->Token = i++;
    }

    // add library to symbol table
    for(int i = OPEN;i <= EXIT;) {
        next();
        current_id->Class = Sys;
        current_id->Type = INT;
        current_id->Value = i++;
    }

    next(); current_id->Token = Char; // handle void type
    next(); idmain = current_id; // keep track of main


    src = &file_content[0];

    program();

    if (!(pc = (intptr_t *)(idmain->Value))) {
        LOG.AddErrorLog("main() not defined\n");
        return -1;
    }


    // 初始化栈
    intptr_t *tmp;
    sp = (intptr_t *)((intptr_t)stack + _pool_size);
    *--sp = EXIT; // main返回时调用EXIT
    *--sp = PUSH; tmp = sp;
    *--sp = (intptr_t)tmp;

    return eval();
}

Interpreter::Interpreter() {
    delete_text = text = (intptr_t *)new char[_pool_size];
    delete_data = data = (char *)new char[_pool_size];
    delete_stack = stack = (intptr_t *)new char[_pool_size];
    delete_src = src = old_src = (char *)new char[_pool_size];
    symbols = (Symbols *)new char[_pool_size];
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