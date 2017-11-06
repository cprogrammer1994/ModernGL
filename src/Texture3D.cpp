#include "Texture3D.hpp"

#include "Error.hpp"
#include "InvalidObject.hpp"
#include "Buffer.hpp"

#include "InlineMethods.hpp"

PyObject * MGLTexture3D_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
	MGLTexture3D * self = (MGLTexture3D *)type->tp_alloc(type, 0);

	if (self) {
	}

	return (PyObject *)self;
}

void MGLTexture3D_tp_dealloc(MGLTexture3D * self) {
	MGLTexture3D_Type.tp_free((PyObject *)self);
}

int MGLTexture3D_tp_init(MGLTexture3D * self, PyObject * args, PyObject * kwargs) {
	MGLError_Set("cannot create mgl.Texture3D manually");
	return -1;
}

PyObject * MGLTexture3D_read(MGLTexture3D * self, PyObject * args) {
	int alignment;

	int args_ok = PyArg_ParseTuple(
		args,
		"I",
		&alignment
	);

	if (!args_ok) {
		return 0;
	}

	if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
		MGLError_Set("the alignment must be 1, 2, 4 or 8");
		return 0;
	}

	if (self->samples) {
		MGLError_Set("multisample textures cannot be read directly");
		return 0;
	}

	int expected_size = self->width * self->components * (self->floats ?  4 : 1);
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * self->height * self->depth;

	PyObject * result = PyBytes_FromStringAndSize(0, expected_size);
	char * data = PyBytes_AS_STRING(result);

	const int formats[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};

	int pixel_type = self->floats ? GL_FLOAT : GL_UNSIGNED_BYTE;
	int format = formats[self->components];

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

	gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
	gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
	gl.GetTexImage(GL_TEXTURE_3D, 0, format, pixel_type, data);

	return result;
}

PyObject * MGLTexture3D_read_into(MGLTexture3D * self, PyObject * args) {
	PyObject * data;
	int alignment;
	Py_ssize_t write_offset;

	int args_ok = PyArg_ParseTuple(
		args,
		"OIn",
		&data,
		&alignment,
		&write_offset
	);

	if (!args_ok) {
		return 0;
	}

	if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
		MGLError_Set("the alignment must be 1, 2, 4 or 8");
		return 0;
	}

	if (self->samples) {
		MGLError_Set("multisample textures cannot be read directly");
		return 0;
	}

	int expected_size = self->width * self->components * (self->floats ? 4 : 1);
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * self->height * self->depth;

	const int formats[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};

	int pixel_type = self->floats ? GL_FLOAT : GL_UNSIGNED_BYTE;
	int format = formats[self->components];

	if (Py_TYPE(data) == &MGLBuffer_Type) {

		MGLBuffer * buffer = (MGLBuffer *)data;

		const GLMethods & gl = self->context->gl;

		gl.BindBuffer(GL_PIXEL_PACK_BUFFER, buffer->buffer_obj);
		gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
		gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.GetTexImage(GL_TEXTURE_3D, 0, format, pixel_type, (void *)write_offset);
		gl.BindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	} else {

		Py_buffer buffer_view;

		int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_WRITABLE);
		if (get_buffer < 0) {
			MGLError_Set("the buffer (%s) does not support buffer interface", Py_TYPE(data)->tp_name);
			return 0;
		}

		if (buffer_view.len < write_offset + expected_size) {
			MGLError_Set("the buffer is too small");
			PyBuffer_Release(&buffer_view);
			return 0;
		}

		char * ptr = (char *)buffer_view.buf + write_offset;

		const GLMethods & gl = self->context->gl;
		gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
		gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.GetTexImage(GL_TEXTURE_3D, 0, format, pixel_type, ptr);

		PyBuffer_Release(&buffer_view);

	}

	Py_RETURN_NONE;
}

