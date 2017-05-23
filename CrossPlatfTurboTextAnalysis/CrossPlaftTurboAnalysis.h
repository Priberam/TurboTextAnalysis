#ifndef CROSSPLATFTURBOTEXTANALYSIS_TURBOANALYSIS_H
#define CROSSPLATFTURBOTEXTANALYSIS_TURBOANALYSIS_H
#include "TurboTextAnalysis/ps_textanalysistemplate.h"
#include "TurboTextAnalysis/TurboTextAnalysis.h"
#include <string>
#include <unordered_map>

#if defined(_MSC_VER)
#   define _DYNAMICLIBRARY_WINDOWS
#   define _DYNAMICLIBRARY_OSNAME "Windows"
#endif

#ifdef __linux__
#   define _DYNAMICLIBRARY_LINUX
#   define _DYNAMICLIBRARY_OSNAME "Linux"
#endif

#ifdef _DYNAMICLIBRARY_WINDOWS
#include <SDKDDKVer.h>
// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform,
// include WinSDKVer.h and set the _WIN32_WINNT macro to the
// platform you wish to support before including SDKDDKVer.h.

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#endif

#ifdef __cplusplus
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END }
#ifdef _DYNAMICLIBRARY_WINDOWS
#ifdef CROSSPLATFTURBOTEXTANALYSIS_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#define EXPORT_FUNCTION DLL_API
#else
#define EXPORT_FUNCTION
#endif
#else
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#define EXPORT_FUNCTION
#endif

class CrossPlatfTurboAnalysis : public CPBSTextAnalysis {
public:
  CrossPlatfTurboAnalysis() {};
  virtual ~CrossPlatfTurboAnalysis() {};

  int LoadLanguage(const char * language,
                   const char * path);

  int Analyse(const char * language,
              const char * text,
              CPBSSink * pbssink);

  int DefineProperty(const char *  module_property,
                     const char *  value);

  int SubmitCandidateAnswerPassage(const char * language,
                                   const char * candidate_answer_passage,
                                   int document_id,
                                   int passage_offset);

  int RetrieveFinalAnswer(const char * language,
                          const char ** answer);

  int ReturnResults(const char *  language,
                    int max_num_results,
                    CPBSSink* pbssink);

  int FinishAnalysis(const char *  language);
protected:
  CTurboTextAnalysis cturbotextanalysis;
  std::unordered_map < std::string, std::string > module_properties;
};

EXTERN_C_BEGIN
EXPORT_FUNCTION CPBSTextAnalysis *create_object();
EXPORT_FUNCTION void destroy_object(CPBSTextAnalysis * object);
EXTERN_C_END

#endif
