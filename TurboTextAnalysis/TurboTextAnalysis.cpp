// TurboTextAnalysis.cpp : Implementation of DLL Exports.

#include "TurboTextAnalysis.h"
#include "TurboParserInterface.h"
#include "TurboWorkers.h"
#include "TurboTokenizer.h"
#include "ICUTokenizer.h"
#include "glogconfig.h"
#include "encoding.h"

namespace {
template <typename TType>
static bool ReadSettingSafe(libconfig::Setting &setting,
                            const char *name,
                            TType &target) throw() {
  if (!setting.exists(name) || !setting.lookupValue(name, target)) {
    std::cerr << "Config file error. Missing property '"
      << name << "' in " << setting.getPath() << std::endl;
    return false;
  }
  return true;
};
template <typename TType>
inline bool ReadorDefaultLibConfigSetting(const libconfig::Setting &setting,
                                          const char *name,
                                          const TType &default_to,
                                          TType &target) throw() {
  if (!setting.exists(name) || !setting.lookupValue(name, target))
    target = default_to;
  return true;
};

void SplitString(const std::string &text,
                 char sep,
                 std::vector<std::string> * output) {
  std::vector<std::string> tokens;
  std::size_t start = 0, end = 0;
  while ((end = text.find(sep, start)) != std::string::npos) {
    std::string temp = text.substr(start, end - start);
    if (temp != "") output->push_back(temp);
    start = end + 1;
  };
  std::string temp = text.substr(start);
  if (temp != "") output->push_back(temp);
};
}

// Map of workers per language.
CTurboWorkerMap _worker_map;
TurboParserInterface::TurboParserInterface *_interface = nullptr;

//Variables to select modes of execution
static bool default_use_tagger = true;
static bool default_use_parser = true;
static bool default_use_morphological_tagger = true;
static bool default_use_entity_recognizer = true;
static bool default_use_semantic_parser = true;
static bool default_use_coreference_resolver = true;

int CTurboTextAnalysis::Analyse(const std::string &language,
                                const std::string &text,
                                CPBSSink* pbssink,
                                AnalyseOptions * options) {
  CTurboWorkerMap::iterator iter = _worker_map.find(language);
  if (iter == _worker_map.end())
    return -1;

  CTurboWorkers *workers = iter->second;

  // For now, assume one sentence per line, words separated by whitespaces.

  // Sentence splitter.
  std::vector<std::string> sentences;
  std::vector<int> sentence_start_positions;
  std::vector<int> sentence_end_positions;

  workers->GetTokenizer()->SplitSentences(text,
                                          &sentences,
                                          &sentence_start_positions,
                                          &sentence_end_positions);

#if 0
  StringSplit(text, "\n", &sentences);

  int num_processed_characters = 0;
  for (int j = 0; j < sentences.size(); ++j) {
    // Compute sentence positions.
    const std::string &sentence = sentences[j];
    int length = sentence.length();
    sentence_start_positions.push_back(num_processed_characters);
    sentence_end_positions.push_back(num_processed_characters + length);
    num_processed_characters += length;
    ++num_processed_characters; // The sentence separator.
  }
  --num_processed_characters; // Remove the extra sentence separator.
#endif

  std::vector<
    std::vector<std::string>
  > sentences_words(sentences.size());
  std::vector<
    std::vector<std::string>
  > original_sentences_words(sentences.size());
  std::vector<
    std::vector<int>
  > sentences_start_positions(sentences.size());
  std::vector<
    std::vector<int>
  > sentences_end_positions(sentences.size());

  for (int j = 0; j < sentences.size(); ++j) {
    const std::string &sentence = sentences[j];

    // Word tokenizer.
    std::vector<std::string> & words = sentences_words[j];
    std::vector<int> & start_positions = sentences_start_positions[j];
    std::vector<int> & end_positions = sentences_end_positions[j];

    workers->GetTokenizer()->TokenizeWords(sentence,
                                           &words,
                                           &start_positions,
                                           &end_positions);

#if 0
    for (int i = 0; i < words.size(); ++i)
      std::cout << words[i] << " " << std::endl;
#endif
#if 0
    for (int i = 0; i < words.size(); ++i) {
      std::cout << "["
        << sentence.substr(start_positions[i],
                           end_positions[i] - start_positions[i])
        << "] ";
    }
    std::cout << std::endl;
#endif

#if 0
    StringSplit(sentence, " ", &words);
    int offset = 0;
    for (int i = 0; i < words.size(); i++) {
      // Compute word positions (relative to the sentence).
      std::string word = words[i];
      int length = word.length();
      start_positions.push_back(offset);
      end_positions.push_back(offset + length);
      offset += length;
      ++offset; // The word separator.
    }
    --offset; //Remove the extra word separator.
#endif
  }

  if (sentences_words.size() != sentences.size()) {
    LOG(ERROR) << "Mismatch in size of vectors";
    return -1;
  }

  if (!workers->tokenizer_outputs_unicode_glyph_aware_offsets()) {
    //Change start and end positions from byte-wise to glyph-wise values
    //The white spaces between tokens (in case of existence), are assumed
    //to be 1 byte each (same size either byte-wise to glyph-wise).
    //Current white spaces are " " "\t" "\r" "\n" "\f" "\v", as described in
    //variable kTurboWhitespaces in TurboTokenizer.cpp
    std::vector<int> glyphaware_sentence_start_positions;
    std::vector<
      std::vector<int> > glyphaware_sentences_start_positions(sentences.size());
    std::vector<
      std::vector<int> > glyphaware_sentences_end_positions(sentences.size());

    int glyphaware_sentence_curr_pos = 0;
    for (int i = 0; i < sentences.size(); ++i) {
      const std::vector<std::string> & sentence_words = sentences_words[i];
      std::vector<std::string> & original_sentence_words = original_sentences_words[i];

      glyphaware_sentence_curr_pos +=
        (i == 0 ? 0 :
        (
         (sentence_start_positions[i] +
          sentences_start_positions[i][0]) -
          (sentence_start_positions[i - 1] +
           sentences_end_positions[i - 1][sentences_end_positions[i - 1].size() - 1])
          )
         );

      glyphaware_sentence_start_positions.push_back(glyphaware_sentence_curr_pos);

      int glyphaware_word_curr_pos = 0;
      for (int j = 0; j < sentence_words.size(); ++j) {
        const std::string & word = sentence_words[j];
        std::string original_word =
          sentences[i].substr(sentences_start_positions[i][j],
                              sentences_end_positions[i][j] - sentences_start_positions[i][j]);
        original_sentence_words.push_back(original_word);

        glyphaware_word_curr_pos +=
          (j == 0 ? 0 : (sentences_start_positions[i][j] -
                         sentences_end_positions[i][j - 1]));

        int glyph_len = (int)CountUtf8Glyphs(original_word);

        glyphaware_sentences_start_positions[i].push_back(glyphaware_word_curr_pos);
        glyphaware_sentences_end_positions[i].push_back(glyphaware_word_curr_pos + glyph_len);

        glyphaware_word_curr_pos += glyph_len;
      }
      glyphaware_sentence_curr_pos += glyphaware_word_curr_pos;
    }

    return Analyse(language,
                   sentences_words,
                   original_sentences_words,
                   glyphaware_sentence_start_positions,
                   glyphaware_sentences_start_positions,
                   glyphaware_sentences_end_positions,
                   pbssink,
                   options);
  } else {
    return Analyse(language,
                   sentences_words,
                   original_sentences_words,
                   sentence_start_positions,
                   sentences_start_positions,
                   sentences_end_positions,
                   pbssink,
                   options);
  }


}

