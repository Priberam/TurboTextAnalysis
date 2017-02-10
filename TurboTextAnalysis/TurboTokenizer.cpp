#include "TurboTokenizer.h"
#include "StringUtils.h"
#include "glog/logging.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>

//#define ORIGINAL_PENN_TREEBANK_TOKENIZER

// TODO(atm): handle non-ASCII apostrophe.
// TODO(atm): handle non-ASCII non-breaking space.
const std::string kTurboSentenceStartSymbols = "¿¡";
const std::string kTurboSentenceEndSymbols = ".!?";
const std::string kTurboAbbreviatonMarkers = ".";
const std::string kTurboWhitespaces = " \t\r\n\f\v"; // TODO: add non-breaking space.
const std::string kTurboLeftBrackets = "{[(<";
const std::string kTurboRightBrackets = "}])>";

const char *kTurboPunctuationSymbols = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

const std::string kTurboNonBreakingSpace = " ";

const std::string kTurboApostrophe = "’";
const std::string kTurboManualApostrophe = "'";

const std::string kTurboLeftFrenchQuotationMark = "«";
const std::string kTurboRightFrenchQuotationMark = "»";
const std::string kTurboLeftDoubleQuotationMark = "“";
const std::string kTurboRightDoubleQuotationMark = "”";
const std::string kTurboLeftSingleQuotationMark = "‘";
const std::string kTurboRightSingleQuotationMark = "’";
const std::string kTurboTwoLeftSingleQuotationMarks = "‘‘";
const std::string kTurboTwoRightSingleQuotationMarks = "’’";
const std::string kTurboSingleQuotationMark = "'";
const std::string kTurboDoubleQuotationMark = "\"";
const std::string kTurboManualDoubleQuotationMark = "''";
const std::string kTurboManualLeftDoubleQuotationMark = "``";
const std::string kTurboManualLeftSingleQuotationMark = "`";

const std::string kTurboManualEllipsis = "...";
const std::string kTurboEllipsisSymbol = "…";
const std::string kTurboManualEnDash = "--";
const std::string kTurboEnDashSymbol = "–";
const std::string kTurboManualEmDash = "---";
const std::string kTurboEmDashSymbol = "—";

bool TurboTokenizer::IsWhitespace(const std::string &word,
                                  int position,
                                  int *length) {
#ifdef ORIGINAL_PENN_TREEBANK_TOKENIZER
  if (position >= word.length()) return false;
  for (int i = 0; i < kTurboWhitespaces.size(); ++i) {
    if (word[position] == kTurboWhitespaces[i]) {
      *length = 1;
      return true;
    }
  }
#else
  if (position >= word.length()) return false;
  for (int i = 0; i < kTurboWhitespaces.size(); ++i) {
    if (word[position] == kTurboWhitespaces[i]) {
      *length = 1;
      return true;
    }
  }
  return IsPattern(word, kTurboNonBreakingSpace, position, length);
#endif
}

bool TurboTokenizer::IsApostrophe(const std::string &word,
                                  int position,
                                  int *length) {
#ifdef ORIGINAL_PENN_TREEBANK_TOKENIZER
  return IsPattern(word, kTurboManualApostrophe, position, length);
#else
  return (IsPattern(word, kTurboApostrophe, position, length) ||
          IsPattern(word, kTurboManualApostrophe, position, length));
#endif
}

bool TurboTokenizer::IsSingleQuotationMark(const std::string &word,
                                           int position,
                                           int *length) {
#ifdef ORIGINAL_PENN_TREEBANK_TOKENIZER
  return IsPattern(word, kTurboSingleQuotationMark, position, length);
#else
  return (IsPattern(word, kTurboSingleQuotationMark, position, length) ||
          IsPattern(word, kTurboLeftSingleQuotationMark, position, length) ||
          IsPattern(word, kTurboManualLeftSingleQuotationMark, position,
                    length) ||
          IsPattern(word, kTurboRightSingleQuotationMark, position, length));
#endif
}

