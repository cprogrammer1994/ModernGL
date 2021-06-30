#pragma once

#include "Types.hpp"


typedef void (GLAPI * gl_uniform_reader_proc)(GLuint program, GLint location, void * value);
typedef void (GLAPI * gl_uniform_vector_writer_proc)(GLint location, GLsizei count, const void * value);
typedef void (GLAPI * gl_uniform_matrix_writer_proc)(GLint location, GLsizei count, GLboolean transpose, const void * value);
typedef void (GLAPI * gl_use_program_proc)(GLuint program);
typedef void (GLAPI * gl_get_intergerv_proc)(GLenum pname, GLint * data);

typedef PyObject * (* MGLUniform_Getter)(MGLUniform * self);
typedef int (* MGLUniform_Setter)(MGLUniform * self, PyObject * value);

PyObject * MGLUniform_invalid_getter(MGLUniform * self);

PyObject * MGLUniform_bool_value_getter(MGLUniform * self);
PyObject * MGLUniform_int_value_getter(MGLUniform * self);
PyObject * MGLUniform_uint_value_getter(MGLUniform * self);
PyObject * MGLUniform_float_value_getter(MGLUniform * self);
PyObject * MGLUniform_double_value_getter(MGLUniform * self);

PyObject * MGLUniform_bool_array_value_getter(MGLUniform * self);
PyObject * MGLUniform_int_array_value_getter(MGLUniform * self);
PyObject * MGLUniform_uint_array_value_getter(MGLUniform * self);
PyObject * MGLUniform_float_array_value_getter(MGLUniform * self);
PyObject * MGLUniform_double_array_value_getter(MGLUniform * self);

PyObject * MGLUniform_sampler_value_getter(MGLUniform * self);
PyObject * MGLUniform_sampler_array_value_getter(MGLUniform * self);

int MGLUniform_invalid_setter(MGLUniform * self, PyObject * value);

int MGLUniform_bool_value_setter(MGLUniform * self, PyObject * value);
int MGLUniform_int_value_setter(MGLUniform * self, PyObject * value);
int MGLUniform_uint_value_setter(MGLUniform * self, PyObject * value);
int MGLUniform_float_value_setter(MGLUniform * self, PyObject * value);
int MGLUniform_double_value_setter(MGLUniform * self, PyObject * value);

int MGLUniform_bool_array_value_setter(MGLUniform * self, PyObject * value);
int MGLUniform_int_array_value_setter(MGLUniform * self, PyObject * value);
int MGLUniform_uint_array_value_setter(MGLUniform * self, PyObject * value);
int MGLUniform_float_array_value_setter(MGLUniform * self, PyObject * value);
int MGLUniform_double_array_value_setter(MGLUniform * self, PyObject * value);

int MGLUniform_sampler_value_setter(MGLUniform * self, PyObject * value);
int MGLUniform_sampler_array_value_setter(MGLUniform * self, PyObject * value);

template <int N>
PyObject * MGLUniform_bvec_value_getter(MGLUniform * self);

template <int N>
PyObject * MGLUniform_ivec_value_getter(MGLUniform * self);

template <int N>
PyObject * MGLUniform_uvec_value_getter(MGLUniform * self);

template <int N>
PyObject * MGLUniform_vec_value_getter(MGLUniform * self);

template <int N>
PyObject * MGLUniform_dvec_value_getter(MGLUniform * self);

template <int N>
PyObject * MGLUniform_bvec_array_value_getter(MGLUniform * self);

template <int N>
PyObject * MGLUniform_ivec_array_value_getter(MGLUniform * self);

template <int N>
PyObject * MGLUniform_uvec_array_value_getter(MGLUniform * self);

template <int N>
PyObject * MGLUniform_vec_array_value_getter(MGLUniform * self);

template <int N>
PyObject * MGLUniform_dvec_array_value_getter(MGLUniform * self);

template <int N>
int MGLUniform_bvec_value_setter(MGLUniform * self, PyObject * value);

template <int N>
int MGLUniform_ivec_value_setter(MGLUniform * self, PyObject * value);

template <int N>
int MGLUniform_uvec_value_setter(MGLUniform * self, PyObject * value);

template <int N>
int MGLUniform_vec_value_setter(MGLUniform * self, PyObject * value);

template <int N>
int MGLUniform_dvec_value_setter(MGLUniform * self, PyObject * value);

template <int N>
int MGLUniform_bvec_array_value_setter(MGLUniform * self, PyObject * value);

template <int N>
int MGLUniform_ivec_array_value_setter(MGLUniform * self, PyObject * value);

template <int N>
int MGLUniform_uvec_array_value_setter(MGLUniform * self, PyObject * value);

template <int N>
int MGLUniform_vec_array_value_setter(MGLUniform * self, PyObject * value);

template <int N>
int MGLUniform_dvec_array_value_setter(MGLUniform * self, PyObject * value);

template <typename T, int N, int M>
PyObject * MGLUniform_matrix_value_getter(MGLUniform * self);

template <typename T, int N, int M>
PyObject * MGLUniform_matrix_array_value_getter(MGLUniform * self);

template <typename T, int N, int M>
int MGLUniform_matrix_value_setter(MGLUniform * self, PyObject * value);

template <typename T, int N, int M>
int MGLUniform_matrix_array_value_setter(MGLUniform * self, PyObject * value);
