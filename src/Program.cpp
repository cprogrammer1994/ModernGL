#include "Types.hpp"

#include "Program_Error.hpp"

#include "InlineMethods.hpp"


PyObject * MGLContext_program(MGLContext * self, PyObject * args) {
	PyObject * shaders[5];
	PyObject * outputs;

	int args_ok = PyArg_ParseTuple(
		args,
		"OOOOOO",
		&shaders[0],
		&shaders[1],
		&shaders[2],
		&shaders[3],
		&shaders[4],
		&outputs
	);

	if (!args_ok) {
		return 0;
	}

	int num_outputs = (int)PyTuple_GET_SIZE(outputs);

	for (int i = 0; i < num_outputs; ++i) {
		PyObject * item = PyTuple_GET_ITEM(outputs, i);
		if (Py_TYPE(item) != &PyUnicode_Type) {
			MGLError_Set("varyings[%d] must be a string not %s", i, Py_TYPE(item)->tp_name);
			return 0;
		}
	}

	MGLProgram * program = (MGLProgram *)MGLProgram_Type.tp_alloc(&MGLProgram_Type, 0);

	Py_INCREF(self);
	program->context = self;

	const GLMethods & gl = program->context->gl;

	int program_obj = gl.CreateProgram();

	if (!program_obj) {
		MGLError_Set("cannot create program");
		return 0;
	}

	for (int i = 0; i < NUM_SHADER_SLOTS; ++i) {
		if (shaders[i] == Py_None) {
			continue;
		}

		const char * source_str = PyUnicode_AsUTF8(shaders[i]);

		int shader_obj = gl.CreateShader(SHADER_TYPE[i]);

		if (!shader_obj) {
			MGLError_Set("cannot create shader");
			return 0;
		}

		gl.ShaderSource(shader_obj, 1, &source_str, 0);
		gl.CompileShader(shader_obj);

		int compiled = GL_FALSE;
		gl.GetShaderiv(shader_obj, GL_COMPILE_STATUS, &compiled);

		if (!compiled) {
			const char * SHADER_NAME[] = {
				"vertex_shader",
				"fragment_shader",
				"geometry_shader",
				"tess_control_shader",
				"tess_evaluation_shader",
			};

			const char * SHADER_NAME_UNDERLINE[] = {
				"=============",
				"===============",
				"===============",
				"===================",
				"======================",
			};

			const char * message = "GLSL Compiler failed";
			const char * title = SHADER_NAME[i];
			const char * underline = SHADER_NAME_UNDERLINE[i];

            // Format errors
            std::string errors;
            program_print_errors(errors, gl, shader_obj);

            if (errors.size() > 0) {
                // send formatted shader informations logs
                MGLError_Set("%s\n\n%s\n%s\n%s\n", message, title, underline, errors.c_str());
            }
            else {
                // old path
                const char* log_parser_error = "Can't format shader informations logs (build errors)!";
                // get raw shader info log
                int log_len = 0;
                gl.GetShaderiv(shader_obj, GL_INFO_LOG_LENGTH, &log_len);
                char * log = new char[log_len];
                gl.GetShaderInfoLog(shader_obj, log_len, &log_len, log);
                // send raw shader informations logs
                MGLError_Set("%s\n\n%s\n%s\n%s\n%s\n", message, title, underline, log_parser_error, log);
            }

			gl.DeleteShader(shader_obj);

			return 0;
		}

		gl.AttachShader(program_obj, shader_obj);
	}

	if (num_outputs) {
		const char ** varyings_array = new const char * [num_outputs];

		for (int i = 0; i < num_outputs; ++i) {
			varyings_array[i] = PyUnicode_AsUTF8(PyTuple_GET_ITEM(outputs, i));
		}

		gl.TransformFeedbackVaryings(program_obj, num_outputs, varyings_array, GL_INTERLEAVED_ATTRIBS);

		delete[] varyings_array;
	}

	gl.LinkProgram(program_obj);

	int linked = GL_FALSE;
	gl.GetProgramiv(program_obj, GL_LINK_STATUS, &linked);

	if (!linked) {
		const char * message = "GLSL Linker failed";
		const char * title = "Program";
		const char * underline = "=======";

		int log_len = 0;
		gl.GetProgramiv(program_obj, GL_INFO_LOG_LENGTH, &log_len);

		char * log = new char[log_len];
		gl.GetProgramInfoLog(program_obj, log_len, &log_len, log);

		gl.DeleteProgram(program_obj);

		MGLError_Set("%s\n\n%s\n%s\n%s\n", message, title, underline, log);

		delete[] log;
		return 0;
	}

	program->program_obj = program_obj;

	// int num_vertex_shader_subroutine_locations = 0;
	// int num_fragment_shader_subroutine_locations = 0;
	// int num_geometry_shader_subroutine_locations = 0;
	// int num_tess_evaluation_shader_subroutine_locations = 0;
	// int num_tess_control_shader_subroutine_locations = 0;

	int num_vertex_shader_subroutines = 0;
	int num_fragment_shader_subroutines = 0;
	int num_geometry_shader_subroutines = 0;
	int num_tess_evaluation_shader_subroutines = 0;
	int num_tess_control_shader_subroutines = 0;

	int num_vertex_shader_subroutine_uniforms = 0;
	int num_fragment_shader_subroutine_uniforms = 0;
	int num_geometry_shader_subroutine_uniforms = 0;
	int num_tess_evaluation_shader_subroutine_uniforms = 0;
	int num_tess_control_shader_subroutine_uniforms = 0;

	if (program->context->version_code >= 400) {
		if (shaders[VERTEX_SHADER_SLOT] != Py_None) {
			// gl.GetProgramStageiv(
			// 	program_obj,
			// 	GL_VERTEX_SHADER,
			// 	GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
			// 	&num_vertex_shader_subroutine_locations
			// );
			gl.GetProgramStageiv(
				program_obj,
				GL_VERTEX_SHADER,
				GL_ACTIVE_SUBROUTINES,
				&num_vertex_shader_subroutines
			);
			gl.GetProgramStageiv(
				program_obj,
				GL_VERTEX_SHADER,
				GL_ACTIVE_SUBROUTINE_UNIFORMS,
				&num_vertex_shader_subroutine_uniforms
			);
		}

		if (shaders[FRAGMENT_SHADER_SLOT] != Py_None) {
			// gl.GetProgramStageiv(
			// 	program_obj,
			// 	GL_FRAGMENT_SHADER,
			// 	GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
			// 	&num_fragment_shader_subroutine_locations
			// );
			gl.GetProgramStageiv(
				program_obj,
				GL_FRAGMENT_SHADER,
				GL_ACTIVE_SUBROUTINES,
				&num_fragment_shader_subroutines
			);
			gl.GetProgramStageiv(
				program_obj,
				GL_FRAGMENT_SHADER,
				GL_ACTIVE_SUBROUTINE_UNIFORMS,
				&num_fragment_shader_subroutine_uniforms
			);
		}

		if (shaders[GEOMETRY_SHADER_SLOT] != Py_None) {
			// gl.GetProgramStageiv(
			// 	program_obj,
			// 	GL_GEOMETRY_SHADER,
			// 	GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
			// 	&num_geometry_shader_subroutine_locations
			// );
			gl.GetProgramStageiv(
				program_obj,
				GL_GEOMETRY_SHADER,
				GL_ACTIVE_SUBROUTINES,
				&num_geometry_shader_subroutines
			);
			gl.GetProgramStageiv(
				program_obj,
				GL_GEOMETRY_SHADER,
				GL_ACTIVE_SUBROUTINE_UNIFORMS,
				&num_geometry_shader_subroutine_uniforms
			);
		}

		if (shaders[TESS_EVALUATION_SHADER_SLOT] != Py_None) {
			// gl.GetProgramStageiv(
			// 	program_obj,
			// 	GL_TESS_EVALUATION_SHADER,
			// 	GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
			// 	&num_tess_evaluation_shader_subroutine_locations
			// );
			gl.GetProgramStageiv(
				program_obj,
				GL_TESS_EVALUATION_SHADER,
				GL_ACTIVE_SUBROUTINES,
				&num_tess_evaluation_shader_subroutines
			);
			gl.GetProgramStageiv(
				program_obj,
				GL_TESS_EVALUATION_SHADER,
				GL_ACTIVE_SUBROUTINE_UNIFORMS,
				&num_tess_evaluation_shader_subroutine_uniforms
			);
		}

		if (shaders[TESS_CONTROL_SHADER_SLOT] != Py_None) {
			// gl.GetProgramStageiv(
			// 	program_obj,
			// 	GL_TESS_CONTROL_SHADER,
			// 	GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
			// 	&num_tess_control_shader_subroutine_locations
			// );
			gl.GetProgramStageiv(
				program_obj,
				GL_TESS_CONTROL_SHADER,
				GL_ACTIVE_SUBROUTINES,
				&num_tess_control_shader_subroutines
			);
			gl.GetProgramStageiv(
				program_obj,
				GL_TESS_CONTROL_SHADER,
				GL_ACTIVE_SUBROUTINE_UNIFORMS,
				&num_tess_control_shader_subroutine_uniforms
			);
		}
	}

	if (shaders[GEOMETRY_SHADER_SLOT] != Py_None) {

		int geometry_in = 0;
		int geometry_out = 0;
		program->geometry_vertices = 0;

		gl.GetProgramiv(program_obj, GL_GEOMETRY_INPUT_TYPE, &geometry_in);
		gl.GetProgramiv(program_obj, GL_GEOMETRY_OUTPUT_TYPE, &geometry_out);
		gl.GetProgramiv(program_obj, GL_GEOMETRY_VERTICES_OUT, &program->geometry_vertices);

		switch (geometry_in) {
			case GL_TRIANGLES:
				program->geometry_input = GL_TRIANGLES;
				break;

			case GL_TRIANGLE_STRIP:
				program->geometry_input = GL_TRIANGLE_STRIP;
				break;

			case GL_TRIANGLE_FAN:
				program->geometry_input = GL_TRIANGLE_FAN;
				break;

			case GL_LINES:
				program->geometry_input = GL_LINES;
				break;

			case GL_LINE_STRIP:
				program->geometry_input = GL_LINE_STRIP;
				break;

			case GL_LINE_LOOP:
				program->geometry_input = GL_LINE_LOOP;
				break;

			case GL_POINTS:
				program->geometry_input = GL_POINTS;
				break;

			case GL_LINE_STRIP_ADJACENCY:
				program->geometry_input = GL_LINE_STRIP_ADJACENCY;
				break;

			case GL_LINES_ADJACENCY:
				program->geometry_input = GL_LINES_ADJACENCY;
				break;

			case GL_TRIANGLE_STRIP_ADJACENCY:
				program->geometry_input = GL_TRIANGLE_STRIP_ADJACENCY;
				break;

			case GL_TRIANGLES_ADJACENCY:
				program->geometry_input = GL_TRIANGLES_ADJACENCY;
				break;

			default:
				program->geometry_input = -1;
				break;
		}

		switch (geometry_out) {
			case GL_TRIANGLES:
				program->geometry_output = GL_TRIANGLES;
				break;

			case GL_TRIANGLE_STRIP:
				program->geometry_output = GL_TRIANGLE_STRIP;
				break;

			case GL_TRIANGLE_FAN:
				program->geometry_output = GL_TRIANGLE_FAN;
				break;

			case GL_LINES:
				program->geometry_output = GL_LINES;
				break;

			case GL_LINE_STRIP:
				program->geometry_output = GL_LINE_STRIP;
				break;

			case GL_LINE_LOOP:
				program->geometry_output = GL_LINE_LOOP;
				break;

			case GL_POINTS:
				program->geometry_output = GL_POINTS;
				break;

			case GL_LINE_STRIP_ADJACENCY:
				program->geometry_output = GL_LINE_STRIP_ADJACENCY;
				break;

			case GL_LINES_ADJACENCY:
				program->geometry_output = GL_LINES_ADJACENCY;
				break;

			case GL_TRIANGLE_STRIP_ADJACENCY:
				program->geometry_output = GL_TRIANGLE_STRIP_ADJACENCY;
				break;

			case GL_TRIANGLES_ADJACENCY:
				program->geometry_output = GL_TRIANGLES_ADJACENCY;
				break;

			default:
				program->geometry_output = -1;
				break;
		}

	} else {
		program->geometry_input = -1;
		program->geometry_output = -1;
		program->geometry_vertices = 0;
	}

	if (PyErr_Occurred()) {
		Py_DECREF(program);
		return 0;
	}

	Py_INCREF(program);

	int num_attributes = 0;
	int num_varyings = 0;
	int num_uniforms = 0;
	int num_uniform_blocks = 0;

	gl.GetProgramiv(program->program_obj, GL_ACTIVE_ATTRIBUTES, &num_attributes);
	gl.GetProgramiv(program->program_obj, GL_TRANSFORM_FEEDBACK_VARYINGS, &num_varyings);
	gl.GetProgramiv(program->program_obj, GL_ACTIVE_UNIFORMS, &num_uniforms);
	gl.GetProgramiv(program->program_obj, GL_ACTIVE_UNIFORM_BLOCKS, &num_uniform_blocks);

	int num_subroutines = num_vertex_shader_subroutines + num_fragment_shader_subroutines + num_geometry_shader_subroutines + num_tess_evaluation_shader_subroutines + num_tess_control_shader_subroutines;
	int num_subroutine_uniforms = num_vertex_shader_subroutine_uniforms + num_fragment_shader_subroutine_uniforms + num_geometry_shader_subroutine_uniforms + num_tess_evaluation_shader_subroutine_uniforms + num_tess_control_shader_subroutine_uniforms;

	program->num_vertex_shader_subroutines = num_vertex_shader_subroutine_uniforms;
	program->num_fragment_shader_subroutines = num_fragment_shader_subroutine_uniforms;
	program->num_geometry_shader_subroutines = num_geometry_shader_subroutine_uniforms;
	program->num_tess_evaluation_shader_subroutines = num_tess_evaluation_shader_subroutine_uniforms;
	program->num_tess_control_shader_subroutines = num_tess_control_shader_subroutine_uniforms;

	program->num_varyings = num_varyings;

	PyObject * attributes_lst = PyTuple_New(num_attributes);
	PyObject * varyings_lst = PyTuple_New(num_varyings);
	PyObject * uniforms_lst = PyTuple_New(num_uniforms);
	PyObject * uniform_blocks_lst = PyTuple_New(num_uniform_blocks);
	PyObject * subroutines_lst = PyTuple_New(num_subroutines);
	PyObject * subroutine_uniforms_lst = PyTuple_New(num_subroutine_uniforms);

	for (int i = 0; i < num_attributes; ++i) {
		int type = 0;
		int array_length = 0;
		int name_len = 0;
		char name[256];

		gl.GetActiveAttrib(program->program_obj, i, 256, &name_len, &array_length, (GLenum *)&type, name);
		int location = gl.GetAttribLocation(program->program_obj, name);

		clean_glsl_name(name, name_len);

		MGLAttribute * mglo = (MGLAttribute *)MGLAttribute_Type.tp_alloc(&MGLAttribute_Type, 0);
		mglo->type = type;
		mglo->location = location;
		mglo->array_length = array_length;
		mglo->program_obj = program->program_obj;
		MGLAttribute_Complete(mglo, gl);

		PyObject * item = PyTuple_New(6);
		PyTuple_SET_ITEM(item, 0, (PyObject *)mglo);
		PyTuple_SET_ITEM(item, 1, PyLong_FromLong(location));
		PyTuple_SET_ITEM(item, 2, PyLong_FromLong(array_length));
		PyTuple_SET_ITEM(item, 3, PyLong_FromLong(mglo->dimension));
		PyTuple_SET_ITEM(item, 4, PyUnicode_FromFormat("%c", mglo->shape));
		PyTuple_SET_ITEM(item, 5, PyUnicode_FromStringAndSize(name, name_len));

		PyTuple_SET_ITEM(attributes_lst, i, item);
	}

	for (int i = 0; i < num_varyings; ++i) {
		int type = 0;
		int array_length = 0;
		int dimension = 0;
		int name_len = 0;
		char name[256];

		gl.GetTransformFeedbackVarying(program->program_obj, i, 256, &name_len, &array_length, (GLenum *)&type, name);

		PyObject * item = PyTuple_New(4);
		PyTuple_SET_ITEM(item, 0, PyLong_FromLong(i));
		PyTuple_SET_ITEM(item, 1, PyLong_FromLong(array_length));
		PyTuple_SET_ITEM(item, 2, PyLong_FromLong(dimension));
		PyTuple_SET_ITEM(item, 3, PyUnicode_FromStringAndSize(name, name_len));

		PyTuple_SET_ITEM(varyings_lst, i, item);
	}

	int uniform_counter = 0;
	for (int i = 0; i < num_uniforms; ++i) {
		int type = 0;
		int array_length = 0;
		int name_len = 0;
		char name[256];

		gl.GetActiveUniform(program->program_obj, i, 256, &name_len, &array_length, (GLenum *)&type, name);
		int location = gl.GetUniformLocation(program->program_obj, name);

		clean_glsl_name(name, name_len);

		if (location < 0) {
			continue;
		}

		MGLUniform * mglo = (MGLUniform *)MGLUniform_Type.tp_alloc(&MGLUniform_Type, 0);
		mglo->type = type;
		mglo->location = location;
		mglo->array_length = array_length;
		mglo->program_obj = program->program_obj;
		MGLUniform_Complete(mglo, gl);

		PyObject * item = PyTuple_New(5);
		PyTuple_SET_ITEM(item, 0, (PyObject *)mglo);
		PyTuple_SET_ITEM(item, 1, PyLong_FromLong(location));
		PyTuple_SET_ITEM(item, 2, PyLong_FromLong(array_length));
		PyTuple_SET_ITEM(item, 3, PyLong_FromLong(mglo->dimension));
		PyTuple_SET_ITEM(item, 4, PyUnicode_FromStringAndSize(name, name_len));

		PyTuple_SET_ITEM(uniforms_lst, uniform_counter, item);
		++uniform_counter;
	}

	if (uniform_counter != num_uniforms) {
		_PyTuple_Resize(&uniforms_lst, uniform_counter);
	}

	for (int i = 0; i < num_uniform_blocks; ++i) {
		int size = 0;
		int name_len = 0;
		char name[256];

		gl.GetActiveUniformBlockName(program->program_obj, i, 256, &name_len, name);
		int index = gl.GetUniformBlockIndex(program->program_obj, name);
		gl.GetActiveUniformBlockiv(program->program_obj, index, GL_UNIFORM_BLOCK_DATA_SIZE, &size);

		clean_glsl_name(name, name_len);

		MGLUniformBlock * mglo = (MGLUniformBlock *)MGLUniformBlock_Type.tp_alloc(&MGLUniformBlock_Type, 0);

		mglo->index = index;
		mglo->size = size;
		mglo->program_obj = program->program_obj;
		mglo->gl = &gl;

		PyObject * item = PyTuple_New(4);
		PyTuple_SET_ITEM(item, 0, (PyObject *)mglo);
		PyTuple_SET_ITEM(item, 1, PyLong_FromLong(index));
		PyTuple_SET_ITEM(item, 2, PyLong_FromLong(size));
		PyTuple_SET_ITEM(item, 3, PyUnicode_FromStringAndSize(name, name_len));

		PyTuple_SET_ITEM(uniform_blocks_lst, i, item);
	}

	int subroutine_uniforms_base = 0;
	int subroutines_base = 0;

	if (program->context->version_code >= 400) {
		const int shader_type[5] = {
			GL_VERTEX_SHADER,
			GL_FRAGMENT_SHADER,
			GL_GEOMETRY_SHADER,
			GL_TESS_EVALUATION_SHADER,
			GL_TESS_CONTROL_SHADER,
		};

		for (int st = 0; st < 5; ++st) {
			int num_subroutines = 0;
			gl.GetProgramStageiv(program_obj, shader_type[st], GL_ACTIVE_SUBROUTINES, &num_subroutines);

			int num_subroutine_uniforms = 0;
			gl.GetProgramStageiv(program_obj, shader_type[st], GL_ACTIVE_SUBROUTINE_UNIFORMS, &num_subroutine_uniforms);

			for (int i = 0; i < num_subroutines; ++i) {
				int name_len = 0;
				char name[256];

				gl.GetActiveSubroutineName(program_obj, shader_type[st], i, 256, &name_len, name);
				int index = gl.GetSubroutineIndex(program_obj, shader_type[st], name);

				PyObject * item = PyTuple_New(2);
				PyTuple_SET_ITEM(item, 0, PyLong_FromLong(index));
				PyTuple_SET_ITEM(item, 1, PyUnicode_FromStringAndSize(name, name_len));
				PyTuple_SET_ITEM(subroutines_lst, subroutines_base + i, item);
			}

			for (int i = 0; i < num_subroutine_uniforms; ++i) {
				int name_len = 0;
				char name[256];

				gl.GetActiveSubroutineUniformName(program_obj, shader_type[st], i, 256, &name_len, name);
				int location = subroutine_uniforms_base + gl.GetSubroutineUniformLocation(program_obj, shader_type[st], name);
				PyTuple_SET_ITEM(subroutine_uniforms_lst, location, PyUnicode_FromStringAndSize(name, name_len));
			}

			subroutine_uniforms_base += num_subroutine_uniforms;
			subroutines_base += num_subroutines;
		}
	}

	PyObject * geom_info = PyTuple_New(3);
	if (program->geometry_input != -1) {
		PyTuple_SET_ITEM(geom_info, 0, PyLong_FromLong(program->geometry_input));
	} else {
		Py_INCREF(Py_None);
		PyTuple_SET_ITEM(geom_info, 0, Py_None);
	}
	if (program->geometry_output != -1) {
		PyTuple_SET_ITEM(geom_info, 1, PyLong_FromLong(program->geometry_output));
	} else {
		Py_INCREF(Py_None);
		PyTuple_SET_ITEM(geom_info, 1, Py_None);
	}
	PyTuple_SET_ITEM(geom_info, 2, PyLong_FromLong(program->geometry_vertices));

	PyObject * result = PyTuple_New(9);
	PyTuple_SET_ITEM(result, 0, (PyObject *)program);
	PyTuple_SET_ITEM(result, 1, attributes_lst);
	PyTuple_SET_ITEM(result, 2, varyings_lst);
	PyTuple_SET_ITEM(result, 3, uniforms_lst);
	PyTuple_SET_ITEM(result, 4, uniform_blocks_lst);
	PyTuple_SET_ITEM(result, 5, subroutines_lst);
	PyTuple_SET_ITEM(result, 6, subroutine_uniforms_lst);
	PyTuple_SET_ITEM(result, 7, geom_info);
	PyTuple_SET_ITEM(result, 8, PyLong_FromLong(program->program_obj));
	return result;
}

