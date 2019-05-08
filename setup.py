import os
import platform
import re
import sys
from distutils.sysconfig import get_config_var
from distutils.version import LooseVersion

from setuptools import Extension, setup

# pylint: disable=C0103, W0212

if sys.version_info < (3, 0):
    raise Exception('Python 2 is not supported!')

PLATFORMS = {'windows', 'linux', 'darwin', 'cygwin', 'android'}

target = platform.system().lower()

if 'pydroid3' in sys.executable.lower():
    target = 'android'

for known in PLATFORMS:
    if target.startswith(known):
        target = known

if target not in PLATFORMS:
    target = 'linux'

# For mac, ensure extensions are built for macos 10.9 when compiling on a
# 10.9 system or above, overriding distuitls behaviour which is to target
# the version that python was built for. This may be overridden by setting
# MACOSX_DEPLOYMENT_TARGET before calling setup.py
if target == 'darwin':
    if 'MACOSX_DEPLOYMENT_TARGET' not in os.environ:
        current_system = LooseVersion(platform.mac_ver()[0])
        python_target = LooseVersion(get_config_var('MACOSX_DEPLOYMENT_TARGET'))
        if python_target < '10.9' and current_system >= '10.9':
            os.environ['MACOSX_DEPLOYMENT_TARGET'] = '10.9'

if target in ['linux', 'cygwin']:
    from distutils import sysconfig
    cvars = sysconfig.get_config_vars()
    
    if hasattr(sysconfig, '_config_vars') and sysconfig._config_vars is not None:
        if 'OPT' in cvars:
            sysconfig._config_vars['OPT'] = cvars['OPT'].replace('-Wstrict-prototypes', '')
            sysconfig._config_vars['OPT'] = cvars['OPT'].replace('-Wimplicit-function-declaration', '')

        if 'CFLAGS' in cvars:
            sysconfig._config_vars['CFLAGS'] = cvars['CFLAGS'].replace('-Wstrict-prototypes', '')
            sysconfig._config_vars['CFLAGS'] = cvars['CFLAGS'].replace('-Wimplicit-function-declaration', '')

libraries = {
    'windows': ['gdi32', 'opengl32', 'user32'],
    'linux': ['GL', 'dl', 'X11'],
    'cygwin': ['GL', 'X11'],
    'darwin': [],
    'android': [],
}

extra_compile_args = {
    'windows': [],
    'linux': [],
    'cygwin': [],
    'darwin': ['-Wno-deprecated-declarations'],
    'android': [],
}

extra_linker_args = {
    'windows': [],
    'linux': [],
    'cygwin': [],
    'darwin': ['-framework', 'OpenGL', '-Wno-deprecated'],
    'android': [],
}

