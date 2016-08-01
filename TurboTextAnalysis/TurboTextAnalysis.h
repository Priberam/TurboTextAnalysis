/*
* TurboTextAnalysis.h
*
*  Created on: Jan 20, 2015
*      Author: atm
*  Updated on: May 20, 2016
*      Author: dan
*/

#ifndef TURBOTEXTANALYSIS_H_
#define TURBOTEXTANALYSIS_H_

#include "TurboTokenAnalysis.h"
#include <string>
#include <vector>

class CPBSSink
{
public:
  CPBSSink() {};
  virtual ~CPBSSink() {};

  virtual int PutToken(const std::string &word,
                       int len, 
                       int start_pos, 
                       internal_TokenKind kind) = 0;
  virtual int PutFeature(const std::string &feature, 
                         const std::string &value) = 0;
  virtual int EndSentence() = 0;
  virtual int PutDocumentFeature(const std::string &feature, 
                                 const std::string &value) = 0;
};


class CTurboTextAnalysis
{
public:

  int Analyse(const std::string &language, 
              const std::string &text, 
              CPBSSink* pbssink);
  int Analyse(const std::string &language, 
              const std::vector<std::vector<std::string>> &words,
              const std::vector<int> &sentence_start_positions,
              const std::vector<std::vector<int> > &sentences_start_positions,
              const std::vector<std::vector<int> > &sentences_end_positions,
              CPBSSink* pbssink);
  int LoadLanguage(const std::string &language, const std::string &path);

protected:
  // Object for token-level analysis.
  CTurboTokenAnalysis m_token_analysis;
};


#endif /* TURBOTEXTANALYSIS_H_ */