bool TurboTokenizer::IsDoubleQuotationMark(const std::string &word,
                                           int position,
                                           int *length) {
#ifdef ORIGINAL_PENN_TREEBANK_TOKENIZER
  return (IsPattern(word, kTurboDoubleQuotationMark, position, length) ||
          IsPattern(word, kTurboManualDoubleQuotationMark, position, length) ||
          IsPattern(word, kTurboManualLeftDoubleQuotationMark, position,
                    length));
#else
  return (IsPattern(word, kTurboDoubleQuotationMark, position, length) ||
          IsPattern(word, kTurboManualDoubleQuotationMark, position, length) ||
          IsPattern(word, kTurboLeftDoubleQuotationMark, position, length) ||
          IsPattern(word, kTurboTwoLeftSingleQuotationMarks, position,
                    length) ||
          IsPattern(word, kTurboLeftFrenchQuotationMark, position,
                    length) ||
          IsPattern(word, kTurboManualLeftDoubleQuotationMark, position,
                    length) ||
          IsPattern(word, kTurboRightDoubleQuotationMark, position, length) ||
          IsPattern(word, kTurboTwoRightSingleQuotationMarks, position,
                    length) ||
          IsPattern(word, kTurboRightFrenchQuotationMark, position,
                    length));
#endif
}

bool TurboTokenizer::IsQuotationMark(const std::string &word,
                                     int position,
                                     int *length) {
  // Check first if it's a double quotation mark because it sometimes includes
  // a single quotation mark as a substring.
  return (IsDoubleQuotationMark(word, position, length) ||
          IsSingleQuotationMark(word, position, length));
}

bool TurboTokenizer::IsEllipsis(const std::string &word, int position,
                                int *length) {
#ifdef ORIGINAL_PENN_TREEBANK_TOKENIZER
  return IsPattern(word, kTurboManualEllipsis, position, length);
#else
  return (IsPattern(word, kTurboManualEllipsis, position, length) ||
          IsPattern(word, kTurboEllipsisSymbol, position, length));
#endif
}

bool TurboTokenizer::IsEnDash(const std::string &word, int position,
                              int *length) {
#ifdef ORIGINAL_PENN_TREEBANK_TOKENIZER
  return IsPattern(word, kTurboManualEnDash, position, length);
#else
  return (IsPattern(word, kTurboManualEnDash, position, length) ||
          IsPattern(word, kTurboEnDashSymbol, position, length));
#endif

}

bool TurboTokenizer::IsEmDash(const std::string &word, int position,
                              int *length) {
#ifdef ORIGINAL_PENN_TREEBANK_TOKENIZER
  return IsPattern(word, kTurboManualEmDash, position, length);
#else
  return (IsPattern(word, kTurboManualEmDash, position, length) ||
          IsPattern(word, kTurboEmDashSymbol, position, length));
#endif
}

bool TurboTokenizer::IsDash(const std::string &word, int position,
                            int *length) {
  // Need to test "em dash" first, because it may contain a "en dash" inside
  // if it's written manually.
  return (IsEmDash(word, position, length) || IsEnDash(word, position, length));
}

bool TurboTokenizer::IsWhitespace(char c) {
  return kTurboWhitespaces.find(c) != string::npos;
}

bool TurboTokenizer::IsLeftBracket(char c) {
  return kTurboLeftBrackets.find(c) != string::npos;
}

bool TurboTokenizer::IsRightBracket(char c) {
  return kTurboRightBrackets.find(c) != string::npos;
}

void TurboTokenizer::LoadAbbreviations(const std::string filepath) {
  std::ifstream is;
  std::string line;

  abbreviation_dictionary_.clear();
  case_sensitive_abbreviations_.clear();

  LOG(INFO) << "Loading abbreviations...";

  abbreviation_dictionary_.AllowGrowth();

  is.open(filepath.c_str(), ifstream::in);
  CHECK(is.good()) << "Could not open "
                   << filepath << ".";
  if (is.is_open()) {
    while (!is.eof()) {
      getline(is, line);
      TrimComments("#", &line);
      if (line == "") continue; // Ignore blank lines.
      std::vector<std::string> fields;
      StringSplit(line, " \t", &fields, true); // Break on tabs or spaces.
      std::string abbreviation;
      bool case_sensitive = false;
      if (fields.size() < 2) {
        CHECK_EQ(fields.size(), 1);
        abbreviation = fields[0];
       // LOG(INFO) << "Warning: abbreviation " << abbreviation
       //           << " missing the \"case-sensitive\" option! Taken as \"0\".";
      } else {
        abbreviation = fields[0];
        case_sensitive = (fields[1] != "0");
      }
      if (abbreviation.length() > 0 &&
          abbreviation[abbreviation.length()-1] != '.')
        abbreviation += ".";
      // Note: the lower-case transformation below will not work for UTF-8
      // strings.
      if (!case_sensitive)
        std::transform(abbreviation.begin(), abbreviation.end(),
                       abbreviation.begin(), ::tolower);
      if (abbreviation_dictionary_.Contains(abbreviation)) {
        LOG(INFO) << "Warning: repeated abbreviation: " << abbreviation << ".";
      } else {
        int id;
        if (case_sensitive)
          id = abbreviation_dictionary_.Insert(abbreviation);
        else
          id = abbreviation_dictionary_.Insert(abbreviation + "#LC");
        case_sensitive_abbreviations_.resize(id + 1, false);
        case_sensitive_abbreviations_[id] = case_sensitive;
      }
    }
  }
  is.close();

  abbreviation_dictionary_.StopGrowth();
}

