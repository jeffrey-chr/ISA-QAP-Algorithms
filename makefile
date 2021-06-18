# C++ Compiler command
CXX = g++

# C++ Compiler options
CXXFLAGS = -O3 -std=gnu++11
# CXXFLAGS = -O0
SYSTEM = x86-64_linux
LIBFORMAT = static_pic

CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD

all: solvers

solvers: BLS BMA BLS-bench BMA-bench

debug: BLS-debug BMA-debug

BLS: ./src/BLS_code.cpp	
	$(CXX) $(CXXFLAGS) -c -D BLS -o ./obj/handler-BLS.o ./src/QAPhandler.cpp \

	$(CXX) $(CXXFLAGS) -c -o ./obj/BLS.o ./src/BLS_code.cpp \
		
	$(CXX) $(CXXFLAGS) -o ./bin/BLS ./obj/BLS.o ./obj/handler-BLS.o \

BMA: ./src/BMA.cpp	
	$(CXX) $(CXXFLAGS) -c -D BMA -o ./obj/handler-BMA.o ./src/QAPhandler.cpp \

	$(CXX) $(CXXFLAGS) -c -o ./obj/BMA.o ./src/BMA.cpp \
		
	$(CXX) $(CXXFLAGS) -o ./bin/BMA ./obj/BMA.o ./obj/handler-BMA.o \

BLS-bench: ./src/BLS_code.cpp	
	$(CXX) $(CXXFLAGS) -c -D BLS -D BENCH -o ./obj/handler-BLS-bench.o ./src/QAPhandler.cpp \

	$(CXX) $(CXXFLAGS) -c -o ./obj/BLS-bench.o ./src/BLS_code.cpp \
		
	$(CXX) $(CXXFLAGS) -o ./bin/BLS-debug ./obj/BLS-bench.o ./obj/handler-BLS-bench.o \

BMA-bench: ./src/BMA.cpp	
	$(CXX) $(CXXFLAGS) -c -D BMA -D BENCH -o ./obj/handler-BMA-bench.o ./src/QAPhandler.cpp \

	$(CXX) $(CXXFLAGS) -c -o ./obj/BMA-bench.o ./src/BMA.cpp \
		
	$(CXX) $(CXXFLAGS) -o ./bin/BMA-debug ./obj/BMA-bench.o ./obj/handler-BMA-bench.o \


BLS-debug: ./src/BLS_code.cpp	
	$(CXX) $(CXXFLAGS) -c -D BLS -D DEBUG -o ./obj/handler-BLS-debug.o ./src/QAPhandler.cpp \

	$(CXX) $(CXXFLAGS) -c -D DEBUG -o ./obj/BLS-debug.o ./src/BLS_code.cpp \
		
	$(CXX) $(CXXFLAGS) -o ./bin/BLS-debug ./obj/BLS-debug.o ./obj/handler-BLS-debug.o \

BMA-debug: ./src/BMA.cpp	
	$(CXX) $(CXXFLAGS) -c -D BMA -D DEBUG -o ./obj/handler-BMA-debug.o ./src/QAPhandler.cpp \

	$(CXX) $(CXXFLAGS) -c -D DEBUG -o ./obj/BMA-debug.o ./src/BMA.cpp \
		
	$(CXX) $(CXXFLAGS) -o ./bin/BMA-debug ./obj/BMA-debug.o ./obj/handler-BMA-debug.o \


handler: ./src/QAPhandler.cpp
	$(CXX) $(CXXFLAGS) -c -D NONE -o ./obj/handler.o ./src/QAPhandler.cpp \
		
	$(CXX) $(CXXFLAGS) -o ./bin/handler ./obj/handler.o \
	
clean:
	rm -f ./bin/* ./obj/*.o