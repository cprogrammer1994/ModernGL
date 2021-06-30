#include "UniformGetSetters.hpp"

#include "Types.hpp"

int MGLUniform_invalid_setter(MGLUniform * self, PyObject * value) {
	MGLError_Set("cannot detect uniform type");
	return -1;
}

int MGLUniform_bool_value_setter(MGLUniform * self, PyObject * value) {
	int c_value;

	if (value == Py_True) {
		c_value = 1;
	} else if (value == Py_False) {
		c_value = 0;
	} else {
		MGLError_Set("the value must be a bool not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, 1, &c_value);

	return 0;
}

int MGLUniform_int_value_setter(MGLUniform * self, PyObject * value) {
	int c_value = PyLong_AsLong(value);

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to int");
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, 1, &c_value);

	return 0;
}

int MGLUniform_uint_value_setter(MGLUniform * self, PyObject * value) {
	unsigned c_value = PyLong_AsUnsignedLong(value);

	if (PyErr_Occurred()) {
		MGLError_Set("the value must be an unsigned int not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, 1, &c_value);

	return 0;
}

int MGLUniform_float_value_setter(MGLUniform * self, PyObject * value) {
	float c_value = (float)PyFloat_AsDouble(value);

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to float");
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, 1, &c_value);


	return 0;
}

int MGLUniform_double_value_setter(MGLUniform * self, PyObject * value) {
	double c_value = PyFloat_AsDouble(value);

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to double");
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, 1, &c_value);

	return 0;
}

int MGLUniform_sampler_value_setter(MGLUniform * self, PyObject * value) {
	int c_value = PyLong_AsLong(value);

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to int");
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, 1, &c_value);

	return 0;
}