mgl = Extension(
    name='moderngl.mgl',
    include_dirs=['src', 'moderngl', 'moderngl/mgl'],
    define_macros=[],
    libraries=libraries[target],
    extra_compile_args=extra_compile_args[target],
    extra_link_args=extra_linker_args[target],
    sources=[
        'moderngl/mgl/buffer.cpp',
        'moderngl/mgl/compute_shader.cpp',
        'moderngl/mgl/configuration.cpp',
        'moderngl/mgl/context.cpp',
        'moderngl/mgl/extensions.cpp',
        'moderngl/mgl/framebuffer.cpp',
        'moderngl/mgl/limits.cpp',
        'moderngl/mgl/mgl.cpp',
        'moderngl/mgl/program.cpp',
        'moderngl/mgl/query.cpp',
        'moderngl/mgl/recorder.cpp',
        'moderngl/mgl/refholder.cpp',
        'moderngl/mgl/renderbuffer.cpp',
        'moderngl/mgl/sampler.cpp',
        'moderngl/mgl/scope.cpp',
        'moderngl/mgl/texture.cpp',
        'moderngl/mgl/vertex_array.cpp',
        'moderngl/mgl/adaptor/adaptor.cpp',
        'moderngl/mgl/internal/bytecode.cpp',
        'moderngl/mgl/internal/compare_func.cpp',
        'moderngl/mgl/internal/data_type.cpp',
        'moderngl/mgl/internal/glsl.cpp',
        'moderngl/mgl/internal/intern.cpp',
        'moderngl/mgl/internal/python.cpp',
        'moderngl/mgl/internal/tools.cpp',
        'moderngl/mgl/internal/uniform.cpp',
        'moderngl/mgl/internal/wrapper.cpp',
        'moderngl/mgl/internal/opengl/gl_context_%s.cpp' % target,
        'moderngl/mgl/internal/opengl/gl_methods.cpp',

        'src/Sampler.cpp',
        'src/Attribute.cpp',
        'src/Buffer.cpp',
        'src/BufferFormat.cpp',
        'src/ComputeShader.cpp',
        'src/Context.cpp',
        'src/DataType.cpp',
        'src/Error.cpp',
        'src/Framebuffer.cpp',
        'src/InvalidObject.cpp',
        'src/ModernGL.cpp',
        'src/Program.cpp',
        'src/Program_Error.cpp',
        'src/Query.cpp',
        'src/Renderbuffer.cpp',
        'src/Scope.cpp',
        'src/Texture.cpp',
        'src/Texture3D.cpp',
        'src/TextureArray.cpp',
        'src/TextureCube.cpp',
        'src/Uniform.cpp',
        'src/UniformBlock.cpp',
        'src/UniformGetters.cpp',
        'src/UniformSetters.cpp',
        'src/VertexArray.cpp',
    ],
    depends=[
        'moderngl/mgl/buffer.hpp',
        'moderngl/mgl/compute_shader.hpp',
        'moderngl/mgl/configuration.hpp',
        'moderngl/mgl/context.hpp',
        'moderngl/mgl/extensions.hpp',
        'moderngl/mgl/framebuffer.hpp',
        'moderngl/mgl/limits.hpp',
        'moderngl/mgl/mgl.hpp',
        'moderngl/mgl/program.hpp',
        'moderngl/mgl/query.hpp',
        'moderngl/mgl/recorder.hpp',
        'moderngl/mgl/refholder.hpp',
        'moderngl/mgl/renderbuffer.hpp',
        'moderngl/mgl/sampler.hpp',
        'moderngl/mgl/scope.hpp',
        'moderngl/mgl/texture.hpp',
        'moderngl/mgl/vertex_array.hpp',
        'moderngl/mgl/adaptor/adaptor.hpp',
        'moderngl/mgl/internal/bytecode.hpp',
        'moderngl/mgl/internal/compare_func.hpp',
        'moderngl/mgl/internal/data_type.hpp',
        'moderngl/mgl/internal/glsl.hpp',
        'moderngl/mgl/internal/intern.hpp',
        'moderngl/mgl/internal/python.hpp',
        'moderngl/mgl/internal/tools.hpp',
        'moderngl/mgl/internal/uniform.hpp',
        'moderngl/mgl/internal/wrapper.hpp',
        'moderngl/mgl/internal/opengl/gl_context.hpp',
        'moderngl/mgl/internal/opengl/gl_methods.hpp',
        'moderngl/mgl/internal/opengl/opengl.hpp',

        'src/BufferFormat.hpp',
        'src/Error.hpp',
        'src/InlineMethods.hpp',
        'src/OpenGL.hpp',
        'src/Python.hpp',
        'src/Types.hpp',
        'src/UniformGetSetters.hpp',
    ],
)

short_description = 'ModernGL: High performance rendering for Python 3'

with open('README.md') as f:
    long_description = f.read()
    long_description = re.sub(r'</?div[^>]*>|\r', '', long_description, flags=re.M)

keywords = [
    'ModernGL',
    'OpenGL',
    'PyOpenGL',
    'rendering',
    'graphics',
    'shader',
    'GLSL',
    'GPU',
    'visualization',
    '2D',
    '3D',
]

classifiers = [
    'Development Status :: 5 - Production/Stable',
    'License :: OSI Approved :: MIT License',
    'Operating System :: OS Independent',
    'Topic :: Games/Entertainment',
    'Topic :: Multimedia :: Graphics',
    'Topic :: Multimedia :: Graphics :: 3D Rendering',
    'Topic :: Scientific/Engineering :: Visualization',
    'Programming Language :: Python :: 3 :: Only',
]

setup(
    name='moderngl',
    version='5.5.0',
    description=short_description,
    long_description=long_description,
    long_description_content_type='text/markdown',
    url='https://github.com/cprogrammer1994/ModernGL',
    author='Szabolcs Dombi',
    author_email='cprogrammer1994@gmail.com',
    license='MIT',
    classifiers=classifiers,
    keywords=keywords,
    packages=['moderngl', 'moderngl.program_members'],
    ext_modules=[mgl],
    platforms=['any'],
)
