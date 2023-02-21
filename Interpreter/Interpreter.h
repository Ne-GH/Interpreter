#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string_view>

#include "../Logs/Log.h"

class Interpreter {
public:
	Interpreter(Log & log);
	~Interpreter();
	
	void Run(std::string &file_content);

private:
	Log& _log;

};


#endif