void TurboTokenizer::LoadContractions(const std::string filepath) {
  std::ifstream is;
  std::string line;

  contraction_dictionary_.clear();
  contractions_.clear();

  LOG(INFO) << "Loading contractions...";

  contraction_dictionary_.AllowGrowth();

  is.open(filepath.c_str(), ifstream::in);
  CHECK(is.good()) << "Could not open "
                   << filepath << ".";
  if (is.is_open()) {
    while (!is.eof()) {
      getline(is, line);
      TrimComments("#", &line);
      if (line == "") continue; // Ignore blank lines.
      std::vector<std::string> fields;
      StringSplit(line, " \t", &fields, true); // Break on tabs or spaces.
      CHECK_GE(fields.size(), 2);
      bool case_sensitive = false;
      std::string contraction = fields[0];
      int num_words = 0;
      std::stringstream ss(fields[1]);
      ss >> num_words;
      CHECK_EQ(fields.size(), 2+3*num_words) << contraction;
      TurboTokenContraction* token_contraction = new TurboTokenContraction;
      std::vector<std::string> contraction_words;
      std::vector<int> start_positions;
      std::vector<int> end_positions;
      for (int k = 0; k < num_words; ++k) {
        contraction_words.push_back(fields[2+k]);
      }
      for (int k = 0; k < num_words; ++k) {
      std::stringstream ssl;
        int start, end;
        
    std::stringstream(fields[2 + num_words + 2 * k]) >> start;
        start_positions.push_back(start);
    std::stringstream(fields[2 + num_words + 2 * k + 1]) >> end;
        end_positions.push_back(end);
      }
      token_contraction->set_words(contraction_words);
      token_contraction->set_start_positions(start_positions);
      token_contraction->set_end_positions(end_positions);

      // Note: the lower-case transformation below will not work for UTF-8
      // strings.
      if (!case_sensitive)
        std::transform(contraction.begin(), contraction.end(),
                       contraction.begin(), ::tolower);
      if (contraction_dictionary_.Contains(contraction)) {
        LOG(INFO) << "Warning: repeated contraction: " << contraction << ".";
      } else {
        int id;
        if (case_sensitive) {
          id = contraction_dictionary_.Insert(contraction);
        } else {
          id = contraction_dictionary_.Insert(contraction + "#LC");
        }
        contractions_.resize(id + 1, NULL);
        contractions_[id] = token_contraction;
      }
    }
  }
  is.close();

  abbreviation_dictionary_.StopGrowth();
}

void TurboTokenizer::LoadContractionSuffixes(const std::string filepath) {
  std::ifstream is;
  std::string line;

  contraction_suffix_dictionary_.clear();
  canonical_suffixes_.clear();

  LOG(INFO) << "Loading contraction suffixes...";

  contraction_suffix_dictionary_.AllowGrowth();

  is.open(filepath.c_str(), ifstream::in);
  CHECK(is.good()) << "Could not open "
                   << filepath << ".";
  if (is.is_open()) {
    while (!is.eof()) {
      getline(is, line);
      TrimComments("#", &line);
      if (line == "") continue; // Ignore blank lines.
      std::vector<std::string> fields;
      StringSplit(line, " \t", &fields, true); // Break on tabs or spaces.
      std::string suffix;
      std::string canonical_suffix;
      CHECK_EQ(fields.size(), 2);
      suffix = fields[0];
      canonical_suffix = fields[1];
      bool case_sensitive = false;
      // Note: the lower-case transformation below will not work for UTF-8
      // strings.
      if (!case_sensitive)
        std::transform(suffix.begin(), suffix.end(),
                       suffix.begin(), ::tolower);
      if (contraction_suffix_dictionary_.Contains(suffix)) {
        LOG(INFO) << "Warning: repeated suffix: " << suffix << ".";
      } else {
        int id;
        if (case_sensitive) {
          id = contraction_suffix_dictionary_.Insert(suffix);
        } else {
          id = contraction_suffix_dictionary_.Insert(suffix + "#LC");
        }
        canonical_suffixes_.resize(id + 1, "");
        canonical_suffixes_[id] = canonical_suffix;
      }
    }
  }
  is.close();

  contraction_suffix_dictionary_.StopGrowth();
}

