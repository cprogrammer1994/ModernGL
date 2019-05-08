#ifndef __PROGRAM_FORMATERRORS__HPP__
#define __PROGRAM_FORMATERRORS__HPP__

#include <string>

#include "GLMethods.hpp"

int print_errors( std::string& errors, const char *log, const char *source );
int program_print_errors(std::string& errors, const GLMethods & gl, GLint shader_obj);

#endif