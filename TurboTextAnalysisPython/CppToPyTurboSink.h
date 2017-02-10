#ifndef CTOPYTURBOSINK_H_
#define CTOPYTURBOSINK_H_

#include <Python.h>
#include <object.h>
#include "TurboTextAnalysis.h"
#include "TurboTokenAnalysis.h"

class CppToPyTurboSink : public CPBSSink {
public:
  CppToPyTurboSink(PyObject * pysink);
  virtual ~CppToPyTurboSink();

  int PutToken(const char * word,
               int len,
               int start_pos,
               TokenKind kind);
  int PutFeature(const char * feature,
                 const char * value);
  int EndSentence();
  int PutDocumentFeature(const char * feature,
                         const char * value);

  PyObject *m_pysink;
};

#endif /* CTOPYTURBOSINK_H_ */