PyObject * MGLTexture3D_write(MGLTexture3D * self, PyObject * args) {

	// TODO: test this method

	PyObject * data;
	PyObject * viewport;
	int alignment;

	int args_ok = PyArg_ParseTuple(
		args,
		"OOI",
		&data,
		&viewport,
		&alignment
	);

	if (!args_ok) {
		return 0;
	}

	if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8) {
		MGLError_Set("the alignment must be 1, 2, 4 or 8");
		return 0;
	}

	if (self->samples) {
		MGLError_Set("multisample textures cannot be written directly");
		return 0;
	}

	int x = 0;
	int y = 0;
	int z = 0;
	int width = self->width;
	int height = self->height;
	int depth = self->depth;

	Py_buffer buffer_view;

	if (viewport != Py_None) {
		if (Py_TYPE(viewport) != &PyTuple_Type) {
			MGLError_Set("the viewport must be a tuple not %s", Py_TYPE(viewport)->tp_name);
			return 0;
		}

		if (PyTuple_GET_SIZE(viewport) == 6) {

			x = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 0));
			y = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 1));
			z = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 2));
			width = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 3));
			height = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 4));
			depth = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 5));

		} else if (PyTuple_GET_SIZE(viewport) == 3) {

			width = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 0));
			height = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 1));
			depth = PyLong_AsLong(PyTuple_GET_ITEM(viewport, 2));

		} else {

			MGLError_Set("the viewport size %d is invalid", PyTuple_GET_SIZE(viewport));
			return 0;

		}

		if (PyErr_Occurred()) {
			MGLError_Set("wrong values in the viewport");
			return 0;
		}

	}

	int expected_size = width * self->components * (self->floats ? 4 : 1);
	expected_size = (expected_size + alignment - 1) / alignment * alignment;
	expected_size = expected_size * height * depth;

	const int formats[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};

	int pixel_type = self->floats ? GL_FLOAT : GL_UNSIGNED_BYTE;
	int format = formats[self->components];

	if (Py_TYPE(data) == &MGLBuffer_Type) {

		MGLBuffer * buffer = (MGLBuffer *)data;

		const GLMethods & gl = self->context->gl;

		gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer->buffer_obj);
		gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
		gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);
		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.TexSubImage3D(GL_TEXTURE_3D, 0, x, y, z, width, height, depth, format, pixel_type, 0);
		gl.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	} else {

		int get_buffer = PyObject_GetBuffer(data, &buffer_view, PyBUF_SIMPLE);
		if (get_buffer < 0) {
			MGLError_Set("data (%s) does not support buffer interface", Py_TYPE(data)->tp_name);
			return 0;
		}

		if (buffer_view.len != expected_size) {
			MGLError_Set("data size mismatch %d != %d", buffer_view.len, expected_size);
			if (data != Py_None) {
				PyBuffer_Release(&buffer_view);
			}
			return 0;
		}

		const GLMethods & gl = self->context->gl;

		gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
		gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

		gl.PixelStorei(GL_PACK_ALIGNMENT, alignment);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl.TexSubImage3D(GL_TEXTURE_3D, 0, x, y, z, width, height, depth, format, pixel_type, buffer_view.buf);

		PyBuffer_Release(&buffer_view);

	}

	Py_RETURN_NONE;
}

PyObject * MGLTexture3D_clear(MGLTexture3D * self, PyObject * args) {

	// TODO:

	PyErr_Format(PyExc_NotImplementedError, "NYI");
	return 0;
}

PyObject * MGLTexture3D_use(MGLTexture3D * self, PyObject * args) {

	// TODO: test this method

	int index;

	int args_ok = PyArg_ParseTuple(
		args,
		"I",
		&index
	);

	if (!args_ok) {
		return 0;
	}

	const GLMethods & gl = self->context->gl;
	gl.ActiveTexture(GL_TEXTURE0 + index);
	gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

	Py_RETURN_NONE;
}

PyObject * MGLTexture3D_build_mipmaps(MGLTexture3D * self, PyObject * args) {

	// TODO: test this method

	int base = 0;
	int max = 1000;

	int args_ok = PyArg_ParseTuple(
		args,
		"II",
		&base,
		&max
	);

	if (!args_ok) {
		return 0;
	}


	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

	gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, base);
	gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, max);

	gl.GenerateMipmap(GL_TEXTURE_3D);

	gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// TODO: filter attrib

	Py_RETURN_NONE;
}

PyObject * MGLTexture3D_release(MGLTexture3D * self) {
	MGLTexture3D_Invalidate(self);
	Py_RETURN_NONE;
}

