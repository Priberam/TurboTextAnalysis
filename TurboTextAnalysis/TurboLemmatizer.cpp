//This module is part of “Priberam’s TurboTextAnalysis”, a TurboParser's wrapper for easy text analysis, allowing it to be readily used in production systems.
//Copyright 2018 by PRIBERAM INFORMÁTICA, S.A. - www.priberam.com
//Usage subject to The terms & Conditions of the "Priberam TurboTextAnalysis OS Software License" available at https://www.priberam.pt/docs/Priberam_TurboTextAnalysis_OS_Software_License.pdf

#include "TurboLemmatizer.h"
#include "StringUtils.h"
#include "glog/logging.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>


void TurboLemmatizer::LoadLemmas(const std::string filepath) {
  std::ifstream is;
  std::string line;

  forms_and_pos_to_lemmas_.clear();

  LOG(INFO) << "Loading lemmas...";

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
      std::string form;
      std::string pos;
      std::string lemma;
      if (fields.size() < 3) {
        LOG_IF(INFO, fields.size() == 3)
          << "Warning: Incorrect format. Form\tPOS\tLemma was expected.";
        continue;
      } else {
        form = fields[0];
        pos = fields[1];
        lemma = fields[2];

        forms_and_pos_to_lemmas_[std::make_pair(form, pos)] = lemma;
      }
    }
  }
  is.close();
};

void TurboLemmatizer::LemmatizeSentence(const std::vector<std::string> &forms,
                                        const std::vector<std::string> &pos,
                                        std::vector<std::string>* lemmas) {
  CHECK(forms.size() == pos.size());
  if (lemmas->size() != forms.size()) {
    lemmas->clear();
    lemmas->resize(forms.size());
  }
  for (int i = 0; i < forms.size(); i++) {
    auto it = forms_and_pos_to_lemmas_.find(
      std::pair<std::string, std::string>(forms[i], pos[i]));
    if (it != forms_and_pos_to_lemmas_.end())
      (*lemmas)[i] = it->second;
    else
      (*lemmas)[i] = forms[i];
  }
}
