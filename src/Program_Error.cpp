// Links:
// - http://perso.univ-lyon1.fr/jean-claude.iehl/Public/educ/M1IMAGE/html/group__tuto2GL.html
// - https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glShaderSource.xhtml
#include <vector>
#ifndef INT_MAX
// http://www.cplusplus.com/reference/climits/
#include <climits>
#endif

#ifdef _MSC_VER
    #define SSCANF sscanf_s
#else
    #include <cstdio>
    #define SSCANF std::sscanf
#endif

#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;

#include "Program_Error.hpp"


#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif


static
void print_line( std::string& errors, const char *source, const int begin_id, const int line_id )
 {
     int line= 0;
     char last= '\n';
     for(unsigned int i= 0; source[i] != 0; i++)
     {
         if(line > line_id)
             break;

         if(last == '\n')
         {
             line++;
             if(line >= begin_id && line <= line_id)
             {
                   std::stringstream buffer;
                   buffer << "  " << setfill('0') << setw(4) << line << "  ";
                   errors.append(buffer.str());
             }
         }

         if(line >= begin_id && line <= line_id)
         {
             if(source[i] == '\t')
                 errors.append("    ");
             else
                 errors.push_back(source[i]);
         }
         last= source[i];
     }
 }

int print_errors( std::string& errors, const char *log, const char *source )
{
     int first_error= INT_MAX;
     int last_string= -1;
     int last_line= -1;
     for(int i= 0; log[i] != 0; i++)
     {
         // get source code line associate to error
         int string_id= 0, line_id= 0, position= 0;
         if(SSCANF(&log[i], "%d ( %d ) : %n", &string_id, &line_id, &position) == 2           // nvidia syntax
         || SSCANF(&log[i], "%d : %d (%*d) : %n", &string_id, &line_id, &position) == 2       // mesa syntax
         || SSCANF(&log[i], "ERROR : %d : %d : %n", &string_id, &line_id, &position) == 2     // ati syntax
         || SSCANF(&log[i], "WARNING : %d : %d : %n", &string_id, &line_id, &position) == 2)  // ati syntax
         {
             if(string_id != last_string || line_id != last_line)
             {
                 // save the first error
                 first_error=MIN(first_error, line_id);

                 // extract source code line ... (with error)
                 errors.append("\n");
                 print_line(errors, source, last_line +1, line_id);
                 errors.append("\n");
             }
         }
         // output the associated error ...
         for(i+= position; log[i] != 0; i++)
         {
             errors.push_back(log[i]);
             if(log[i] == '\n')
                 break;
         }

         last_string= string_id;
         last_line= line_id;
     }
     errors.append("\n");
     print_line(errors, source, last_line +1, 1000);
     errors.append("\n");

     return first_error;
}

int program_print_errors(std::string& errors, const GLMethods & gl, GLint shader_obj)
{
    // Get logs infos about compile errors
    GLint log_len = 0;
    gl.GetShaderiv(shader_obj, GL_INFO_LOG_LENGTH, &log_len);
    std::vector<char> log(log_len+1, 0);
    gl.GetShaderInfoLog(shader_obj, log_len, NULL, &log.front());

    // Retrieve shader source from device side (OpenGL)
    GLint source_len = 0;
    gl.GetShaderiv(shader_obj, GL_SHADER_SOURCE_LENGTH, &source_len);
    std::vector<char> source(source_len+1, 0);
    gl.GetShaderSource(shader_obj, source_len, NULL, &source.front());

    // Format errors
    int last_error = print_errors(errors, &log.front(), &source.front());

    return last_error;
}