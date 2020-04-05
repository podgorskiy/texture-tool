#
# Copyright 2017-2019 Stanislav Pidhorskyi. All rights reserved.
# License: https://raw.githubusercontent.com/podgorskiy/impy/master/LICENSE.txt
#

from setuptools import setup, Extension, find_packages
from distutils.errors import *
from distutils.dep_util import newer_group
from distutils import log
from distutils.command.build_ext import build_ext

from codecs import open
import os
import sys
import platform
import re
import glob

target_os = 'none'

if sys.platform == 'darwin':
    target_os = 'darwin'
elif os.name == 'posix':
    target_os = 'posix'
elif platform.system() == 'Windows':
    target_os = 'win32'

here = os.path.abspath(os.path.dirname(__file__))

# with open(os.path.join(here, 'README.rst'), encoding='utf-8') as f:
#     long_description = f.read()


def filter_sources(sources):
    """Filters sources into c, cpp, objc and asm"""
    cpp_ext_match = re.compile(r'.*[.](cpp|cxx|cc)\Z', re.I).match
    c_ext_match = re.compile(r'.*[.](c|C)\Z', re.I).match
    objc_ext_match = re.compile(r'.*[.]m\Z', re.I).match
    asm_ext_match = re.compile(r'.*[.](asm|s|S)\Z', re.I).match

    c_sources = []
    cpp_sources = []
    objc_sources = []
    asm_sources = []
    other_sources = []
    for source in sources:
        if c_ext_match(source):
            c_sources.append(source)
        elif cpp_ext_match(source):
            cpp_sources.append(source)
        elif objc_ext_match(source):
            objc_sources.append(source)
        elif asm_ext_match(source):
            asm_sources.append(source)
        else:
            other_sources.append(source)
    return c_sources, cpp_sources, objc_sources, asm_sources, other_sources


def build_extension(self, ext):
    """Modified version of build_extension method from distutils.
       Can handle compiler args for different files"""

    sources = ext.sources
    if sources is None or not isinstance(sources, (list, tuple)):
        raise DistutilsSetupError(
              "in 'ext_modules' option (extension '%s'), "
              "'sources' must be present and must be "
              "a list of source filenames" % ext.name)

    sources = list(sources)
    ext_path = self.get_ext_fullpath(ext.name)
    depends = sources + ext.depends
    if not (self.force or newer_group(depends, ext_path, 'newer')):
        log.debug("skipping '%s' extension (up-to-date)", ext.name)
        return
    else:
        log.info("building '%s' extension", ext.name)

    sources = self.swig_sources(sources, ext)

    extra_args = ext.extra_compile_args or []
    extra_c_args = getattr(ext, "extra_compile_c_args", [])
    extra_cpp_args = getattr(ext, "extra_compile_cpp_args", [])
    extra_objc_args = getattr(ext, "extra_compile_objc_args", [])
    extra_asm_args = getattr(ext, "extra_compile_asm_args", [])
    file_specific_definitions = getattr(ext, "file_specific_definitions", {})
    asm_include = getattr(ext, "asm_include", [])

    macros = ext.define_macros[:]
    for undef in ext.undef_macros:
        macros.append((undef,))

    c_sources, cpp_sources, objc_sources, asm_sources, other_sources = filter_sources(sources)

    self.compiler.src_extensions += ['.asm']

    self.compiler.set_executable('assembler', ['nasm'])

    def _compile(src, args):
        obj = []
        for s in src:
            additional_macros = []
            if s in file_specific_definitions.keys():
                additional_macros += file_specific_definitions[s]
            obj += self.compiler.compile([s],
                                         output_dir=self.build_temp,
                                         macros=macros + additional_macros,
                                         include_dirs=ext.include_dirs,
                                         debug=self.debug,
                                         extra_postargs=extra_args + args,
                                         depends=ext.depends)
        return obj

    def _compile_asm(src):
        obj = []
        for s in src:
            additional_macros = []
            if s in file_specific_definitions.keys():
                additional_macros += file_specific_definitions[s]
            macros_, objects, extra_postargs, asm_args, build = \
                self.compiler._setup_compile(self.build_temp, macros + additional_macros, asm_include, [s],
                                             depends, extra_asm_args)

            for o in objects:
                try:
                    src, ext = build[o]
                except KeyError:
                    continue
                try:
                    self.spawn(self.compiler.assembler + extra_postargs + asm_args + ['-o', o, src])
                except DistutilsExecError as msg:
                    raise CompileError(msg)
            obj += objects

        return obj

    objects = []
    objects += _compile_asm(asm_sources)
    objects += _compile(c_sources, extra_c_args)
    objects += _compile(cpp_sources, extra_cpp_args)
    objects += _compile(objc_sources, extra_objc_args)
    objects += _compile(other_sources, [])

    self._built_objects = objects[:]
    if ext.extra_objects:
        objects.extend(ext.extra_objects)

    extra_args = ext.extra_link_args or []

    language = ext.language or self.compiler.detect_language(sources)
    self.compiler.link_shared_object(
        objects, ext_path,
        libraries=self.get_libraries(ext),
        library_dirs=ext.library_dirs,
        runtime_library_dirs=ext.runtime_library_dirs,
        extra_postargs=extra_args,
        export_symbols=self.get_export_symbols(ext),
        debug=self.debug,
        build_temp=self.build_temp,
        target_lang=language)

