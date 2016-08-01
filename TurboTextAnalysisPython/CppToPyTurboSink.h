#ifndef CTOPYTURBOSINK_H_
#define CTOPYTURBOSINK_H_

#include <Python.h>
#include <object.h>
#include "TurboTextAnalysis.h"
#include "TurboTokenAnalysis.h"

class CppToPyTurboSink : public CPBSSink {
public:
  CppToPyTurboSink(PyObject * pysink) ;
  virtual ~CppToPyTurboSink();

  int PutToken(const std::string &word, int len, int start_pos, internal_TokenKind kind);
  int PutFeature(const std::string &feature, const std::string &value);
  int EndSentence();
  int PutDocumentFeature(const std::string &feature, const std::string &value);

  PyObject *m_pysink;
};

#endif /* CTOPYTURBOSINK_H_ */



