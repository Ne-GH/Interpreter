#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string_view>

#include "../Logs/Log.h"

class Interpreter {
public:
	Interpreter();
	~Interpreter();
	
	long long Run(std::string &file_content);

private:

};


#endif
