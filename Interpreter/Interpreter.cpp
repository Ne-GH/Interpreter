#include "Interpreter.h"
#include "../RunWidget/RunWidget.h"
#include <fstream>
#define COUTASM(message) \
    if (_assembly == true) { \
        RUNRESULT.Output(std::string(message) + "\n"); \
    }


/*******************************************************************************
 * 词法分析
 * 每次处理一个标记_token
*******************************************************************************/
void Interpreter::Next() {
    char *last_pos;
    intptr_t hash = 0;

    while (_token = *_src) {
        LOG.AddLog("Next" + std::to_string(_token));
        _src++;

        // 变量
        if (std::isalpha(_token) || (_token == '_')) {
            last_pos = _src - 1;
            hash = _token;
            while (std::isalnum(*_src) || (*_src == '_')) {
                hash = hash * 147 + *_src;
                _src++;
            }

            for (auto& tmp : symbols) {
                current_id = &tmp;
                if (tmp._hash == hash && tmp._name == std::string(last_pos, (int)(_src - last_pos))) {
                    _token = tmp._token;
                    return;
                }
            }
            //current_id = symbols;
            //while (current_id->_token) {
            //    // 该标记在符号表中不存在，即hash和name均相同
            //    if (current_id->_hash == hash && current_id->_name == std::string(last_pos,(int)(_src-last_pos))) {
            //        _token = current_id->_token;
            //        return;
            //    }
            //    current_id++;
            //    //current_id = current_id + IdSize;
            //}
            Symbol tmp;
            tmp._name = std::string(last_pos, (int)(_src - last_pos));
            tmp._hash = hash;
            _token = tmp._token = Id;
            symbols.push_back(tmp);
            current_id = &symbols.back();
            // current_id->_name = std::string(last_pos,(int)(_src-last_pos));
            // current_id->_hash = hash;
            //_token = current_id->_token = Id;
            return;
        }
        // 数字字面量
        else if (std::isdigit(_token) != false) {
            token_val = _token - '0';
            // 十进制
            if (token_val > 0) {
                while (std::isdigit(*_src)) {
                    token_val = token_val*10 + *_src++ - '0';
                }
            }
            else {
                // 16进制
                if (*_src == 'x' || *_src == 'X') {
                    _token = *++_src;
                    while (std::isdigit(_token) || (_token >= 'a' && _token <= 'f') || (_token >= 'A' && _token <= 'F')) {
                        // 取_token的低4位，后根据是大写还是小写进行+9 或 +0的修正
                        token_val = token_val * 16 + (_token & 15) + (_token >= 'A' ? 9 : 0);
                        _token = *++_src;
                    }
                }
                // 8进制
                else {
                    while (*_src >= '0' && *_src <= '7') {
                        token_val = token_val*8 + *_src++ - '0';
                    }
                }
            }
            _token = Num;
            return;
        }
        else switch (_token) {
            case '\n':
                ++_line;
                break;
            case '#':
                while (*_src != 0 && *_src != '\n') {
                    _src ++;
                }
                break;
            case '/':   // //
                if (*_src == '/') {
                    while (*_src != 0 && *_src != '\n') {
                        ++_src;
                    }
                }
                else {  // /
                    _token = Div;
                    return;
                }
                break;
            case '"':
            case '\'':
                last_pos = _data;
                // 字符串是否结束
                while (*_src != 0 && *_src != _token) {
                    // 如果是单个字符，仅会保留最后一个字符
                    token_val = *_src++;
                    // 支持转义'\n'
                    if (token_val == '\\') {
                        token_val = *_src++;
                        if (token_val == 'n') {
                            token_val = '\n';
                        }
                    }
                    // 如果是字符串，则存储在_data中
                    if (_token == '"') {
                        *_data++ = token_val;
                    }
                }
                _src++;
                if (_token == '"') {
                    token_val = (intptr_t)last_pos;
                }
                // 单个字符被认为是数字
                else {
                    _token = Num;
                }
                return;
                break;
            case '=':
                if (*_src == '=') {  // ==
                    _src ++;
                    _token = Eq;
                }
                else {              // =
                    _token = Assign;
                }
                return;
                break;
            case '+':
                if (*_src == '+') {  // ++
                    _src ++;
                    _token = Inc;
                }
                else {              // +
                    _token = Add;
                }
                return;
                break;
            case '-':
                if (*_src == '-') {  // --
                    _src ++;
                    _token = Dec;
                }
                else {              // -
                    _token = Sub;
                }
                return;
                break;
            case '!':
                if (*_src == '=') {  // !=
                    _src++;
                    _token = Ne;
                }
                return;
                break;
            case '<':
                if (*_src == '=') {      // <=
                    _src ++;
                    _token = Le;
                }
                else if (*_src == '<') { // <<
                    _src ++;
                    _token = Shl;
                }
                else {                  // <
                    _token = Lt;
                }
                return;
                break;
            case '>':
                if (*_src == '=') {      // >=
                    _src ++;
                    _token = Ge;
                }
                else if (*_src == '>') { // >>
                    _src ++;
                    _token = Shr;
                }
                else {                  // >
                    _token = Gt;
                }
                return;
                break;
            case '|':
                if (*_src == '|') {      // ||
                    _src ++;
                    _token = Lor;
                }
                else {                  // |
                    _token = Or;
                }
                return;
                break;
            case '&':
                if (*_src == '&') {      // &&
                    _src ++;
                    _token = Lan;
                }
                else {                  // &
                    _token = And;
                }
                return;
                break;
            case '^':
                _token = Xor;
                return;
                break;
            case '%':
                _token = Mod;
                return;
                break;
            case '*':
                _token = Mul;
                return;
                break;
            case '[':
                _token = Brak;
                return;
                break;
            case '?':
                _token = Cond;
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
                // 无需进一步处理，直接将字符本身作为_token返回
                return;
                break;
        }
    }
}
/*******************************************************************************
 * 查看当前_token是否是我们的期望
 * 如果与期望的_token一致则获取下一个_token
 * 否则exit -1
*******************************************************************************/
void Interpreter::Match(int token) {
    if (_token == token) {
        Next();
    }
    else {
        LOG.AddErrorLog(
                "在第" + std::to_string(_line) + "行,"
                + "期望的_token为:" + std::to_string(token)
                + "实际的_token为:" + std::to_string(_token));
        exit(-1);
    }
}

