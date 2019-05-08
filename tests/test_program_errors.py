import unittest

from common import get_context


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ctx = get_context()

    def test_error_in_vertex_shader(self):
        try:
            self.ctx.program(
                vertex_shader='''
                            #version 330

                            uniform vec2 pos;
                            uniform float scale;

                            in vec2 vert;
                            out vec2 v_vert;

                            toto;

                            void main() {
                                gl_Position = vec4(pos + vert * scale, 0.0, 1.0);
                                v_vert = vert + ;
                            }
                        ''',
            )
        except Exception as e:
            self.assertEqual(
                e.args[0],
                """\
GLSL Compiler failed

vertex_shader
=============

  0001  
  0002                              #version 330
  0003  
  0004                              uniform vec2 pos;
  0005                              uniform float scale;
  0006  
  0007                              in vec2 vert;
  0008                              out vec2 v_vert;
  0009  
  0010                              toto;

error C0000: syntax error, unexpected ';', expecting "::" at token ";"

  0011  
  0012                              void main() {
  0013                                  gl_Position = vec4(pos + vert * scale, 0.0, 1.0);
  0014                                  v_vert = vert + ;

error C0000: syntax error, unexpected ';', expecting "::" at token ";"

  0015                              }
  0016                          

"""
            )

    def test_error_in_fragment_shader(self):
        try:
            self.ctx.program(
                vertex_shader='''
                            #version 330

                            uniform vec2 pos;
                            uniform float scale;

                            in vec2 vert;
                            out vec2 v_vert;

                            void main() {
                                gl_Position = vec4(pos + vert * scale, 0.0, 1.0);
                                v_vert = vert;
                            }
                        ''',
                fragment_shader='''
                                #version 330

                                in vec2 v_vert;
                                out vec4 color;

                                void main() {
                                    color = vec4(v_vert, 0.0, 1.0);
                                    color = texture(Texture, v_vert);
                                }
                            ''',
            )
        except Exception as e:
            self.assertEqual(
                e.args[0],
                """\
GLSL Compiler failed

fragment_shader
===============

  0001  
  0002                                  #version 330
  0003  
  0004                                  in vec2 v_vert;
  0005                                  out vec4 color;
  0006  
  0007                                  void main() {
  0008                                      color = vec4(v_vert, 0.0, 1.0);
  0009                                      color = texture(Texture, v_vert);

error C1008: undefined variable "Texture"

  0010                                  }
  0011                              

"""
            )

    def test_error_in_geometry_shader(self):
        try:
            self.ctx.program(
                vertex_shader='''
                            #version 330

                            uniform vec2 pos;
                            uniform float scale;

                            in vec2 vert;
                            out vec2 v_vert;

                            void main() {
                                gl_Position = vec4(pos + vert * scale, 0.0, 1.0);
                                v_vert = vert;
                            }
                        ''',
                geometry_shader='''
                            #version 330
                            layout(points) in;
                            uniform vec4 Position;
                            layout(line_strip, max_vertices = 2) out;
                            out vec4 color;
                            void main() {
                                gl_Position = gl_in[0].gl_Position + Position;
                                EmitVertex();
                                gl_Position = gl_in[0].gl_Position - Position;
                                mitVertex();
                                EndPrimitive();
                            }
                        '''
            )
        except Exception as e:
            self.assertEqual(
                e.args[0],
                """\
GLSL Compiler failed

geometry_shader
===============

  0001  
  0002                              #version 330
  0003                              layout(points) in;
  0004                              uniform vec4 Position;
  0005                              layout(line_strip, max_vertices = 2) out;
  0006                              out vec4 color;
  0007                              void main() {
  0008                                  gl_Position = gl_in[0].gl_Position + Position;
  0009                                  EmitVertex();
  0010                                  gl_Position = gl_in[0].gl_Position - Position;
  0011                                  mitVertex();

error C1008: undefined variable "mitVertex"

  0012                                  EndPrimitive();
  0013                              }
  0014                          

"""
            )

    def test_error_in_tess_evaluation_shader(self):
        try:
            self.ctx.program(
                vertex_shader='''
                        #version 400 core

                        in vec2 in_pos;

                        void main() { gl_Position = vec4(in_pos, 0.0, 1.0); }
                        ''',
                tess_control_shader='''
                        #version 400 core

                        layout(vertices = 4) out;

                        void main() {
                          // set tesselation levels, TODO compute dynamically
                          gl_TessLevelOuter[0] = 1;
                          gl_TessLevelOuter[1] = 32;

                          // pass through vertex positions
                          gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
                        }
                        ''',
                tess_evaluation_shader='''
                        #version 400 core

                        layout(isolines, fractional_even_spacing, ccw) in;

                        // compute a point on a bezier curve with the points p0, p1, p2, p3
                        // the parameter u is in [0, 1] and determines the position on the curve
                        vec3 bezier(float u, vec3 p0, vec3 p1, vec3 p2, vec3 p3) {
                          float B0 = (1.0 - u) * (1.0 - u) * (1.0 - u);
                          float B1 = 3.0 * (1.0 - u) * (1.0 - u) * u;
                          float B2 = 3.0 * (1.0 - u) * u * u;
                          float B3 = u * u * u;

                          return B0 * p0 + B1 * p1 + B2 * p2 + B3 * p3;
                        }

                        void main() {
                          float u = gl_TessCoord.x;

                          vec3 p0 = vec3(gl_in[0].gl_Position);
                          vec3 p1 = vec3(gl_in[1].gl_Position);
                          vec3 p2 = vec3(gl_in[2].gl_Position);
                          vec3 p3 = vec3(gl_in[3].gl_Position);

                          gl_Position = vec4(bezier(u, p0, p1, p2, p3), 1.0);
                          
                          gl_Position = toto;
                        }
                        ''',
                fragment_shader='''
                        #version 400 core

                        out vec4 frag_color;

                        void main() { frag_color = vec4(1.0); }
                        '''
            )
        except Exception as e:
            self.assertEqual(
                e.args[0],
                """\
GLSL Compiler failed

tess_evaluation_shader
======================

  0001  
  0002                          #version 400 core
  0003  
  0004                          layout(isolines, fractional_even_spacing, ccw) in;
  0005  
  0006                          // compute a point on a bezier curve with the points p0, p1, p2, p3
  0007                          // the parameter u is in [0, 1] and determines the position on the curve
  0008                          vec3 bezier(float u, vec3 p0, vec3 p1, vec3 p2, vec3 p3) {
  0009                            float B0 = (1.0 - u) * (1.0 - u) * (1.0 - u);
  0010                            float B1 = 3.0 * (1.0 - u) * (1.0 - u) * u;
  0011                            float B2 = 3.0 * (1.0 - u) * u * u;
  0012                            float B3 = u * u * u;
  0013  
  0014                            return B0 * p0 + B1 * p1 + B2 * p2 + B3 * p3;
  0015                          }
  0016  
  0017                          void main() {
  0018                            float u = gl_TessCoord.x;
  0019  
  0020                            vec3 p0 = vec3(gl_in[0].gl_Position);
  0021                            vec3 p1 = vec3(gl_in[1].gl_Position);
  0022                            vec3 p2 = vec3(gl_in[2].gl_Position);
  0023                            vec3 p3 = vec3(gl_in[3].gl_Position);
  0024  
  0025                            gl_Position = vec4(bezier(u, p0, p1, p2, p3), 1.0);
  0026                            
  0027                            gl_Position = toto;

error C1008: undefined variable "toto"

  0028                          }
  0029                          

"""
            )

    def test_error_in_tess_control_shader(self):
        try:
            self.ctx.program(
                vertex_shader='''
                        #version 400 core

                        in vec2 in_pos;

                        void main() { gl_Position = vec4(in_pos, 0.0, 1.0); }
                        ''',
                tess_control_shader='''
                        #version 400 core

                        layout(vertices = 4) out;

                        void main() {
                          // set tesselation levels, TODO compute dynamically
                          gl_TessLevelOuter[0] = 1;
                          gl_TessLevelOuter[1] = 32;
                          
                          l_TessLevelOuter[2] = toto;

                          // pass through vertex positions
                          gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
                        }
                        ''',
                tess_evaluation_shader='''
                        #version 400 core

                        layout(isolines, fractional_even_spacing, ccw) in;

                        // compute a point on a bezier curve with the points p0, p1, p2, p3
                        // the parameter u is in [0, 1] and determines the position on the curve
                        vec3 bezier(float u, vec3 p0, vec3 p1, vec3 p2, vec3 p3) {
                          float B0 = (1.0 - u) * (1.0 - u) * (1.0 - u);
                          float B1 = 3.0 * (1.0 - u) * (1.0 - u) * u;
                          float B2 = 3.0 * (1.0 - u) * u * u;
                          float B3 = u * u * u;

                          return B0 * p0 + B1 * p1 + B2 * p2 + B3 * p3;
                        }

                        void main() {
                          float u = gl_TessCoord.x;

                          vec3 p0 = vec3(gl_in[0].gl_Position);
                          vec3 p1 = vec3(gl_in[1].gl_Position);
                          vec3 p2 = vec3(gl_in[2].gl_Position);
                          vec3 p3 = vec3(gl_in[3].gl_Position);

                          gl_Position = vec4(bezier(u, p0, p1, p2, p3), 1.0);
                        }
                        ''',
                fragment_shader='''
                        #version 400 core

                        out vec4 frag_color;

                        void main() { frag_color = vec4(1.0); }
                        '''
            )
        except Exception as e:
            self.assertEqual(
                e.args[0],
                """\
GLSL Compiler failed

tess_control_shader
===================

  0001  
  0002                          #version 400 core
  0003  
  0004                          layout(vertices = 4) out;
  0005  
  0006                          void main() {
  0007                            // set tesselation levels, TODO compute dynamically
  0008                            gl_TessLevelOuter[0] = 1;
  0009                            gl_TessLevelOuter[1] = 32;
  0010                            
  0011                            l_TessLevelOuter[2] = toto;

error C1008: undefined variable "l_TessLevelOuter"
error C1008: undefined variable "toto"

  0012  
  0013                            // pass through vertex positions
  0014                            gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  0015                          }
  0016                          

"""
            )


if __name__ == '__main__':
    unittest.main()