# patching
build_ext.build_extension = build_extension

sources = list(glob.glob('sources/**/*.c*', recursive=True))

definitions = {
    'darwin': [('HAVE_SSE42', 0), ('HAVE_PTHREAD', 0)],
    'posix': [('HAVE_SSE42', 0), ('HAVE_PTHREAD', 0)],
    'win32': [('HAVE_SSE42', 0)],
}

libs = {
    'darwin': [],
    'posix': ["rt", "m", "stdc++fs"],
    'win32': [],
}

extra_link = {
    'darwin': [],
    'posix': ['-static-libstdc++', '-static-libgcc', '-L 3dparty/PVRTexTool/Linux_x86_64/libPVRTexLib.so', '-lpthread'],
    'win32': [],
}

extra_compile_args = {
    'darwin': ['-fPIC', '-g', '-msse2', '-msse3', '-msse4', '-mpopcnt', '-funsafe-math-optimizations'],
    'posix': ['-fPIC', '-g', '-msse2', '-msse3', '-msse4', '-mpopcnt', '-funsafe-math-optimizations'],
    'win32': ['/MT', '/fp:fast', '/GL', '/GR-'],
}

extra_compile_cpp_args = {
    'darwin': ['-std=c++14', '-lstdc++fs', '-Ofast'],
    'posix': ['-std=c++14', '-lstdc++fs', '-Ofast'],
    'win32': [],
}

extra_compile_c_args = {
    'darwin': ['-std=c99', '-Ofast'],
    'posix': ['-std=c99', '-Ofast'],
    'win32': [],
}

extra_compile_asm_args = ['-DELF', '-D__x86_64__', '-DPIC', '-DTURBO', '-g', '-f elf64', '-Ox']

extension = Extension("_pypvrtex",
                      sources,
                             define_macros = definitions[target_os],
                             include_dirs=[
                                 "3dparty/pybind11/include",
                                 "3dparty/PVRTexTool/Include",
                                 "sources",
                             ],
                             extra_compile_args=extra_compile_args[target_os],
                             extra_link_args=extra_link[target_os],
                             libraries = libs[target_os])

extension.extra_compile_cpp_args = extra_compile_cpp_args[target_os]
extension.extra_compile_c_args = extra_compile_c_args[target_os]
extension.extra_compile_asm_args = extra_compile_asm_args

setup(
    name='pypvrtex',

    version='0.0.1',

    description='pypvrtex',
    # long_description=long_description,

    url='https://github.com/podgorskiy/pypvrtex',

    author='Stanislav Pidhorskyi',
    author_email='stanislav@podgorskiy.com',

    license='MIT',

    classifiers=[
        'Development Status :: 3 - Alpha',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3.6',
    ],

    keywords='pypvrtex',

    packages=['pypvrtex'],

    ext_modules=[extension],
)