/*******************************************************************************
 * 递归下降 BNF
 * 语法分析,解析表达式，递归生成语法树
 * 参数level为当前运算符的优先级
*******************************************************************************/
void Interpreter::Expression(int level) {
    if (!_token) {
        LOG.AddErrorLog(std::to_string(_line) + ":遇到EOF,意外结束");
        exit(-1);
    }
    LOG.AddLog("Expression" + std::to_string(_token) + "level" + std::to_string(level));
    switch (_token) {
        case Num: {
            Match(Num);

            *++_text = IMM;
            *++_text = token_val;
            _expr_type = INT;
            break;

        }
        case '"': {
            *++_text = IMM;
            *++_text = token_val;

            // 多行字符串支持
            Match('"');
            while (_token == '"') {
                Match('"');
            }

            // 内存对齐，并添加NUL
            _data = (char *)(((intptr_t)_data + sizeof(intptr_t)) & (-sizeof(intptr_t)));
            _expr_type = PTR;        // 字符串表达式的结果是指针
            break;

        }
        case Sizeof: {

            Match(Sizeof);
            Match('(');
            _expr_type = INT;

            if (_token == Int) {
                Match(Int);
            }
            else if (_token == Char) {
                Match(Char);
                _expr_type = CHAR;
            }
            // 多重指针
            while (_token == Mul) {
                Match(Mul);
                _expr_type = _expr_type + PTR;
            }

            Match(')');

            *++_text = IMM;
            *++_text = (_expr_type == CHAR) ? sizeof(char) : sizeof(intptr_t);

            _expr_type = INT;            // sizeof表达式的结果是int
            break;
        }
        case Id: {
            Match(Id);
            auto id = current_id;

            // 函数调用
            if (_token == '(') {
                Match('(');

                intptr_t arg_num = 0;
                while (_token != ')') {
                    Expression(Assign);
                    *++_text = PUSH;
                    arg_num ++;

                    if (_token == ',') {
                        Match(',');
                    }

                }
                Match(')');

                // 系统调用,直接给出系统函数地址，无需call
                if (id->_class == Sys) {
                    *++_text = id->_value;
                }
                    // 函数调用
                else if (id->_class == Fun) {
                    *++_text = CALL;
                    *++_text = id->_value;
                }
                else {
                    LOG.AddErrorLog(std::to_string(_line) + "错误的函数调用");
                    exit(-1);
                }

                // 清除栈中的参数
                if (arg_num > 0) {
                    *++_text = ADJ;
                    *++_text = arg_num;
                }
                _expr_type = id->_type;
            }
                // 枚举
            else if (id->_class == Num) {
                *++_text = IMM;
                *++_text = id->_value;
                _expr_type = INT;
            }
                // 变量
            else {
                // 局部变量
                if (id->_class == Loc) {
                    *++_text = LEA;
                    *++_text = _index_of_bp - id->_value;
                }
                    // 全局变量
                else if (id->_class == Glo) {
                    *++_text = IMM;
                    *++_text = id->_value;
                }
                else {
                    LOG.AddErrorLog(std::to_string(_line) + "未定义的变量");
                    exit(-1);
                }

                _expr_type = id->_type;
                *++_text = (_expr_type == CHAR) ? LC : LI;
            }
            break;

        }
        case '(': { // 类型转换
            Match('(');
            if (_token == Int || _token == Char) {
                int tmp_type = (_token == Char) ? CHAR : INT; // 类型转换
                Match(_token);
                while (_token == Mul) {
                    Match(Mul);
                    tmp_type = tmp_type + PTR;
                }

                Match(')');

                Expression(Inc); // 处理优先级

                _expr_type  = tmp_type;
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
            if (_expr_type >= PTR) {
                _expr_type = _expr_type - PTR;
            }
            else {
                LOG.AddErrorLog(std::to_string(_line) + "错误的解引用");
                exit(-1);
            }

            *++_text = (_expr_type == CHAR) ? LC : LI;
            break;
        }
        case And: {
            // 取地址
            Match(And);
            Expression(Inc);
            // 避免递归解析时产生的指令影响指令运行
            if (*_text == LC || *_text == LI) {
                _text --;
            }
            else {
                LOG.AddErrorLog(std::to_string(_line) + "错误的取地址");
                exit(-1);
            }

            _expr_type = _expr_type + PTR;
            break;
        }
        case '!': {
            Match('!');
            Expression(Inc);

            *++_text = PUSH;
            *++_text = IMM;
            *++_text = 0;
            *++_text = EQ;

            _expr_type = INT;
            break;
        }
        case '~': {
            // num ^ 0xfff
            Match('~');
            Expression(Inc);
            *++_text = PUSH;
            *++_text = IMM;
            *++_text = -1;
            *++_text = XOR;

            _expr_type = INT;
            break;
        }
        case Add: {
            // 正
            Match(Add);
            Expression(Inc);

            _expr_type = INT;
            break;
        }
        case Sub: {
            // 负
            Match(Sub);
            // 数字直接取负
            if (_token == Num) {
                *++_text = IMM;
                *++_text = -token_val;
                Match(Num);
            }
            // 表达式递归求值 * -1
            else {
                *++_text = IMM;
                *++_text = -1;
                *++_text = PUSH;
                Expression(Inc);
                *++_text = MUL;
            }

            _expr_type = INT;
            break;
        }
        case Inc:
        case Dec: {
            // 前置 ++ --
            intptr_t old_token = _token;
            Match(_token);
            Expression(Inc);
            // LC和LI的出现表示当前操作的是数组或指针
            if (*_text == LC) {
                *_text = PUSH;
                *++_text = LC;
            }
            else if (*_text == LI) {
                *_text = PUSH;
                *++_text = LI;
            }
            else {
                LOG.AddErrorLog(std::to_string(_line) + "不可前置操作的左值");
                exit(-1);
            }
            *++_text = PUSH;
            *++_text = IMM;
            *++_text = (_expr_type > PTR) ? sizeof(intptr_t) : sizeof(char);
            *++_text = (old_token == Inc) ? ADD : SUB;
            *++_text = (_expr_type == CHAR) ? SC : SI;
            break;
        }
        default:
            LOG.AddErrorLog(std::to_string(_line) + "编译错误");
            exit(-1);

            break;
    }
    // 为了处理右侧所有优先级更高的情况，因此这里用while,不断向右扫描，直到遇到优先级小于当前优先级的运算符
    while (_token >= level) {
        // 根据当前优先级处理
        intptr_t tmp_type = _expr_type;
        switch (_token) {
            // 赋值运算符
            case Assign:
                Match(Assign);
                if (*_text == LC || *_text == LI) {
                    *_text = PUSH;
                }
                else {
                    LOG.AddErrorLog(std::to_string(_line) + ":赋值时出现错误的左值");
                    exit(-1);
                }
                Expression(Assign);

                _expr_type = tmp_type;
                *++_text = (_expr_type == CHAR) ? SC : SI;
                break;
            // 三目运算符(if)
            case Cond: {
                Match(Cond);
                *++_text = JZ;
                intptr_t *addr = ++_text;
                Expression(Assign);
                if (_token == ':') {
                    Match(':');
                }
                else {
                    LOG.AddErrorLog(std::to_string(_line) + ":三目运算符缺少':'");
                    exit(-1);
                }
                *addr = (intptr_t)(_text + 3);
                *++_text = JMP;
                addr = ++_text;
                Expression(Cond);
                *addr = (intptr_t)(_text + 1);
                break;
            }
            // ||
            case Lor: {
                Match(Lor);
                *++_text = JNZ;
                intptr_t* addr = ++_text;
                Expression(Lan);
                *addr = (intptr_t)(_text + 1);
                _expr_type = INT;
                break;
            }
            // &&
            case Lan: {
                Match(Lan);
                *++_text = JZ;
                intptr_t *addr = ++_text;
                Expression(Or);
                *addr = (intptr_t)(_text + 1);
                _expr_type = INT;
                break;
            }
            // |
            case Or:
                Match(Or);
                *++_text = PUSH;
                Expression(Xor);
                *++_text = OR;
                _expr_type = INT;
                break;
            // ^
            case Xor:
                Match(Xor);
                *++_text = PUSH;
                Expression(And);
                *++_text = XOR;
                _expr_type = INT;
                break;
            // &
            case And:
                Match(And);
                *++_text = PUSH;
                Expression(Eq);
                *++_text = AND;
                _expr_type = INT;
                break;
            // ==
            case Eq:
                Match(Eq);
                *++_text = PUSH;
                Expression(Ne);
                *++_text = EQ;
                _expr_type = INT;
                break;
            // !=
            case Ne:
                Match(Ne);
                *++_text = PUSH;
                Expression(Lt);
                *++_text = NE;
                _expr_type = INT;
                break;
            // <
            case Lt:
                Match(Lt);
                *++_text = PUSH;
                Expression(Shl);
                *++_text = LT;
                _expr_type = INT;
                break;
            case Gt:
                Match(Gt);
                *++_text = PUSH;
                Expression(Shl);
                *++_text = GT;
                _expr_type = INT;
                break;
            // <=
            case Le:
                Match(Le);
                *++_text = PUSH;
                Expression(Shl);
                *++_text = LE;
                _expr_type = INT;
                break;
            // >=
            case Ge:
                Match(Ge);
                *++_text = PUSH;
                Expression(Shl);
                *++_text = GE;
                _expr_type = INT;
                break;
            // <<
            case Shl:
                Match(Shl);
                *++_text = PUSH;
                Expression(Add);
                *++_text = SHL;
                _expr_type = INT;
                break;
            // >>
            case Shr:
                Match(Shr);
                *++_text = PUSH;
                Expression(Add);
                *++_text = SHR;
                _expr_type = INT;
                break;
            // +
            case Add: {
                Match(Add);
                *++_text = PUSH;
                Expression(Mul);

                _expr_type = tmp_type;
                // 指针 + val ==> 值 + sizeof(intptr_t) * val
                if (_expr_type > PTR) {
                    *++_text = PUSH;
                    *++_text = IMM;
                    *++_text = sizeof(intptr_t);
                    *++_text = MUL;
                }
                *++_text = ADD;
                break;
            }
            // -
            case Sub: {
                Match(Sub);
                *++_text = PUSH;
                Expression(Mul);
                // 指针相减
                if (tmp_type > PTR && tmp_type == _expr_type) {
                    *++_text = SUB;
                    *++_text = PUSH;
                    *++_text = IMM;
                    *++_text = sizeof(intptr_t);
                    *++_text = DIV;
                    _expr_type = INT;
                }
                // 指针减法
                else if (tmp_type > PTR) {
                    *++_text = PUSH;
                    *++_text = IMM;
                    *++_text = sizeof(intptr_t);
                    *++_text = MUL;
                    *++_text = SUB;
                    _expr_type = tmp_type;
                }
                // 数字减法
                else {
                    *++_text = SUB;
                    _expr_type = tmp_type;
                }
                break;
            }
            // *
            case Mul:
                Match(Mul);
                *++_text = PUSH;
                Expression(Inc);
                *++_text = MUL;
                _expr_type = tmp_type;
                break;
            // /
            case Div:
                Match(Div);
                *++_text = PUSH;
                Expression(Inc);
                *++_text = DIV;
                _expr_type = tmp_type;
                break;
            // %
            case Mod:
                Match(Mod);
                *++_text = PUSH;
                Expression(Inc);
                *++_text = MOD;
                _expr_type = tmp_type;
                break;
            // 后置++ --
            case Inc:
            case Dec:
                if (*_text == LI) {
                    *_text = PUSH;
                    *++_text = LI;
                }
                else if (*_text == LC) {
                    *_text = PUSH;
                    *++_text = LC;
                }
                else {
                    LOG.AddErrorLog(std::to_string(_line) + ":错误的后置自增");
                    exit(-1);
                }

                *++_text = PUSH;
                *++_text = IMM;
                *++_text = (_expr_type > PTR) ? sizeof(intptr_t) : sizeof(char);
                *++_text = (_token == Inc) ? ADD : SUB;
                *++_text = (_expr_type == CHAR) ? SC : SI;
                *++_text = PUSH;
                *++_text = IMM;
                *++_text = (_expr_type > PTR) ? sizeof(intptr_t) : sizeof(char);
                *++_text = (_token == Inc) ? SUB : ADD;
                Match(_token);
                break;
            // [] ==> *(arr + val)
            case Brak:
                Match(Brak);
                *++_text = PUSH;
                Expression(Assign);
                Match(']');

                if (tmp_type > PTR) {
                    *++_text = PUSH;
                    *++_text = IMM;
                    *++_text = sizeof(intptr_t);
                    *++_text = MUL;
                }
                else if (tmp_type < PTR) {
                    LOG.AddErrorLog(std::to_string(_line) + ":期望得到指针类型");
                    exit(-1);
                }
                _expr_type = tmp_type - PTR;
                *++_text = ADD;
                *++_text = (_expr_type == CHAR) ? LC : LI;
                break;
            default:
                    LOG.AddErrorLog(std::to_string(_line) + ":编译错误,_token = " + std::to_string(_token));
                    exit(-1);
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
    LOG.AddLog("Statement" + std::to_string(_token));
    if (_token == If) {
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

        *++_text = JZ;
        b = ++_text;

        Statement();         // 解析语句
        if (_token == Else) { // 解析else
            Match(Else);

            *b = (intptr_t)(_text + 3);
            *++_text = JMP;
            b = ++_text;

            Statement();
        }

        *b = (intptr_t)(_text + 1);
    }
    else if (_token == While) {
        //
        // a:                     a:
        //    while (<cond>)        <cond>
        //                          JZ b
        //     <Statement>          <Statement>
        //                          JMP a
        // b:                     b:
        Match(While);

        a = _text + 1;

        Match('(');
        Expression(Assign);
        Match(')');

        *++_text = JZ;
        b = ++_text;

        Statement();

        *++_text = JMP;
        *++_text = (intptr_t)a;
        *b = (intptr_t)(_text + 1);
    }
    else if (_token == '{') {
        Match('{');

        while (_token != '}') {
            Statement();
        }

        Match('}');
    }
    else if (_token == Return) {
        // return [Expression];
        Match(Return);

        if (_token != ';') {
            Expression(Assign);
        }

        Match(';');

        // 返回
        *++_text = LEV;
    }
    else if (_token == ';') {
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
    while (_token != '}') {
        if (_token != Id) {
            LOG.AddErrorLog(std::to_string(_line) + ":错误enum标识符");
            exit(-1);
        }
        Next();
        // {VAL = 10,VAL2 = 20}
        if (_token == Assign) {
            Next();
            if (_token != Num) {
                LOG.AddErrorLog(std::to_string(_line) + "错误的enum初始化值");
                exit(-1);
            }
            enum_val = token_val;
            Next();
        }

        current_id->_class = Num;
        current_id->_type = INT;
        current_id->_value = enum_val++;

        if (_token == ',') {
            Next();
        }
    }
}

/*******************************************************************************
 * 解析函数参数
*******************************************************************************/
void Interpreter::FunctionParameter() {
    intptr_t params = 0;
    while (_token != ')') {
        intptr_t type = INT;
        if (_token == Int) {
            Match(Int);
        }
        else if (_token == Char) {
            type = CHAR;
            Match(Char);
        }

        // 指针类型
        while (_token == Mul) {
            Match(Mul);
            type = type + PTR;
        }

        // 参数名
        if (_token != Id) {
            LOG.AddErrorLog(std::to_string(_line) + ":错误的参数声明");
            exit(-1);
        }
        if (current_id->_class == Loc) {
            LOG.AddErrorLog(std::to_string(_line) + ":重复的参数声明");
            exit(-1);
        }

        Match(Id);
        // 局部覆盖全局
        current_id->_backup_class = current_id->_class; current_id->_class  = Loc;
        current_id->_backup_type  = current_id->_type;  current_id->_type   = type;
        current_id->_backup_value = current_id->_value; current_id->_value  = params++;

        if (_token == ',') {
            Match(',');
        }
    }
    // params 保存参数数量，并修改_index_of_bp来调整函数的栈帧
    _index_of_bp = params+1;
}

/*******************************************************************************
 * 解析函数体
*******************************************************************************/
void Interpreter::FunctionBody() {
    intptr_t pos_local = _index_of_bp; // 本地变量所在栈地址
    intptr_t type;

    while (_token == Int || _token == Char) {
        _basetype = (_token == Int) ? INT : CHAR;
        Match(_token);

        while (_token != ';') {
            type = _basetype;
            while (_token == Mul) {
                Match(Mul);
                type = type + PTR;
            }

            if (_token != Id) {
                LOG.AddErrorLog(std::to_string(_line) + ":错误的本地变量声明");
                exit(-1);
            }
            if (current_id->_class == Loc) {
                LOG.AddErrorLog(std::to_string(_line) + "本地变量重复声明");
                exit(-1);
            }
            Match(Id);

            // 保存本地变量
            current_id->_backup_class = current_id->_class; current_id->_class  = Loc;
            current_id->_backup_type  = current_id->_type;  current_id->_type   = type;
            current_id->_backup_value = current_id->_value; current_id->_value  = ++pos_local;

            if (_token == ',') {
                Match(',');
            }
        }
        Match(';');
    }

    // 在函数调用时开辟参数所需大小
    *++_text = ENT;
    *++_text = pos_local - _index_of_bp;

    while (_token != '}') {
        Statement();
    }

    // 函数返回
    *++_text = LEV;
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
    for (auto& up : symbols) {
        current_id = &up;
        if (up._class == Loc) {
            up._class = up._backup_class;
            up._type = up._backup_type;
            up._value = up._backup_value;
        }
    }

    //current_id = symbols;
    //while (current_id->_token) {
    //    if (current_id->_class == Loc) {
    //        current_id->_class = current_id->_backup_class;
    //        current_id->_type  = current_id->_backup_type;
    //        current_id->_value = current_id->_backup_value;
    //    }
    //    current_id++;
    //    //current_id = current_id + IdSize;
    //}
}

/*******************************************************************************
 * 全局的变量定义，类型定义（仅支持enum），函数定义
*******************************************************************************/
void Interpreter::GlobalDeclaration() {
    // int [*]id [; | (...) {...}]
    _basetype = INT;

    // 解析enum
    // enum [id] { a = 10, b = 20, ... }
    if (_token == Enum) {
        Match(Enum);
        if (_token != '{') {
            Match(Id); // 匹配id
        }
        if (_token == '{') {
            Match('{');
            EnumDeclaration();
            Match('}');
        }

        Match(';');
        return;
    }

    // 解析类型信息
    if (_token == Int) {
        Match(Int);
    }
    else if (_token == Char) {
        Match(Char);
        _basetype = CHAR;
    }

    // 解析','分隔的变量声明
    while (_token != ';' && _token != '}') {
        int type = _basetype;
        // 解析指针类型，为了处理多重指针，使用while
        while (_token == Mul) {
            Match(Mul);
            type = type + PTR;
        }

        // 是否存在标识符
        if (_token != Id) {
            LOG.AddErrorLog(std::to_string(_line) + ":错误的全局声明");
            exit(-1);
        }
        if (current_id->_class) {
            LOG.AddErrorLog(std::to_string(_line) + ":重复的全局声明");
            exit(-1);
        }
        Match(Id);
        current_id->_type = type;

        // 如果是函数
        if (_token == '(') {
            current_id->_class = Fun;
            current_id->_value = (intptr_t)(_text + 1);
            FunctionDeclaration();
        }
        // 变量类型
        else {
            current_id->_class = Glo;
            current_id->_value = (intptr_t)_data;
            _data = _data + sizeof(intptr_t);
        }
        if (_token == ',') {
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
    while (_token > 0) {
        GlobalDeclaration();
    }
}

/*******************************************************************************
 * 虚拟机，模拟计算机堆栈运行
*******************************************************************************/
int Interpreter::Eval() {
    while (true) {
        intptr_t op = *_rip++;
        switch(op) {
            case IMM:
                _rax = *_rip ++;
                COUTASM("mov %_rip,_rax");
                break;
            case LC:
                _rax = *(char *)_rax;
                COUTASM("LC _rax,_rax");
                break;
            case LI:
                _rax = *(intptr_t *)_rax;
                COUTASM("LI _rax,_rax");
                break;
            case SC:
                _rax = *(char *)_rsp = _rax;
                _rsp ++;
                COUTASM("mov _rax,_rsp");
                break;
            case SI:
                *(intptr_t *)*_rsp = _rax;
                _rsp ++;
                COUTASM("mov _rax,_rsp");
                break;
            case PUSH:
                *--_rsp = _rax;
                COUTASM("push _rax");
                break;
            case JMP:
                _rip = (intptr_t *)*_rip;
                COUTASM("JMP _rip");
                break;
            case JZ:
                if (_rax != 0) {
                    _rip ++;
                }
                else {
                    _rip = (intptr_t *)*_rip;
                }
                COUTASM("JZ _rip");
                break;
            case JNZ:
                if (_rax != 0) {
                    _rip = (intptr_t *)*_rip;
                }
                else {
                    _rip ++;
                }
                COUTASM("JNZ _rip");
                break;
            case CALL:
                *--_rsp = (intptr_t)(_rip + 1);
                _rip = (intptr_t *)*_rip;
                COUTASM("CALL " + std::to_string((intptr_t)_rip));
                break;
            case ENT:
                *--_rsp = (intptr_t)_rbp;
                _rbp = _rsp;
                _rsp = _rsp - *_rip;
                _rip ++;
                COUTASM("ENT");
                break;
            case ADJ:
                _rsp = _rsp + *_rip;
                _rip ++;
                COUTASM("ADJ");
                break;
            case LEV:
                _rsp = _rbp;
                _rbp = (intptr_t *)*_rsp;
                _rsp ++;
                _rip = (intptr_t *)*_rsp;
                _rsp ++;
                COUTASM("LEV");
                break;
            case LEA:
                _rax = (intptr_t)(_rbp + *_rip);
                _rip ++;
                COUTASM("LEA");
                break;
#define OPERATOR_BREAK(op) _rax = *_rsp++ op _rax;break
            case OR:
                OPERATOR_BREAK(|);
                COUTASM("OR *_rsp,_rax");
            case XOR:
                OPERATOR_BREAK(^);
                COUTASM("XOR _rsp,_rax");
            case AND:
                OPERATOR_BREAK(&);
                COUTASM("AND _rsp,_rax");
            case EQ:
                OPERATOR_BREAK(==);
                COUTASM("EQ _rsp,_rax");
            case NE:
                OPERATOR_BREAK(!=);
                COUTASM("NE _rsp,_rax");
            case LT:
                OPERATOR_BREAK(<);
                COUTASM("LT _rsp,_rax");
            case LE:
                OPERATOR_BREAK(<=);
                COUTASM("LE _rsp,_rax");
            case GT:
                OPERATOR_BREAK(>);
                COUTASM("GT _rsp,_rax");
            case GE:
                OPERATOR_BREAK(>=);
                COUTASM("GE _rsp,_rax");
            case SHL:
                OPERATOR_BREAK(<<);
                COUTASM("SHL _rsp,_rax");
            case SHR:
                OPERATOR_BREAK(>>);
                COUTASM("SHR _rsp,_rax");
            case ADD:
                OPERATOR_BREAK(+);
                COUTASM("ADD _rsp,_rax");
            case SUB:
                OPERATOR_BREAK(-);
                COUTASM("SUB _rsp,_rax");
            case MUL:
                OPERATOR_BREAK(*);
                COUTASM("MUL _rsp,_rax");
            case DIV:
                OPERATOR_BREAK(/);
                COUTASM("DIV _rsp,_rax");
            case MOD:
                OPERATOR_BREAK(%);
                COUTASM("MOD _rsp,_rax");
#undef OPERATOR
            case EXIT:
                if (*_rsp == 0){
                    LOG.AddLog("exit(" + std::to_string(*_rsp) + ")");
                }
                else {
                    LOG.AddErrorLog("exit(" + std::to_string(*_rsp) + ")");
                }
                return *_rsp;
                break;
            case PRTF: {
                intptr_t *tmp = _rsp + _rip[1];
                char buf[1024] = {0};
                _rax = sprintf(buf,(char *)tmp[-1], tmp[-2], tmp[-3], tmp[-4], tmp[-5], tmp[-6]);

                if (!_assembly)
                    RUNRESULT.Output(std::string(buf));
                break;
            }
            case MALC:
                _rax = (intptr_t)malloc(*_rsp);
                break;
            case MSET:
                _rax = (intptr_t)memset((char*)_rsp[2],_rsp[1],*_rsp);
                break;
            case MCMP:
                _rax = memcmp((char*)_rsp[2],(char*)_rsp[1],*_rsp);
                break;
            default:

//        else if (op == OPEN) { _rax = open((char *)_rsp[1], _rsp[0]); }
//        else if (op == CLOS) { _rax = close(*_rsp);}
//        else if (op == READ) { _rax = read(_rsp[2], (char *)_rsp[1], *_rsp); }
                LOG.AddErrorLog("未知的指令:" + std::to_string(op));
                return -1;
                break;
        }
    }
}

int Interpreter::Run(std::string& file_content) {
    _line = 1;

    memset(_text, 0, _pool_size);
    memset(_data, 0, _pool_size);
    memset(_stack, 0, _pool_size);


#ifdef _MSC_VER
    std::string temp = "char else enum if int return sizeof while "
          "open read close printf malloc memset memcmp exit void main";
    _src = &temp[0];
#else
    _src = "char else enum if int return sizeof while "
          "open read close printf malloc memset memcmp exit void main";
#endif

    // 添加关键字到符号表
    for(int i = Char;i <= While;) {
        Next();
        current_id->_token = i++;
    }

    // add library to symbol table
    for(int i = OPEN;i <= EXIT;) {
        Next();
        current_id->_class = Sys;
        current_id->_type = INT;
        current_id->_value = i++;
    }

    Next(); 
    current_id->_token = Char;
    Next(); 


    _src = &file_content[0];

    Program();

    Symbol main_id;
    for (const auto& tmp : symbols) {
        if (tmp._name == "main") {
            main_id = tmp;
        }
    }
    if (!(_rip = (intptr_t *)(main_id._value))) {
        LOG.AddErrorLog("main 函数未定义\n");
        return -1;
    }


    // 初始化栈
    intptr_t *tmp;
    _rsp = (intptr_t *)((intptr_t)_stack + _pool_size);
    *--_rsp = EXIT; // main返回时调用EXIT
    *--_rsp = PUSH; tmp = _rsp;
    *--_rsp = (intptr_t)tmp;

    return Eval();
}

Interpreter::Interpreter() {
    _delete_text = _text = (intptr_t *)new char[_pool_size];
    _delete_data = _data = (char *)new char[_pool_size];
    _delete_stack = _stack = (intptr_t *)new char[_pool_size];
    _delete_src = _src = _old_src = (char *)new char[_pool_size];
    if (_text == nullptr || _data == nullptr
        || _stack == nullptr
        || _src == nullptr || _old_src == nullptr) {
        LOG.AddErrorLog("为虚拟机分配内存失败");
    }
}
Interpreter::~Interpreter() {
    delete _delete_text;
    delete _delete_data;
    delete _delete_stack;
    delete _delete_src;
    _text = nullptr;
    _data = nullptr;
    _stack = nullptr;
    _src = _old_src = nullptr;
}