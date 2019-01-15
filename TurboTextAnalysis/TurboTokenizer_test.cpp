//This module is part of “Priberam’s TurboTextAnalysis”, a TurboParser's wrapper for easy text analysis, allowing it to be readily used in production systems.
//Copyright 2018 by PRIBERAM INFORMÁTICA, S.A. - www.priberam.com
//Usage subject to The terms & Conditions of the "Priberam TurboTextAnalysis OS Software License" available at https://www.priberam.pt/docs/Priberam_TurboTextAnalysis_OS_Software_License.pdf

#include "TurboTokenizer.h"
#include <iostream>

int main(int argc, char **argv) {
  TurboTokenizer tokenizer;
  tokenizer.LoadAbbreviations("english_abbreviations.txt");
  tokenizer.LoadContractionSuffixes("english_contraction_suffixes.txt");
  tokenizer.LoadContractions("english_contractions.txt");

  std::string flag = "";
  if (argc >= 2) {
    flag = argv[1];
  }

  if (flag == "--tokenize") {
    for (std::string line; std::getline(std::cin, line);) {
      std::vector<std::string> words;
      std::vector<int> start_positions;
      std::vector<int> end_positions;
      const std::string sentence = line;
      //std::cerr << "Tokenizing " << sentence << std::endl;
      tokenizer.TokenizeWords(sentence, 
                              &words, 
                              &start_positions, 
                              &end_positions);

      for (int i = 0; i < words.size(); ++i) {
        std::cout << words[i] + " ";
      }
      std::cout << std::endl;
    }
  } else {
    // Sentence splitting.
    std::string document = "";
    for (std::string line; std::getline(std::cin, line);) {
      if (line == "") {
        // Split document into sentences.
        std::vector<std::string> sentences;
        std::vector<int> start_positions;
        std::vector<int> end_positions;
        tokenizer.SplitSentences(document,  
                                 &sentences,
                                 &start_positions, 
                                 &end_positions);
        for (int i = 0; i < sentences.size(); ++i) {
          std::vector<std::string> words;
          std::vector<int> word_start_positions;
          std::vector<int> word_end_positions;
          const std::string sentence = sentences[i];
          tokenizer.TokenizeWords(sentence, 
                                  &words,
                                  &word_start_positions,
                                  &word_end_positions);

          for (int j = 0; j < words.size(); ++j) {
            CHECK_EQ(words[j].find_first_of("\f\t\r\n"), words[j].npos);
            std::cout << words[j] << " ";
          }
          std::cout << std::endl;

#if 0
          for (int k = 0; k < sentences[i].length(); ++k) {
            if (sentences[i][k] == '\n' || sentences[i][k] == '\r') {
              sentences[i][k] = ' ';
            }
          }
          if (sentences[i] != "" &&
              sentences[i][sentences[i].size() - 1] == ' ') {
            sentences[i].resize(sentences[i].size() - 1);
          }
          if (sentences[i] != "") {
            std::cout << sentences[i] << std::endl;
          }
#endif
        }
        document = "";
      }
      document += line + "\n";
    }
  }

  return 0;
}
