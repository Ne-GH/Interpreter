#include "./Interpreter.h"
int token;
int line;
char* src, * old_src;
int poolsize;
int line_number;
void next() {
    token = *src++;
    return;
}

void expression(int level) {
    // do nothing
}

void program(Log &log) {
    next();
    while (token > 0) {
        // qDebug() << token;
        log.AddLog("token is: " + std::to_string(token));
        // printf("token is: %c\n", token);
        next();
    }
}


int eval() { // do nothing yet
    return 0;
}
Interpreter::Interpreter(Log& log) : _log(log) { 

    
}

void Interpreter::Run(std::string& file_content) {

    poolsize = 256 * 1024;
    line = 1;

    src = &file_content[0];
    program(_log);
    eval();
}

Interpreter::~Interpreter() {  }
