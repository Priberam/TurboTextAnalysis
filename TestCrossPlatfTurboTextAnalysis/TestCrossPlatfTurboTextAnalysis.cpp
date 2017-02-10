#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <functional>
#include "DynamicLibraryHelper.h"
#include "TurboTextAnalysis/ps_textanalysistemplate.h"

class StemSink : public CPBSSink {
public:
  StemSink() {};
  virtual ~StemSink() {};

  virtual int PutToken(const char * word,
                       int len,
                       int start_pos,
                       enum TokenKind kind) {
    std::cout << "Token: " << word << " - " <<
      len << " - " <<
      start_pos << " - " <<
      static_cast<int>(kind) << std::endl;
    return 0;
  };
  virtual int PutFeature(const char * feature,
                         const char * value) {
    std::cout << "\t\tFeature: " << feature << " - " << value << std::endl;
    return 0;
  };
  virtual int EndSentence() {
    std::cout << "EndSentence" << std::endl;
    return 0;
  };
  virtual int PutDocumentFeature(const char * feature,
                                 const char * value) {
    std::cout << "Document feature: " << feature << " - " << value << std::endl;
    return 0;
  };
};

int main(int argc, char **argv) {
  //1) Open dynamic lib;
  //2) Create class objects using object generator functions
  //    (Get "create_object" functions addresses of those libs)
  //3) Close lib handle
  //
  //NOTE: library that provides the class derived from shape must
  //      provide a way to create objects of the new class.
  {
    LibHandle lib_handle;
    LibraryInfo library_info;

    library_info.linux_lib_path = "../CrossPlatfTurboTextAnalysis/turbotextanalysis.so";
    library_info.windows_lib_path = "../x64/Release/CrossPlatfTurboTextAnalysis.dll";
    if (!OpenDynLibPlugIn(library_info, &lib_handle)) {
      std::cerr << "Error opening" << std::endl; return -1;
    }

    CPBSTextAnalysis *(*textanalysis_maker)();
    if (!GetDynLibFunctionPointer("create_object", lib_handle,
      (void**)&textanalysis_maker)) {
      std::cerr << "Error getting function" << std::endl; return -1;
    };

    CPBSTextAnalysis *textanalysis = static_cast<CPBSTextAnalysis *>((*textanalysis_maker)());

    if (textanalysis->DefineProperty("load_tagger", "true") != 0) {
      std::cerr << "Error in DefineProperty" << std::endl; return -1;
    };
    //if (textanalysis->DefineProperty("load_parser", "true") != 0) {
    //  std::cerr << "Error in DefineProperty" << std::endl; return -1;
    //};
    if (textanalysis->DefineProperty("load_morphological_tagger", "true") != 0) {
      std::cerr << "Error in DefineProperty" << std::endl; return -1;
    };
    if (textanalysis->DefineProperty("load_entity_recognizer", "true") != 0) {
      std::cerr << "Error in DefineProperty" << std::endl; return -1;
    };

    if (textanalysis->LoadLanguage("en",
#if defined(_MSC_VER)
                                   "C:\\Projects\\TurboTextAnalysis\\Data\\"
#else
                                   "/mnt/c/Projects/TurboTextAnalysis/Data/"
#endif
    ) != 0) {
      std::cerr << "Error in LoadLanguage" << std::endl; return -1;
    };

    StemSink sink;

    if (textanalysis->Analyse("en", "Priberam sells software.", &sink) != 0) {
      std::cerr << "Error in Analyse" << std::endl; return -1;
    };
    if (textanalysis->Analyse("en", "Obama came to Portugal.", &sink) != 0) {
      std::cerr << "Error in Analyse" << std::endl; return -1;
    };

    if (textanalysis->DefineProperty("use_tagger", "true") != 0) {
      std::cerr << "Error in DefineProperty" << std::endl; return -1;
    };

    if (textanalysis->DefineProperty("use_entity_recognizer", "true") != 0) {
      std::cerr << "Error in DefineProperty" << std::endl; return -1;
    };
    if (textanalysis->Analyse("en", "Obama came to Portugal. He stayed 3 days.", &sink) != 0) {
      std::cerr << "Error in Analyse" << std::endl; return -1;
    };

    void(*textanalysis_destroyer)(CPBSTextAnalysis *);
    if (!GetDynLibFunctionPointer("destroy_object", lib_handle,
      (void**)&textanalysis_destroyer)) {
      std::cerr << "Error getting function" << std::endl; return -1;
    };

    (*textanalysis_destroyer)(textanalysis);

    if (!CloseDynLibPlugIn(lib_handle)) {
      std::cerr << "Error closing" << std::endl; return -1;
    }
  }

  return 0;
}