void TurboTokenizer::SplitSentences(const std::string &text,
                                    bool split_on_new_lines,
                                    bool split_on_empty_lines,
                                    std::vector<std::string>* sentences,
                                    std::vector<int>* start_positions,
                                    std::vector<int>* end_positions) {
  std::string delim = kTurboSentenceEndSymbols;
  int position = 0;
  int start_position = 0;
  int delim_position = 0;
  EatWhitespaces(text, position, &start_position);
  position = start_position;
  while ((delim_position = ((int)text.find_first_of(delim, position))) != (int)text.npos) {
    // Validate delimiter (e.g. check if it's not part of an abbreviation).
    if (ValidateSentenceTerminator(text, start_position, &delim_position)) {
      std::string sentence = text.substr(start_position,
                                         delim_position - start_position);
      int end_position = delim_position+1;
      sentences->push_back(sentence);
      start_positions->push_back(start_position);
      end_positions->push_back(end_position);

      position = delim_position+1;
      EatWhitespaces(text, position, &start_position);
      position = start_position;
    } else {
      position = delim_position+1;
    }
  }

  // Last chunk.
  //start_position = position;
  int end_position = (int)text.length();
  if (start_position < end_position) {
    std::string sentence = text.substr(start_position);
    sentences->push_back(sentence);
    start_positions->push_back(start_position);
    end_positions->push_back(end_position);
  }
}

void TurboTokenizer::TokenizeWords(const std::string &sentence,
                                   std::vector<std::string>* words,
                                   std::vector<int>* start_positions,
                                   std::vector<int>* end_positions) {
  int start_position = 0;
  int word_start_position;
  int word_end_position;
  GetNextWord(sentence, start_position, &word_start_position,
              &word_end_position);
  while (word_start_position < sentence.length()) {
    const std::string &word =
      sentence.substr(word_start_position,
                      word_end_position - word_start_position);
    //std::cerr << "Initial word: " << word << std::endl;

    // Split this word into subwords, if needed.
    std::vector<std::string> subwords;
    std::vector<int> subword_start_positions;
    std::vector<int> subword_end_positions;
    LookForSubwords(sentence, word_start_position, word_end_position,
                    &subwords, &subword_start_positions,
                    &subword_end_positions);
    //for (int k = 0; k < subwords.size(); ++k) {
    //  std::cerr << "Subword: " << subwords[k] << std::endl;
    //}

    // Sort the subword positions.
    std::vector<std::pair<int, int> > sorted_positions;
    for (int i = 0; i < subword_start_positions.size(); ++i) {
      sorted_positions.
        push_back(std::pair<int,int>(subword_start_positions[i], i));
    }
    std::sort(sorted_positions.begin(), sorted_positions.end());
    std::vector<std::string> sorted_subwords(subwords.size(), "");
    std::vector<int> sorted_subword_start_positions(subwords.size(), -1);
    std::vector<int> sorted_subword_end_positions(subwords.size(), -1);
    for (int i = 0; i < sorted_positions.size(); ++i) {
      int k = sorted_positions[i].second;
      sorted_subword_start_positions[i] = sorted_positions[i].first;
      sorted_subword_end_positions[i] = subword_end_positions[k];
      sorted_subwords[i] = subwords[k];
    }

    // Insert the subwords and their positions, already sorted.
    words->insert(words->end(), sorted_subwords.begin(), sorted_subwords.end());
    start_positions->insert(start_positions->end(),
                            sorted_subword_start_positions.begin(),
                            sorted_subword_start_positions.end());
    end_positions->insert(end_positions->end(),
                          sorted_subword_end_positions.begin(),
                          sorted_subword_end_positions.end());

    //words->push_back(word);
    //start_positions->push_back(word_start_position);
    //end_positions->push_back(word_end_position);

    // Look for the next word.
    start_position = word_end_position;
    GetNextWord(sentence, start_position, &word_start_position,
                &word_end_position);
  }

  //std::cerr << "Tokenized sentence: ";
  //for (int i = 0; i < words->size(); ++i) {
  //  std::cerr << (*words)[i] + " ";
  //}
  //std::cerr << std::endl;
}

