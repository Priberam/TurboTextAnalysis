#include "CrossPlaftTurboAnalysis.h"

int CrossPlatfTurboAnalysis::LoadLanguage(const char * language,
                                          const char * path) {
  LoadOptions load_options;

  if (!module_properties.empty()) {
    auto load_tagger = module_properties.find("load_tagger");
    if (load_tagger != module_properties.end())
      load_options.load_tagger =
      (load_tagger->second == "true") ? true : false;

    auto load_parser = module_properties.find("load_parser");
    if (load_parser != module_properties.end())
      load_options.load_parser =
      (load_parser->second == "true") ? true : false;

    auto load_morphological_tagger = module_properties.find("load_morphological_tagger");
    if (load_morphological_tagger != module_properties.end())
      load_options.load_morphological_tagger =
      (load_morphological_tagger->second == "true") ? true : false;

    auto load_entity_recognizer = module_properties.find("load_entity_recognizer");
    if (load_entity_recognizer != module_properties.end())
      load_options.load_entity_recognizer =
      (load_entity_recognizer->second == "true") ? true : false;

    auto load_semantic_parser = module_properties.find("load_semantic_parser");
    if (load_semantic_parser != module_properties.end())
      load_options.load_semantic_parser =
      (load_semantic_parser->second == "true") ? true : false;

    auto load_coreference_resolver = module_properties.find("load_coreference_resolver");
    if (load_coreference_resolver != module_properties.end())
      load_options.load_coreference_resolver =
      (load_coreference_resolver->second == "true") ? true : false;
  }
  return cturbotextanalysis.
    LoadLanguage(language,
                 path,
                 (module_properties.empty()) ? nullptr : &load_options);
};

int CrossPlatfTurboAnalysis::Analyse(const char * language,
                                     const char * text,
                                     CPBSSink * pbssink) {
  AnalyseOptions use_options;

  if (!module_properties.empty()) {
    auto use_tagger = module_properties.find("use_tagger");
    if (use_tagger != module_properties.end())
      use_options.use_tagger =
      (use_tagger->second == "true") ? true : false;

    auto use_parser = module_properties.find("use_parser");
    if (use_parser != module_properties.end())
      use_options.use_parser =
      (use_parser->second == "true") ? true : false;

    auto use_morphological_tagger = module_properties.find("use_morphological_tagger");
    if (use_morphological_tagger != module_properties.end())
      use_options.use_morphological_tagger =
      (use_morphological_tagger->second == "true") ? true : false;

    auto use_entity_recognizer = module_properties.find("use_entity_recognizer");
    if (use_entity_recognizer != module_properties.end())
      use_options.use_entity_recognizer =
      (use_entity_recognizer->second == "true") ? true : false;

    auto use_semantic_parser = module_properties.find("use_semantic_parser");
    if (use_semantic_parser != module_properties.end())
      use_options.use_semantic_parser =
      (use_semantic_parser->second == "true") ? true : false;

    auto use_coreference_resolver = module_properties.find("use_coreference_resolver");
    if (use_coreference_resolver != module_properties.end())
      use_options.use_coreference_resolver =
      (use_coreference_resolver->second == "true") ? true : false;
  }
  return cturbotextanalysis.
    Analyse(language,
            text,
            pbssink,
            (module_properties.empty()) ? nullptr : &use_options);
};

int CrossPlatfTurboAnalysis::DefineProperty(const char * module_property,
                                            const char * value) {
  auto mod_prop = module_properties.find(module_property);
  if (mod_prop == module_properties.end()) {
    module_properties.insert({ module_property,value });
  } else {
    mod_prop->second = value;
  }
  return 0;
}

int CrossPlatfTurboAnalysis::SubmitCandidateAnswerPassage(const char * language,
                                                          const char * candidate_answer_passage,
                                                          int document_id,
                                                          int passage_offset) {
  return -1;
};

int CrossPlatfTurboAnalysis::RetrieveFinalAnswer(const char * language,
                                                 const char ** answer) {
  return -1;
};

int CrossPlatfTurboAnalysis::ReturnResults(const char * language,
                                           int max_num_results,
                                           CPBSSink* pbssink) {
  return -1;
};

int CrossPlatfTurboAnalysis::FinishAnalysis(const char * language) {
  return -1;
}EXTERN_C_BEGIN

;
CPBSTextAnalysis *create_object() {
  return new CrossPlatfTurboAnalysis;
}
void destroy_object(CPBSTextAnalysis * object) {
  delete object;
}
EXTERN_C_END
