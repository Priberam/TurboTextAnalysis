//This module is part of “Priberam’s TurboTextAnalysis”, a TurboParser's wrapper for easy text analysis, allowing it to be readily used in production systems.
//Copyright 2018 by PRIBERAM INFORMÁTICA, S.A. - www.priberam.com
//Usage subject to The terms & Conditions of the "Priberam TurboTextAnalysis OS Software License" available at https://www.priberam.pt/docs/Priberam_TurboTextAnalysis_OS_Software_License.pdf

/**
* @file ps_textanalysistemplate.h
* @brief Template for the definition of language models for text analysis.
* @author  dan
* @date  08/02/2017
*/

#ifndef COREINDEXLIB_PS_TEXTANALYSISTEMPLATE_H
#define COREINDEXLIB_PS_TEXTANALYSISTEMPLATE_H
typedef enum TokenKind {
  unknown = 0,
  sep_space = (1 << 1),
  sep_sentence = (1 << 2),
  sep_paragraph = (1 << 3),
  sep_quote = (1 << 4),
  sep_dash = (1 << 5),
  sep_punct = (1 << 6),
  number = (1 << 7),
  alphanum = (1 << 8),
  alpha = (1 << 9),
  multi = (1 << 10)
}
TokenKind;

class CPBSSink {
public:
  CPBSSink() {};
  virtual ~CPBSSink() {};

  virtual int PutToken(const char * word,
                       int len,
                       int start_pos,
                       enum TokenKind kind) = 0;
  virtual int PutFeature(const char * feature,
                         const char * value) = 0;
  virtual int EndSentence() = 0;
  virtual int PutDocumentFeature(const char * feature,
                                 const char * value) = 0;
};

class CPBSTextAnalysis {
public:
  CPBSTextAnalysis() {};
  virtual ~CPBSTextAnalysis() {};

  virtual int LoadLanguage(const char * language,
                           const char * path) = 0;

  virtual int Analyse(const char * language,
                      const char * text,
                      CPBSSink * pbssink) = 0;

  virtual int DefineProperty(const char *  module_property,
                             const char *  value) = 0;

  virtual int SubmitCandidateAnswerPassage(const char * language,
                                           const char * candidate_answer_passage,
                                           int document_id,
                                           int passage_offset) = 0;

  virtual int RetrieveFinalAnswer(const char * language,
                                  const char ** answer) = 0;

  virtual int ReturnResults(const char *  language,
                            int max_num_results,
                            CPBSSink* pbssink) = 0;

  virtual int FinishAnalysis(const char *  language) = 0;
};

#endif
