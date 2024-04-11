#include "Interpreter.h"
#include "../RunWidget/RunWidget.h"
#include <fstream>
#include <iostream>

template <typename T>
void Output(T &&message,bool cli) {
    if (cli) {
        std::cout << message;
        std::cout.flush();
    }
    else {
        RUNRESULT.Output(std::string(message));
    }
}

void Log(std::string message,bool cli) {
    if (cli) {
        // std::cerr << message << std::endl;
    }
    else {
        LOG.AddMessage(message);
    }

}


/*******************************************************************************
 * 词法分析
 * 每次处理一个标记token_
*******************************************************************************/
void Interpreter::Next() {
    char *last_pos;
    intptr_t hash = 0;

    while (token_ = *src_) {
//        LOG.AddLog();
        Log("Next" + std::to_string(token_),CLI);
        src_++;

        // 变量
        if (std::isalpha(token_) || (token_ == '_')) {
            last_pos = src_ - 1;
            hash = token_;
            while (std::isalnum(*src_) || (*src_ == '_')) {
                hash = hash * 147 + *src_;
                src_++;
            }

            // 符号表中存在
            if (auto it = symbols_map_.find(std::string(last_pos, (int)(src_ - last_pos))); it != symbols_map_.end()) {
                current_id_ = &it->second;
                token_ = it->second.token_;
                return;
            }

            Symbol tmp;
            tmp.name = std::string(last_pos, (int)(src_ - last_pos));
            token_ = tmp.token_ = Id;
            symbols_map_.insert({ tmp.name,tmp });
            current_id_ = &symbols_map_.find(tmp.name)->second;

            //symbols_.push_back(tmp);
            //current_id_ = &symbols_.back();
            return;
        }
        // 数字字面量
        else if (std::isdigit(token_) != false) {
            token_val_ = token_ - '0';
            // 十进制
            if (token_val_ > 0) {
                while (std::isdigit(*src_)) {
                    token_val_ = token_val_*10 + *src_++ - '0';
                }
            }
            else {
                // 16进制
                if (*src_ == 'x' || *src_ == 'X') {
                    token_ = *++src_;
                    while (std::isdigit(token_) || (token_ >= 'a' && token_ <= 'f') || (token_ >= 'A' && token_ <= 'F')) {
                        // 取token_的低4位，后根据是大写还是小写进行+9 或 +0的修正
                        token_val_ = token_val_ * 16 + (token_ & 15) + (token_ >= 'A' ? 9 : 0);
                        token_ = *++src_;
                    }
                }
                // 8进制
                else {
                    while (*src_ >= '0' && *src_ <= '7') {
                        token_val_ = token_val_*8 + *src_++ - '0';
                    }
                }
            }
            token_ = Num;
            return;
        }
        else switch (token_) {
            case '\n':
                ++line_;
                break;
            case '#':
                while (*src_ != 0 && *src_ != '\n') {
                    src_ ++;
                }
                break;
            case '/':   // //
                if (*src_ == '/') {
                    while (*src_ != 0 && *src_ != '\n') {
                        ++src_;
                    }
                }
                else {  // /
                    token_ = Div;
                    return;
                }
                break;
            case '"':
            case '\'':
                last_pos = data_;
                // 字符串是否结束
                while (*src_ != 0 && *src_ != token_) {
                    // 如果是单个字符，仅会保留最后一个字符
                    token_val_ = *src_++;
                    // 支持转义'\n'
                    if (token_val_ == '\\') {
                        token_val_ = *src_++;
                        if (token_val_ == 'n') {
                            token_val_ = '\n';
                        }
                    }
                    // 如果是字符串，则存储在data_中
                    if (token_ == '"') {
                        *data_++ = token_val_;
                    }
                }
                src_++;
                if (token_ == '"') {
                    token_val_ = (intptr_t)last_pos;
                }
                // 单个字符被认为是数字
                else {
                    token_ = Num;
                }
                return;
                break;
            case '=':
                if (*src_ == '=') {  // ==
                    src_ ++;
                    token_ = Eq;
                }
                else {              // =
                    token_ = Assign;
                }
                return;
                break;
            case '+':
                if (*src_ == '+') {  // ++
                    src_ ++;
                    token_ = Inc;
                }
                else {              // +
                    token_ = Add;
                }
                return;
                break;
            case '-':
                if (*src_ == '-') {  // --
                    src_ ++;
                    token_ = Dec;
                }
                else {              // -
                    token_ = Sub;
                }
                return;
                break;
            case '!':
                if (*src_ == '=') {  // !=
                    src_++;
                    token_ = Ne;
                }
                return;
                break;
            case '<':
                if (*src_ == '=') {      // <=
                    src_ ++;
                    token_ = Le;
                }
                else if (*src_ == '<') { // <<
                    src_ ++;
                    token_ = Shl;
                }
                else {                  // <
                    token_ = Lt;
                }
                return;
                break;
            case '>':
                if (*src_ == '=') {      // >=
                    src_ ++;
                    token_ = Ge;
                }
                else if (*src_ == '>') { // >>
                    src_ ++;
                    token_ = Shr;
                }
                else {                  // >
                    token_ = Gt;
                }
                return;
                break;
            case '|':
                if (*src_ == '|') {      // ||
                    src_ ++;
                    token_ = Lor;
                }
                else {                  // |
                    token_ = Or;
                }
                return;
                break;
            case '&':
                if (*src_ == '&') {      // &&
                    src_ ++;
                    token_ = Lan;
                }
                else {                  // &
                    token_ = And;
                }
                return;
                break;
            case '^':
                token_ = Xor;
                return;
                break;
            case '%':
                token_ = Mod;
                return;
                break;
            case '*':
                token_ = Mul;
                return;
                break;
            case '[':
                token_ = Brak;
                return;
                break;
            case '?':
                token_ = Cond;
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
                // 无需进一步处理，直接将字符本身作为token_返回
                return;
                break;
        }
    }
}
/*******************************************************************************
 * 查看当前token_是否是我们的期望
 * 如果与期望的token_一致则获取下一个token_
 * 否则exit -1
*******************************************************************************/
void Interpreter::Match(int token) {
    if (token_ == token) {
        Next();
    }
    else {
        Log(
                "在第" + std::to_string(line_) + "行,"
                + "期望的token_为:" + std::to_string(token)
                + "实际的token_为:" + std::to_string(token_),CLI);
        return;
    }
}

