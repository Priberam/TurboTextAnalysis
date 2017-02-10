// PBSAnalysis.h : Declaration of the CPBSAnalysis

#pragma once
//#include "resource.h"       // main symbols

#include <string>
//#include "TRSessionManager.h"

//#ifdef __unix__
//typedef int STDMETHODIMP;
//#define S_OK 0
//#define E_FAIL -1
//#endif

#include "ps_textanalysistemplate.h"

class CTurboToken {
public:

  CTurboToken() {}
  virtual ~CTurboToken() {}

  // Get/set methods.
  const std::string &text() { return m_text; };
  void set_text(const std::string &text) { m_text = text; }

  int start_position() { return m_start_position; }
  void set_start_position(int start_position) { m_start_position = start_position; }

  int end_position() { return m_end_position; }
  void set_end_position(int end_position) { m_end_position = end_position; }

  bool multi_word() { return m_multi_word; }
  void set_multi_word(bool multi_word) { m_multi_word = multi_word; }

  const std::string &tag() { return m_tag; }
  void set_tag(const std::string &tag) { m_tag = tag; }

  const std::string &entity_tag() { return m_entity_tag; }
  void set_entity_tag(const std::string &entity_tag) { m_entity_tag = entity_tag; }

  bool HasDigit() {
    const char* word = m_text.c_str();
    int len = (int)m_text.length();
    for (int i = 0; i < len; ++i)
      if (IsDigit(word[i]))
        return true;
    return false;
  }

protected:
  // Various case/digit functions.
  bool IsUpperCase(char c) { return (c >= 'A' && c <= 'Z'); }
  bool IsLowerCase(char c) { return (c >= 'a' && c <= 'z'); }
  bool IsDigit(char c) { return (c >= '0' && c <= '9'); }

protected:
  std::string m_text;
  int m_start_position;
  int m_end_position;
  bool m_multi_word;
  std::string m_tag;
  std::string m_entity_tag;
};

#if 0
// CPBSAnalysis

class ATL_NO_VTABLE CPBSAnalysis :
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CPBSAnalysis, &CLSID_PBSAnalysis>,
  public IDispatchImpl<IPBSAnalysis, &IID_IPBSAnalysis, &LIBID_PBSTextAnalysisLib, /*wMajor =*/ 1, /*wMinor =*/ 0> {
public:
  CPBSAnalysis() {}

  DECLARE_REGISTRY_RESOURCEID(IDR_PBSANALYSIS)

  BEGIN_COM_MAP(CPBSAnalysis)
    COM_INTERFACE_ENTRY(IPBSAnalysis)
    COM_INTERFACE_ENTRY(IDispatch)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct() {
    return S_OK;
  }

  void FinalRelease() {}
#endif

  class CPBSSink;

  class CTurboTokenAnalysis {
  public:
#if 0
    void processToken(TRSessionManager *sess,
                      CTRAnalise*an, CTRToken *tk,
                      CPBSSink* pbssink,
                      std::map<short, int> &onts_doc,
                      char * _textChunk,
                      int offset);
    void processSepToken(TRSessionManager *sess,
                         CTKToken *tk1,
                         CTKToken *tk,
                         CPBSSink* pbssink,
                         char * _textChunk,
                         int offset);
    void processSepSentenceToken(TRSessionManager *sess,
                                 CTKToken *tk1,
                                 int cchProc,
                                 CPBSSink* pbssink,
                                 char * _textChunk,
                                 int offset);
#endif

    TokenKind GetTokenKind(CTurboToken *token);
    TokenKind GetSepTokenKind(const char *text, int len);
  };

#if 0
  OBJECT_ENTRY_AUTO(__uuidof(PBSAnalysis), CPBSAnalysis)
#endif
