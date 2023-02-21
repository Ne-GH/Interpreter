#include "./Interpreter.h"

Interpreter::Interpreter(Log& log) : _log(log) { 
	_log.AddLog("Interpreter");
}


Interpreter::~Interpreter() {  }