void TurboTokenizer::LookForSubwords(const std::string &sentence,
                                     int start_position,
                                     int end_position,
                                     std::vector<std::string>* words,
                                     std::vector<int>* word_start_positions,
                                     std::vector<int>* word_end_positions) {
  // If the word is empty, there is nothing to look for.
  if (end_position <= start_position) return;

  // The word in this span.
  std::string word = sentence.substr(start_position,
                                     end_position - start_position);

  //std::cout << "Processing: " << word << endl;

  // For each pattern expression, proceed left to right trying to match it.
  for (int position = 0; position < end_position - start_position; ++position) {
    // Current position in the global buffer.
    int current_position = start_position + position;
    int match_length = 0;
    int previous_match_length;
    // Create a token for quotation marks.
    // TODO: handle quotation strings such as "\'\'" or "``".
    // TODO: do the same for single quotation marks, which are incorrectly
    // parsed by the origin Penn Treebank tokenizer (e.g. "'gifts '").
    if (IsDoubleQuotationMark(word, position, &match_length)) {
      if (//IsLeftQuotationMark(word[position]) ||
          current_position == 0 ||
          IsWhitespace(sentence, current_position - 1,
                       &previous_match_length) ||
          IsLeftBracket(sentence[current_position - 1])) {
        AddWordToken("``", 
                     current_position, 
                     current_position + match_length,
                     words, 
                     word_start_positions, 
                     word_end_positions);
        position += match_length;

        // Now take care of the reminiscent part of the word.
        LookForSubwords(sentence, 
                        start_position + position, 
                        end_position,
                        words, 
                        word_start_positions, 
                        word_end_positions);

        end_position = current_position;
        word = sentence.substr(start_position, end_position - start_position);
        break;
      }
      // Else handle the other quotations...?
    }
  }

  for (int position = 0; position < end_position - start_position; ++position) {
    // Current position in the global buffer.
    int current_position = start_position + position;
    int match_length = 0;

    // If it's a colon (":") or a comma (",") and it is not followed by a
    // digit, it becomes a token.
    if ((IsColon(word[position]) || IsComma(word[position])) &&
        current_position+1 < sentence.length() &&
        !IsDigit(sentence[current_position+1])) {
      match_length = 1;
      std::string token_word = word.substr(position, match_length);
      AddWordToken(token_word, 
                   current_position,
                   current_position + match_length,
                   words,
                   word_start_positions, 
                   word_end_positions);
      position += match_length;

      // Now take care of the reminiscent part of the word.
      LookForSubwords(sentence,
                      start_position + position,
                      end_position,
                      words, 
                      word_start_positions,
                      word_end_positions);

      end_position = current_position;
      word = sentence.substr(start_position, end_position - start_position);
      break;
    }
  }

  for (int position = 0; position < end_position - start_position; ++position) {
    // Current position in the global buffer.
    int current_position = start_position + position;
    int match_length = 0;

    // Ellipsis ("...") become a single token.
    // TODO: Handle the special ellipsis character.
    if (IsEllipsis(word, position, &match_length)) {
      AddWordToken("...", 
                   current_position, 
                   current_position + match_length,
                   words,
                   word_start_positions,
                   word_end_positions);
      position += match_length;

      // Now take care of the reminiscent part of the word.
      LookForSubwords(sentence, 
                      start_position + position,
                      end_position,
                      words, 
                      word_start_positions, 
                      word_end_positions);

      end_position = current_position;
      word = sentence.substr(start_position, end_position - start_position);
      break;
    }
  }

  for (int position = 0; position < end_position - start_position; ++position) {
    // Current position in the global buffer.
    int current_position = start_position + position;
    int match_length = 0;

    // Any character in ";@#$%&" becomes a single token.
    if ((IsSemiColon(word[position]) || word[position] == '@' ||
         word[position] == '#' || word[position] == '$' ||
         word[position] == '%' || word[position] == '&')) {
      match_length = 1;
      std::string token_word = word.substr(position, match_length);
      AddWordToken(token_word, 
                   current_position,
                   current_position + match_length,
                   words, 
                   word_start_positions,
                   word_end_positions);
      position += match_length;

      // Now take care of the reminiscent part of the word.
      LookForSubwords(sentence,
                      start_position + position,
                      end_position,
                      words,
                      word_start_positions,
                      word_end_positions);

      end_position = current_position;
      word = sentence.substr(start_position, end_position - start_position);
      break;
    }
  }

  for (int position = 0; position < end_position - start_position; ++position) {
    // Current position in the global buffer.
    int current_position = start_position + position;
    int match_length = 0;

    // Any period preceded by something which is not a period and followed by an
    // optional ending bracket (in "]})>") or quote (in "\"'"), optional
    // whitespace and end-of-sentence, becomes a token together with the
    // optional bracket.
    // Process ending point.
    // Note: we don't create a token together with the bracket.
    if (IsPeriod(word[position]) &&
        (current_position == 0 || !IsPeriod(sentence[current_position - 1]))) {
      int next_word_position = current_position + 1;
      
      EatRightBracketsAndQuotationMarks(sentence, next_word_position,
                                        &next_word_position);
      EatWhitespaces(sentence, next_word_position, &next_word_position);
      if (next_word_position >= sentence.length()) {
        // End of sentence.
        match_length = 1;
        AddWordToken(".", 
                     current_position,
                     current_position + match_length,
                     words,
                     word_start_positions,
                     word_end_positions);
        position += match_length;

        // Now take care of the reminiscent part of the word.
        LookForSubwords(sentence, 
                        start_position + position,
                        end_position,
                        words, 
                        word_start_positions, 
                        word_end_positions);

        end_position = current_position;
        word = sentence.substr(start_position, end_position - start_position);
        break;
      }
    }
  }

  for (int position = 0; position < end_position - start_position; ++position) {
    // Current position in the global buffer.
    int current_position = start_position + position;
    int match_length = 0;

    // Each punctuation in "!?" becomes a single token.
    if (IsExclamationMark(word[position]) || IsQuestionMark(word[position])) {
      match_length = 1;
      std::string token_word = word.substr(position, match_length);
      AddWordToken(token_word, 
                   current_position,
                   current_position + match_length,
                   words,
                   word_start_positions,
                   word_end_positions);
      position += match_length;

      // Now take care of the reminiscent part of the word.
      LookForSubwords(sentence,
                      start_position + position, 
                      end_position,
                      words, 
                      word_start_positions, 
                      word_end_positions);

      end_position = current_position;
      word = sentence.substr(start_position, end_position - start_position);
      break;
    }
  }

  for (int position = 0; position < end_position - start_position; ++position) {
    // Current position in the global buffer.
    int current_position = start_position + position;
    int match_length = 0;

    // A single quote which is not preceded by another single quote and
    // followed by whitespace becomes a single token.
    int previous_match_length, next_match_length;
    if (IsSingleQuotationMark(word, position, &match_length) &&
        (current_position == 0 ||
         !IsSingleQuotationMark(sentence, current_position - 1,
                                &previous_match_length)) &&
        (current_position + match_length >= sentence.length() ||
         IsWhitespace(sentence, current_position + match_length,
                      &next_match_length))) {
      std::string token_word = "'"; //word.substr(position, match_length);
      //std::cerr << "AddWordToken " << token_word << " " << current_position
      //          << " " << current_position + match_length << std::endl;
      AddWordToken(token_word,
                   current_position,
                   current_position + match_length,
                   words,
                   word_start_positions, 
                   word_end_positions);
      position += match_length;

      // Now take care of the reminiscent part of the word.
      LookForSubwords(sentence, 
                      start_position + position, 
                      end_position,
                      words, 
                      word_start_positions,
                      word_end_positions);

      end_position = current_position;
      word = sentence.substr(start_position, end_position - start_position);
      break;
    }
  }

  for (int position = 0; position < end_position - start_position; ++position) {
    // Current position in the global buffer.
    int current_position = start_position + position;
    int match_length = 0;

    // Any bracket in "<({[]})>" comes a single token.
    if (IsLeftBracket(word[position]) || IsRightBracket(word[position])) {
      match_length = 1;
      std::string token_word = word.substr(position, match_length);
      AddWordToken(token_word,
                   current_position,
                   current_position + match_length,
                   words,
                   word_start_positions,
                   word_end_positions);
      position += match_length;

      // Now take care of the reminiscent part of the word.
      LookForSubwords(sentence,
                      start_position + position,
                      end_position,
                      words,
                      word_start_positions,
                      word_end_positions);

      end_position = current_position;
      word = sentence.substr(start_position, end_position - start_position);
      break;
    }
  }

  for (int position = 0; position < end_position - start_position; ++position) {
    // Current position in the global buffer.
    int current_position = start_position + position;
    int match_length = 0;

    // Each dash "--" becomes a single token. TODO: also em dashes "---".
    if (IsDash(word, position, &match_length)) {
      //std::cerr << "match_length: " << match_length << std::endl;
      AddWordToken("--", 
                   current_position, 
                   current_position + match_length,
                   words,
                   word_start_positions, 
                   word_end_positions);
      position += match_length;

      // Now take care of the reminiscent part of the word.
      LookForSubwords(sentence, 
                      start_position + position,
                      end_position,
                      words, 
                      word_start_positions,
                      word_end_positions);
      for (int k = 0; k < words->size(); ++k) {
        //std::cerr << "subword found: " << (*words)[k] << std::endl;
      }

      end_position = current_position;
      //std::cerr << "start position: " << start_position << std::endl;
      //std::cerr << "end position: " << end_position << std::endl;
      word = sentence.substr(start_position, end_position - start_position);
      break;
    }
  }

  for (int position = 0; position < end_position - start_position; ++position) {
    // Current position in the global buffer.
    int current_position = start_position + position;
    int match_length = 0;

    // Every quotation mark ("\"") not previously detected as a
    // starting quote becomes a token and represented as "''".
    // Every quotation mark ("\"" or "''") not previously detected as a
    // starting quote and preceded by a non-whitespace becomes a token.
    // TODO: process also "''".
    if (IsDoubleQuotationMark(word, position, &match_length)) {
      std::string token_word = "''";
      //std::cerr << "AddWordToken " << token_word << " " << current_position
      //          << " " << current_position + match_length << std::endl;
      AddWordToken(token_word, 
                   current_position,
                   current_position + match_length,
                   words, 
                   word_start_positions, 
                   word_end_positions);
      position += match_length;

      // Now take care of the reminiscent part of the word.
      LookForSubwords(sentence, 
                      start_position + position, 
                      end_position,
                      words, 
                      word_start_positions, 
                      word_end_positions);

      end_position = current_position;
      word = sentence.substr(start_position, end_position - start_position);
      break;
    }
  }

  for (int position = 0; position < end_position - start_position; ++position) {
    // Current position in the global buffer.
    int current_position = start_position + position;
    int match_length = 0;

    // The word in this span.
    const std::string &contraction_word =
      sentence.substr(current_position, end_position - current_position);

    // English contractions like "'s", "'d", "'m", "'", "'ll", "'re", "'ve",
    // "n't" not preceded by whitespace or "'" and followed by whitespace or
    // EOS will be split (NOTE: lower or upper-cased).
    
    std::string canonical_suffix;
    if (IsContractionSuffix(contraction_word, &canonical_suffix) &&
        current_position - 1 >= 0 &&
        sentence[current_position - 1] != '\'') {

      match_length = (int)contraction_word.length();
      std::string token_word = canonical_suffix; //word.substr(position, match_length);
      AddWordToken(token_word,
                   current_position,
                   current_position + match_length,
                   words,
                   word_start_positions, 
                   word_end_positions);
      position += match_length;

      // Now take care of the reminiscent part of the word.
      LookForSubwords(sentence, 
                      start_position + position, 
                      end_position,
                      words, 
                      word_start_positions, 
                      word_end_positions);

      end_position = current_position;
      word = sentence.substr(start_position, end_position - start_position);
      break;
    }
  }

  if (start_position == end_position) return;

  // Current position in the global buffer.
  int current_position = start_position;
  int match_length = 0;

  // The word in this span.
  const std::string &current_word =
    sentence.substr(current_position, end_position - current_position);

  // Other English contraction specified directly (exact word match with
  // whitespace boundaries is necessary):
  // can+not
  // d+'ye
  // gim+me
  // gon+na
  // got+ta
  // lem+me
  // more+'n
  // wan+na
  // 't+is
  // 't+was
  TurboTokenContraction *contraction = LookupContraction(current_word);
  if (contraction) {
    for (int i = 0; i < contraction->GetNumWords(); ++i) {

      std::string token_word = contraction->words()[i];
      AddWordToken(token_word,
                   current_position + contraction->start_positions()[i],
                   current_position + contraction->end_positions()[i],
                   words, 
                   word_start_positions,
                   word_end_positions);
    }
  } else {
    // Regular word.
    //std::cerr << "Regular word: [" << current_word << "]" << std::endl;
    std::string token_word = current_word;
    AddWordToken(token_word, 
                 current_position,
                 end_position,
                 words,
                 word_start_positions,
                 word_end_positions);
  }
}