/*******************************************************************************
 * 递归下降 BNF
 * 语法分析,解析表达式，递归生成语法树
 * 参数level为当前运算符的优先级
*******************************************************************************/
void Interpreter::Expression(int level) {
    if (!token_) {
        Log(std::to_string(line_) + ":遇到EOF,意外结束",CLI);
        return;
    }
    Log("Expression" + std::to_string(token_) + "level" + std::to_string(level),CLI);
    switch (token_) {
        case Num: {
            Match(Num);

            *++text_ = IMM;
            *++text_ = token_val_;
            exprtype_ = INT;
            break;

        }
        case '"': {
            *++text_ = IMM;
            *++text_ = token_val_;

            // 多行字符串支持
            Match('"');
            while (token_ == '"') {
                Match('"');
            }

            // 内存对齐，并添加NUL
            data_ = (char *)(((intptr_t)data_ + sizeof(intptr_t)) & (-sizeof(intptr_t)));
            exprtype_ = PTR;        // 字符串表达式的结果是指针
            break;

        }
        case Sizeof: {

            Match(Sizeof);
            Match('(');
            exprtype_ = INT;

            if (token_ == Int) {
                Match(Int);
            }
            else if (token_ == Char) {
                Match(Char);
                exprtype_ = CHAR;
            }
            // 多重指针
            while (token_ == Mul) {
                Match(Mul);
                exprtype_ = exprtype_ + PTR;
            }

            Match(')');

            *++text_ = IMM;
            *++text_ = (exprtype_ == CHAR) ? sizeof(char) : sizeof(intptr_t);

            exprtype_ = INT;            // sizeof表达式的结果是int
            break;
        }
        case Id: {
            Match(Id);
            auto id = current_id_;

            // 函数调用
            if (token_ == '(') {
                Match('(');

                intptr_t arg_num = 0;
                while (token_ != ')') {
                    Expression(Assign);
                    *++text_ = PUSH;
                    arg_num ++;

                    if (token_ == ',') {
                        Match(',');
                    }

                }
                Match(')');

                // 系统调用,直接给出系统函数地址，无需call
                if (id->symbol_class == Sys) {
                    *++text_ = id->value;
                }
                    // 函数调用
                else if (id->symbol_class == Fun) {
                    *++text_ = CALL;
                    *++text_ = id->value;
                }
                else {
                    Log(std::to_string(line_) + "错误的函数调用",CLI);
                    return;
                }

                // 清除栈中的参数
                if (arg_num > 0) {
                    *++text_ = ADJ;
                    *++text_ = arg_num;
                }
                exprtype_ = id->type;
            }
                // 枚举
            else if (id->symbol_class == Num) {
                *++text_ = IMM;
                *++text_ = id->value;
                exprtype_ = INT;
            }
                // 变量
            else {
                // 局部变量
                if (id->symbol_class == Loc) {
                    *++text_ = LEA;
                    *++text_ = index_of_bp_ - id->value;
                }
                    // 全局变量
                else if (id->symbol_class == Glo) {
                    *++text_ = IMM;
                    *++text_ = id->value;
                }
                else {
                    Log(std::to_string(line_) + "未定义的变量",CLI);
                    return;
                }

                exprtype_ = id->type;
                *++text_ = (exprtype_ == CHAR) ? LC : LI;
            }
            break;

        }
        case '(': { // 类型转换
            Match('(');
            if (token_ == Int || token_ == Char) {
                int tmptype = (token_ == Char) ? CHAR : INT; // 类型转换
                Match(token_);
                while (token_ == Mul) {
                    Match(Mul);
                    tmptype = tmptype + PTR;
                }

                Match(')');

                Expression(Inc); // 处理优先级

                exprtype_  = tmptype;
            }
            // 如果不是类型转换就按普通的表达式处理
            else {
                Expression(Assign);
                Match(')');
            }
            break;
        }
        case Mul: {
            Match(Mul);
            Expression(Inc); // 处理优先级
            // 是指针的解引用
            if (exprtype_ >= PTR) {
                exprtype_ = exprtype_ - PTR;
            }
            else {
                Log(std::to_string(line_) + "错误的解引用",CLI);
                return;
            }

            *++text_ = (exprtype_ == CHAR) ? LC : LI;
            break;
        }
        case And: {
            // 取地址
            Match(And);
            Expression(Inc);
            // 避免递归解析时产生的指令影响指令运行
            if (*text_ == LC || *text_ == LI) {
                text_ --;
            }
            else {
                Log(std::to_string(line_) + "错误的取地址",CLI);
                return;
            }

            exprtype_ = exprtype_ + PTR;
            break;
        }
        case '!': {
            Match('!');
            Expression(Inc);

            *++text_ = PUSH;
            *++text_ = IMM;
            *++text_ = 0;
            *++text_ = EQ;

            exprtype_ = INT;
            break;
        }
        case '~': {
            // num ^ 0xfff
            Match('~');
            Expression(Inc);
            *++text_ = PUSH;
            *++text_ = IMM;
            *++text_ = -1;
            *++text_ = XOR;

            exprtype_ = INT;
            break;
        }
        case Add: {
            // 正
            Match(Add);
            Expression(Inc);

            exprtype_ = INT;
            break;
        }
        case Sub: {
            // 负
            Match(Sub);
            // 数字直接取负
            if (token_ == Num) {
                *++text_ = IMM;
                *++text_ = -token_val_;
                Match(Num);
            }
            // 表达式递归求值 * -1
            else {
                *++text_ = IMM;
                *++text_ = -1;
                *++text_ = PUSH;
                Expression(Inc);
                *++text_ = MUL;
            }

            exprtype_ = INT;
            break;
        }
        case Inc:
        case Dec: {
            // 前置 ++ --
            intptr_t oldtoken_ = token_;
            Match(token_);
            Expression(Inc);
            // LC和LI的出现表示当前操作的是数组或指针
            if (*text_ == LC) {
                *text_ = PUSH;
                *++text_ = LC;
            }
            else if (*text_ == LI) {
                *text_ = PUSH;
                *++text_ = LI;
            }
            else {
                Log(std::to_string(line_) + "不可前置操作的左值",CLI);
//                LOG.AddErrorLog();
                return;
            }
            *++text_ = PUSH;
            *++text_ = IMM;
            *++text_ = (exprtype_ > PTR) ? sizeof(intptr_t) : sizeof(char);
            *++text_ = (oldtoken_ == Inc) ? ADD : SUB;
            *++text_ = (exprtype_ == CHAR) ? SC : SI;
            break;
        }
        default:
            Log(std::to_string(line_) + "编译错误",CLI);
            return;

            break;
    }
    // 为了处理右侧所有优先级更高的情况，因此这里用while,不断向右扫描，直到遇到优先级小于当前优先级的运算符
    while (token_ >= level) {
        // 根据当前优先级处理
        intptr_t tmptype = exprtype_;
        switch (token_) {
            // 赋值运算符
            case Assign:
                Match(Assign);
                if (*text_ == LC || *text_ == LI) {
                    *text_ = PUSH;
                }
                else {
                    Log(std::to_string(line_) + ":赋值时出现错误的左值",CLI);
//                    LOG.AddErrorLog();
                    return;
                }
                Expression(Assign);

                exprtype_ = tmptype;
                *++text_ = (exprtype_ == CHAR) ? SC : SI;
                break;
            // 三目运算符(if)
            case Cond: {
                Match(Cond);
                *++text_ = JZ;
                intptr_t *addr = ++text_;
                Expression(Assign);
                if (token_ == ':') {
                    Match(':');
                }
                else {
                    Log(std::to_string(line_) + ":三目运算符缺少':'",CLI);
//                    LOG.AddErrorLog();
                    return;
                }
                *addr = (intptr_t)(text_ + 3);
                *++text_ = JMP;
                addr = ++text_;
                Expression(Cond);
                *addr = (intptr_t)(text_ + 1);
                break;
            }
            // ||
            case Lor: {
                Match(Lor);
                *++text_ = JNZ;
                intptr_t* addr = ++text_;
                Expression(Lan);
                *addr = (intptr_t)(text_ + 1);
                exprtype_ = INT;
                break;
            }
            // &&
            case Lan: {
                Match(Lan);
                *++text_ = JZ;
                intptr_t *addr = ++text_;
                Expression(Or);
                *addr = (intptr_t)(text_ + 1);
                exprtype_ = INT;
                break;
            }
            // |
            case Or:
                Match(Or);
                *++text_ = PUSH;
                Expression(Xor);
                *++text_ = OR;
                exprtype_ = INT;
                break;
            // ^
            case Xor:
                Match(Xor);
                *++text_ = PUSH;
                Expression(And);
                *++text_ = XOR;
                exprtype_ = INT;
                break;
            // &
            case And:
                Match(And);
                *++text_ = PUSH;
                Expression(Eq);
                *++text_ = AND;
                exprtype_ = INT;
                break;
            // ==
            case Eq:
                Match(Eq);
                *++text_ = PUSH;
                Expression(Ne);
                *++text_ = EQ;
                exprtype_ = INT;
                break;
            // !=
            case Ne:
                Match(Ne);
                *++text_ = PUSH;
                Expression(Lt);
                *++text_ = NE;
                exprtype_ = INT;
                break;
            // <
            case Lt:
                Match(Lt);
                *++text_ = PUSH;
                Expression(Shl);
                *++text_ = LT;
                exprtype_ = INT;
                break;
            case Gt:
                Match(Gt);
                *++text_ = PUSH;
                Expression(Shl);
                *++text_ = GT;
                exprtype_ = INT;
                break;
            // <=
            case Le:
                Match(Le);
                *++text_ = PUSH;
                Expression(Shl);
                *++text_ = LE;
                exprtype_ = INT;
                break;
            // >=
            case Ge:
                Match(Ge);
                *++text_ = PUSH;
                Expression(Shl);
                *++text_ = GE;
                exprtype_ = INT;
                break;
            // <<
            case Shl:
                Match(Shl);
                *++text_ = PUSH;
                Expression(Add);
                *++text_ = SHL;
                exprtype_ = INT;
                break;
            // >>
            case Shr:
                Match(Shr);
                *++text_ = PUSH;
                Expression(Add);
                *++text_ = SHR;
                exprtype_ = INT;
                break;
            // +
            case Add: {
                Match(Add);
                *++text_ = PUSH;
                Expression(Mul);

                exprtype_ = tmptype;
                // 指针 + val ==> 值 + sizeof(intptr_t) * val
                if (exprtype_ > PTR) {
                    *++text_ = PUSH;
                    *++text_ = IMM;
                    *++text_ = sizeof(intptr_t);
                    *++text_ = MUL;
                }
                *++text_ = ADD;
                break;
            }
            // -
            case Sub: {
                Match(Sub);
                *++text_ = PUSH;
                Expression(Mul);
                // 指针相减
                if (tmptype > PTR && tmptype == exprtype_) {
                    *++text_ = SUB;
                    *++text_ = PUSH;
                    *++text_ = IMM;
                    *++text_ = sizeof(intptr_t);
                    *++text_ = DIV;
                    exprtype_ = INT;
                }
                // 指针减法
                else if (tmptype > PTR) {
                    *++text_ = PUSH;
                    *++text_ = IMM;
                    *++text_ = sizeof(intptr_t);
                    *++text_ = MUL;
                    *++text_ = SUB;
                    exprtype_ = tmptype;
                }
                // 数字减法
                else {
                    *++text_ = SUB;
                    exprtype_ = tmptype;
                }
                break;
            }
            // *
            case Mul:
                Match(Mul);
                *++text_ = PUSH;
                Expression(Inc);
                *++text_ = MUL;
                exprtype_ = tmptype;
                break;
            // /
            case Div:
                Match(Div);
                *++text_ = PUSH;
                Expression(Inc);
                *++text_ = DIV;
                exprtype_ = tmptype;
                break;
            // %
            case Mod:
                Match(Mod);
                *++text_ = PUSH;
                Expression(Inc);
                *++text_ = MOD;
                exprtype_ = tmptype;
                break;
            // 后置++ --
            case Inc:
            case Dec:
                if (*text_ == LI) {
                    *text_ = PUSH;
                    *++text_ = LI;
                }
                else if (*text_ == LC) {
                    *text_ = PUSH;
                    *++text_ = LC;
                }
                else {
                    Log(std::to_string(line_) + ":错误的后置自增",CLI);
//                    LOG.AddErrorLog();
                    return;
                }

                *++text_ = PUSH;
                *++text_ = IMM;
                *++text_ = (exprtype_ > PTR) ? sizeof(intptr_t) : sizeof(char);
                *++text_ = (token_ == Inc) ? ADD : SUB;
                *++text_ = (exprtype_ == CHAR) ? SC : SI;
                *++text_ = PUSH;
                *++text_ = IMM;
                *++text_ = (exprtype_ > PTR) ? sizeof(intptr_t) : sizeof(char);
                *++text_ = (token_ == Inc) ? SUB : ADD;
                Match(token_);
                break;
            // [] ==> *(arr + val)
            case Brak:
                Match(Brak);
                *++text_ = PUSH;
                Expression(Assign);
                Match(']');

                if (tmptype > PTR) {
                    *++text_ = PUSH;
                    *++text_ = IMM;
                    *++text_ = sizeof(intptr_t);
                    *++text_ = MUL;
                }
                else if (tmptype < PTR) {
                    Log(std::to_string(line_) + ":期望得到指针类型",CLI);
                    return;
                }
                exprtype_ = tmptype - PTR;
                *++text_ = ADD;
                *++text_ = (exprtype_ == CHAR) ? LC : LI;
                break;
            default:
                Log(std::to_string(line_) + ":编译错误,token_ = " + std::to_string(token_),CLI);
                return;
                break;
        }

    }

}