PyMethodDef MGLTexture3D_tp_methods[] = {
	{"read", (PyCFunction)MGLTexture3D_read, METH_VARARGS, 0},
	{"read_into", (PyCFunction)MGLTexture3D_read_into, METH_VARARGS, 0},
	{"write", (PyCFunction)MGLTexture3D_write, METH_VARARGS, 0},
	{"clear", (PyCFunction)MGLTexture3D_clear, METH_VARARGS, 0},
	{"use", (PyCFunction)MGLTexture3D_use, METH_VARARGS, 0},
	{"build_mipmaps", (PyCFunction)MGLTexture3D_build_mipmaps, METH_VARARGS, 0},
	{"release", (PyCFunction)MGLTexture3D_release, METH_NOARGS, 0},
	{0},
};

PyObject * MGLTexture3D_get_repeat_x(MGLTexture3D * self) {
	return PyBool_FromLong(self->repeat_x);
}

int MGLTexture3D_set_repeat_x(MGLTexture3D * self, PyObject * value) {

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

	if (value == Py_True) {
		gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		self->repeat_x = true;
		return 0;
	} else if (value == Py_False) {
		gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		self->repeat_x = false;
		return 0;
	} else {
		MGLError_Set("invalid value for texture_x");
		return -1;
	}
}

PyObject * MGLTexture3D_get_repeat_y(MGLTexture3D * self) {
	return PyBool_FromLong(self->repeat_y);
}

int MGLTexture3D_set_repeat_y(MGLTexture3D * self, PyObject * value) {

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

	if (value == Py_True) {
		gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		self->repeat_y = true;
		return 0;
	} else if (value == Py_False) {
		gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		self->repeat_y = false;
		return 0;
	} else {
		MGLError_Set("invalid value for texture_y");
		return -1;
	}
}

PyObject * MGLTexture3D_get_repeat_z(MGLTexture3D * self) {
	return PyBool_FromLong(self->repeat_z);
}

int MGLTexture3D_set_repeat_z(MGLTexture3D * self, PyObject * value) {

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

	if (value == Py_True) {
		gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		self->repeat_z = true;
		return 0;
	} else if (value == Py_False) {
		gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		self->repeat_z = false;
		return 0;
	} else {
		MGLError_Set("invalid value for texture_z");
		return -1;
	}
}

PyObject * MGLTexture3D_get_filter(MGLTexture3D * self) {
	Py_INCREF(self->filter->wrapper);
	return self->filter->wrapper;
}

int MGLTexture3D_set_filter(MGLTexture3D * self, PyObject * value) {
	if (Py_TYPE(value) != &MGLTextureFilter_Type) {
		MGLError_Set("the value must be a TextureFilter not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	MGLTextureFilter * filter = (MGLTextureFilter *)value;

	Py_INCREF(filter);
	Py_DECREF(self->filter);
	self->filter = filter;

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);
	gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter->min_filter);
	gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filter->mag_filter);

	return 0;
}

PyObject * MGLTexture3D_get_swizzle(MGLTexture3D * self, void * closure) {

	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

	int swizzle_r = 0;
	int swizzle_g = 0;
	int swizzle_b = 0;
	int swizzle_a = 0;

	gl.GetTexParameteriv(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_R, &swizzle_r);
	gl.GetTexParameteriv(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_G, &swizzle_g);
	gl.GetTexParameteriv(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_B, &swizzle_b);
	gl.GetTexParameteriv(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_A, &swizzle_a);

	char swizzle[5] = {
		char_from_swizzle(swizzle_r),
		char_from_swizzle(swizzle_g),
		char_from_swizzle(swizzle_b),
		char_from_swizzle(swizzle_a),
		0,
	};

	return PyUnicode_FromStringAndSize(swizzle, 4);
}

int MGLTexture3D_set_swizzle(MGLTexture3D * self, PyObject * value, void * closure) {
	const char * swizzle = PyUnicode_AsUTF8(value);

	if (!swizzle[0]) {
		MGLError_Set("the swizzle is empty");
		return -1;
	}

	int tex_swizzle[4] = {-1, -1, -1, -1};

	for (int i = 0; swizzle[i]; ++i) {
		if (i > 3) {
			MGLError_Set("the swizzle is too long");
			return -1;
		}

		tex_swizzle[i] = swizzle_from_char(swizzle[i]);

		if (tex_swizzle[i] == -1) {
			MGLError_Set("'%c' is not a valid swizzle parameter", swizzle[i]);
			return -1;
		}
	}


	const GLMethods & gl = self->context->gl;

	gl.ActiveTexture(GL_TEXTURE0 + self->context->default_texture_unit);
	gl.BindTexture(GL_TEXTURE_3D, self->texture_obj);

	gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_R, tex_swizzle[0]);
	if (tex_swizzle[1] != -1) {
		gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_G, tex_swizzle[1]);
		if (tex_swizzle[2] != -1) {
			gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_B, tex_swizzle[2]);
			if (tex_swizzle[3] != -1) {
				gl.TexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_A, tex_swizzle[3]);
			}
		}
	}

	return 0;
}