bool TurboTokenizer::IsContractionSuffix(const std::string &word,
                                         std::string *canonical_suffix) {
  std::string word_lc = word;
  // Note: the lower-case transformation below will not work for UTF-8 strings.
  std::transform(word.begin(), word.end(), word_lc.begin(), ::tolower);
  int id = contraction_suffix_dictionary_.Lookup(word_lc + "#LC");
  if (id >= 0) {
    *canonical_suffix = canonical_suffixes_[id];
    return true;
  }
  return false;
}

bool TurboTokenizer::HasContractionSuffix(const std::string &word,
                                          int *position) {
  // Deprecated.
  for (Alphabet::iterator iter = contraction_suffix_dictionary_.begin();
       iter != contraction_suffix_dictionary_.end();
       ++iter) {
    const std::string &contraction_suffix = iter->first;
    int len = (int)contraction_suffix.length();
    *position = (int)word.length() - len;
    if (word.substr(*position) == contraction_suffix) return true;
  }
  return false;
}

bool TurboTokenizer::ValidateSentenceTerminator(const std::string &text,
                                                int start_position,
                                                int *terminator_position) {
  int word_start_position, word_end_position;
  GetWordBoundaries(text, *terminator_position, start_position, (int)text.length()-1,
                    &word_start_position, &word_end_position);

  // There are alphanumeric characters after the termination mark; don't break.
  if (word_end_position > (*terminator_position) + 1) return false;

  if (IsPeriod(text[*terminator_position])) {
    // If there is a single character before don't break (could be a middle
    // initial).
    if (word_start_position == (*terminator_position) - 1) return false;

    const std::string &word =
      text.substr(word_start_position,
                  word_end_position - word_start_position);

    // Check if the word is an abbreviation.
    std::string word_lc = word;
    // Note: the lower-case transformation below will not work for UTF-8 strings.
    std::transform(word.begin(), word.end(), word_lc.begin(), ::tolower);

    int id = abbreviation_dictionary_.Lookup(word);
    int id_lc = abbreviation_dictionary_.Lookup(word_lc + "#LC");
    if (id >= 0 || id_lc >= 0) {
      //LOG(INFO) << "Cannot break on " << word << " (abbreviation).";
      return false;
    }
  }

  // Skip eventual right brackets and/or quotations.
  EatRightBracketsAndQuotationMarks(text, word_end_position,
                                    &word_end_position);
  *terminator_position = word_end_position;
  return true;
}