/*******************************************************************************
 * 解析语句
*******************************************************************************/
void Interpreter::Statement() {
    // 支持如下6种语句
    // 1. if (...) <Statement> [else <Statement>]
    // 2. while (...) <Statement>
    // 3. { <Statement> }
    // 4. return xxx;
    // 5. <empty Statement>;
    // 6. Expression; (Expression end with semicolon)

    intptr_t *a = nullptr, *b = nullptr;
    Log("Statement" + std::to_string(token_),CLI);
    if (token_ == If) {
        // if (...) <Statement> [else <Statement>]
        //
        //   if (...)           <cond>
        //                      JZ a
        //     <Statement>      <Statement>
        //   else:              JMP b
        // a:
        //     <Statement>      <Statement>
        // b:                   b:
        //
        //
        Match(If);
        Match('(');
        Expression(Assign);  // 解析条件
        Match(')');

        *++text_ = JZ;
        b = ++text_;

        Statement();         // 解析语句
        if (token_ == Else) { // 解析else
            Match(Else);

            *b = (intptr_t)(text_ + 3);
            *++text_ = JMP;
            b = ++text_;

            Statement();
        }

        *b = (intptr_t)(text_ + 1);
    }
    else if (token_ == While) {
        //
        // a:                     a:
        //    while (<cond>)        <cond>
        //                          JZ b
        //     <Statement>          <Statement>
        //                          JMP a
        // b:                     b:
        Match(While);

        a = text_ + 1;

        Match('(');
        Expression(Assign);
        Match(')');

        *++text_ = JZ;
        b = ++text_;

        Statement();

        *++text_ = JMP;
        *++text_ = (intptr_t)a;
        *b = (intptr_t)(text_ + 1);
    }
    else if (token_ == '{') {
        Match('{');

        while (token_ != '}') {
            Statement();
        }

        Match('}');
    }
    else if (token_ == Return) {
        // return [Expression];
        Match(Return);

        if (token_ != ';') {
            Expression(Assign);
        }

        Match(';');

        // 返回
        *++text_ = LEV;
    }
    else if (token_ == ';') {
        // 空语句
        Match(';');
    }
    else {
        // 表达式
        Expression(Assign);
        Match(';');
    }
}