int CTurboTextAnalysis::Analyse(const std::string &language,
                                const std::vector<std::vector<std::string>> &sentences_words,
                                const std::vector<std::vector<std::string>> &original_sentences_words,
                                const std::vector<int> &sentence_start_positions,
                                const std::vector<std::vector<int> > &sentences_start_positions,
                                const std::vector<std::vector<int> > &sentences_end_positions,
                                CPBSSink* pbssink,
                                AnalyseOptions * options) {
  CTurboWorkerMap::iterator iter = _worker_map.find(language);
  if (iter == _worker_map.end())
    return -1;

  CTurboWorkers *workers = iter->second;

  TurboLemmatizer * lemmatizer;
  TurboParserInterface::TurboTaggerWorker *tagger;
  TurboParserInterface::TurboMorphologicalTaggerWorker *morphological_tagger;
  TurboParserInterface::TurboEntityRecognizerWorker *entity_recognizer;
  TurboParserInterface::TurboParserWorker *parser;
  TurboParserInterface::TurboSemanticParserWorker *semantic_parser;
  TurboParserInterface::TurboCoreferenceResolverWorker * coreference_resolver;

  //Check validity of options
  if (options != nullptr) {
    if (options->use_morphological_tagger && !options->use_tagger) {
      options->use_tagger = true;
      LOG(WARNING) << "OPTIONS ARGUMENT OVERRIDE:" << std::endl;
      LOG(WARNING) << "Requesting morphological tagger "
        << "will activate tagger "
        << "despite TAGGER value in options be \'false\'." << std::endl;
    }

    if (options->use_entity_recognizer && !options->use_tagger) {
      options->use_tagger = true;
      LOG(WARNING) << "OPTIONS ARGUMENT OVERRIDE:" << std::endl;
      LOG(WARNING) << "Requesting entity recognizer "
        << "will activate tagger "
        << "despite TAGGER value in options be \'false\'." << std::endl;
    }

    if (options->use_semantic_parser && !options->use_tagger) {
      options->use_tagger = true;
      LOG(WARNING) << "OPTIONS ARGUMENT OVERRIDE:" << std::endl;
      LOG(WARNING) << "Requesting semantic parser "
        << "will activate tagger "
        << "despite TAGGER value in options be \'false\'." << std::endl;
    }
    if (options->use_semantic_parser && !options->use_parser) {
      options->use_parser = true;
      LOG(WARNING) << "OPTIONS ARGUMENT OVERRIDE:" << std::endl;
      LOG(WARNING) << "Requesting semantic parser "
        << "will activate dependency parser "
        << "despite PARSER value in options be \'false\'." << std::endl;
    }

    if (options->use_coreference_resolver && !options->use_tagger) {
      options->use_tagger = true;
      LOG(WARNING) << "OPTIONS ARGUMENT OVERRIDE:" << std::endl;
      LOG(WARNING) << "Requesting coreference resolver "
        << "will activate tagger "
        << "despite TAGGER value in options be \'false\'." << std::endl;
    }
    if (options->use_coreference_resolver && !options->use_parser) {
      options->use_parser = true;
      LOG(WARNING) << "OPTIONS ARGUMENT OVERRIDE:" << std::endl;
      LOG(WARNING) << "Requesting coreference resolver "
        << "will activate dependency parser "
        << "despite PARSER value in options be \'false\'." << std::endl;
    }
  }

  if ((options == nullptr && default_use_tagger) ||
    (options != nullptr && options->use_tagger))
    lemmatizer = workers->GetLemmatizer();
  if ((options == nullptr && default_use_tagger) ||
    (options != nullptr && options->use_tagger))
    tagger = workers->GetTagger();
  if ((options == nullptr && default_use_parser) ||
    (options != nullptr && options->use_parser))
    parser = workers->GetParser();
  if ((options == nullptr && default_use_morphological_tagger) ||
    (options != nullptr && options->use_morphological_tagger))
    morphological_tagger = workers->GetMorphologicalTagger();
  if ((options == nullptr && default_use_entity_recognizer) ||
    (options != nullptr && options->use_entity_recognizer))
    entity_recognizer = workers->GetEntityRecognizer();
  if ((options == nullptr && default_use_semantic_parser) ||
    (options != nullptr && options->use_semantic_parser))
    semantic_parser = workers->GetSemanticParser();
  if ((options == nullptr && default_use_coreference_resolver) ||
    (options != nullptr && options->use_coreference_resolver))
    coreference_resolver = workers->GetCoreferenceResolver();

  std::vector<
    std::vector<std::string>
  > sentences_tagger_tags(sentences_words.size());;

  std::vector<
    std::vector<std::string>
  > sentences_lemmas(sentences_words.size());;

  std::vector<
    std::vector<std::string>
  > sentences_entity_tags(sentences_words.size());;
  std::vector<
    std::vector<EntitySpan*>
  > sentences_entity_spans(sentences_words.size());;

  std::vector<
    std::vector<std::string>
  > sentences_morphological_tagger_tags(sentences_words.size());;
  std::vector<
    std::vector<std::vector<std::string>>
  > sentences_morphological_tagger_tags_splitted(sentences_words.size());;

  std::vector<
    std::vector<std::string>
  > sentences_deprels(sentences_words.size());;
  std::vector<
    std::vector<int>
  > sentences_heads(sentences_words.size());;

  std::vector<
    std::vector<std::string>
  > sentences_predicate_names(sentences_words.size());;
  std::vector<
    std::vector<int>
  > sentences_predicate_indices(sentences_words.size());;
  std::vector<
    std::vector<std::vector<std::string>>
  > sentences_argument_roles(sentences_words.size());;
  std::vector<
    std::vector<std::vector<int>>
  > sentences_argument_indices(sentences_words.size());;
  std::vector<
    std::vector<std::string>
  > sentences_predicates(sentences_words.size());;
  std::vector<
    std::vector<std::vector<std::string>>
  > sentences_argument_lists(sentences_words.size());;

  std::vector<
    std::vector<std::string>
  > sentences_coref_info(sentences_words.size());;

  for (int j = 0; j < sentences_words.size(); ++j) {
    const std::vector<std::string> & words = sentences_words[j];

    // Run the POS tagger.
    std::vector<std::string> & tagger_tags = sentences_tagger_tags[j];
    if (tagger != nullptr &&
      ((options == nullptr && default_use_tagger) ||
        (options != nullptr && options->use_tagger))) {
      SequenceInstance tagged_sentence;
      tagger_tags.resize(words.size(), "_");
      tagged_sentence.Initialize(words,
                                 tagger_tags);
      tagger->TagSentence(&tagged_sentence);
      tagger_tags = tagged_sentence.tags();
    }

    std::vector<std::string> & lemmas = sentences_lemmas[j];
    if (tagger != nullptr &&
      ((options == nullptr && default_use_tagger) ||
        (options != nullptr && options->use_tagger))) {
      //Run the Lemmatizer.
      lemmas.resize(words.size(), "_");
      lemmatizer->LemmatizeSentence(words, tagger_tags, &lemmas);
    }

    // Run the entity recognizer.
    if (entity_recognizer != nullptr &&
      ((options == nullptr && default_use_entity_recognizer) ||
        (options != nullptr && options->use_entity_recognizer))) {
      EntityInstance entity_tagged_sentence;
      std::vector<std::string> & entity_tags = sentences_entity_tags[j];
      entity_tags.resize(words.size(), "_");

      entity_tagged_sentence.Initialize(words,
                                        tagger_tags,
                                        entity_tags);
      entity_recognizer->TagSentence(&entity_tagged_sentence);
      entity_tags = entity_tagged_sentence.tags();

      std::vector<EntitySpan*> & entity_spans = sentences_entity_spans[j];
      entity_tagged_sentence.CreateSpansFromTags(entity_tags,
                                                 &entity_spans);
    }

    //Run the Morphological tagger.
    if (morphological_tagger != nullptr &&
      ((options == nullptr && default_use_morphological_tagger) ||
        (options != nullptr && options->use_morphological_tagger))) {
      MorphologicalInstance morpho_tagged_sentence;
      std::vector<std::string> & morphological_tagger_tags =
        sentences_morphological_tagger_tags[j];
      std::vector<std::vector<std::string>> & morphological_tagger_tags_splitted =
        sentences_morphological_tagger_tags_splitted[j];
      morphological_tagger_tags.resize(words.size(), "_");
      morphological_tagger_tags_splitted.resize(words.size());
      morpho_tagged_sentence.Initialize(words,
                                        lemmas,
                                        tagger_tags,
                                        morphological_tagger_tags);
      morphological_tagger->TagSentence(&morpho_tagged_sentence);
      morphological_tagger_tags = morpho_tagged_sentence.tags();

      for (int i = 0; i < morphological_tagger_tags.size(); i++) {
        if (morphological_tagger_tags[i] == "_")
          morphological_tagger_tags_splitted[i] = {};
        else
          SplitString(morphological_tagger_tags[i],
                      '|',
                      &morphological_tagger_tags_splitted[i]);
      }
    }

    {
      //Run the Dependency parser.
      std::vector<std::vector<std::string>> feats(words.size());

      std::vector<std::string> & deprels = sentences_deprels[j];
      deprels.resize(words.size(), "_");
      std::vector<int>  & heads = sentences_heads[j];
      heads.resize(words.size(), 0);

      auto words_with_root = words;
      auto lemmas_with_root = lemmas;
      auto tagger_tags_with_root = tagger_tags;
      auto feats_with_root = feats;
      auto deprels_with_root = deprels;
      auto heads_with_root = heads;

      words_with_root.insert(words_with_root.begin(), "_root_");
      lemmas_with_root.insert(lemmas_with_root.begin(), "_root_");
      tagger_tags_with_root.insert(tagger_tags_with_root.begin(), "_root_");
      feats_with_root.insert(feats_with_root.begin(), { "_root_" });
      deprels_with_root.insert(deprels_with_root.begin(), "_root_");
      heads_with_root.insert(heads_with_root.begin(), -1);

      if (parser != nullptr &&
        ((options == nullptr && default_use_parser) ||
          (options != nullptr && options->use_parser))) {
        DependencyInstance parsed_sentence;

        parsed_sentence.Initialize(words_with_root,
                                   lemmas_with_root,
                                   tagger_tags_with_root,
                                   tagger_tags_with_root,
                                   feats_with_root,
                                   deprels_with_root,
                                   heads_with_root);
        parser->ParseSentence(&parsed_sentence);

        for (int i = 0; i < words.size(); ++i) {
          heads[i] = parsed_sentence.GetHead(i + 1) - 1;
          deprels[i] = parsed_sentence.GetDependencyRelation(i + 1);
        }
        //Update deprels with root
        deprels_with_root = deprels;
        deprels_with_root.insert(deprels_with_root.begin(), "_root_");
        //Update heads with root
        heads_with_root = heads;
        heads_with_root.insert(heads_with_root.begin(), -1);
      }

      //Run the Semantic parser
      if (semantic_parser != nullptr &&
        ((options == nullptr && default_use_semantic_parser) ||
          (options != nullptr && options->use_semantic_parser))) {
        SemanticInstance semantic_parsed_sentence;
        std::vector<std::string> & predicate_names = sentences_predicate_names[j];
        predicate_names.resize(words.size(), "_");
        std::vector<int> & predicate_indices = sentences_predicate_indices[j];
        predicate_indices.resize(words.size(), 0);
        std::vector< std::vector<std::string> > &argument_roles = sentences_argument_roles[j];
        argument_roles.resize(words.size());
        std::vector< std::vector<int> > & argument_indices = sentences_argument_indices[j];
        argument_indices.resize(words.size());

        auto heads_shifted = heads;
        for (int i = 0; i < heads.size(); i++)
          heads_shifted[i]++;
        auto heads_shifted_with_root = heads_shifted;
        heads_shifted_with_root.insert(heads_shifted_with_root.begin(), -1);

        semantic_parsed_sentence.Initialize("",
                                            words_with_root,
                                            lemmas_with_root,
                                            tagger_tags_with_root,
                                            tagger_tags_with_root,
                                            feats_with_root,
                                            deprels_with_root,
                                            heads_shifted_with_root,
                                            predicate_names,
                                            predicate_indices,
                                            argument_roles,
                                            argument_indices);
        semantic_parser->ParseSemanticDependenciesFromSentence(&semantic_parsed_sentence);

        auto num_predicates = semantic_parsed_sentence.GetNumPredicates();
        predicate_names.resize(num_predicates);
        predicate_indices.resize(num_predicates);
        argument_roles.resize(num_predicates);
        argument_indices.resize(num_predicates);
        for (int k = 0; k < num_predicates; k++) {
          predicate_names[k] = semantic_parsed_sentence.GetPredicateName(k);
          predicate_indices[k] = semantic_parsed_sentence.GetPredicateIndex(k) - 1;

          auto num_arguments_predicate = semantic_parsed_sentence.GetNumArgumentsPredicate(k);
          argument_roles[k].resize(num_arguments_predicate);
          argument_indices[k].resize(num_arguments_predicate);
          for (int l = 0; l < num_arguments_predicate; l++) {
            argument_roles[k][l] = semantic_parsed_sentence.GetArgumentRole(k, l);
            argument_indices[k][l] = semantic_parsed_sentence.GetArgumentIndex(k, l) - 1;
          }
        }

        std::vector<std::string> & predicates = sentences_predicates[j];
        predicates.resize(words.size(), "_");
        std::vector<std::vector<std::string>> & argument_lists = sentences_argument_lists[j];
        argument_lists.resize(words.size(), std::vector<std::string>(num_predicates, "_"));

        for (int k = 0; k < num_predicates; k++) {
          predicates[predicate_indices[k]] = predicate_names[k];
          for (int l = 0; l < argument_roles[k].size(); l++) {
            argument_lists[argument_indices[k][l]][k] = argument_roles[k][l];
          }
        }
      }
    }
  }

  //Run the Coreference resolver
  if (coreference_resolver != nullptr &&
    ((options == nullptr && default_use_coreference_resolver) ||
      (options != nullptr && options->use_coreference_resolver))) {
    CoreferenceDocument coref_document;
    std::vector<CoreferenceSentence> coref_sentences(sentences_words.size());
    for (int j = 0; j < sentences_words.size(); ++j) {
      const std::vector<std::string> & words = sentences_words[j];
      const std::vector<std::string> & lemmas = sentences_lemmas[j];
      const std::vector<std::string> & tagger_tags = sentences_tagger_tags[j];
      const std::vector<std::string> & deprels = sentences_deprels[j];
      const std::vector<int>  & heads = sentences_heads[j];

      std::vector<std::vector<std::string>> feats(words.size());
      std::vector<std::string> speakers(words.size(), "-");

      auto heads_shifted = heads;
      for (int i = 0; i < heads.size(); i++)
        heads_shifted[i]++;

      auto words_with_root = words;
      auto lemmas_with_root = lemmas;
      auto tagger_tags_with_root = tagger_tags;
      auto feats_with_root = feats;
      auto deprels_with_root = deprels;
      auto heads_shifted_with_root = heads_shifted;
      auto speakers_with_root = speakers;

      words_with_root.insert(words_with_root.begin(), "_root_");
      lemmas_with_root.insert(lemmas_with_root.begin(), "_root_");
      tagger_tags_with_root.insert(tagger_tags_with_root.begin(), "_root_");
      feats_with_root.insert(feats_with_root.begin(), { "_root_" });
      deprels_with_root.insert(deprels_with_root.begin(), "_root_");
      heads_shifted_with_root.insert(heads_shifted_with_root.begin(), -1);
      speakers_with_root.insert(speakers_with_root.begin(), "__");

      coref_sentences[j].Initialize("",
                                    words_with_root,
                                    lemmas_with_root,
                                    tagger_tags_with_root,
                                    tagger_tags_with_root,
                                    feats_with_root,
                                    deprels_with_root,
                                    heads_shifted_with_root,
                                    {},
                                    {},
                                    {},
                                    {},
                                    speakers_with_root,
                                    {},
                                    {},
                                    {}
      );
    }

    std::vector<CoreferenceSentence*> coref_sentences_ptr;
    for (auto & coref_sentence : coref_sentences) {
      coref_sentences_ptr.push_back(&coref_sentence);
    };
    coref_document.Initialize("", 0, coref_sentences_ptr);
    coreference_resolver->ResolveCoreferencesFromDocument(&coref_document);

    for (int j = 0; j < coref_document.GetNumSentences(); ++j) {
      const CoreferenceSentence* coreference_sentence = coref_document.GetSentence(j);
      const std::vector<NamedSpan*>& coreference_spans = coreference_sentence->GetCoreferenceSpans();

      //Convert back to 0-based indexing.
      std::vector<NamedSpan> coref_spans;
      for (const auto & span : coreference_spans) {
        coref_spans.push_back({ span->start() - 1, span->end() - 1, span->name() });
      };

      std::vector<std::string> & coref_info = sentences_coref_info[j];
      coref_info.resize(sentences_words[j].size());

      for (const auto & span : coref_spans) {
        if (span.start() == span.end()) {//Single-word mention.
          std::string to_add = "";
          if (coref_info[span.start()] != "")
            to_add = "|(" + span.name() + ")";
          else
            to_add = "(" + span.name() + ")";
          coref_info[span.start()] += to_add;
        } else {
          if (coref_info[span.start()] != "") {
            coref_info[span.start()] = "|" + coref_info[span.start()];
          }
          coref_info[span.start()] = "(" + span.name() + coref_info[span.start()];

          if (coref_info[span.end()] != "") {
            coref_info[span.end()] += "|";
          }
          coref_info[span.end()] += span.name() + ")";
        };
      };
      for (int i = 0; i < sentences_words[j].size(); i++) {
        if (coref_info[i] == "")
          coref_info[i] = "_";
      };
    };
  }

  // TO OUTPUT : TOKEN->FEATURES
  // 1.  words
  // 2.  lemmas
  // 3.  tagger_tags
  // 4.  morphological_tagger_tags
  // 5.  heads
  // 6.  deprels
  // 7.  entity_tags
  // 8.  coref_tags
  // 9.  predicates
  // 10. argument_lists
  // 11. coref_info

  for (int j = 0; j < sentences_words.size(); ++j) {
    const std::vector<std::string> & words = sentences_words[j];
    const std::vector<int> & start_positions = sentences_start_positions[j];
    const std::vector<int> & end_positions = sentences_end_positions[j];
    const std::vector<std::string> & tagger_tags = sentences_tagger_tags[j];
    const std::vector<std::string> & lemmas = sentences_lemmas[j];
    const std::vector<std::string> & entity_tags = sentences_entity_tags[j];
    //const std::vector<EntitySpan*> & entity_spans = sentences_entity_spans[j];
    const std::vector<std::string> & morphological_tagger_tags = sentences_morphological_tagger_tags[j];
    const std::vector<std::string> & deprels = sentences_deprels[j];
    const std::vector<int> & heads = sentences_heads[j];
    const std::vector<std::string> & predicates = sentences_predicates[j];
    const std::vector<std::vector<std::string>> & argument_lists = sentences_argument_lists[j];
    const std::vector<std::string> coref_info = sentences_coref_info[j];

    int offset_sentence = sentence_start_positions[j];
    for (int i = 0; i < words.size(); i++) {
      // Process token.
      int start_position = start_positions[i];
      int end_position = end_positions[i];
      std::string token_text = words[i];
      CTurboToken token;
      token.set_text(words[i]);
      token.set_start_position(start_position);
      token.set_end_position(end_position);
      if (tagger != nullptr &&
        ((options == nullptr && default_use_tagger) ||
          (options != nullptr && options->use_tagger))) {
        token.set_tag(tagger_tags[i]);
      }
      token.set_multi_word(false);
      token.set_entity_tag("");

      pbssink->PutToken(token_text.c_str(),
                        end_position - start_position,
                        offset_sentence + start_position,
                        m_token_analysis.GetTokenKind(&token));

      pbssink->PutFeature("sentence_id", std::to_string(j + 1).c_str());
      pbssink->PutFeature("sentence_token_id", std::to_string(i + 1).c_str());
      if (original_sentences_words.size() > j && original_sentences_words[j].size() > i)
        pbssink->PutFeature("original_token", (original_sentences_words[j][i]).c_str());
      if (tagger != nullptr &&
        ((options == nullptr && default_use_tagger) ||
          (options != nullptr && options->use_tagger))) {
        pbssink->PutFeature("lemma", (lemmas[i]).c_str());
      }
      if (tagger != nullptr &&
        ((options == nullptr && default_use_tagger) ||
          (options != nullptr && options->use_tagger))) {
        pbssink->PutFeature("pos_tag", (tagger_tags[i]).c_str());
      }
      if (morphological_tagger != nullptr &&
        ((options == nullptr && default_use_morphological_tagger) ||
          (options != nullptr && options->use_morphological_tagger))) {
        pbssink->PutFeature("morphological_feats", (morphological_tagger_tags[i]).c_str());
      }
      if (parser != nullptr &&
        ((options == nullptr && default_use_parser) ||
          (options != nullptr && options->use_parser))) {
        pbssink->PutFeature("dependency_head", std::to_string(heads[i] + 1).c_str());
        pbssink->PutFeature("dependency_relation", (deprels[i]).c_str());
      }
      if (entity_recognizer != nullptr &&
        ((options == nullptr && default_use_entity_recognizer) ||
          (options != nullptr && options->use_entity_recognizer))) {
        pbssink->PutFeature("entity_tag", (entity_tags[i]).c_str());
      }
      if (semantic_parser != nullptr &&
        ((options == nullptr && default_use_semantic_parser) ||
          (options != nullptr && options->use_semantic_parser))) {
        pbssink->PutFeature("semantic_predicate", (predicates[i]).c_str());
        std::string semantic_args;
        int k = 0;
        for (const auto & elem : argument_lists[i]) {
          if (k > 0)
            semantic_args += "|";
          semantic_args += elem;
          k++;
        }
        pbssink->PutFeature("semantic_arguments_list", semantic_args.c_str());
      }
      if (coreference_resolver != nullptr &&
        ((options == nullptr && default_use_coreference_resolver) ||
          (options != nullptr && options->use_coreference_resolver))) {
        pbssink->PutFeature("coref_info", (coref_info[i]).c_str());
      }
    }
    pbssink->EndSentence();
  }
  return 0;
}

