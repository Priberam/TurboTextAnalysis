/*
 * TurboTokenizer.h
 *
 *  Created on: Jan 21, 2015
 *      Author: atm
 */

#ifndef TURBOTOKENIZER_H_
#define TURBOTOKENIZER_H_

#include <string>
#include <vector>
#include <algorithm>
#include "Alphabet.h"
#include "Tokenizer.h"

class TurboTokenContraction {
public:
  int GetNumWords() { return (int)words_.size(); }
  const std::vector<std::string> &words() { return words_; }
  const std::vector<int> &start_positions() { return start_positions_; }
  const std::vector<int> &end_positions() { return end_positions_; }
  void set_words(const std::vector<std::string> &words) { words_ = words; }
  void set_start_positions(const std::vector<int> &start_positions) {
    start_positions_ = start_positions;
  }
  void set_end_positions(const std::vector<int> &end_positions) {
    end_positions_ = end_positions;
  }

protected:
  std::vector<std::string> words_;
  std::vector<int> start_positions_;
  std::vector<int> end_positions_;
};

class TurboTokenizer : public Tokenizer {
public:
  struct TaskOptions {
    bool break_token_on_hyphen{ true };
  };
  TurboTokenizer() {};
  TurboTokenizer(const TaskOptions & options) : task_options_(options) {};
  virtual ~TurboTokenizer() {
    for (int i = 0; i < contractions_.size(); ++i) {
      delete contractions_[i];
    }
  }

  void LoadAbbreviations(const std::string filepath);
  void LoadContractions(const std::string filepath);
  void LoadContractionSuffixes(const std::string filepath);

  void SplitSentences(const std::string &text,
                      std::vector<std::string>* sentences,
                      std::vector<int>* start_positions,
                      std::vector<int>* end_positions);

  void TokenizeWords(const std::string &sentence,
                     std::vector<std::string>* words,
                     std::vector<int>* start_positions,
                     std::vector<int>* end_positions);

protected:
  void LookForSubwords(const std::string &sentence,
                       int start_position,
                       int end_position,
                       std::vector<std::string>* words,
                       std::vector<int>* word_start_positions,
                       std::vector<int>* word_end_positions);

  // Move to next word (which may have punctuation symbols attached to it),
  // starting from text[start_position].
  void GetNextWord(const std::string &text,
                   int start_position,
                   int *word_start_position,
                   int *word_end_position);

  // Eat whitespaces starting from text[start_position] and puts the position
  // of the first non-whitespace in end_position.
  void EatWhitespaces(const std::string &text,
                      int start_position, int *end_position);

  // Same, for right brackets or quotation marks.
  void EatRightBracketsAndQuotationMarks(const std::string &text,
                                         int start_position,
                                         int *end_position);

  // Eat non-whitespaces starting from text[mid_position] backwards and puts the
  // position of the first whitespace in start_position.
  void GetPreviousWhitespace(const std::string &text,
                             int *start_position, int mid_position);

  // Eat non-whitespaces starting from text[start_position] and puts the
  // position of the first whitespace in end_position.
  void GetNextWhitespace(const std::string &text,
                         int start_position, int *end_position);

  // Returns true if there is a valid sentence terminator at (or slightly
  // after) terminator_position. If necessary, adjusts the terminator
  // position (for example, if there are closing brackets or quotes after
  // a punctuation sign.
  bool ValidateSentenceTerminator(const std::string &text,
                                  int start_position, 
                                  int *terminator_position);

  // Returns true if there is, for each opening bracket/quotation mark, 
  // a valid closing bracket/quotation mark.
  bool ValidateTextBracketsQuotationsCoherence(const std::string & text);
    
    // Returns true if there is a valid closing bracket/quotation mark within 
  // the desired window for the opening bracket/quotation mark at start_position.
  bool ValidateBracketsQuotationsClosureLookAhead(const std::string & text,
                                                  int start_position,
                                                  int max_lookahead_window = 500);

  // Get alphanumeric word boundaries.
  void GetWordBoundaries(const std::string &text, int position,
                         int lower_bound_start, int upper_bound_end,
                         int *start_position, int *end_position);

protected:
  void AddWordToken(const std::string &word,
                    int start_position,
                    int end_position,
                    std::vector<std::string>* words,
                    std::vector<int>* start_positions,
                    std::vector<int>* end_positions) {
    words->push_back(word);
    start_positions->push_back(start_position);
    end_positions->push_back(end_position);
  }

  // Matches against a given pattern. "position" is the start position
  // within "word". Returns "true" if there was a match and the
  // length of the match.
  bool IsPattern(const std::string &word, const std::string &pattern,
                 int position, int *length) {
    if (0 == word.compare(position, pattern.length(), pattern)) {
      *length = (int)pattern.length();
      return true;
    }
    return false;
  }

  size_t FindFirstOf(const std::string& input_text,
                     std::vector<std::string> delims,
                     int position,
                     int * length);

  bool IsSentenceStartSymbol(const std::string &word,
                             int position,
                             int *length);
  bool IsSentenceEndSymbol(const std::string &word,
                           int position,
                           int *length);
  bool IsLeftBracket(const std::string &word,
                     int position,
                     int *length);
  bool IsRightBracket(const std::string &word,
                      int position,
                      int *length);

  bool IsColon(char c) { return c == ':'; }
  bool IsSemiColon(char c) { return c == ';'; }
  bool IsComma(char c) { return c == ','; }
  bool IsPeriod(char c) { return c == '.'; }
  bool IsExclamationMark(char c) { return c == '!'; }
  bool IsQuestionMark(char c) { return c == '?'; }
  bool IsHyphen(char c) { return c == '-'; }