/*******************************************************************************
 * 解析enum
*******************************************************************************/
void Interpreter::EnumDeclaration() {
    // parse enum [id] { a = 1, b = 3, ...}
    intptr_t enum_val = 0;
    while (token_ != '}') {
        if (token_ != Id) {
            Log(std::to_string(line_) + ":错误enum标识符",CLI);
            return;
        }
        Next();
        // {VAL = 10,VAL2 = 20}
        if (token_ == Assign) {
            Next();
            if (token_ != Num) {
                Log(std::to_string(line_) + "错误的enum初始化值",CLI);
                return;
            }
            enum_val = token_val_;
            Next();
        }

        current_id_->symbol_class = Num;
        current_id_->type = INT;
        current_id_->value = enum_val++;

        if (token_ == ',') {
            Next();
        }
    }
}

/*******************************************************************************
 * 解析函数参数
*******************************************************************************/
void Interpreter::FunctionParameter() {
    intptr_t params = 0;
    while (token_ != ')') {
        intptr_t type = INT;
        if (token_ == Int) {
            Match(Int);
        }
        else if (token_ == Char) {
            type = CHAR;
            Match(Char);
        }

        // 指针类型
        while (token_ == Mul) {
            Match(Mul);
            type = type + PTR;
        }

        // 参数名
        if (token_ != Id) {
            Log(std::to_string(line_) + ":错误的参数声明",CLI);
            return;
        }
        if (current_id_->symbol_class == Loc) {
            Log(std::to_string(line_) + ":重复的参数声明",CLI);
            return;
        }

        Match(Id);
        // 局部覆盖全局
        current_id_->backup_class = current_id_->symbol_class; current_id_->symbol_class  = Loc;
        current_id_->backup_type  = current_id_->type;  current_id_->type   = type;
        current_id_->backup_value = current_id_->value; current_id_->value  = params++;

        if (token_ == ',') {
            Match(',');
        }
    }
    // params 保存参数数量，并修改index_of_bp_来调整函数的栈帧
    index_of_bp_ = params+1;
}

