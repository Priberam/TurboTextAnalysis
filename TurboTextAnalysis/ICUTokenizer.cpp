/*
* ICUTokenizer.cpp
*
*  Created on: May 12, 2017
*      Author: dan
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unicode/brkiter.h>
#include "ICUTokenizer.h"


//Codes for the Representation of Names of Languages
//Codes arranged alphabetically by alpha - 3 / ISO 639 - 2 Code
// http://www.loc.gov/standards/iso639-2/php/code_list.php

const std::unordered_map<const char*, const char*>&
ICUTokenizer::ISO639_2Code_to_ISO639_1Code() {
  //ISO 639-2 Code,ISO 639-1 Code,English name of Language
  static const std::unordered_map<const char*, const char*> ISO639_2Code_to_ISO639_1Code =
  { { "aar","aa" }//Afar
    ,{ "abk","ab" }//Abkhazian
    ,{ "afr","af" }//Afrikaans
    ,{ "aka","ak" }//Akan
    ,{ "alb","sq" }//Albanian
    ,{ "amh","am" }//Amharic
    ,{ "ara","ar" }//Arabic
    ,{ "arg","an" }//Aragonese
    ,{ "arm","hy" }//Armenian
    ,{ "asm","as" }//Assamese
    ,{ "ava","av" }//Avaric
    ,{ "ave","ae" }//Avestan
    ,{ "aym","ay" }//Aymara
    ,{ "aze","az" }//Azerbaijani
    ,{ "bak","ba" }//Bashkir
    ,{ "bam","bm" }//Bambara
    ,{ "baq","eu" }//Basque
    ,{ "bel","be" }//Belarusian
    ,{ "ben","bn" }//Bengali
    ,{ "bih","bh" }//Bihari languages
    ,{ "bis","bi" }//Bislama
    ,{ "bod","bo" }//Tibetan
    ,{ "bos","bs" }//Bosnian
    ,{ "bre","br" }//Breton
    ,{ "bul","bg" }//Bulgarian
    ,{ "bur","my" }//Burmese
    ,{ "cat","ca" }//Catalan; Valencian
    ,{ "ces","cs" }//Czech
    ,{ "cha","ch" }//Chamorro
    ,{ "che","ce" }//Chechen
    ,{ "chi","zh" }//Chinese
    ,{ "chu","cu" }//Church Slavic; Old Slavonic; Church Slavonic; Old Bulgarian; Old Church Slavonic
    ,{ "chv","cv" }//Chuvash
    ,{ "cor","kw" }//Cornish
    ,{ "cos","co" }//Corsican
    ,{ "cre","cr" }//Cree
    ,{ "cym","cy" }//Welsh
    ,{ "cze","cs" }//Czech
    ,{ "dan","da" }//Danish
    ,{ "deu","de" }//German
    ,{ "div","dv" }//Divehi; Dhivehi; Maldivian
    ,{ "dut","nl" }//Dutch; Flemish
    ,{ "dzo","dz" }//Dzongkha
    ,{ "ell","el" }//"Greek, Modern (1453-)"
    ,{ "eng","en" }//English
    ,{ "epo","eo" }//Esperanto
    ,{ "est","et" }//Estonian
    ,{ "eus","eu" }//Basque
    ,{ "ewe","ee" }//Ewe
    ,{ "fao","fo" }//Faroese
    ,{ "fas","fa" }//Persian
    ,{ "fij","fj" }//Fijian
    ,{ "fin","fi" }//Finnish
    ,{ "fra","fr" }//French
    ,{ "fre","fr" }//French
    ,{ "fry","fy" }//Western Frisian
    ,{ "ful","ff" }//Fulah
    ,{ "geo","ka" }//Georgian
    ,{ "ger","de" }//German
    ,{ "gla","gd" }//Gaelic; Scottish Gaelic
    ,{ "gle","ga" }//Irish
    ,{ "glg","gl" }//Galician
    ,{ "glv","gv" }//Manx
    ,{ "gre","el" }//"Greek, Modern (1453-)"
    ,{ "grn","gn" }//Guarani
    ,{ "guj","gu" }//Gujarati
    ,{ "hat","ht" }//Haitian; Haitian Creole
    ,{ "hau","ha" }//Hausa
    ,{ "heb","he" }//Hebrew
    ,{ "her","hz" }//Herero
    ,{ "hin","hi" }//Hindi
    ,{ "hmo","ho" }//Hiri Motu
    ,{ "hrv","hr" }//Croatian
    ,{ "hun","hu" }//Hungarian
    ,{ "hye","hy" }//Armenian
    ,{ "ibo","ig" }//Igbo
    ,{ "ice","is" }//Icelandic
    ,{ "ido","io" }//Ido
    ,{ "iii","ii" }//Sichuan Yi; Nuosu
    ,{ "iku","iu" }//Inuktitut
    ,{ "ile","ie" }//Interlingue; Occidental
    ,{ "ina","ia" }//Interlingua (International Auxiliary Language Association)
    ,{ "ind","id" }//Indonesian
    ,{ "ipk","ik" }//Inupiaq
    ,{ "isl","is" }//Icelandic
    ,{ "ita","it" }//Italian
    ,{ "jav","jv" }//Javanese
    ,{ "jpn","ja" }//Japanese
    ,{ "kal","kl" }//Kalaallisut; Greenlandic
    ,{ "kan","kn" }//Kannada
    ,{ "kas","ks" }//Kashmiri
    ,{ "kat","ka" }//Georgian
    ,{ "kau","kr" }//Kanuri
    ,{ "kaz","kk" }//Kazakh
    ,{ "khm","km" }//Central Khmer
    ,{ "kik","ki" }//Kikuyu; Gikuyu
    ,{ "kin","rw" }//Kinyarwanda
    ,{ "kir","ky" }//Kirghiz; Kyrgyz
    ,{ "kom","kv" }//Komi
    ,{ "kon","kg" }//Kongo
    ,{ "kor","ko" }//Korean
    ,{ "kua","kj" }//Kuanyama; Kwanyama
    ,{ "kur","ku" }//Kurdish
    ,{ "lao","lo" }//Lao
    ,{ "lat","la" }//Latin
    ,{ "lav","lv" }//Latvian
    ,{ "lim","li" }//Limburgan; Limburger; Limburgish
    ,{ "lin","ln" }//Lingala
    ,{ "lit","lt" }//Lithuanian
    ,{ "ltz","lb" }//Luxembourgish; Letzeburgesch
    ,{ "lub","lu" }//Luba-Katanga
    ,{ "lug","lg" }//Ganda
    ,{ "mac","mk" }//Macedonian
    ,{ "mah","mh" }//Marshallese
    ,{ "mal","ml" }//Malayalam
    ,{ "mao","mi" }//Maori
    ,{ "mar","mr" }//Marathi
    ,{ "may","ms" }//Malay
    ,{ "mkd","mk" }//Macedonian
    ,{ "mlg","mg" }//Malagasy
    ,{ "mlt","mt" }//Maltese
    ,{ "mon","mn" }//Mongolian
    ,{ "mri","mi" }//Maori
    ,{ "msa","ms" }//Malay
    ,{ "mya","my" }//Burmese
    ,{ "nau","na" }//Nauru
    ,{ "nav","nv" }//Navajo; Navaho
    ,{ "nbl","nr" }//"Ndebele, South; South Ndebele"
    ,{ "nde","nd" }//"Ndebele, North; North Ndebele"
    ,{ "ndo","ng" }//Ndonga
    ,{ "nep","ne" }//Nepali
    ,{ "nld","nl" }//Dutch; Flemish
    ,{ "nno","nn" }//"Norwegian Nynorsk; Nynorsk, Norwegian"
    ,{ "nob","nb" }//"Bokmål, Norwegian; Norwegian Bokmål"
    ,{ "nor","no" }//Norwegian
    ,{ "nya","ny" }//Chichewa; Chewa; Nyanja
    ,{ "oci","oc" }//Occitan (post 1500)
    ,{ "oji","oj" }//Ojibwa
    ,{ "ori","or" }//Oriya
    ,{ "orm","om" }//Oromo
    ,{ "oss","os" }//Ossetian; Ossetic
    ,{ "pan","pa" }//Panjabi; Punjabi
    ,{ "per","fa" }//Persian
    ,{ "pli","pi" }//Pali
    ,{ "pol","pl" }//Polish
    ,{ "por","pt" }//Portuguese
    ,{ "pus","ps" }//Pushto; Pashto
    ,{ "que","qu" }//Quechua
    ,{ "roh","rm" }//Romansh
    ,{ "ron","ro" }//Romanian; Moldavian; Moldovan
    ,{ "rum","ro" }//Romanian; Moldavian; Moldovan
    ,{ "run","rn" }//Rundi
    ,{ "rus","ru" }//Russian
    ,{ "sag","sg" }//Sango
    ,{ "san","sa" }//Sanskrit
    ,{ "sin","si" }//Sinhala; Sinhalese
    ,{ "slk","sk" }//Slovak
    ,{ "slo","sk" }//Slovak
    ,{ "slv","sl" }//Slovenian
    ,{ "sme","se" }//Northern Sami
    ,{ "smo","sm" }//Samoan
    ,{ "sna","sn" }//Shona
    ,{ "snd","sd" }//Sindhi
    ,{ "som","so" }//Somali
    ,{ "sot","st" }//"Sotho, Southern"
    ,{ "spa","es" }//Spanish; Castilian
    ,{ "sqi","sq" }//Albanian
    ,{ "srd","sc" }//Sardinian
    ,{ "srp","sr" }//Serbian
    ,{ "ssw","ss" }//Swati
    ,{ "sun","su" }//Sundanese
    ,{ "swa","sw" }//Swahili
    ,{ "swe","sv" }//Swedish
    ,{ "tah","ty" }//Tahitian
    ,{ "tam","ta" }//Tamil
    ,{ "tat","tt" }//Tatar
    ,{ "tel","te" }//Telugu
    ,{ "tgk","tg" }//Tajik
    ,{ "tgl","tl" }//Tagalog
    ,{ "tha","th" }//Thai
    ,{ "tib","bo" }//Tibetan
    ,{ "tir","ti" }//Tigrinya
    ,{ "ton","to" }//Tonga (Tonga Islands)
    ,{ "tsn","tn" }//Tswana
    ,{ "tso","ts" }//Tsonga
    ,{ "tuk","tk" }//Turkmen
    ,{ "tur","tr" }//Turkish
    ,{ "twi","tw" }//Twi
    ,{ "uig","ug" }//Uighur; Uyghur
    ,{ "ukr","uk" }//Ukrainian
    ,{ "urd","ur" }//Urdu
    ,{ "uzb","uz" }//Uzbek
    ,{ "ven","ve" }//Venda
    ,{ "vie","vi" }//Vietnamese
    ,{ "vol","vo" }//Volapük
    ,{ "wel","cy" }//Welsh
    ,{ "wln","wa" }//Walloon
    ,{ "wol","wo" }//Wolof
    ,{ "xho","xh" }//Xhosa
    ,{ "yid","yi" }//Yiddish
    ,{ "yor","yo" }//Yoruba
    ,{ "zha","za" }//Zhuang; Chuang
    ,{ "zho","zh" }//Chinese
    ,{ "zul","zu" }//Zulu
  };
  return ISO639_2Code_to_ISO639_1Code;
};

namespace {

const std::string Whitespaces = " \t\r\n\f\v";
const std::string NonBreakingSpace = { '\xc2', '\xa0' };
const std::string ZeroWidthSpace = { '\xe2', '\x80', '\x8b' };
const std::string ZeroWidthNonBreakingSpace = { '\xef', '\xbb', '\xbf' };
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
bool IsWhitespace(const std::string &word,
                  int position,
                  int *length) {

  if (position >= word.length()) return false;
  for (int i = 0; i < Whitespaces.size(); ++i) {
    if (word[position] == Whitespaces[i]) {
      *length = 1;
      return true;
    }
  }
  return IsPattern(word, NonBreakingSpace, position, length)
    || IsPattern(word, ZeroWidthSpace, position, length)
    || IsPattern(word, ZeroWidthNonBreakingSpace, position, length);
}
}

void ICUTokenizer::SplitSentences(const std::string &text,
                                  std::vector<std::string>* sentences,
                                  std::vector<int>* start_positions,
                                  std::vector<int>* end_positions) {
  BreakIterator* boundary;
  StringPiece strpiece = StringPiece(text.c_str(),
                                     text.length());
  UnicodeString stringToExamine = UnicodeString::fromUTF8(strpiece);
  UErrorCode status = U_ZERO_ERROR;
  // TODO (dan) : Consider storing some BreakIterator (both sentence and word level) 
  // in the object tokenizer, as they may be some time consuming in some languages 
  // that require loading dictionaries
  boundary = BreakIterator::createSentenceInstance(Locale(m_language.c_str())
                                                   , status);
  if (!U_SUCCESS(status))
    std::cerr << "Failed ICU BreakIterator::createSentenceInstance";

  boundary->setText(stringToExamine);

  int32_t start = boundary->first();
  for (int32_t end = boundary->next();
       end != BreakIterator::DONE;
       start = end, end = boundary->next()) {
    start_positions->push_back(start);
    end_positions->push_back(end);
    UnicodeString substring = UnicodeString(stringToExamine, start, end - start);
    std::string utf8bytes;
    utf8bytes = substring.toUTF8String(utf8bytes);
    sentences->push_back(utf8bytes);
  }
};

void ICUTokenizer::TokenizeWords(const std::string &sentence,
                                 std::vector<std::string>* words,
                                 std::vector<int>* start_positions,
                                 std::vector<int>* end_positions) {
  BreakIterator* boundary;
  StringPiece strpiece = StringPiece(sentence.c_str(),
                                     sentence.length());
  UnicodeString stringToExamine = UnicodeString::fromUTF8(strpiece);
  UErrorCode status = U_ZERO_ERROR;
  // TODO (dan) : Consider storing some BreakIterator (both sentence and word level) 
  // in the object tokenizer, as they may be some time consuming in some languages 
  // that require loading dictionaries
  boundary = BreakIterator::createWordInstance(Locale(m_language.c_str())
                                               , status);
  if (!U_SUCCESS(status))
    std::cerr << "Failed ICU BreakIterator::createWordInstance";

  boundary->setText(stringToExamine);

  int32_t start = boundary->first();
  for (int32_t end = boundary->next();
       end != BreakIterator::DONE;
       start = end, end = boundary->next()) {
    bool is_ponctuation = false;
    int breakType = boundary->getRuleStatus();
    if (breakType == UBRK_WORD_NONE) {
      is_ponctuation = true;
    }

    UnicodeString substring = UnicodeString(stringToExamine, start, end - start);
    std::string utf8bytes;
    utf8bytes = substring.toUTF8String(utf8bytes);

    int size_matched = 0;
    bool is_whitespace = false;
    if (is_ponctuation)
      is_whitespace = IsWhitespace(utf8bytes, 0, &size_matched);
    if (!is_whitespace) {
      start_positions->push_back(start);
      end_positions->push_back(end);
      words->push_back(utf8bytes);
    }
  }
};

