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


//Variables to select modes of execution
static bool default_use_tagger = true;
static bool default_use_parser = true;
static bool default_use_morphological_tagger = true;
static bool default_use_entity_recognizer = true;
static bool default_use_semantic_parser = true;
static bool default_use_coreference_resolver = true;
static bool default_emit_entity_strings = true;

class AnalyseOptions {
public:
  AnalyseOptions() {
    use_tagger = default_use_tagger;
    use_parser = default_use_parser;
    use_morphological_tagger = default_use_morphological_tagger;
    use_entity_recognizer = default_use_entity_recognizer;
    use_semantic_parser = default_use_semantic_parser;
    use_coreference_resolver = default_use_coreference_resolver;
    emit_entity_strings = default_emit_entity_strings;
  }
  bool use_tagger{ false };
  bool use_parser{ false };
  bool use_morphological_tagger{ false };
  bool use_entity_recognizer{ false };
  bool use_semantic_parser{ false };
  bool use_coreference_resolver{ false };
  bool emit_entity_strings{ false };
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