/*******************************************************************************
 * 解析函数体
*******************************************************************************/
void Interpreter::FunctionBody() {
    intptr_t pos_local = index_of_bp_; // 本地变量所在栈地址
    intptr_t type;

    while (token_ == Int || token_ == Char) {
        basetype_ = (token_ == Int) ? INT : CHAR;
        Match(token_);

        while (token_ != ';') {
            type = basetype_;
            while (token_ == Mul) {
                Match(Mul);
                type = type + PTR;
            }

            if (token_ != Id) {
                Log(std::to_string(line_) + ":错误的本地变量声明",CLI);
                return;
            }
            if (current_id_->symbol_class == Loc) {
                Log(std::to_string(line_) + "本地变量重复声明",CLI);
                return;
            }
            Match(Id);

            // 保存本地变量
            current_id_->backup_class = current_id_->symbol_class; current_id_->symbol_class  = Loc;
            current_id_->backup_type  = current_id_->type;  current_id_->type   = type;
            current_id_->backup_value = current_id_->value; current_id_->value  = ++pos_local;

            if (token_ == ',') {
                Match(',');
            }
        }
        Match(';');
    }

    // 在函数调用时开辟参数所需大小
    *++text_ = ENT;
    *++text_ = pos_local - index_of_bp_;

    while (token_ != '}') {
        Statement();
    }

    // 函数返回
    *++text_ = LEV;
}

