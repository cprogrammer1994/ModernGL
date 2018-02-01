Context
=======

.. py:module:: moderngl
.. py:currentmodule:: moderngl

Examples
--------

ModernGL Context in a Window
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

    import moderngl
    # create a window
    ctx = moderngl.create_context()
    print(ctx.version_code)

Standalone ModernGL Context
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

    import moderngl
    ctx = moderngl.create_standalone_context()
    print(ctx.version_code)

Context Creation
----------------

.. autofunction:: moderngl.create_context(require=None) -> Context
.. autofunction:: moderngl.create_standalone_context(require=None) -> Context

ModernGL Objects
----------------

.. automethod:: Context.buffer(data=None, reserve=0, dynamic=False) -> Buffer
.. automethod:: Context.texture(size, components, data=None, samples=0, alignment=1, dtype='u1') -> Texture
.. automethod:: Context.texture3d(size, components, data=None, alignment=1, dtype='u1') -> Texture3D
.. automethod:: Context.texture_cube(size, components, data=None, alignment=1, dtype='u1') -> TextureCube
.. automethod:: Context.depth_texture(size, data=None, samples=0, alignment=4) -> Texture
.. automethod:: Context.vertex_array(program, content, index_buffer=None) -> VertexArray
.. automethod:: Context.simple_vertex_array(program, buffer, *attributes, index_buffer=None) -> VertexArray
.. automethod:: Context.program(shaders, varyings=()) -> Program
.. automethod:: Context.query(samples=False, any_samples=False, time=False, primitives=False) -> Query
.. automethod:: Context.scope(framebuffer, enable_only, textures, uniform_buffers, shader_storage_buffers) -> Scope
.. automethod:: Context.vertex_shader(source) -> Shader
.. automethod:: Context.fragment_shader(source) -> Shader
.. automethod:: Context.geometry_shader(source) -> Shader
.. automethod:: Context.tess_evaluation_shader(source) -> Shader
.. automethod:: Context.tess_control_shader(source) -> Shader
.. automethod:: Context.simple_framebuffer(size, components=4, samples=0, dtype='u1') -> Framebuffer
.. automethod:: Context.framebuffer(color_attachments, depth_attachment=None) -> Framebuffer
.. automethod:: Context.renderbuffer(size, components=4, samples=0, dtype='u1') -> Renderbuffer
.. automethod:: Context.depth_renderbuffer(size, samples=0) -> Renderbuffer
.. automethod:: Context.compute_shader(source) -> ComputeShader

Methods
-------

.. automethod:: Context.clear(red=0.0, green=0.0, blue=0.0, alpha=0.0, depth=1.0, viewport=None)
.. automethod:: Context.enable_only(flags)
.. automethod:: Context.enable(flags)
.. automethod:: Context.disable(flags)
.. automethod:: Context.finish()
.. automethod:: Context.copy_buffer(dst, src, size=-1, read_offset=0, write_offset=0)
.. automethod:: Context.copy_framebuffer(dst, src)
.. automethod:: Context.detect_framebuffer(glo=None) -> Framebuffer

Attributes
----------

.. autoattribute:: Context.line_width
.. autoattribute:: Context.point_size
.. autoattribute:: Context.viewport
.. autoattribute:: Context.max_samples
.. autoattribute:: Context.max_integer_samples
.. autoattribute:: Context.max_texture_units
.. autoattribute:: Context.default_texture_unit
.. autoattribute:: Context.screen
.. autoattribute:: Context.wireframe
.. autoattribute:: Context.error
.. autoattribute:: Context.version_code
.. autoattribute:: Context.info
.. autoattribute:: Context.front_face

.. toctree::
    :maxdepth: 2