#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string_view>

#include "../Logs/Log.h"

class Interpreter {
public:
	explicit Interpreter(Log & log);
	~Interpreter();
	
	// 词法分析
	void next();
	// 语法分析
	void expression(int level);
	// 解析表达式
	void program();
	// 虚拟机入口
	int eval();
private:
	Log& _log;
	std::string_view _file_string;
	int _token;
	char* _src, * old_src;
	int _poolsize;
	int _line_number;
};


#endif
