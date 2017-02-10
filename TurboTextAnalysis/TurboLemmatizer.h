/*
* TurboTokenizer.h
*
*  Created on: Jan 21, 2015
*      Author: atm
*/

#ifndef TURBOLEMMATIZER_H_
#define TURBOLEMMATIZER_H_

#include <string>
#include <vector>
#include <algorithm>
#include "hashing.h"
#include "Alphabet.h"

class TurboLemmatizer {
public:
  TurboLemmatizer() {};
  ~TurboLemmatizer() {};

  void LoadLemmas(const std::string filepath);

  void LemmatizeSentence(const std::vector<std::string> &forms,
                         const std::vector<std::string> &pos,
                         std::vector<std::string>* lemmas);
protected:

  typedef HashablePair<std::string, std::string> FormAndPosPair;
  std::unordered_map<FormAndPosPair, std::string, FormAndPosPair, FormAndPosPair>
    forms_and_pos_to_lemmas_;
};

#endif /* TURBOLEMMATIZER_H_ */
