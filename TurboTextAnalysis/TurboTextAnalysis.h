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
#include "ps_textanalysistemplate.h"
#include <string>
#include <vector>

class AnalyseOptions {
public:
  bool use_tagger{ false };
  bool use_parser{ false };
  bool use_morphological_tagger{ false };
  bool use_entity_recognizer{ false };
  bool use_semantic_parser{ false };
  bool use_coreference_resolver{ false };
};

class LoadOptions {
public:
  bool load_tagger{ false };
  bool load_parser{ false };
  bool load_morphological_tagger{ false };
  bool load_entity_recognizer{ false };
  bool load_semantic_parser{ false };
  bool load_coreference_resolver{ false };
};

class CTurboTextAnalysis {
public:

  int Analyse(const std::string &language,
              const std::string &text,
              CPBSSink* pbssink,
              AnalyseOptions * options = nullptr);
  int Analyse(const std::string &language,
              const std::vector<std::vector<std::string>> &words,
              const std::vector<std::vector<std::string>> &original_sentences_words,
              const std::vector<int> &sentence_start_positions,
              const std::vector<std::vector<int> > &sentences_start_positions,
              const std::vector<std::vector<int> > &sentences_end_positions,
              CPBSSink* pbssink,
              AnalyseOptions * options = nullptr);
  int LoadLanguage(const std::string &language,
                   const std::string &path,
                   LoadOptions * options = nullptr);

protected:
  // Object for token-level analysis.
  CTurboTokenAnalysis m_token_analysis;
};

#endif /* TURBOTEXTANALYSIS_H_ */
