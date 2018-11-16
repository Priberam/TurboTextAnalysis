//This module is part of “Priberam’s TurboTextAnalysis”, a TurboParser's wrapper for easy text analysis, allowing it to be readily used in production systems.
//Copyright 2018 by PRIBERAM INFORMÁTICA, S.A. - www.priberam.com
//Usage subject to The terms & Conditions of the "Priberam TurboTextAnalysis OS Software License" available at https://www.priberam.pt/docs/Priberam_TurboTextAnalysis_OS_Software_License.pdf

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
