from distutils.core import setup, Extension
from Cython.Distutils import build_ext
from Cython.Build import cythonize
import os


if os.name == 'nt':    
    TURBO_PARSER_PATH = "C:\\Projects\\TurboParser"
    TURBO_TEXT_ANALYSIS_PATH = "C:\\Projects\\TurboTextAnalysis"
    
    ext_modules=[Extension("turboparser",
    ["turboparser.pyx", "CppToPyTurboSink.cpp"],
    language="c++",
    extra_compile_args=["/Zi", "/O2",  "/DGOOGLE_GLOG_DLL_DECL=", "/DGFLAGS_DLL_DECL="],
    #extra_link_args=["/DEBUG"],
    include_dirs=
    [".",
    "..",
    os.path.join(TURBO_PARSER_PATH, "libturboparser"),
    os.path.join(TURBO_PARSER_PATH, "src", "util"),
    os.path.join(TURBO_PARSER_PATH, "src", "classifier"),
    os.path.join(TURBO_PARSER_PATH, "src", "sequence"),
    os.path.join(TURBO_PARSER_PATH, "src", "tagger"),
    os.path.join(TURBO_PARSER_PATH, "src", "parser"),
    os.path.join(TURBO_PARSER_PATH, "src", "entity_recognizer"),
    os.path.join(TURBO_PARSER_PATH, "src", "semantic_parser"),
    os.path.join(TURBO_PARSER_PATH, "src", "coreference_resolver"),
    os.path.join(TURBO_PARSER_PATH, "src", "morphological_tagger"),
    os.path.join(TURBO_PARSER_PATH, "deps", "AD3-2.0.2", "ad3"),
    os.path.join(TURBO_PARSER_PATH, "deps", "AD3-2.0.2"),
    os.path.join(TURBO_PARSER_PATH, "deps", "glog-0.3.2", "src", "windows"),
    os.path.join(TURBO_PARSER_PATH, "deps", "gflags-2.0", "src", "windows"),
    os.path.join(TURBO_PARSER_PATH, "deps", "eigen-eigen-c58038c56923"),
    os.path.join(TURBO_PARSER_PATH, "deps", "googletest", "src"),
    os.path.join(TURBO_TEXT_ANALYSIS_PATH, "TurboTextAnalysis"),
    os.path.join(TURBO_TEXT_ANALYSIS_PATH, "TurboTextAnalysisPython"),
    os.path.join(TURBO_TEXT_ANALYSIS_PATH, "Deps")],
    library_dirs=
    [os.path.join(TURBO_TEXT_ANALYSIS_PATH, "x64", "Release"),
    os.path.join(TURBO_PARSER_PATH, "vsprojects", "x64", "Release"),    
    os.path.join(TURBO_PARSER_PATH, "deps", "glog-0.3.2", "x64", "Release"),           
    os.path.join(TURBO_PARSER_PATH, "deps", "gflags-2.0", "x64", "Release"),            
    os.path.join(TURBO_PARSER_PATH, "deps", "AD3-2.0.2", "vsprojects", "x64", "Release"),
    os.path.join(TURBO_PARSER_PATH, "deps", "googletest", "msvc", "x64", "Release"),
    os.path.join(TURBO_TEXT_ANALYSIS_PATH, "Deps", "libconfig-1.4.9", "x64", "Release")],     
    extra_objects=
    ["TurboTextAnalysis.lib",
    "libturboparser.lib", 
    "AD3_140mdx64.lib",                 
    "libgflags_140mdx64.lib",           
    "libglog_static_140mdx64.lib",      
    "gtest-md_140mdx64.lib",
    "libconfig++.lib"]
    )]          
    setup(cmdclass={'build_ext': build_ext},
    ext_modules = cythonize(ext_modules, gdb_debug=True)
    )
else:
    TURBO_PARSER_PATH = "/home/user/Projects/TurboParser"
    TURBO_TEXT_ANALYSIS_PATH = "/home/user/Projects/TurboTextAnalysis"

    ext_modules=[Extension("turboparser", 
    ["turboparser.pyx", "CppToPyTurboSink.cpp"],
    language="c++",
    extra_compile_args=["-std=c++11", "-std=gnu++11", "-O2"],
    include_dirs=
    [".",
    "..", 
    os.path.join(TURBO_PARSER_PATH, "libturboparser"), 
    os.path.join(TURBO_PARSER_PATH, "src", "util"),
    os.path.join(TURBO_PARSER_PATH, "src", "classifier"),
    os.path.join(TURBO_PARSER_PATH, "src", "sequence"),
    os.path.join(TURBO_PARSER_PATH, "src", "tagger"),
    os.path.join(TURBO_PARSER_PATH, "src", "entity_recognizer"),
    os.path.join(TURBO_PARSER_PATH, "src", "parser"),
    os.path.join(TURBO_PARSER_PATH, "src", "semantic_parser"),
    os.path.join(TURBO_PARSER_PATH, "src", "coreference_resolver"),
    os.path.join(TURBO_PARSER_PATH, "src", "morphological_tagger"),
    os.path.join(TURBO_PARSER_PATH, "deps", "local", "include"),
    os.path.join(TURBO_TEXT_ANALYSIS_PATH, "TurboTextAnalysis"),
    os.path.join(TURBO_TEXT_ANALYSIS_PATH, "TurboTextAnalysisPython"),
    os.path.join(TURBO_TEXT_ANALYSIS_PATH, "Deps")],
    library_dirs=[os.path.join(TURBO_TEXT_ANALYSIS_PATH, "TurboTextAnalysis/"),
    os.path.join(TURBO_PARSER_PATH, "libturboparser/"), 
    os.path.join(TURBO_PARSER_PATH, "deps", "local", "lib/"),
    os.path.join(TURBO_TEXT_ANALYSIS_PATH, "Deps", "libconfig-1.4.9", "lib", ".libs/")],
    libraries=["turbotextanalysis",
    "turboparser",
    "gflags", 
    "glog", 
    "ad3",
    "config++"])]
    setup(cmdclass={'build_ext': build_ext},
    ext_modules = ext_modules, gdb_debug=True)