PyObject * MGLTexture3D_get_width(MGLTexture3D * self, void * closure) {
	return PyLong_FromLong(self->width);
}

PyObject * MGLTexture3D_get_height(MGLTexture3D * self, void * closure) {
	return PyLong_FromLong(self->height);
}

PyObject * MGLTexture3D_get_depth(MGLTexture3D * self, void * closure) {
	return PyLong_FromLong(self->depth);
}

PyObject * MGLTexture3D_get_components(MGLTexture3D * self, void * closure) {
	return PyLong_FromLong(self->components);
}

PyObject * MGLTexture3D_get_floats(MGLTexture3D * self, void * closure) {
	return PyBool_FromLong(self->floats);
}

MGLContext * MGLTexture3D_get_context(MGLTexture3D * self, void * closure) {
	Py_INCREF(self->context);
	return self->context;
}

PyObject * MGLTexture3D_get_glo(MGLTexture3D * self, void * closure) {
	return PyLong_FromLong(self->texture_obj);
}

PyGetSetDef MGLTexture3D_tp_getseters[] = {
	{(char *)"repeat_x", (getter)MGLTexture3D_get_repeat_x, (setter)MGLTexture3D_set_repeat_x, 0, 0},
	{(char *)"repeat_y", (getter)MGLTexture3D_get_repeat_y, (setter)MGLTexture3D_set_repeat_y, 0, 0},
	{(char *)"repeat_z", (getter)MGLTexture3D_get_repeat_z, (setter)MGLTexture3D_set_repeat_z, 0, 0},
	{(char *)"filter", (getter)MGLTexture3D_get_filter, (setter)MGLTexture3D_set_filter, 0, 0},
	{(char *)"swizzle", (getter)MGLTexture3D_get_swizzle, (setter)MGLTexture3D_set_swizzle, 0, 0},

	{(char *)"width", (getter)MGLTexture3D_get_width, 0, 0, 0},
	{(char *)"height", (getter)MGLTexture3D_get_height, 0, 0, 0},
	{(char *)"depth", (getter)MGLTexture3D_get_depth, 0, 0, 0},
	{(char *)"components", (getter)MGLTexture3D_get_components, 0, 0, 0},
	{(char *)"floats", (getter)MGLTexture3D_get_floats, 0, 0, 0},
	{(char *)"context", (getter)MGLTexture3D_get_context, 0, 0, 0},
	{(char *)"glo", (getter)MGLTexture3D_get_glo, 0, 0, 0},
	{0},
};

PyTypeObject MGLTexture3D_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"mgl.Texture3D",                                        // tp_name
	sizeof(MGLTexture3D),                                   // tp_basicsize
	0,                                                      // tp_itemsize
	(destructor)MGLTexture3D_tp_dealloc,                    // tp_dealloc
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
	MGLTexture3D_tp_methods,                                // tp_methods
	0,                                                      // tp_members
	MGLTexture3D_tp_getseters,                              // tp_getset
	0,                                                      // tp_base
	0,                                                      // tp_dict
	0,                                                      // tp_descr_get
	0,                                                      // tp_descr_set
	0,                                                      // tp_dictoffset
	(initproc)MGLTexture3D_tp_init,                         // tp_init
	0,                                                      // tp_alloc
	MGLTexture3D_tp_new,                                    // tp_new
};

MGLTexture3D * MGLTexture3D_New() {
	MGLTexture3D * self = (MGLTexture3D *)MGLTexture3D_tp_new(&MGLTexture3D_Type, 0, 0);
	return self;
}

void MGLTexture3D_Invalidate(MGLTexture3D * texture) {
	if (Py_TYPE(texture) == &MGLInvalidObject_Type) {
		return;
	}

	texture->context->gl.DeleteTextures(1, (GLuint *)&texture->texture_obj);

	Py_DECREF(texture->context);
	Py_TYPE(texture) = &MGLInvalidObject_Type;
	Py_DECREF(texture);
}
