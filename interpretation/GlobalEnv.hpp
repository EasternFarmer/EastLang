#pragma once
#include <iostream>
#include "ValueTypes.hpp"

void print_runtime_val(RuntimeVal* var);

RuntimeVal* print(std::vector<RuntimeVal*> args);

RuntimeVal* type(std::vector<RuntimeVal*> args);

Environment* makeGlobalEnv();