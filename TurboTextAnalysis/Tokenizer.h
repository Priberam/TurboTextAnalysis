/*
*  Tokenizer.h
*
*  Created on: May 17, 2017
*      Author: dan
*/
#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>

class Tokenizer {
public:
  virtual  void SplitSentences(const std::string &text,
                               std::vector<std::string>* sentences,
                               std::vector<int>* start_positions,
                               std::vector<int>* end_positions) = 0;

  virtual void TokenizeWords(const std::string &sentence,
                             std::vector<std::string>* words,
                             std::vector<int>* start_positions,
                             std::vector<int>* end_positions) = 0;

protected:
  bool IsPotentialWebSite(std::string s) {
    for (const auto & web_word : web_words_) {
      if (s.find(web_word) != std::string::npos)
        return true;
    }
    return false;
  }
  bool IsPotentialEmail(std::string s) {
    auto at_pos = s.find("@");
    if (at_pos != std::string::npos) {
      if (at_pos > 0 && at_pos < s.size() - 3) {
        auto dot_pos = s.substr(at_pos).find(".");
        if (dot_pos != std::string::npos && dot_pos - at_pos > 1 && dot_pos < s.size() - 1) {
          return true;
        } else
          return false;
      } else
        return false;
    } else
      return false;
  }

  std::unordered_set<std::string> web_words_ =
  { "http:","https:",
    "www", "ww3",
    ".edu", ".com", ".net", ".gov", ".org",
    ".html"
  };
};

#endif