int MGLUniform_bool_array_value_setter(MGLUniform * self, PyObject * value) {

	if (Py_TYPE(value) != &PyList_Type) {
		MGLError_Set("the value must be a list not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyList_GET_SIZE(value);

	if (size != self->array_length) {
		MGLError_Set("the value must be a list of size %d not %d", self->array_length, size);
		return -1;
	}

	int * c_values = new int[size];

	for (int k = 0; k < size; ++k) {
		PyObject * v = PyList_GET_ITEM(value, k);

		if (v == Py_True) {
			c_values[k] = 1;
		} else if (v == Py_False) {
			c_values[k] = 0;
		} else {
			MGLError_Set("value[%d] must be a bool not %s", k, Py_TYPE(value)->tp_name);
			delete[] c_values;
			return -1;
		}
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, size, c_values);

	delete[] c_values;
	return 0;
}

int MGLUniform_int_array_value_setter(MGLUniform * self, PyObject * value) {

	if (Py_TYPE(value) != &PyList_Type) {
		MGLError_Set("the value must be a list not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyList_GET_SIZE(value);

	if (size != self->array_length) {
		MGLError_Set("the value must be a list of size %d not %d", self->array_length, size);
		return -1;
	}

	int * c_values = new int[size];

	for (int k = 0; k < size; ++k) {
		c_values[k] = PyLong_AsLong(PyList_GET_ITEM(value, k));
	}

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to int");
		delete[] c_values;
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, size, c_values);

	delete[] c_values;
	return 0;
}

int MGLUniform_uint_array_value_setter(MGLUniform * self, PyObject * value) {

	if (Py_TYPE(value) != &PyList_Type) {
		MGLError_Set("the value must be a list not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyList_GET_SIZE(value);

	if (size != self->array_length) {
		MGLError_Set("the value must be a list of size %d not %d", self->array_length, size);
		return -1;
	}

	unsigned * c_values = new unsigned[size];

	for (int k = 0; k < size; ++k) {
		c_values[k] = PyLong_AsUnsignedLong(PyList_GET_ITEM(value, k));
	}

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to unsigned int");
		delete[] c_values;
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, size, c_values);

	delete[] c_values;
	return 0;
}

int MGLUniform_float_array_value_setter(MGLUniform * self, PyObject * value) {

	if (Py_TYPE(value) != &PyList_Type) {
		MGLError_Set("the value must be a list not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyList_GET_SIZE(value);

	if (size != self->array_length) {
		MGLError_Set("the value must be a list of size %d not %d", self->array_length, size);
		return -1;
	}

	float * c_values = new float[size];

	for (int k = 0; k < size; ++k) {
		c_values[k] = (float)PyFloat_AsDouble(PyList_GET_ITEM(value, k));
	}

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to float");
		delete[] c_values;
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, size, c_values);

	delete[] c_values;
	return 0;
}

int MGLUniform_double_array_value_setter(MGLUniform * self, PyObject * value) {

	if (Py_TYPE(value) != &PyList_Type) {
		MGLError_Set("the value must be a list not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyList_GET_SIZE(value);

	if (size != self->array_length) {
		MGLError_Set("the value must be a list of size %d not %d", self->array_length, size);
		return -1;
	}

	double * c_values = new double[size];

	for (int k = 0; k < size; ++k) {
		c_values[k] = PyFloat_AsDouble(PyList_GET_ITEM(value, k));
	}

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to double");
		delete[] c_values;
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, size, c_values);

	delete[] c_values;
	return 0;
}

int MGLUniform_sampler_array_value_setter(MGLUniform * self, PyObject * value) {

	if (Py_TYPE(value) != &PyList_Type) {
		MGLError_Set("the value must be a list not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyList_GET_SIZE(value);

	if (size != self->array_length) {
		MGLError_Set("the value must be a list of size %d not %d", self->array_length, size);
		return -1;
	}

	int * c_values = new int[size];

	for (int k = 0; k < size; ++k) {
		c_values[k] = PyLong_AsLong(PyList_GET_ITEM(value, k));
	}

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to int");
		delete[] c_values;
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, size, c_values);

	delete[] c_values;
	return 0;
}

template <int N>
int MGLUniform_bvec_value_setter(MGLUniform * self, PyObject * value) {
	int c_values[N];

	if (Py_TYPE(value) != &PyTuple_Type) {
		MGLError_Set("the value must be a tuple not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyTuple_GET_SIZE(value);

	if (size != N) {
		MGLError_Set("the value must be a tuple of size %d not %d", N, size);
		return -1;
	}

	for (int i = 0; i < N; ++i) {
		PyObject * v = PyTuple_GET_ITEM(value, i);

		if (v == Py_True) {
			c_values[i] = 1;
		} else if (v == Py_False) {
			c_values[i] = 0;
		} else {
			MGLError_Set("value[%d] must be a bool not %s", i, Py_TYPE(value)->tp_name);
			return -1;
		}
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, 1, c_values);

	return 0;
}

template <int N>
int MGLUniform_ivec_value_setter(MGLUniform * self, PyObject * value) {
	int c_values[N];

	if (Py_TYPE(value) != &PyTuple_Type) {
		MGLError_Set("the value must be a tuple not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyTuple_GET_SIZE(value);

	if (size != N) {
		MGLError_Set("the value must be a tuple of size %d not %d", N, size);
		return -1;
	}

	for (int i = 0; i < N; ++i) {
		c_values[i] = PyLong_AsLong(PyTuple_GET_ITEM(value, i));
	}

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to int");
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, 1, c_values);

	return 0;
}

template <int N>
int MGLUniform_uvec_value_setter(MGLUniform * self, PyObject * value) {
	unsigned c_values[N];

	if (Py_TYPE(value) != &PyTuple_Type) {
		MGLError_Set("the value must be a tuple not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyTuple_GET_SIZE(value);

	if (size != N) {
		MGLError_Set("the value must be a tuple of size %d not %d", N, size);
		return -1;
	}

	for (int i = 0; i < N; ++i) {
		c_values[i] = PyLong_AsUnsignedLong(PyTuple_GET_ITEM(value, i));
	}

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to unsigned int");
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, 1, c_values);

	return 0;
}

template <int N>
int MGLUniform_vec_value_setter(MGLUniform * self, PyObject * value) {
	float c_values[N];

	if (Py_TYPE(value) != &PyTuple_Type) {
		MGLError_Set("the value must be a tuple not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyTuple_GET_SIZE(value);

	if (size != N) {
		MGLError_Set("the value must be a tuple of size %d not %d", N, size);
		return -1;
	}

	for (int i = 0; i < N; ++i) {
		c_values[i] = (float)PyFloat_AsDouble(PyTuple_GET_ITEM(value, i));
	}

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to float");
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, 1, c_values);

	return 0;
}

template <int N>
int MGLUniform_dvec_value_setter(MGLUniform * self, PyObject * value) {
	double c_values[N];

	if (Py_TYPE(value) != &PyTuple_Type) {
		MGLError_Set("the value must be a tuple not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyTuple_GET_SIZE(value);

	if (size != N) {
		MGLError_Set("the value must be a tuple of size %d not %d", N, size);
		return -1;
	}

	for (int i = 0; i < N; ++i) {
		c_values[i] = PyFloat_AsDouble(PyTuple_GET_ITEM(value, i));
	}

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to double");
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, 1, c_values);

	return 0;
}

template <int N>
int MGLUniform_bvec_array_value_setter(MGLUniform * self, PyObject * value) {

	if (Py_TYPE(value) != &PyList_Type) {
		MGLError_Set("the value must be a list not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyList_GET_SIZE(value);

	if (size != self->array_length) {
		MGLError_Set("the value must be a list of size %d not %d", self->array_length, size);
		return -1;
	}

	int cnt = 0;
	int * c_values = new int[size * N];

	for (int k = 0; k < size; ++k) {
		PyObject * tuple = PyList_GET_ITEM(value, k);

		if (Py_TYPE(tuple) != &PyTuple_Type) {
			MGLError_Set("value[%d] must be a tuple not %s", k, Py_TYPE(value)->tp_name);
			delete[] c_values;
			return -1;
		}

		int tuple_size = (int)PyTuple_GET_SIZE(tuple);

		if (tuple_size != N) {
			MGLError_Set("value[%d] must be a tuple of size %d not %d", k, N, tuple_size);
			delete[] c_values;
			return -1;
		}

		for (int i = 0; i < N; ++i) {
			PyObject * v = PyTuple_GET_ITEM(tuple, i);

			if (v == Py_True) {
				c_values[cnt++] = 1;
			} else if (v == Py_False) {
				c_values[cnt++] = 0;
			} else {
				MGLError_Set("value[%d][%d] must be a bool not %s", k, i, Py_TYPE(value)->tp_name);
				delete[] c_values;
				return -1;
			}
		}
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, size * N, c_values);

	delete[] c_values;
	return 0;
}

template <int N>
int MGLUniform_ivec_array_value_setter(MGLUniform * self, PyObject * value) {

	if (Py_TYPE(value) != &PyList_Type) {
		MGLError_Set("the value must be a list not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyList_GET_SIZE(value);

	if (size != self->array_length) {
		MGLError_Set("the value must be a list of size %d not %d", self->array_length, size);
		return -1;
	}

	int cnt = 0;
	int * c_values = new int[size * N];

	for (int k = 0; k < size; ++k) {
		PyObject * tuple = PyList_GET_ITEM(value, k);

		if (Py_TYPE(tuple) != &PyTuple_Type) {
			MGLError_Set("value[%d] must be a tuple not %s", k, Py_TYPE(value)->tp_name);
			delete[] c_values;
			return -1;
		}

		int tuple_size = (int)PyTuple_GET_SIZE(tuple);

		if (tuple_size != N) {
			MGLError_Set("value[%d] must be a tuple of size %d not %d", k, N, tuple_size);
			delete[] c_values;
			return -1;
		}

		for (int i = 0; i < N; ++i) {
			c_values[cnt++] = PyLong_AsLong(PyTuple_GET_ITEM(tuple, i));
		}
	}

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to int");
		delete[] c_values;
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, size * N, c_values);

	delete[] c_values;
	return 0;
}

template <int N>
int MGLUniform_uvec_array_value_setter(MGLUniform * self, PyObject * value) {

	if (Py_TYPE(value) != &PyList_Type) {
		MGLError_Set("the value must be a list not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyList_GET_SIZE(value);

	if (size != self->array_length) {
		MGLError_Set("the value must be a list of size %d not %d", self->array_length, size);
		return -1;
	}

	int cnt = 0;
	unsigned * c_values = new unsigned[size * N];

	for (int k = 0; k < size; ++k) {
		PyObject * tuple = PyList_GET_ITEM(value, k);

		if (Py_TYPE(tuple) != &PyTuple_Type) {
			MGLError_Set("value[%d] must be a tuple not %s", k, Py_TYPE(value)->tp_name);
			delete[] c_values;
			return -1;
		}

		int tuple_size = (int)PyTuple_GET_SIZE(tuple);

		if (tuple_size != N) {
			MGLError_Set("value[%d] must be a tuple of size %d not %d", k, N, tuple_size);
			delete[] c_values;
			return -1;
		}

		for (int i = 0; i < N; ++i) {
			c_values[cnt++] = PyLong_AsUnsignedLong(PyTuple_GET_ITEM(tuple, i));
		}
	}

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to unsigned int");
		delete[] c_values;
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, size * N, c_values);

	delete[] c_values;
	return 0;
}

template <int N>
int MGLUniform_vec_array_value_setter(MGLUniform * self, PyObject * value) {

	if (Py_TYPE(value) != &PyList_Type) {
		MGLError_Set("the value must be a list not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyList_GET_SIZE(value);

	if (size != self->array_length) {
		MGLError_Set("the value must be a list of size %d not %d", self->array_length, size);
		return -1;
	}

	int cnt = 0;
	float * c_values = new float[size * N];

	for (int k = 0; k < size; ++k) {
		PyObject * tuple = PyList_GET_ITEM(value, k);

		if (Py_TYPE(tuple) != &PyTuple_Type) {
			MGLError_Set("value[%d] must be a tuple not %s", k, Py_TYPE(value)->tp_name);
			delete[] c_values;
			return -1;
		}

		int tuple_size = (int)PyTuple_GET_SIZE(tuple);

		if (tuple_size != N) {
			MGLError_Set("value[%d] must be a tuple of size %d not %d", k, N, tuple_size);
			delete[] c_values;
			return -1;
		}

		for (int i = 0; i < N; ++i) {
			c_values[cnt++] = (float)PyFloat_AsDouble(PyTuple_GET_ITEM(tuple, i));
		}
	}

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to float");
		delete[] c_values;
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, size * N, c_values);

	delete[] c_values;
	return 0;
}

template <int N>
int MGLUniform_dvec_array_value_setter(MGLUniform * self, PyObject * value) {

	if (Py_TYPE(value) != &PyList_Type) {
		MGLError_Set("the value must be a list not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyList_GET_SIZE(value);

	if (size != self->array_length) {
		MGLError_Set("the value must be a list of size %d not %d", self->array_length, size);
		return -1;
	}

	int cnt = 0;
	double * c_values = new double[size * N];

	for (int k = 0; k < size; ++k) {
		PyObject * tuple = PyList_GET_ITEM(value, k);

		if (Py_TYPE(tuple) != &PyTuple_Type) {
			MGLError_Set("value[%d] must be a tuple not %s", k, Py_TYPE(value)->tp_name);
			delete[] c_values;
			return -1;
		}

		int tuple_size = (int)PyTuple_GET_SIZE(tuple);

		if (tuple_size != N) {
			MGLError_Set("value[%d] must be a tuple of size %d not %d", k, N, tuple_size);
			delete[] c_values;
			return -1;
		}

		for (int i = 0; i < N; ++i) {
			c_values[cnt++] = PyFloat_AsDouble(PyTuple_GET_ITEM(tuple, i));
		}
	}

	if (PyErr_Occurred()) {
		MGLError_Set("cannot convert value to double");
		delete[] c_values;
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_vector_writer_proc)self->gl_value_writer_proc)(self->location, size * N, c_values);

	delete[] c_values;
	return 0;
}

template <typename T, int N, int M>
int MGLUniform_matrix_value_setter(MGLUniform * self, PyObject * value) {
	T c_values[N * M];

	if (Py_TYPE(value) != &PyTuple_Type) {
		MGLError_Set("the value must be a tuple not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyTuple_GET_SIZE(value);

	if (size != N * M) {
		MGLError_Set("the value must be a tuple of size %d not %d", N * M, size);
		return -1;
	}

	for (int i = 0; i < N * M; ++i) {
		c_values[i] = (T)PyFloat_AsDouble(PyTuple_GET_ITEM(value, i));
	}

	if (PyErr_Occurred()) {
		MGLError_Set("invalid values");
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_matrix_writer_proc)self->gl_value_writer_proc)(self->location, 1, false, c_values);

	return 0;
}

template <typename T, int N, int M>
int MGLUniform_matrix_array_value_setter(MGLUniform * self, PyObject * value) {

	if (Py_TYPE(value) != &PyList_Type) {
		MGLError_Set("the value must be a list not %s", Py_TYPE(value)->tp_name);
		return -1;
	}

	int size = (int)PyList_GET_SIZE(value);

	if (size != self->array_length) {
		MGLError_Set("the value must be a list of size %d not %d", self->array_length, size);
		return -1;
	}

	int cnt = 0;
	T * c_values = new T[size * N * M];

	for (int k = 0; k < size; ++k) {
		PyObject * tuple = PyList_GET_ITEM(value, k);

		if (Py_TYPE(tuple) != &PyTuple_Type) {
			MGLError_Set("value[%d] must be a tuple not %s", k, Py_TYPE(value)->tp_name);
			delete[] c_values;
			return -1;
		}

		int tuple_size = (int)PyTuple_GET_SIZE(tuple);

		if (tuple_size != N * M) {
			MGLError_Set("value[%d] must be a tuple of size %d not %d", k, N * M, tuple_size);
			delete[] c_values;
			return -1;
		}

		for (int i = 0; i < N * M; ++i) {
			c_values[cnt++] = (T)PyFloat_AsDouble(PyTuple_GET_ITEM(tuple, i));
		}
	}

	if (PyErr_Occurred()) {
		MGLError_Set("invalid values");
		delete[] c_values;
		return -1;
	}

    ((gl_use_program_proc)self->gl_use_program_prog)(self->program_obj);
	((gl_uniform_matrix_writer_proc)self->gl_value_writer_proc)(self->location, size, false, c_values);

	delete[] c_values;
	return 0;
}

template int MGLUniform_bvec_value_setter<2>(MGLUniform * self, PyObject * value);
template int MGLUniform_bvec_value_setter<3>(MGLUniform * self, PyObject * value);
template int MGLUniform_bvec_value_setter<4>(MGLUniform * self, PyObject * value);

template int MGLUniform_ivec_value_setter<2>(MGLUniform * self, PyObject * value);
template int MGLUniform_ivec_value_setter<3>(MGLUniform * self, PyObject * value);
template int MGLUniform_ivec_value_setter<4>(MGLUniform * self, PyObject * value);

template int MGLUniform_uvec_value_setter<2>(MGLUniform * self, PyObject * value);
template int MGLUniform_uvec_value_setter<3>(MGLUniform * self, PyObject * value);
template int MGLUniform_uvec_value_setter<4>(MGLUniform * self, PyObject * value);

template int MGLUniform_vec_value_setter<2>(MGLUniform * self, PyObject * value);
template int MGLUniform_vec_value_setter<3>(MGLUniform * self, PyObject * value);
template int MGLUniform_vec_value_setter<4>(MGLUniform * self, PyObject * value);

template int MGLUniform_dvec_value_setter<2>(MGLUniform * self, PyObject * value);
template int MGLUniform_dvec_value_setter<3>(MGLUniform * self, PyObject * value);
template int MGLUniform_dvec_value_setter<4>(MGLUniform * self, PyObject * value);

template int MGLUniform_bvec_array_value_setter<2>(MGLUniform * self, PyObject * value);
template int MGLUniform_bvec_array_value_setter<3>(MGLUniform * self, PyObject * value);
template int MGLUniform_bvec_array_value_setter<4>(MGLUniform * self, PyObject * value);

template int MGLUniform_ivec_array_value_setter<2>(MGLUniform * self, PyObject * value);
template int MGLUniform_ivec_array_value_setter<3>(MGLUniform * self, PyObject * value);
template int MGLUniform_ivec_array_value_setter<4>(MGLUniform * self, PyObject * value);

template int MGLUniform_uvec_array_value_setter<2>(MGLUniform * self, PyObject * value);
template int MGLUniform_uvec_array_value_setter<3>(MGLUniform * self, PyObject * value);
template int MGLUniform_uvec_array_value_setter<4>(MGLUniform * self, PyObject * value);

template int MGLUniform_vec_array_value_setter<2>(MGLUniform * self, PyObject * value);
template int MGLUniform_vec_array_value_setter<3>(MGLUniform * self, PyObject * value);
template int MGLUniform_vec_array_value_setter<4>(MGLUniform * self, PyObject * value);

template int MGLUniform_dvec_array_value_setter<2>(MGLUniform * self, PyObject * value);
template int MGLUniform_dvec_array_value_setter<3>(MGLUniform * self, PyObject * value);
template int MGLUniform_dvec_array_value_setter<4>(MGLUniform * self, PyObject * value);

template int MGLUniform_matrix_value_setter<float, 2, 2>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_value_setter<float, 2, 3>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_value_setter<float, 2, 4>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_value_setter<float, 3, 2>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_value_setter<float, 3, 3>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_value_setter<float, 3, 4>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_value_setter<float, 4, 2>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_value_setter<float, 4, 3>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_value_setter<float, 4, 4>(MGLUniform * self, PyObject * value);

template int MGLUniform_matrix_value_setter<double, 2, 2>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_value_setter<double, 2, 3>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_value_setter<double, 2, 4>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_value_setter<double, 3, 2>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_value_setter<double, 3, 3>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_value_setter<double, 3, 4>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_value_setter<double, 4, 2>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_value_setter<double, 4, 3>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_value_setter<double, 4, 4>(MGLUniform * self, PyObject * value);

template int MGLUniform_matrix_array_value_setter<float, 2, 2>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_array_value_setter<float, 2, 3>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_array_value_setter<float, 2, 4>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_array_value_setter<float, 3, 2>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_array_value_setter<float, 3, 3>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_array_value_setter<float, 3, 4>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_array_value_setter<float, 4, 2>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_array_value_setter<float, 4, 3>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_array_value_setter<float, 4, 4>(MGLUniform * self, PyObject * value);

template int MGLUniform_matrix_array_value_setter<double, 2, 2>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_array_value_setter<double, 2, 3>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_array_value_setter<double, 2, 4>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_array_value_setter<double, 3, 2>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_array_value_setter<double, 3, 3>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_array_value_setter<double, 3, 4>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_array_value_setter<double, 4, 2>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_array_value_setter<double, 4, 3>(MGLUniform * self, PyObject * value);
template int MGLUniform_matrix_array_value_setter<double, 4, 4>(MGLUniform * self, PyObject * value);
