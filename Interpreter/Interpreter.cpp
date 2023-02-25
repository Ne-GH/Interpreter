#include "./Interpreter.h"



int token;
int line = 0;
char* src, * old_src;
int pool_size = 1024 * 256;
Log *log = nullptr;


// 堆栈信息
int *text,*old_text;
intptr_t *stack;
char *data;


// 寄存器信息
int *pc,*bp,*sp,cycle;
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

void next() {
//    token = *src++;
    token = *text ++;
    return;
}

void expression(int level) {
    // do nothing
}

void program() {
    next();
    while (token > 0) {
        // qDebug() << token;
        log->AddLog("token is: " + std::to_string(token));
        // printf("token is: %c\n", token);
        next();
    }
}


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
                pc = (int *)*pc;
                break;
            case JZ:
                if (ax != 0) {
                    pc = pc + 1;
                }
                else {
                    pc = (int *)*pc;
                }
                break;
            case JNZ:
                if(ax != 0) {
                    pc = (int *)*pc;
                }
                else {
                    pc = pc + 1;
                }
                break;

            case CALL:
                *--sp = (intptr_t)(pc + 1);
                pc = (int *)*pc;
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
                bp = (int *)*sp ++;
                pc = (int *)*sp ++;
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
                log->AddLog("ret:" + std::to_string(*sp));
                return *sp;
                break;
            case OPEN:
                break;
            case CLOS:
                break;
            case READ:
                break;
            case PRTF:{
                int *tmp = sp + pc[1];
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
                log->AddErrorLog("未知的指令"+std::to_string(op));
                return -1;
        }

    }
    return 0;
}
Interpreter::Interpreter(Log& log_tmp) : _log(log_tmp) {

}

void Interpreter::Run(std::string& file_content) {
    log = &_log;
    // 从ui获取文件内容
    // src = &file_content[0];

    text = old_text = (int *)new char[pool_size]();
    stack = (intptr_t *)new char[pool_size]();
    data = new char[pool_size]();
    if(text == nullptr || old_text == nullptr
        || stack == nullptr || data == nullptr) {
        log->AddErrorLog("为虚拟机分配内存失败");
        return;
    }

    bp = sp = (int *)((intptr_t)stack + pool_size);
    ax = 0;

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

Interpreter::~Interpreter() {  }
