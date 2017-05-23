/*
 * TurboWorkers.h
 *
 *  Created on: Jan 20, 2015
 *      Author: atm
 */

#ifndef TURBOWORKERS_H_
#define TURBOWORKERS_H_

#include "Tokenizer.h"
#include "TurboLemmatizer.h"
#include <string>
#include <map>
#include<mutex>

class CTurboWorkers {
public:
  CTurboWorkers(
    Tokenizer *tokenizer,
    bool tokenizer_outputs_unicode_glyph_aware_offsets,
    TurboLemmatizer *lemmatizer,
    TurboParserInterface::TurboTaggerWorker *tagger,
    TurboParserInterface::TurboMorphologicalTaggerWorker *morphological_tagger,
    TurboParserInterface::TurboEntityRecognizerWorker *entity_recognizer,
    TurboParserInterface::TurboParserWorker *parser,
    TurboParserInterface::TurboSemanticParserWorker *semantic_parser,
    TurboParserInterface::TurboCoreferenceResolverWorker * coreference_resolver) {
    m_tokenizer = tokenizer;
    m_tokenizer_outputs_unicode_glyph_aware_offsets = 
      tokenizer_outputs_unicode_glyph_aware_offsets;
    m_lemmatizer = lemmatizer;
    m_tagger = tagger;
    m_morphological_tagger = morphological_tagger;
    m_entity_recognizer = entity_recognizer;
    m_parser = parser;
    m_semantic_parser = semantic_parser;
    m_coreference_resolver = coreference_resolver;
  }
  virtual ~CTurboWorkers() {}

  Tokenizer *GetTokenizer() {
    return m_tokenizer;
  }
  bool tokenizer_outputs_unicode_glyph_aware_offsets() {
    return m_tokenizer_outputs_unicode_glyph_aware_offsets;
  }
  TurboLemmatizer *GetLemmatizer() {
    return m_lemmatizer;
  }
  TurboParserInterface::TurboTaggerWorker *GetTagger() {
    return m_tagger;
  }
  TurboParserInterface::TurboMorphologicalTaggerWorker *GetMorphologicalTagger() {
    return m_morphological_tagger;
  }
  TurboParserInterface::TurboEntityRecognizerWorker *GetEntityRecognizer() {
    return m_entity_recognizer;
  }
  TurboParserInterface::TurboParserWorker *GetParser() {
    return m_parser;
  }
  TurboParserInterface::TurboSemanticParserWorker *GetSemanticParser() {
    return m_semantic_parser;
  }
  TurboParserInterface::TurboCoreferenceResolverWorker *GetCoreferenceResolver() {
    return m_coreference_resolver;
  }

protected:
  Tokenizer *m_tokenizer;
  bool m_tokenizer_outputs_unicode_glyph_aware_offsets;
  TurboLemmatizer *m_lemmatizer;
  TurboParserInterface::TurboTaggerWorker *m_tagger;
  TurboParserInterface::TurboMorphologicalTaggerWorker *m_morphological_tagger;
  TurboParserInterface::TurboEntityRecognizerWorker *m_entity_recognizer;
  TurboParserInterface::TurboParserWorker *m_parser;
  TurboParserInterface::TurboSemanticParserWorker *m_semantic_parser;
  TurboParserInterface::TurboCoreferenceResolverWorker *m_coreference_resolver;
};

class CTurboWorkerMap : public std::map<std::string, CTurboWorkers*> {
public:
  CTurboWorkerMap() {}
  virtual ~CTurboWorkerMap() {
    for (CTurboWorkerMap::iterator iter = begin(); iter != end(); ++iter) {
      // The tokenizer is currently owned by the worker map.
      delete iter->second->GetTokenizer();
      // The lemmatizer is currently owned by the worker map.
      delete iter->second->GetLemmatizer();
      delete iter->second;
    }
    clear();
  }

public:
  void AddLanguageWorkers(const std::string &language,
                          Tokenizer *tokenizer,
                          bool tokenizer_outputs_unicode_glyph_aware_offsets,
                          TurboLemmatizer *lemmatizer,
                          TurboParserInterface::TurboTaggerWorker *tagger,
                          TurboParserInterface::TurboMorphologicalTaggerWorker *morphological_tagger,
                          TurboParserInterface::TurboEntityRecognizerWorker *entity_recognizer,
                          TurboParserInterface::TurboParserWorker *parser,
                          TurboParserInterface::TurboSemanticParserWorker *semantic_parser,
                          TurboParserInterface::TurboCoreferenceResolverWorker * coreference_resolver) {
    insert(std::pair<std::string, CTurboWorkers*>
      (language,
       new CTurboWorkers(tokenizer,
                         tokenizer_outputs_unicode_glyph_aware_offsets,
                         lemmatizer,
                         tagger,
                         morphological_tagger,
                         entity_recognizer,
                         parser,
                         semantic_parser,
                         coreference_resolver)));
  }

  bool FindLanguageWorkers(const std::string &language,
                           CTurboWorkers* turbo_workers) {
    auto it = find(language);
    if (it != end()) {
      turbo_workers = it->second;
      return true;
    } else {
      return false;
    }
  }
public:
  //To lock LoadLanguage access
  std::mutex m_lock;
};

#endif /* TURBOWORKERS_H_ */
