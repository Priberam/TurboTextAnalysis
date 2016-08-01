# distutils: language = c++
# distutils: sources = CppToPyTurboSink.cpp
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool
from cpython.ref cimport PyObject

import sys
import pdb

# Get the classes from the c++ headers.

cdef extern from "../TurboTextAnalysis/TurboTokenAnalysis.h":
    cpdef cppclass internal_TokenKind:
        pass
 
cdef extern from "../TurboTextAnalysis/TurboTokenAnalysis.h" namespace "internal_TokenKind":
    cdef internal_TokenKind unknown
    cdef internal_TokenKind sep_space
    cdef internal_TokenKind sep_sentence
    cdef internal_TokenKind sep_paragraph
    cdef internal_TokenKind sep_quote
    cdef internal_TokenKind sep_dash
    cdef internal_TokenKind sep_punct
    cdef internal_TokenKind number
    cdef internal_TokenKind alphanum
    cdef internal_TokenKind alpha

cdef extern from "../TurboTextAnalysis/TurboTextAnalysis.h":
    cdef cppclass CPBSSink:
        CPBSSink() except +
        int PutToken(string word, 
                    int len,
                    int start_pos,
                    internal_TokenKind kind)
        int PutFeature(string feature, string value) 
        int EndSentence()
        int PutDocumentFeature(string feature, string value)

    cdef cppclass CTurboTextAnalysis:
        int Analyse(string language, string text, CPBSSink* pbssink)
        int Analyse(string language,
                    vector[vector[string]] sentences_words,
                    vector[int] sentence_start_positions,
                    vector[vector[int]] sentences_start_positions,
                    vector[vector[int]] sentences_end_positions,
                    CPBSSink* pbssink) 
        int LoadLanguage(string language, string path) 

cdef extern from "CppToPyTurboSink.h":
    cdef cppclass CppToPyTurboSink(CPBSSink):
        CppToPyTurboSink(PyObject * pysink)
        int PutToken(string word, 
                    int len,
                    int start_pos,
                    internal_TokenKind kind)
        int PutFeature(string feature, string value) 
        int EndSentence()
        int PutDocumentFeature(string feature, string value)

# Wrap them into python extension types.

cdef class Pyinternal_TokenKind:
  cdef internal_TokenKind thisobj
  def __cinit__(self, int val):
    self.thisobj = <internal_TokenKind> val

  def get_internal_TokenKind_type(self):
    cdef c = {<int>unknown : "unknown", 
    <int> sep_space : "sep_space", 
    <int> sep_sentence : "sep_sentence", 
    <int> sep_paragraph : "sep_paragraph", 
    <int> sep_quote : "sep_quote", 
    <int> sep_dash : "sep_dash", 
    <int> sep_punct : "sep_punct", 
    <int> number : "number", 
    <int> alphanum : "alphanum", 
    <int> alpha : "alpha"	}
    return c[<int>self.thisobj]

cdef class PyCPBSSink:	
    cdef CPBSSink* c_sink
    def __cinit__(self, allocate=True):
        pass

    def __dealloc__(self):
        pass

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

    def load_language(self, str language, str path):
        retval = self.c_turbotextanalysis.LoadLanguage(language.encode(encoding='UTF-8'), 
                                                        path.encode(encoding='UTF-8'))
        return retval

    def analyse(self, str language, str text, PyCPBSSink pycbssink):		
        retval = self.c_turbotextanalysis.Analyse(language.encode(encoding='UTF-8'), 
                                                text.encode(encoding='UTF-8'), 
                                                pycbssink.c_sink)	
        return retval
    def analyse_with_tokens(self, 
                str language, 
                sentences_words,
                vector[int] sentence_start_positions,
                vector[vector[int]] sentences_start_positions,
                vector[vector[int]] sentences_end_positions, 
                PyCPBSSink pycbssink):		
        retval = self.c_turbotextanalysis.Analyse(language.encode(encoding='UTF-8'), 
        [[sentence_word.encode(encoding='UTF-8') for sentence_word in sentence_words] for sentence_words in sentences_words],
        sentence_start_positions,
        sentences_start_positions,
        sentences_end_positions,
        pycbssink.c_sink)	
        return retval

