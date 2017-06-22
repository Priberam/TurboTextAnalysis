#include "CppToPyTurboSink.h"
#include <iostream>
#include <string>

//decode(encoding = 'UTF-8') is implicit with :
//  https ://docs.python.org/3/c-api/arg.html#c.Py_BuildValue
//  s(str or None)[char *]
//    Convert a null - terminated C string to a Python str object using 'utf-8' encoding.If the C string pointer is NULL, None is used.

CppToPyTurboSink::CppToPyTurboSink(PyObject * pysink) : m_pysink(pysink) {
  Py_XINCREF(this->m_pysink);
  m_pysink = pysink;
}
CppToPyTurboSink::~CppToPyTurboSink() {
  Py_XDECREF(this->m_pysink);
};

int CppToPyTurboSink::PutToken(const char * word,
                               int len,
                               int start_pos,
                               TokenKind kind) {
  PyObject* myResult = PyObject_CallMethod(m_pysink,
                                           (char*)"put_token",
                                           (char*)"(siii)",
                                           (char*)word,
                                           len,
                                           start_pos,
                                           (int)kind);
  return 0;
};

int CppToPyTurboSink::PutFeature(const char * feature,
                                 const char * value) {
  PyObject* myResult = PyObject_CallMethod(m_pysink,
                                           (char*)"put_feature",
                                           (char*)"(ss)",
                                           (char*)feature,
                                           (char*)value);

  return 0;
};

int CppToPyTurboSink::EndSentence() {
  PyObject* myResult = PyObject_CallMethod(m_pysink,
                                           (char*)"end_sentence",
                                           NULL);
  return 0;
};

int CppToPyTurboSink::PutDocumentFeature(const char * feature,
                                         const char * value) {
  PyObject* myResult = PyObject_CallMethod(m_pysink,
                                           (char*)"put_document_feature",
                                           (char*)"(ss)",
                                           (char*)feature,
                                           (char*)value);
  return 0;
};
