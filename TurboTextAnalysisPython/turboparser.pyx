# distutils: language = c++
# distutils: sources = CppToPyTurboSink.cpp
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool
from cpython.ref cimport PyObject

import sys
import pdb

# Get the classes from the c++ headers.
 
cdef extern from "../TurboTextAnalysis/TurboTextAnalysis.h":
    cdef cppclass CPBSSink:
        CPBSSink() except +
        int PutToken(string word, 
                    int len,
                    int start_pos,
                    int kind)
        int PutFeature(string feature, 
                       string value) 
        int EndSentence()
        int PutDocumentFeature(string feature,
                               string value)

    cdef cppclass AnalyseOptions:
        bool use_tagger
        bool use_parser
        bool use_morphological_tagger
        bool use_entity_recognizer
        bool use_semantic_parser
        bool use_coreference_resolver

    cdef cppclass LoadOptions:
        bool load_tagger
        bool load_parser
        bool load_morphological_tagger
        bool load_entity_recognizer
        bool load_semantic_parser
        bool load_coreference_resolver

    cdef cppclass CTurboTextAnalysis: 
        int LoadLanguage(string language, 
                         string path) 
        int LoadLanguage(string language, 
                         string path,
                         LoadOptions*options)        
        int Analyse(string language, 
                    string text, 
                    CPBSSink* pbssink)
        int Analyse(string language, 
                    string text, 
                    CPBSSink* pbssink, 
                    AnalyseOptions*options)
        int Analyse(string language,
                    vector[vector[string]] sentences_words,
                    vector[vector[string]] original_sentences_words,
                    vector[int] sentence_start_positions,
                    vector[vector[int]] sentences_start_positions,
                    vector[vector[int]] sentences_end_positions,
                    CPBSSink* pbssink)
        int Analyse(string language,
                    vector[vector[string]] sentences_words,
                    vector[vector[string]] original_sentences_words,
                    vector[int] sentence_start_positions,
                    vector[vector[int]] sentences_start_positions,
                    vector[vector[int]] sentences_end_positions,
                    CPBSSink* pbssink, 
                    AnalyseOptions*options)

cdef extern from "CppToPyTurboSink.h":
    cdef cppclass CppToPyTurboSink(CPBSSink):
        CppToPyTurboSink(PyObject * pysink)
        int PutToken(string word, 
                    int len,
                    int start_pos,
                    int kind)
        int PutFeature(string feature, string value) 
        int EndSentence()
        int PutDocumentFeature(string feature, string value)

# Wrap them into python extension types.

cdef class PyAnalyseOptions:
    cdef public bool use_tagger
    cdef public bool use_parser
    cdef public bool use_morphological_tagger
    cdef public bool use_entity_recognizer
    cdef public bool use_semantic_parser
    cdef public bool use_coreference_resolver
    def __cinit__(self):
        self.use_tagger = False
        self.use_parser = False
        self.use_morphological_tagger = False
        self.use_entity_recognizer = False
        self.use_semantic_parser = False
        self.use_coreference_resolver = False

    def __dealloc__(self):
        pass

cdef class PyLoadOptions:
    cdef public bool load_tagger
    cdef public bool load_parser
    cdef public bool load_morphological_tagger
    cdef public bool load_entity_recognizer
    cdef public bool load_semantic_parser
    cdef public bool load_coreference_resolver
    def __cinit__(self):
        self.load_tagger = False
        self.load_parser = False
        self.load_morphological_tagger = False
        self.load_entity_recognizer = False
        self.load_semantic_parser = False
        self.load_coreference_resolver = False

    def __dealloc__(self):
        pass

cdef class PyCPBSSink:	
    cdef CPBSSink* c_sink
    def __cinit__(self, allocate=True):
        pass

    def __dealloc__(self):
        pass

# decode(encoding='UTF-8') is implicit with:
#   https://docs.python.org/3/c-api/arg.html#c.Py_BuildValue
#   s (str or None) [char *]
#     Convert a null-terminated C string to a Python str object using 'utf-8' encoding. If the C string pointer is NULL, None is used.

cdef class PyCppToPyTurboSink(PyCPBSSink):	
    cdef CppToPyTurboSink* c_derivedsink
    cdef bool allocate

    cdef str last_word
    cdef int current_index
    cdef object default_py_token_data
    cdef object default_py_document_data
    
    def __cinit__(self, allocate=True):
        self.allocate = allocate
        if allocate:
            self.c_derivedsink = new CppToPyTurboSink(<PyObject*>self)
            self.c_sink = self.c_derivedsink
        self.last_word = 'default_word'
        self.current_index = 0
        self.default_py_token_data=[]
        self.default_py_document_data={}

    def __dealloc__(self):
        if self.allocate:
          del self.c_derivedsink

    def put_token(self,
                    str word, 
                    int length,
                    int start_pos,
                    int kind):
        self.last_word = word
        self.current_index = len(self.default_py_token_data)
        self.default_py_token_data.append({})
        self.default_py_token_data[self.current_index]["word"] = word
        self.default_py_token_data[self.current_index]["len"] = length
        self.default_py_token_data[self.current_index]["start_pos"] = start_pos
        self.default_py_token_data[self.current_index]["kind"] = kind
        self.default_py_token_data[self.current_index]["features"] =  {}
    
    def put_feature(self, str feature, str value):
        self.default_py_token_data[self.current_index]["features"][feature] = value

    def end_sentence(self):
        pass

    def put_document_feature(self, str feature, str value):
        self.default_py_document_data[feature] = value

    def get_tokens_info(self):
        return self.default_py_token_data
        
    def get_document_info(self):
        return self.default_py_document_data