  std::string GetBracketsQuotationsClosingSymbol(const std::string & input,
                               bool *matched = nullptr);

  bool IsParagrapgh(const std::string &word, int position, int *length);
  bool IsWhitespace(const std::string &word, int position, int *length);
  bool IsApostrophe(const std::string &word, int position, int *length);
  bool IsLRSingleQuotationMark(const std::string &word, int position,
                               int *length);
  bool IsLeftSingleQuotationMark(const std::string &word, int position,
                                 int *length);
  bool IsRightSingleQuotationMark(const std::string &word, int position,
                                  int *length);
  bool IsSingleQuotationMark(const std::string &word, int position,
                             int *length);
  bool IsLRDoubleQuotationMark(const std::string &word, int position,
                               int *length);
  bool IsLeftDoubleQuotationMark(const std::string &word, int position,
                                 int *length);
  bool IsRightDoubleQuotationMark(const std::string &word, int position,
                                  int *length);
  bool IsDoubleQuotationMark(const std::string &word, int position,
                             int *length);
  bool IsQuotationMark(const std::string &word, int position, int *length);
  bool IsEllipsis(const std::string &word, int position, int *length);
  bool IsEnDash(const std::string &word, int position, int *length);
  bool IsEmDash(const std::string &word, int position, int *length);
  bool IsDash(const std::string &word, int position, int *length);

  bool IsUpperCase(char c) { return (c >= 'A' && c <= 'Z'); }
  bool IsLowerCase(char c) { return (c >= 'a' && c <= 'z'); }
  bool IsLetter(char c) { return IsUpperCase(c) || IsLowerCase(c); }
  bool IsDigit(char c) { return (c >= '0' && c <= '9'); }
  // Note: underscore is considered alphanumeric (also in Python regexps).
  bool IsAlphanumeric(char c) { return IsLetter(c) || IsDigit(c) || c == '_'; }
  bool IsPunctuation(const std::string & text,
                     int position,
                     int *length);
  bool AllUpperCase(const char* word, int len) {
    for (int i = 0; i < len; ++i) {
      if (!IsUpperCase(word[i])) return false;
    }
    return true;
  }

  bool AllLowerCase(const char* word, int len) {
    for (int i = 0; i < len; ++i) {
      if (!IsLowerCase(word[i])) return false;
    }
    return true;
  }

  bool IsCapitalized(const char* word, int len) {
    if (len <= 0) return false;
    return IsUpperCase(word[0]);
  }

  bool IsMixedCase(const char* word, int len) {
    if (len <= 0) return false;
    if (!IsLowerCase(word[0])) return false;
    for (int i = 1; i < len; ++i) {
      if (IsUpperCase(word[i])) return true;
    }
    return false;
  }

  bool EndsWithPeriod(const char* word, int len) {
    if (len <= 0) return false;
    return IsPeriod(word[len - 1]);
  }

  bool HasInternalPeriod(const char* word, int len) {
    if (len <= 0) return false;
    for (int i = 0; i < len - 1; ++i) {
      if (IsPeriod(word[i])) return true;
    }
    return false;
  }

  bool HasInternalPunctuation(const char* word, int len,
                              int *matched_length) {
    if (len <= 0) return false;
    for (int i = 0; i < len - 1; ++i) {
      if (IsPunctuation(word, i, matched_length)) return true;
    }
    return false;
  }

  int CountDigits(const char* word, int len) {
    int num_digits = 0;
    for (int i = 0; i < len; ++i) {
      if (IsDigit(word[i])) ++num_digits;
    }
    return num_digits;
  }

  bool HasUpperCaseLetters(const char* word, int len) {
    for (int i = 0; i < len; ++i) {
      if (IsUpperCase(word[i])) return true;
    }
    return false;
  }

  bool HasHyphen(const char* word, int len) {
    for (int i = 0; i < len; ++i) {
      if (IsHyphen(word[i])) return true;
    }
    return false;
  }

  bool AllDigits(const char* word, int len) {
    for (int i = 0; i < len; ++i) {
      if (!IsDigit(word[i])) return false;
    }
    return true;
  }

  bool AllDigitsWithPunctuation(const char* word, int len,
                                int *matched_length) {
    bool has_digits = false;
    bool has_punctuation = false;
    for (int i = 0; i < len; ++i) {
      if (IsDigit(word[i])) {
        has_digits = true;
      } else if (IsPunctuation(word, i, matched_length)) {
        has_punctuation = true;
      } else {
        return false;
      }
    }
    return has_digits && has_punctuation;
  }

  bool IsContractionSuffix(const std::string &word,
                           std::string *canonical_suffix);

  bool HasContractionSuffix(const std::string &word, int *position);

  TurboTokenContraction* LookupContraction(const std::string &word) {
    std::string word_lc = word;
    // Note: the lower-case transformation below will not work for UTF-8 strings.
    std::transform(word.begin(), word.end(), word_lc.begin(), ::tolower);
    int id = contraction_dictionary_.Lookup(word_lc + "#LC");
    return (id >= 0) ? contractions_[id] : NULL;
  }

protected:
  Alphabet abbreviation_dictionary_;
  std::vector<bool> case_sensitive_abbreviations_;

  Alphabet contraction_dictionary_;
  std::vector<TurboTokenContraction*> contractions_;

  Alphabet contraction_suffix_dictionary_;
  std::vector<std::string> canonical_suffixes_;

  TaskOptions task_options_;
};

#endif /* TURBOTOKENIZER_H_ */