void TurboTokenizer::GetNextWord(const std::string &text,
                                 int start_position,
                                 int *word_start_position,
                                 int *word_end_position) {
  //std::cout << "GetNextWord " << start_position << " ";
  // Find beginning of word.
  EatWhitespaces(text, start_position, word_start_position);
  if (*word_start_position >= text.length()) {
    // No more words.
    *word_end_position = (int)text.length();
    //std::cerr << " --> " << *word_start_position << " " << *word_end_position << endl;
    return;
  }

  // Find end of word.
  GetNextWhitespace(text, *word_start_position, word_end_position);
  //std::cout << " -> " << *word_start_position << " " << *word_end_position << endl;
}

void TurboTokenizer::EatRightBracketsAndQuotationMarks(const std::string &text,
                                                       int start_position,
                                                       int *end_position) {
  int position = start_position;
  int match_length;
  while (position < text.length()) {
    if (IsQuotationMark(text, position, &match_length)) {
      position += match_length;
    } else if (IsRightBracket(text[position])) {
      ++position;
    } else {
      break;
    }
  }

  *end_position = position;
}

void TurboTokenizer::EatWhitespaces(const std::string &text,
                                    int start_position, int *end_position) {
#if 1
  int match_length = 1;
  int position = start_position;
  for (; position < text.length(); position += match_length) {
    match_length = 0;
    if (!IsWhitespace(text, position, &match_length)) break;
  }
  *end_position = position;
#else
  std::string delim = kTurboWhitespaces;
  int position = start_position;
  *end_position = text.find_first_not_of(delim, position);
  if (*end_position == text.npos)
    *end_position = text.length();
#endif
}

