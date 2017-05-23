/*
* ICUTokenizer.h
*
*  Created on: May 12, 2017
*      Author: dan
*/

#ifndef ICUTOKENIZER_H_
#define ICUTOKENIZER_H_

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "Tokenizer.h"

//Codes for the Representation of Names of Languages
//Codes arranged alphabetically by alpha - 3 / ISO 639 - 2 Code
// http://www.loc.gov/standards/iso639-2/php/code_list.php

class ICUTokenizer : public Tokenizer{
public:
  struct TaskOptions {};
  ICUTokenizer(const std::string &language) :
    m_language(language) {};
  ICUTokenizer(const std::string &language,
               const TaskOptions & options) :
    m_language(language),
    m_task_options(options) {};
  ~ICUTokenizer() {}

  void SplitSentences(const std::string &text,
                      std::vector<std::string>* sentences,
                      std::vector<int>* start_positions,
                      std::vector<int>* end_positions);

  void TokenizeWords(const std::string &sentence,
                     std::vector<std::string>* words,
                     std::vector<int>* start_positions,
                     std::vector<int>* end_positions);

  static const std::unordered_map<const char*, const char*>& ISO639_2Code_to_ISO639_1Code();
protected:
  std::string m_language;
  TaskOptions m_task_options;
};

#endif /* ICUTOKENIZER_H_ */
