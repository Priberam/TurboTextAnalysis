//This module is part of “Priberam’s TurboTextAnalysis”, a TurboParser's wrapper for easy text analysis, allowing it to be readily used in production systems.
//Copyright 2018 by PRIBERAM INFORMÁTICA, S.A. - www.priberam.com
//Usage subject to The terms & Conditions of the "Priberam TurboTextAnalysis OS Software License" available at https://www.priberam.pt/docs/Priberam_TurboTextAnalysis_OS_Software_License.pdf

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
#include <unordered_map>
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
