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
};

#endif