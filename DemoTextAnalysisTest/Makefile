all : DemoTextAnalysisTest

DemoTextAnalysisTest: DemoTextAnalysisTest.cpp ../TurboTextAnalysis/TurboTextAnalysis.h
	g++ -g -std=c++11 -std=gnu++11 -O3 -Wall DemoTextAnalysisTest.cpp -I.. -I../TurboTextAnalysis -L../TurboTextAnalysis ../TurboTextAnalysis/libturbotextanalysis.so -I../deps -L../deps/libconfig-1.4.9/lib/.libs ../deps/libconfig-1.4.9/lib/.libs/libconfig++.so.9.1.3 -o DemoTextAnalysisTest