from turboparser import PyCPBSSink, PyCppToPyTurboSink, PyCTurboTextAnalysis, PyLoadOptions, PyAnalyseOptions
import sys
import os 

#print("Hello!")
#input("waiting... press any key to continue.");

turbotextanalysis = PyCTurboTextAnalysis()
language ="en"
path = os.path.join('..', 'Data')
path = path + os.sep

if len(sys.argv) > 1:
    for i in range(1,len(sys.argv)):
        argument = str(sys.argv)
        line = sys.argv[i]
        if line.startswith("Path="):
           path = line[5:]
        if line.startswith("Language="):
           language = line[9:]

load_options = PyLoadOptions()
load_options.load_tagger = True;
load_options.load_parser = False;
load_options.load_morphological_tagger = True;
load_options.load_entity_recognizer = True;
load_options.load_semantic_parser = False;
load_options.load_coreference_resolver = False;
retval = turbotextanalysis.load_language(language, path, load_options)
if retval != 0:
    print("ERROR in PyCTurboTextAnalysis load_language")
    print("Return value: ", retval)
    exit()

#Default TurboTextAnalysis sink.
sink = PyCppToPyTurboSink(True)

text = "Obama visits Portugal."

options = PyAnalyseOptions()
options.use_tagger = True;
options.use_parser = False;
options.use_morphological_tagger = True;
options.use_entity_recognizer = True;
options.use_semantic_parser = False;
options.use_coreference_resolver = False;
retval = turbotextanalysis.analyse(language, text, sink, options)
if retval != 0:
    print("ERROR in PyCTurboTextAnalysis analyse")
    print("Return value: ", retval)
    exit()

tokens_info = sink.get_tokens_info()
print("Token-level Data")
for x in tokens_info:
    print("word",':', x["word"])
    print("\t", "len",':', x["len"])
    print("\t", "start_pos",':', x["start_pos"])
    print("\t", "kind",':', x["kind"])
    print("\t", "features",':')
    for y in x["features"]:
        print ("\t\t",y,':',x["features"][y])

document_info = sink.get_document_info()
print("Document-level Data")
for x in document_info:
    print (x,':',tokens_info[x])


#Custom client-defined sink with custom put_token and put_feature/document_feature.
class ClientSideSinkRedefined(PyCppToPyTurboSink):
    def __init__(self, allocate=True):
        self.client_last_word = 'default_word'
        self.client_current_index = 0
        self.client_token_data=[]
        self.client_document_data={}
    def put_token(self,
                  word, 
                  length,
                  start_pos,
                  kind):
        self.client_last_word = word
        self.client_current_index = len(self.client_token_data)
        self.client_token_data.append({})
        self.client_token_data[self.client_current_index]["word"] = word
        self.client_token_data[self.client_current_index]["len"] = length
        self.client_token_data[self.client_current_index]["start_pos"] = start_pos
        self.client_token_data[self.client_current_index]["kind"] = kind
        self.client_token_data[self.client_current_index]["features"] =  {}
   
    def put_feature(self,  feature,  value):
        self.client_token_data[self.client_current_index]["features"][feature] = value

    def end_sentence(self):
        pass

    def put_document_feature(self, feature, value):
        self.client_document_data[feature] = value


sink = ClientSideSinkRedefined(True)

text = "Obama visits Portugal."
turbotextanalysis.analyse(language, text, sink, options)
if retval != 0:
    print("ERROR in PyCTurboTextAnalysis analyse")
    print("Return value: ", retval)
    exit()

print("Token-level Data")
for x in sink.client_token_data:
    print("word",':', x["word"])
    print("\t", "len",':', x["len"])
    print("\t", "start_pos",':', x["start_pos"])
    print("\t", "kind",':', x["kind"])
    print("\t", "features",':')
    for y in x["features"]:
        print ("\t\t",y,':',x["features"][y])

print("Document-level Data")
for x in sink.client_document_data:
    print (x,':',tokens_info[x])




#*******************
#Example with custom tokenization

sentences_words = [ [ "Obama", "visits", "Portugal", "." ],
  [ "He", "is", "staying", "3", "days","." ]];
sentence_start_positions = [ 0, 23 ];
sentences_start_positions = [[ 0, 6, 13, 21 ],
      [ 0, 3, 6, 14, 16, 20 ]];
sentences_end_position = [[ 5, 12, 21, 22 ],
      [ 2, 5, 13, 15, 20, 21 ]];
original_sentences_words = sentences_words;

turbotextanalysis.analyse_with_tokens(language, 
                                        sentences_words,
                                        original_sentences_words,
                                        sentence_start_positions,
                                        sentences_start_positions,
                                        sentences_end_position, sink, options)
if retval != 0:
    print("ERROR in PyCTurboTextAnalysis analyse")
    print("Return value: ", retval)
    exit()

print("Token-level Data")
for x in sink.client_token_data:
    print("word",':', x["word"])
    print("\t", "len",':', x["len"])
    print("\t", "start_pos",':', x["start_pos"])
    print("\t", "kind",':', x["kind"])
    print("\t", "features",':')
    for y in x["features"]:
        print ("\t\t",y,':',x["features"][y])

print("Document-level Data")
for x in sink.client_document_data:
    print (x,':',tokens_info[x])