cdef class PyCTurboTextAnalysis:
    cdef CTurboTextAnalysis* c_turbotextanalysis
    cdef bool allocate
    def __cinit__(self, allocate=True):
        self.allocate = allocate
        if allocate:
            self.c_turbotextanalysis = new CTurboTextAnalysis()

    def __dealloc__(self):
        if self.allocate:
          del self.c_turbotextanalysis

    def load_language(self,
                      str language, 
                      str path,
                      PyLoadOptions pyloadoptions = None):
        cdef LoadOptions c_options
        if pyloadoptions is None:	
            retval = self.c_turbotextanalysis.LoadLanguage(language.encode(encoding='UTF-8'), 
                                                           path.encode(encoding='UTF-8'))
        else:
            c_options.load_tagger = pyloadoptions.load_tagger
            c_options.load_parser = pyloadoptions.load_parser
            c_options.load_morphological_tagger = pyloadoptions.load_morphological_tagger
            c_options.load_entity_recognizer = pyloadoptions.load_entity_recognizer
            c_options.load_semantic_parser = pyloadoptions.load_semantic_parser
            c_options.load_coreference_resolver = pyloadoptions.load_coreference_resolver
            retval = self.c_turbotextanalysis.LoadLanguage(language.encode(encoding='UTF-8'), 
                                                           path.encode(encoding='UTF-8'),
                                                           &c_options)
        return retval

    def analyse(self, 
                str language, 
                str text, 
                PyCPBSSink pycbssink,
                PyAnalyseOptions pyanalyseoptions = None):
        cdef AnalyseOptions c_options
        if pyanalyseoptions is None:
            retval = self.c_turbotextanalysis.Analyse(language.encode(encoding='UTF-8'),
                                                      text.encode(encoding='UTF-8'), 
                                                      pycbssink.c_sink)
        else:
            c_options.use_tagger = pyanalyseoptions.use_tagger
            c_options.use_parser = pyanalyseoptions.use_parser
            c_options.use_morphological_tagger = pyanalyseoptions.use_morphological_tagger
            c_options.use_entity_recognizer	= pyanalyseoptions.use_entity_recognizer
            c_options.use_semantic_parser = pyanalyseoptions.use_semantic_parser
            c_options.use_coreference_resolver = pyanalyseoptions.use_coreference_resolver
            retval = self.c_turbotextanalysis.Analyse(language.encode(encoding='UTF-8'), 
                                                      text.encode(encoding='UTF-8'), 
                                                      pycbssink.c_sink,
                                                      &c_options)
        return retval
    def analyse_with_tokens(self, 
                            str language, 
                            sentences_words,
                            original_sentences_words,
                            vector[int] sentence_start_positions,
                            vector[vector[int]] sentences_start_positions,
                            vector[vector[int]] sentences_end_positions, 
                            PyCPBSSink pycbssink,
                            PyAnalyseOptions pyanalyseoptions = None):
        cdef AnalyseOptions c_options
        if pyanalyseoptions is None:	
            retval = self.c_turbotextanalysis.Analyse(language.encode(encoding='UTF-8'), 
                                                      [[sentence_word.encode(encoding='UTF-8') for sentence_word in sentence_words] for sentence_words in sentences_words],
                                                      [[original_sentence_word.encode(encoding='UTF-8') for original_sentence_word in original_sentence_words] for original_sentence_words in original_sentences_words],
                                                      sentence_start_positions,
                                                      sentences_start_positions,
                                                      sentences_end_positions,
                                                      pycbssink.c_sink)
        else:
            c_options.use_tagger = pyanalyseoptions.use_tagger
            c_options.use_parser = pyanalyseoptions.use_parser
            c_options.use_morphological_tagger = pyanalyseoptions.use_morphological_tagger
            c_options.use_entity_recognizer	= pyanalyseoptions.use_entity_recognizer
            c_options.use_semantic_parser = pyanalyseoptions.use_semantic_parser
            c_options.use_coreference_resolver = pyanalyseoptions.use_coreference_resolver
            retval = self.c_turbotextanalysis.Analyse(language.encode(encoding='UTF-8'),
                                                      [[sentence_word.encode(encoding='UTF-8') for sentence_word in sentence_words] for sentence_words in sentences_words],
                                                      [[original_sentence_word.encode(encoding='UTF-8') for original_sentence_word in original_sentence_words] for original_sentence_words in original_sentences_words],
                                                      sentence_start_positions,
                                                      sentences_start_positions,
                                                      sentences_end_positions,
                                                      pycbssink.c_sink,
                                                      &c_options)
        return retval