/*******************************************************************************
 * 解析函数声明
*******************************************************************************/
void Interpreter::FunctionDeclaration() {
    Match('(');
    FunctionParameter();
    Match(')');
    Match('{');
    FunctionBody();
    //Match('}');

    // unwind local variable declarations for all local variables.
    //for (auto& up : symbols_) {
    //    current_id_ = &up;
    //    if (up.class == Loc) {
    //        up.class = up.backup_class;
    //        up.type = up.backup_type;
    //        up.value = up.backup_value;
    //    }
    //}

    for (auto& [name, symbol] : symbols_map_) {
        current_id_ = &symbol;
        if (symbol.symbol_class == Loc) {
            symbol.symbol_class = symbol.backup_class;
            symbol.type = symbol.backup_type;
            symbol.value = symbol.backup_value;
        }
    }

 }

/*******************************************************************************
 * 全局的变量定义，类型定义（仅支持enum），函数定义
*******************************************************************************/
void Interpreter::GlobalDeclaration() {
    // int [*]id [; | (...) {...}]
    basetype_ = INT;

    // 解析enum
    // enum [id] { a = 10, b = 20, ... }
    if (token_ == Enum) {
        Match(Enum);
        if (token_ != '{') {
            Match(Id); // 匹配id
        }
        if (token_ == '{') {
            Match('{');
            EnumDeclaration();
            Match('}');
        }

        Match(';');
        return;
    }

    // 解析类型信息
    if (token_ == Int) {
        Match(Int);
    }
    else if (token_ == Char) {
        Match(Char);
        basetype_ = CHAR;
    }

    // 解析','分隔的变量声明
    while (token_ != ';' && token_ != '}') {
        int type = basetype_;
        // 解析指针类型，为了处理多重指针，使用while
        while (token_ == Mul) {
            Match(Mul);
            type = type + PTR;
        }

        // 是否存在标识符
        if (token_ != Id) {
            Log(std::to_string(line_) + ":错误的全局声明",CLI);
            return;
        }
        if (current_id_->symbol_class) {
            Log(std::to_string(line_) + ":重复的全局声明",CLI);
            return;
        }
        Match(Id);
        current_id_->type = type;

        // 如果是函数
        if (token_ == '(') {
            current_id_->symbol_class = Fun;
            current_id_->value = (intptr_t)(text_ + 1);
            FunctionDeclaration();
        }
        // 变量类型
        else {
            current_id_->symbol_class = Glo;
            current_id_->value = (intptr_t)data_;
            data_ = data_ + sizeof(intptr_t);
        }
        if (token_ == ',') {
            Match(',');
        }
    }

    Next();
}

