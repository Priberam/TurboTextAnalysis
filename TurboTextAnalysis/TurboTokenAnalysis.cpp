// TurboTokenAnalysis.cpp : Implementation of CTurboTokenAnalysis

#include "TurboTokenAnalysis.h"
//#include "TRSessionManager.h"

TokenKind CTurboTokenAnalysis::GetTokenKind(CTurboToken *token) {
  if (token->tag() == "." || token->tag() == ":" || token->tag() == ",")
    return TokenKind::sep_punct;
  else if (token->tag() == "\"" || token->tag() == "''")
    return TokenKind::sep_quote;
  else if (token->tag() == "CD")
    return TokenKind::number;
  else if (token->HasDigit())
    return TokenKind::alphanum;
  else
    return TokenKind::alpha;
}

TokenKind CTurboTokenAnalysis::GetSepTokenKind(const char *text, int len) {
  int i;
  for (i = 0; i < len; i++) {
    if (text[i] == '\n' || text[i] == '\r')
      return TokenKind::sep_paragraph;
    if (text[i] == '\"')
      return TokenKind::sep_quote;
    if (text[i] == '-')
      return TokenKind::sep_dash;
#if 0
    // Need to replace this function by something else and put this back.
    if (ispun(text[i]))
      return sep_punct;
#endif
  }
  return TokenKind::sep_space;
}

#if 0
void CPBSAnalysis::processToken(TRSessionManager *sess, CTRAnalise*an, CTRToken *tk, CPBSSink* pbssink, std::map<short, int> &onts_doc, char * _textChunck, int offset) {
  if (!tk)
    return;
  if (tk->obtemTipo() & TK_CONTR_P1) {
    if (tk->obtemTokenLocucao())
      processSepToken(sess, tk, tk->obtemTokenLocucao(), pbssink, _textChunck, offset);

    // obtem o pr�ximo token da locu��o e trata
    processToken(sess, an, (CTRToken*)tk->obtemTokenLocucao(), pbssink, onts_doc, _textChunck, offset);
    return;
  }
  unsigned char doms[255], n_doms = 255, n_onts = 255;
  unsigned short onts[255];

  // Primeiro colocar a locu��o ou token
  char* token = (char *)alloca(tk->obtemLen() + 1);
  strncpy(token, _textChunck + tk->obtemPosInicial(), tk->obtemLen());
  token[tk->obtemLen()] = '\0';
  std::string word = token;
  pbssink->PutToken(word, tk->obtemLen(), tk->obtemPosInicial() + offset, getTokenKind(tk));
  OpenSemTag *tagloc = sess->obtemOSCData()->checkToken(tk);
  if (tagloc) {
    std::string name = tagloc->getName();
    std::string value = tagloc->getValue(an, tk);
    pbssink->PutFeature(name, value);
  }

  OpenSemTag *tag;
  if ((tag = sess->obtemOSCData()->getTagLemma()) != NULL && tag->checkToken(tk)) {
    std::string name = tag->getName();
    std::string value = tag->getValue(an, tk);
    pbssink->PutFeature(name, value);
  }

  if ((tag = sess->obtemOSCData()->getTagHead()) != NULL && tag->checkToken(tk)) {
    std::string name = tag->getName();
    std::string value = tag->getValue(an, tk);
    pbssink->PutFeature(name, value);
  }
  if ((tag = sess->obtemOSCData()->getTagMrcat()) != NULL && tag->checkToken(tk)) {
    std::string name = tag->getName();
    std::string value = tag->getValue(an, tk);
    pbssink->PutFeature(name, value);
  }
  if ((tag = sess->obtemOSCData()->getTagFullcat()) != NULL && tag->checkToken(tk)) {
    std::string name = tag->getName();
    std::string value = tag->getValue(an, tk);
    pbssink->PutFeature(name, value);
  }
  if (sess->obtemOSCData()->getTagOnt1() || sess->obtemOSCData()->getTagOnt1Raw()) {
    tk->obtemInfoOnt(an->obtemTRExec(), doms, n_doms, onts, NULL, n_onts);
    std::map<short, int>::iterator itr;
    for (int i = 0; i < n_onts; i++) {
      if ((itr = onts_doc.find(onts[i])) != onts_doc.end())
        itr->second++;
      else
        onts_doc.insert(std::pair<short, int>(onts[i], 1));
      char stont[200];
      if (sess->obtemOSCData()->getTagOnt1()) {
        unsigned char id_nivel1, id_nivel2, id_nivel3, id_nivel4;
        char* descs[4];
        CTROntDataNos* nos_ont = an->obtemTRData()->obtemTROntData()->obtemTROntDataNos();
        nos_ont->obtemNiveis(onts[i], id_nivel1, id_nivel2, id_nivel3, id_nivel4);
        nos_ont->obtemDescricoesNiveis(descs, id_nivel1, id_nivel2, id_nivel3, id_nivel4);

        sprintf(stont, "%d.%d.%d.%d - %s / %s / %s / %s", id_nivel1 + 1, id_nivel2 + 1, id_nivel3 + 1, id_nivel4 + 1, descs[0], descs[1], descs[2], descs[3]);

        std::string name = sess->obtemOSCData()->getTagOnt1()->getName();
        std::string value = stont;
        pbssink->PutFeature(name, value);
      }
      if (sess->obtemOSCData()->getTagOnt1Raw()) {
        unsigned char id_nivel1, id_nivel2, id_nivel3, id_nivel4;
        CTROntDataNos* nos_ont = an->obtemTRData()->obtemTROntData()->obtemTROntDataNos();
        nos_ont->obtemNiveis(onts[i], id_nivel1, id_nivel2, id_nivel3, id_nivel4);

        sprintf(stont, "%02d.%02d.%02d.%02d", id_nivel1 + 1, id_nivel2 + 1, id_nivel3 + 1, id_nivel4 + 1);

        std::string name = sess->obtemOSCData()->getTagOnt1Raw()->getName();
        std::string value = stont;
        pbssink->PutFeature(name, value);
      }
    }
  }
  std::string name = "pbt.endfeatures";
  std::string value = "";
  pbssink->PutFeature(name, value);
  if (tk->obtemTokenLocucao())
    processSepToken(sess, tk, tk->obtemTokenLocucao(), pbssink, _textChunck, offset);

  // obtem o pr�ximo token da locu��o e trata
  processToken(sess, an, (CTRToken*)tk->obtemTokenLocucao(), pbssink, onts_doc, _textChunck, offset);
}