void TurboTokenizer::GetNextWhitespace(const std::string &text,
                                       int start_position, int *end_position) {
#if 1
  int position = start_position;
  for (; position < text.length(); ++position) {
    int match_length;
    if (IsWhitespace(text, position, &match_length)) break;
  }
  *end_position = position;
#else
  std::string delim = kTurboWhitespaces;
  int position = start_position;
  *end_position = text.find_first_of(delim, position);
  if (*end_position == text.npos) *end_position = text.length();
#endif
}

void TurboTokenizer::GetWordBoundaries(const std::string &text,
                                       int position,
                                       int lower_bound_start,
                                       int upper_bound_end,
                                       int *start_position,
                                       int *end_position) {
  // Get left boundary.
  const char *buffer = text.c_str();
  int i;
  for (i = position-1; i >= lower_bound_start; --i) {
    if (!IsAlphanumeric(buffer[i]) && !IsPeriod(buffer[i])) break;
  }
  *start_position = i+1;

  // Get right boundary.
  for (i = position+1; i <= upper_bound_end; ++i) {
    if (!IsAlphanumeric(buffer[i]) && !IsPeriod(buffer[i])) break;
  }
  *end_position = i;
}

bool TurboTokenizer::IsPunctuation(char c) {
  const char *symbol = kTurboPunctuationSymbols;
  while (*symbol != '\0') {
    if (c == *symbol) return true;
    ++symbol;
  }
  return false;
}