/*******************************************************************************
 * 语法解析入口
*******************************************************************************/
void Interpreter::Program() {
    Next();
    while (token_ > 0) {
        GlobalDeclaration();
    }
}

/*******************************************************************************
 * 虚拟机，模拟计算机堆栈运行
*******************************************************************************/
int Interpreter::Eval() {
#define COUTASM(message) \
    if (assembly_ == true) { \
        Output(std::string(message) + "\n",CLI);\
    }

    while (true) {
        intptr_t op = *rip_++;
        switch(op) {
            case IMM:
                rax_ = *rip_ ++;
                COUTASM("mov %rip,rax");
                break;
            case LC:
                rax_ = *(char *)rax_;
                COUTASM("LC rax,rax");
                break;
            case LI:
                rax_ = *(intptr_t *)rax_;
                COUTASM("LI rax,rax");
                break;
            case SC:
                rax_ = *(char *)rsp_ = rax_;
                rsp_ ++;
                COUTASM("mov rax,rsp");
                break;
            case SI:
                *(intptr_t *)*rsp_ = rax_;
                rsp_ ++;
                COUTASM("mov rax,rsp");
                break;
            case PUSH:
                *--rsp_ = rax_;
                COUTASM("push rax");
                break;
            case JMP:
                rip_ = (intptr_t *)*rip_;
                COUTASM("JMP rip");
                break;
            case JZ:
                if (rax_ != 0) {
                    rip_ ++;
                }
                else {
                    rip_ = (intptr_t *)*rip_;
                }
                COUTASM("JZ rip");
                break;
            case JNZ:
                if (rax_ != 0) {
                    rip_ = (intptr_t *)*rip_;
                }
                else {
                    rip_ ++;
                }
                COUTASM("JNZ rip");
                break;
            case CALL:
                *--rsp_ = (intptr_t)(rip_ + 1);
                rip_ = (intptr_t *)*rip_;
                COUTASM("CALL " + std::to_string((intptr_t)rip_));
                break;
            case ENT:
                *--rsp_ = (intptr_t)rbp_;
                rbp_ = rsp_;
                rsp_ = rsp_ - *rip_;
                rip_ ++;
                COUTASM("ENT");
                break;
            case ADJ:
                rsp_ = rsp_ + *rip_;
                rip_ ++;
                COUTASM("ADJ");
                break;
            case LEV:
                rsp_ = rbp_;
                rbp_ = (intptr_t *)*rsp_;
                rsp_ ++;
                rip_ = (intptr_t *)*rsp_;
                rsp_ ++;
                COUTASM("LEV");
                break;
            case LEA:
                rax_ = (intptr_t)(rbp_ + *rip_);
                rip_ ++;
                COUTASM("LEA");
                break;
#define OPERATOR_BREAK(op) rax_ = *rsp_++ op rax_;break
            case OR:
                OPERATOR_BREAK(|);
                COUTASM("OR rsp,rax");
            case XOR:
                OPERATOR_BREAK(^);
                COUTASM("XOR rsp,rax");
            case AND:
                OPERATOR_BREAK(&);
                COUTASM("AND rsp,rax");
            case EQ:
                OPERATOR_BREAK(==);
                COUTASM("EQ rsp,rax");
            case NE:
                OPERATOR_BREAK(!=);
                COUTASM("NE rsp,rax");
            case LT:
                OPERATOR_BREAK(<);
                COUTASM("LT rsp,rax");
            case LE:
                OPERATOR_BREAK(<=);
                COUTASM("LE rsp,rax");
            case GT:
                OPERATOR_BREAK(>);
                COUTASM("GT rsp,rax");
            case GE:
                OPERATOR_BREAK(>=);
                COUTASM("GE rsp,rax");
            case SHL:
                OPERATOR_BREAK(<<);
                COUTASM("SHL rsp,rax");
            case SHR:
                OPERATOR_BREAK(>>);
                COUTASM("SHR rsp,rax");
            case ADD:
                OPERATOR_BREAK(+);
                COUTASM("ADD rsp,rax");
            case SUB:
                OPERATOR_BREAK(-);
                COUTASM("SUB rsp,rax");
            case MUL:
                OPERATOR_BREAK(*);
                COUTASM("MUL rsp,rax");
            case DIV:
                OPERATOR_BREAK(/);
                COUTASM("DIV rsp,rax");
            case MOD:
                OPERATOR_BREAK(%);
                COUTASM("MOD rsp,rax");
#undef COUTASM
#undef OPERATOR
            case EXIT:
                if (*rsp_ == 0){
                    Log("exit(" + std::to_string(*rsp_) + ")",CLI);
                }
                else {
                    Log("exit(" + std::to_string(*rsp_) + ")",CLI);
                }
                return *rsp_;
            case PRTF: {
                intptr_t *tmp = rsp_ + rip_[1];
                char buf[1024] = {0};
                rax_ = sprintf(buf,(char *)tmp[-1], tmp[-2], tmp[-3], tmp[-4], tmp[-5], tmp[-6]);

                if (!assembly_) {
                    Output(std::string(buf),CLI);
                }
                break;
            }
            case MALC:
                rax_ = (intptr_t)malloc(*rsp_);
                break;
            case MSET:
                rax_ = (intptr_t)memset((char*)rsp_[2],rsp_[1],*rsp_);
                break;
            case MCMP:
                rax_ = memcmp((char*)rsp_[2],(char*)rsp_[1],*rsp_);
                break;
            default:

//        else if (op == OPEN) { rax_ = open((char *)rsp_[1], rsp_[0]); }
//        else if (op == CLOS) { rax_ = close(*rsp_);}
//        else if (op == READ) { rax_ = read(rsp_[2], (char *)rsp_[1], *rsp_); }
                Log("未知的指令:" + std::to_string(op),CLI);
                return -1;
        }
    }
}