void CPBSAnalysis::processSepToken(TRSessionManager *sess, CTKToken *tk1, CTKToken *tk, CPBSSink* pbssink, char * _textChunck, int offset) {
  if (!tk)
    return;

  CTKToken *currTk = tk;

  int posini = 0;
  if (tk1)
    posini = tk1->obtemPosFinal();
  if (currTk->obtemPosInicial() - posini <= 0)
    return;
  char* token = (char *)alloca(currTk->obtemPosInicial() - posini + 1);
  strncpy(token, _textChunck + posini, currTk->obtemPosInicial() - posini);
  token[currTk->obtemPosInicial() - posini] = '\0';

  std::string word = token;
  std::string feature = "pbt.endfeatures";
  std::string value = "";
  pbssink->PutToken(word, currTk->obtemPosInicial() - posini, posini + offset, getSepTokenKind(_textChunck + posini, currTk->obtemPosInicial() - posini));
  pbssink->PutFeature(feature, value);
}

void CPBSAnalysis::processSepSentenceToken(TRSessionManager *sess, CTKToken *tk1, int cchProc, CPBSSink* pbssink, char * _textChunck, int offset) {
  int posini = 0;
  if (tk1)
    posini = tk1->obtemPosFinal();
  if (posini - cchProc >= 0)
    return;

  char* token = (char *)alloca(cchProc - posini + 1);
  strncpy(token, _textChunck + posini, cchProc - posini);
  token[cchProc - posini] = '\0';
  std::string word = token;
  std::string feature = "pbt.endfeatures";
  std::string value = "";
  pbssink->PutToken(word, cchProc - posini, posini + offset, getSepTokenKind(_textChunck + posini, cchProc - posini));
  pbssink->PutFeature(feature, value);
}
#endif