int CTurboTextAnalysis::LoadLanguage(const std::string &lang,
                                     const std::string &path,
                                     LoadOptions * options) {
  // using a local lock_guard to lock m_lock guarantees unlocking on destruction / exception:
  std::lock_guard<std::mutex> lock(_worker_map.m_lock);

  CTurboWorkers*turbo_workers = nullptr;
  if (_worker_map.FindLanguageWorkers(lang, turbo_workers))
    return 0;

  LoadOptions local_options;
  if (options != nullptr)local_options = *options;

  std::string tokenizer_type = "";
  bool tokenizer_outputs_unicode_glyph_aware_offsets = false;
  std::string filepath_abbreviations = "";
  std::string filepath_contractions = "";
  std::string filepath_contraction_suffixes = "";

  std::string filepath_lemmas = "";

  std::string filepath_tagger_model = "";
  std::string filepath_morphological_tagger_model = "";
  std::string filepath_entity_recognizer_model = "";
  std::string filepath_parser_model = "";
  std::string filepath_semantic_parser_model = "";
  std::string filepath_coreference_resolver_model = "";

  TurboTokenizer::TaskOptions tokenizer_options;

  std::string filepath_config = path;
  filepath_config += "config.cfg";

  try {
    libconfig::Config config;
    config.readFile(filepath_config.c_str());

    if (_interface == nullptr) {
      {
        // Configure logging
        //
        if (!config.exists("logging")) {
          LOG(ERROR) << "Missing 'logging' config section in file: "
            << filepath_config;
          return -1;
        }
        libconfig::Setting &logging = config.lookup("logging");
        if (!GLogConfig::SetupGLog(logging)) {
          return -1;
        }
      }
      _interface = new TurboParserInterface::TurboParserInterface();
    }

    {
      // Configure languages
        //
      if (!config.exists("languages")) {
        LOG(ERROR) << "Missing 'languages' config section in file: "
          << filepath_config;
        return -1;
      }
      libconfig::Setting &languages = config.lookup("languages");

      if (languages.getLength() > 0) {
        bool language_found = false;
        for (int i = 0; i < languages.getLength(); ++i) {
          libconfig::Setting &language = languages[i];
          std::string iter_lang;
          bool read_ok{ true };

          read_ok = ReadSettingSafe(language, "language",
                                    iter_lang);
          if (!read_ok)
            continue;

          if (read_ok && iter_lang == lang) {
            language_found = true;
            read_ok = false;
            if (!ReadorDefaultLibConfigSetting(language,
                                               "tokenizer_type",
                                               std::string("standard"),
                                               tokenizer_type))
              return false;
            if (!ReadorDefaultLibConfigSetting(language,
                                               "tokenizer_outputs_unicode_glyph_aware_offsets",
                                               false,
                                               tokenizer_outputs_unicode_glyph_aware_offsets))
              return false;
            read_ok &= ReadorDefaultLibConfigSetting(language,
                                                     "filepath_abbreviations",
                                                     std::string(""),
                                                     filepath_abbreviations);
            read_ok &= ReadorDefaultLibConfigSetting(language,
                                                     "filepath_contractions",
                                                     std::string(""),
                                                     filepath_contractions);
            read_ok &= ReadorDefaultLibConfigSetting(language,
                                                     "filepath_contraction_suffixes",
                                                     std::string(""),
                                                     filepath_contraction_suffixes);
            read_ok &= ReadorDefaultLibConfigSetting(language,
                                                     "filepath_lemmas",
                                                     std::string(""),
                                                     filepath_lemmas);
            read_ok &= ReadorDefaultLibConfigSetting(language,
                                                     "filepath_tagger_model",
                                                     std::string(""),
                                                     filepath_tagger_model);
            read_ok &= ReadorDefaultLibConfigSetting(language,
                                                     "filepath_morphological_tagger_model",
                                                     std::string(""),
                                                     filepath_morphological_tagger_model);
            read_ok &= ReadorDefaultLibConfigSetting(language,
                                                     "filepath_entity_recognizer_model",
                                                     std::string(""),
                                                     filepath_entity_recognizer_model);
            read_ok &= ReadorDefaultLibConfigSetting(language,
                                                     "filepath_parser_model",
                                                     std::string(""),
                                                     filepath_parser_model);
            read_ok &= ReadorDefaultLibConfigSetting(language,
                                                     "filepath_semantic_parser_model",
                                                     std::string(""),
                                                     filepath_semantic_parser_model);
            read_ok &= ReadorDefaultLibConfigSetting(language,
                                                     "filepath_coreference_resolver_model",
                                                     std::string(""),
                                                     filepath_coreference_resolver_model);

            read_ok &= ReadorDefaultLibConfigSetting(language,
                                                     "break_token_on_hyphen",
                                                     true,
                                                     tokenizer_options.break_token_on_hyphen);

            if (filepath_abbreviations != "")
              filepath_abbreviations = path + filepath_abbreviations;
            if (filepath_contractions != "")
              filepath_contractions = path + filepath_contractions;
            if (filepath_contraction_suffixes != "")
              filepath_contraction_suffixes = path + filepath_contraction_suffixes;

            if (filepath_lemmas != "")
              filepath_lemmas = path + filepath_lemmas;

            if (filepath_tagger_model != "")
              filepath_tagger_model = path + filepath_tagger_model;
            if (filepath_morphological_tagger_model != "")
              filepath_morphological_tagger_model = path + filepath_morphological_tagger_model;
            if (filepath_entity_recognizer_model != "")
              filepath_entity_recognizer_model = path + filepath_entity_recognizer_model;
            if (filepath_parser_model != "")
              filepath_parser_model = path + filepath_parser_model;
            if (filepath_semantic_parser_model != "")
              filepath_semantic_parser_model = path + filepath_semantic_parser_model;
            if (filepath_coreference_resolver_model != "")
              filepath_coreference_resolver_model = path + filepath_coreference_resolver_model;
            break;
          } else {
            continue;
          }
        }
        if (!language_found) {
          LOG(ERROR) << "Requested language does not exist in the config file." << std::endl;
          return -1;
        }
      }
    }

    {
      libconfig::Setting &setting = config.lookup("turbotextanalysis");

      // Read from setting
      bool read_ok = true;
      {
        if (options == nullptr) {
          read_ok &= ReadSettingSafe(setting, "load_tagger",
                                     local_options.load_tagger);
          read_ok &= ReadSettingSafe(setting, "load_parser",
                                     local_options.load_parser);
          read_ok &= ReadSettingSafe(setting, "load_morphological_tagger",
                                     local_options.load_morphological_tagger);
          read_ok &= ReadSettingSafe(setting, "load_entity_recognizer",
                                     local_options.load_entity_recognizer);
          read_ok &= ReadSettingSafe(setting, "load_semantic_parser",
                                     local_options.load_semantic_parser);
          read_ok &= ReadSettingSafe(setting, "load_coreference_resolver",
                                     local_options.load_coreference_resolver);
          if (!read_ok) {
            std::cerr << "Missing configuration properties" << std::endl;
            return -1;
          }
        }

        if (local_options.load_morphological_tagger && !local_options.load_tagger) {
          local_options.load_tagger = true;
          LOG(WARNING) << "OPTIONS OVERRIDE:" << std::endl;
          LOG(WARNING) << "Requesting morphological tagger "
            << "will activate tagger "
            << "despite TAGGER value in options be \'false\'." << std::endl;
        }

        if (local_options.load_entity_recognizer && !local_options.load_tagger) {
          local_options.load_tagger = true;
          LOG(WARNING) << "OPTIONS OVERRIDE:" << std::endl;
          LOG(WARNING) << "Requesting entity recognizer "
            << "will activate tagger "
            << "despite TAGGER value in options be \'false\'." << std::endl;
        }

        if (local_options.load_semantic_parser && !local_options.load_tagger) {
          local_options.load_tagger = true;
          LOG(WARNING) << "OPTIONS OVERRIDE:" << std::endl;
          LOG(WARNING) << "Requesting semantic parser "
            << "will activate tagger "
            << "despite TAGGER value in options be \'false\'." << std::endl;
        }
        if (local_options.load_semantic_parser && !local_options.load_parser) {
          local_options.load_parser = true;
          LOG(WARNING) << "OPTIONS OVERRIDE:" << std::endl;
          LOG(WARNING) << "Requesting semantic parser "
            << "will activate dependency parser "
            << "despite PARSER value in options be \'false\'." << std::endl;
        }

        if (local_options.load_coreference_resolver && !local_options.load_tagger) {
          local_options.load_tagger = true;
          LOG(WARNING) << "OPTIONS OVERRIDE:" << std::endl;
          LOG(WARNING) << "Requesting coreference resolver "
            << "will activate tagger "
            << "despite TAGGER value in options be \'false\'." << std::endl;
        }
        if (local_options.load_coreference_resolver && !local_options.load_parser) {
          local_options.load_parser = true;
          LOG(WARNING) << "OPTIONS OVERRIDE:" << std::endl;
          LOG(WARNING) << "Requesting coreference resolver "
            << "will activate dependency parser "
            << "despite PARSER value in options be \'false\'." << std::endl;
        }
      }

      {
        bool read_default_use_tagger = true;
        bool read_default_use_parser = true;
        bool read_default_use_morphological_tagger = true;
        bool read_default_use_entity_recognizer = true;
        bool read_default_use_semantic_parser = true;
        bool read_default_use_coreference_resolver = true;

        read_ok &= ReadSettingSafe(setting, "default_use_tagger",
                                   read_default_use_tagger);
        read_ok &= ReadSettingSafe(setting, "default_use_parser",
                                   read_default_use_parser);
        read_ok &= ReadSettingSafe(setting, "default_use_morphological_tagger",
                                   read_default_use_morphological_tagger);
        read_ok &= ReadSettingSafe(setting, "default_use_entity_recognizer",
                                   read_default_use_entity_recognizer);
        read_ok &= ReadSettingSafe(setting, "default_use_semantic_parser",
                                   read_default_use_semantic_parser);
        read_ok &= ReadSettingSafe(setting, "default_use_coreference_resolver",
                                   read_default_use_coreference_resolver);
        if (!read_ok) {
          std::cerr << "Missing configuration properties" << std::endl;
          return -1;
        }

        default_use_tagger = read_default_use_tagger;
        default_use_parser = read_default_use_parser;
        default_use_morphological_tagger = read_default_use_morphological_tagger;
        default_use_entity_recognizer = read_default_use_entity_recognizer;
        default_use_semantic_parser = read_default_use_semantic_parser;
        default_use_coreference_resolver = read_default_use_coreference_resolver;

        if (default_use_morphological_tagger && !default_use_tagger) {
          default_use_tagger = true;
          LOG(WARNING) << "CONFIG FILE OVERRIDE:" << std::endl;
          LOG(WARNING) << "Requesting morphological tagger "
            << "will activate tagger "
            << "despite TAGGER value in config file be \'false\'." << std::endl;
        }

        if (default_use_entity_recognizer && !default_use_tagger) {
          default_use_tagger = true;
          LOG(WARNING) << "CONFIG FILE OVERRIDE:" << std::endl;
          LOG(WARNING) << "Requesting entity recognizer "
            << "will activate tagger "
            << "despite TAGGER value in config file be \'false\'." << std::endl;
        }

        if (default_use_semantic_parser && !default_use_tagger) {
          default_use_tagger = true;
          LOG(WARNING) << "CONFIG FILE OVERRIDE:" << std::endl;
          LOG(WARNING) << "Requesting semantic parser "
            << "will activate tagger "
            << "despite TAGGER value in config file be \'false\'." << std::endl;
        }
        if (default_use_semantic_parser && !default_use_parser) {
          default_use_parser = true;
          LOG(WARNING) << "CONFIG FILE OVERRIDE:" << std::endl;
          LOG(WARNING) << "Requesting semantic parser "
            << "will activate dependency parser "
            << "despite PARSER value in config file be \'false\'." << std::endl;
        }

        if (default_use_coreference_resolver && !default_use_tagger) {
          default_use_tagger = true;
          LOG(WARNING) << "CONFIG FILE OVERRIDE:" << std::endl;
          LOG(WARNING) << "Requesting coreference resolver "
            << "will activate tagger "
            << "despite TAGGER value in config file be \'false\'." << std::endl;
        }
        if (default_use_coreference_resolver && !default_use_parser) {
          default_use_parser = true;
          LOG(WARNING) << "CONFIG FILE OVERRIDE:" << std::endl;
          LOG(WARNING) << "Requesting coreference resolver "
            << "will activate dependency parser "
            << "despite PARSER value in config file be \'false\'." << std::endl;
        }
      }
    }
  } catch (libconfig::ParseException e) {
    LOG(ERROR) << "ParseException: " << e.what()
      << std::endl << "error: " << e.getError()
      << std::endl << "file: " << e.getFile()
      << std::endl << "line: " << e.getLine();
    return -1;
  } catch (libconfig::FileIOException e) {
    LOG(ERROR) << "FileIOException: " << e.what();
    return -1;
  } catch (libconfig::ConfigException e) {
    LOG(ERROR) << "ConfigException: " << e.what();
    return -1;
  } catch (...) {
    LOG(ERROR) << "Unexpected Exception";
    return -1;
  }

  if (filepath_tagger_model == "")
    local_options.load_tagger = false;
  if (filepath_morphological_tagger_model == "")
    local_options.load_morphological_tagger = false;
  if (filepath_entity_recognizer_model == "")
    local_options.load_entity_recognizer = false;
  if (filepath_parser_model == "")
    local_options.load_parser = false;
  if (filepath_semantic_parser_model == "")
    local_options.load_semantic_parser = false;
  if (filepath_coreference_resolver_model == "")
    local_options.load_coreference_resolver = false;

  if (local_options.load_morphological_tagger && !local_options.load_tagger) {
    local_options.load_morphological_tagger = false;
    LOG(WARNING) << "CONFIG FILE OVERRIDE:" << std::endl;
    LOG(WARNING) << "Requesting morphological tagger "
      << "will not be accepted, as there is now tagger model." << std::endl;
  }

  if (local_options.load_entity_recognizer && !local_options.load_tagger) {
    local_options.load_entity_recognizer = false;
    LOG(WARNING) << "CONFIG FILE OVERRIDE:" << std::endl;
    LOG(WARNING) << "Requesting entity recognizer "
      << "will not be accepted, as there is now tagger model." << std::endl;
  }

  if (local_options.load_semantic_parser && !local_options.load_tagger) {
    local_options.load_semantic_parser = false;
    LOG(WARNING) << "CONFIG FILE OVERRIDE:" << std::endl;
    LOG(WARNING) << "Requesting semantic parser "
      << "will not be accepted, as there is now tagger model." << std::endl;
  }
  if (local_options.load_semantic_parser && !local_options.load_parser) {
    local_options.load_semantic_parser = false;
    LOG(WARNING) << "CONFIG FILE OVERRIDE:" << std::endl;
    LOG(WARNING) << "Requesting semantic parser "
      << "will not be accepted, as there is now parser model." << std::endl;
  }

  if (local_options.load_coreference_resolver && !local_options.load_tagger) {
    local_options.load_coreference_resolver = false;
    LOG(WARNING) << "CONFIG FILE OVERRIDE:" << std::endl;
    LOG(WARNING) << "Requesting coreference resolver "
      << "will not be accepted, as there is now tagger model." << std::endl;
  }
  if (local_options.load_coreference_resolver && !local_options.load_parser) {
    local_options.load_coreference_resolver = false;
    LOG(WARNING) << "CONFIG FILE OVERRIDE:" << std::endl;
    LOG(WARNING) << "Requesting coreference resolver "
      << "will not be accepted, as there is now parser model." << std::endl;
  }

  Tokenizer *tokenizer = nullptr;
  if (tokenizer_type == "icu") {
    tokenizer = new ICUTokenizer(lang);
  } else if (tokenizer_type != "standard") {
    LOG(ERROR) << "tokenizer_type = \"" << tokenizer_type << "\" not recognized." <<
      "tokenizer_type = \"standard\" will be used.";
  }
  if (tokenizer == nullptr) {
    tokenizer = new TurboTokenizer(tokenizer_options);
    if (filepath_abbreviations != "")
      static_cast<TurboTokenizer*>(tokenizer)->LoadAbbreviations(filepath_abbreviations);
    if (filepath_contractions != "")
      static_cast<TurboTokenizer*>(tokenizer)->LoadContractions(filepath_contractions);
    if (filepath_contraction_suffixes != "")
      static_cast<TurboTokenizer*>(tokenizer)->LoadContractionSuffixes(filepath_contraction_suffixes);
  }

  TurboLemmatizer *lemmatizer = new TurboLemmatizer;
  if (filepath_lemmas != "")
    lemmatizer->LoadLemmas(filepath_lemmas);

  TurboParserInterface::TurboTaggerWorker *tagger{ nullptr };
  TurboParserInterface::TurboMorphologicalTaggerWorker *morphological_tagger{ nullptr };
  TurboParserInterface::TurboEntityRecognizerWorker *entity_recognizer{ nullptr };
  TurboParserInterface::TurboParserWorker *parser{ nullptr };
  TurboParserInterface::TurboSemanticParserWorker *semantic_parser{ nullptr };
  TurboParserInterface::TurboCoreferenceResolverWorker * coreference_resolver{ nullptr };

  //***********************
  // tagger
  //***********************

  if (local_options.load_tagger) {
    tagger = _interface->CreateTagger();
    tagger->
      LoadTaggerModel(filepath_tagger_model);
  }
  //***********************
  // morphological_tagger
  //***********************
  if (local_options.load_morphological_tagger) {
    morphological_tagger = _interface->CreateMorphologicalTagger();
    morphological_tagger->
      LoadMorphologicalTaggerModel(filepath_morphological_tagger_model);
  }
  //***********************
  // entity_recognizer
  //***********************
  if (local_options.load_entity_recognizer) {
    entity_recognizer = _interface->CreateEntityRecognizer();
    entity_recognizer->
      LoadEntityRecognizerModel(filepath_entity_recognizer_model);
  }
  //***********************
  // parser
  //***********************
  if (local_options.load_parser) {
    parser = _interface->CreateParser();
    parser->
      LoadParserModel(filepath_parser_model);
  }
  //***********************
  // semantic_parser
  //***********************
  if (local_options.load_semantic_parser) {
    semantic_parser = _interface->CreateSemanticParser();
    semantic_parser->
      LoadSemanticParserModel(filepath_semantic_parser_model);
  }
  //***********************
  // coreference_resolver
  //***********************
  if (local_options.load_coreference_resolver) {
    coreference_resolver = _interface->CreateCoreferenceResolver();
    coreference_resolver->
      LoadCoreferenceResolverModel(filepath_coreference_resolver_model);
  }

  _worker_map.
    AddLanguageWorkers(lang,
                       tokenizer,
                       tokenizer_outputs_unicode_glyph_aware_offsets,
                       (local_options.load_tagger) ? lemmatizer : nullptr,
                       (local_options.load_tagger) ? tagger : nullptr,
                       (local_options.load_morphological_tagger) ? morphological_tagger : nullptr,
                       (local_options.load_entity_recognizer) ? entity_recognizer : nullptr,
                       (local_options.load_parser) ? parser : nullptr,
                       (local_options.load_semantic_parser) ? semantic_parser : nullptr,
                       (local_options.load_coreference_resolver) ? coreference_resolver : nullptr);

  LOG(INFO) << "Finished LoadLanguage";

  return 0;
}