PyObject * MGLProgram_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLProgram * self = (MGLProgram *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLProgram_tp_dealloc(MGLProgram * self) {
	MGLProgram_Type.tp_free((PyObject *)self);
}

PyObject * MGLProgram_release(MGLProgram * self) {
	MGLProgram_Invalidate(self);
	Py_RETURN_NONE;
}

PyMethodDef MGLProgram_tp_methods[] = {
	{"release", (PyCFunction)MGLProgram_release, METH_NOARGS, 0},
	{0},
};

PyTypeObject MGLProgram_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.Program",                                          // tp_name
	sizeof(MGLProgram),                                     // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLProgram_tp_dealloc,                      // tp_dealloc
	0,                                                      // tp_print
	0,                                                      // tp_getattr
	0,                                                      // tp_setattr
	0,                                                      // tp_reserved
	0,                                                      // tp_repr
	0,                                                      // tp_as_number
	0,                                                      // tp_as_sequence
	0,                                                      // tp_as_mapping
	0,                                                      // tp_hash
	0,                                                      // tp_call
	0,                                                      // tp_str
	0,                                                      // tp_getattro
	0,                                                      // tp_setattro
	0,                                                      // tp_as_buffer
	Py_TPFLAGS_DEFAULT,                                     // tp_flags
	0,                                                      // tp_doc
	0,                                                      // tp_traverse
	0,                                                      // tp_clear
	0,                                                      // tp_richcompare
	0,                                                      // tp_weaklistoffset
	0,                                                      // tp_iter
	0,                                                      // tp_iternext
	MGLProgram_tp_methods,                                  // tp_methods
	0,                                                      // tp_members
	0,                                                      // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	0,                                                      // tp_init
	0,                                                      // tp_alloc
	MGLProgram_tp_new,                                      // tp_new
};

void MGLProgram_Invalidate(MGLProgram * program) {
	if (Py_TYPE(program) == &MGLInvalidObject_Type) {
		return;
	}

	// TODO: decref

	const GLMethods & gl = program->context->gl;
	gl.DeleteProgram(program->program_obj);

	Py_TYPE(program) = &MGLInvalidObject_Type;
	Py_DECREF(program);
}