int Interpreter::Run(std::string& file_content) {
    line_ = 1;

    // symbols_.clear();
    symbols_map_.clear();
    memset(delete_text_, 0, pool_size_);
    memset(delete_data_, 0, pool_size_);
    memset(delete_stack_, 0, pool_size_);


#ifdef _MSC_VER
    std::string temp = "char else enum if int return sizeof while "
          "open read close printf malloc memset memcmp exit void main";
    src_ = &temp[0];
#else
    src_ = "char else enum if int return sizeof while "
          "open read close printf malloc memset memcmp exit void main";
#endif

    // 添加关键字到符号表
    for(int i = Char;i <= While;) {
        Next();
        current_id_->token_ = i++;
    }

    // add library to symbol table
    for(int i = OPEN;i <= EXIT;) {
        Next();
        current_id_->symbol_class = Sys;
        current_id_->type = INT;
        current_id_->value = i++;
    }

    Next(); 
    current_id_->token_ = Char;
    Next(); 


    src_ = &file_content[0];

    Program();

    Symbol main_id;
    if (auto main_id_it = symbols_map_.find("main"); main_id_it != symbols_map_.end()) {
        rip_ = (intptr_t *)main_id_it->second.value;
    }
    else {
        Log("main 函数未定义\n",CLI);
        return -1;
    }

    // 初始化栈
    intptr_t *tmp;
    rsp_ = (intptr_t *)((intptr_t)stack_ + pool_size_);
    *--rsp_ = EXIT; // main返回时调用EXIT
    *--rsp_ = PUSH; tmp = rsp_;
    *--rsp_ = (intptr_t)tmp;

    return Eval();
}

Interpreter::Interpreter(bool cli) : CLI(cli) {
    delete_text_ = text_ = (intptr_t *)new char[pool_size_];
    delete_data_ = data_ = (char *)new char[pool_size_];
    delete_stack_ = stack_ = (intptr_t *)new char[pool_size_];
    delete_src_ = src_ = old_src_ = (char *)new char[pool_size_];
    if (text_ == nullptr || data_ == nullptr
        || stack_ == nullptr
        || src_ == nullptr || old_src_ == nullptr) {
        Log("为虚拟机分配内存失败",CLI);
    }
}
Interpreter::~Interpreter() {
    delete delete_text_;
    delete delete_data_;
    delete delete_stack_;
    delete delete_src_;
    text_ = nullptr;
    data_ = nullptr;
    stack_ = nullptr;
    src_ = old_src_ = nullptr;
}