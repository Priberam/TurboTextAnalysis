#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "TurboTextAnalysis/TurboTextAnalysis.h"

struct TokenInfo {
  std::string token_text;
  int token_id;
  int token_len;
  int token_start_pos;
  std::unordered_map<std::string, std::string> token_features;
};

class DocumentSink : public  CPBSSink {
public:
  DocumentSink();
  virtual ~DocumentSink();

  int PutToken(const char * word,
               int len,
               int start_pos,
               TokenKind kind);
  int PutFeature(const char * feature,
                 const char * value);
  int EndSentence();
  int PutDocumentFeature(const char * feature,
                         const char * value);

  void PrintContent();
protected:
  std::string last_word = "default_word";
  int current_index = 0;
public:
  std::vector<TokenInfo> token_data;
  std::unordered_map<std::string, std::string> document_data;
};

DocumentSink::DocumentSink() {}
DocumentSink::~DocumentSink() {}

int DocumentSink::PutToken(const char * word,
                           int len,
                           int start_pos,
                           TokenKind kind) {
  std::cout << "\tPutToken(" << word << ", " << len << ", " << start_pos
    // << ", " << kind
    << ")" << std::endl;

  last_word = word;
  current_index = (int)token_data.size();
  token_data.push_back({});
  token_data[current_index].token_text = word;
  token_data[current_index].token_id = current_index;
  token_data[current_index].token_len = len;
  token_data[current_index].token_start_pos = start_pos;
  token_data[current_index].token_features.clear();
  return 0;
}

int DocumentSink::PutFeature(const char * feature,
                             const char * value) {
  std::cout << "\t\tPutFeature(" << feature << ", " << value << ")" << std::endl;

  token_data[current_index].token_features[feature] = value;
  return 0;
}

int DocumentSink::EndSentence() {
  std::cout << "END SENTENCE" << std::endl;

  return 0;
}

int DocumentSink::PutDocumentFeature(const char * feature,
                                     const char * value) {
  std::cout << "PutDocumentFeature(" << feature << ", " << value << ")" << std::endl;

  document_data[feature] = value;
  return 0;
}

void DocumentSink::PrintContent() {
  for (const auto & elem : token_data) {
    std::cout << "Token id: " << elem.token_id << std::endl;
    std::cout << "  Token text: " << elem.token_text << std::endl;
    std::cout << "  Token len: " << elem.token_len << std::endl;
    std::cout
      << "  Token start pos: " << elem.token_start_pos << std::endl;

    for (const auto & feature_pair : elem.token_features) {
      std::cout
        << "  Features:" << elem.token_id << std::endl;
      std::cout
        << "    " << feature_pair.first
        << " : " << feature_pair.second << std::endl;
    }
  }
}
int main() {
  CTurboTextAnalysis *text_analyser = new CTurboTextAnalysis;

  std::string language = "en";
  std::string path = "../Data/";
  std::string text = u8"Obama visits Portugal. He is staying 3 days.";

  LoadOptions load_options;
  load_options.load_tagger = true;
  load_options.load_parser = false;
  load_options.load_morphological_tagger = true;
  load_options.load_entity_recognizer = true;
  load_options.load_semantic_parser = false;
  load_options.load_coreference_resolver = false;

  int retval = text_analyser->LoadLanguage(language, path, &load_options);
  if (retval != 0)
    std::cerr << "Lexicon initialization failed, check path and language" << std::endl;

  DocumentSink doc_sink; // the output of the Anlyse process will be fed to this

  AnalyseOptions options;
  options.use_tagger = true;
  options.use_parser = false;
  options.use_morphological_tagger = true;
  options.use_entity_recognizer = true;
  options.use_semantic_parser = false;
  options.use_coreference_resolver = false;

  retval = text_analyser->Analyse(language,
                                  text,
                                  &doc_sink,
                                  &options);
  if (retval != 0)
    std::cerr << "Error in CTurboTextAnalysis Analyse" << std::endl;

  //doc_sink.PrintContent();

  std::vector<std::vector<std::string>> sentences_words = { { "Obama", "visits", "Portugal", "." },
  { "He", "is", "staying", "3", "days","." } };
  std::vector<int> sentence_start_positions = { 0, 23 };
  std::vector<std::vector<int> > sentences_start_positions
    = { { 0, 6, 13, 21 },
    { 0, 3, 6, 14, 16, 20 } };
  std::vector<std::vector<int> > sentences_end_position
    = { { 5, 12, 21, 22 },
    { 2, 5, 13, 15, 20, 21 } };

  retval = text_analyser->Analyse(language,
                                  sentences_words,
                                  {},
                                  sentence_start_positions,
                                  sentences_start_positions,
                                  sentences_end_position,
                                  &doc_sink,
                                  &options);
  if (retval != 0)
    std::cerr << "Error in CTurboTextAnalysis Analyse" << std::endl;

  //doc_sink.PrintContent();

  delete text_analyser;
  return 0;
}
