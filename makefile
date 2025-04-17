# C++ Compiler command
CXX = g++

# C++ Compiler options
CXXFLAGS = -O3 -std=gnu++11
# CXXFLAGS = -O0
SYSTEM = x86-64_linux
LIBFORMAT = static_pic

CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD

ACO_DEBUG=0
ACO_TRACE=0

ifneq ($(ACO_DEBUG),0)
ACO_DEBUGFLAGS=-g3 -DMALLOC_CHECK_=3 -DDODEBUG=$(ACO_DEBUG)
else
ACO_DEBUGFLAGS=
endif

ifneq ($(ACO_TRACE),0)
ACO_TRACEFLAGS=-DTRACE=$(ACO_TRACE)
else
ACO_TRACEFLAGS=
endif

ACO_OPT_FLAGS=-O3 -ffast-math
# ACO_WARN_FLAGS=-Wall -Wextra -std=c99 -pedantic
ACO_WARN_FLAGS=-Wall -Wextra -std=gnu++11 -pedantic

ACO_CFLAGS=$(ACO_WARN_FLAGS) $(ACO_OPT_FLAGS) $(ACO_DEBUGFLAGS) $(ACO_TRACEFLAGS)

all: solvers debug

solvers: BLS BMA MMAS 

bench: BLS-bench BMA-bench MMAS-bench

debug: BLS-debug BMA-debug

BLS: ./src/BLS_code.cpp	
	$(CXX) $(CXXFLAGS) -c -D ALGBLS -o ./obj/handler-BLS.o ./src/QAPhandler.cpp \

	$(CXX) $(CXXFLAGS) -c -o ./obj/BLS.o ./src/BLS_code.cpp \
		
	$(CXX) $(CXXFLAGS) -o ./bin/BLS ./obj/BLS.o ./obj/handler-BLS.o \

BMA: ./src/BMA.cpp	
	$(CXX) $(CXXFLAGS) -c -D ALGBMA -o ./obj/handler-BMA.o ./src/QAPhandler.cpp \

	$(CXX) $(CXXFLAGS) -c -o ./obj/BMA.o ./src/BMA.cpp \
		
	$(CXX) $(CXXFLAGS) -o ./bin/BMA ./obj/BMA.o ./obj/handler-BMA.o \

BLS-bench: ./src/BLS_code.cpp	
	$(CXX) $(CXXFLAGS) -c -D ALGBLS -D BENCH -o ./obj/handler-BLS-bench.o ./src/QAPhandler.cpp \

	$(CXX) $(CXXFLAGS) -c -o ./obj/BLS-bench.o ./src/BLS_code.cpp \
		
	$(CXX) $(CXXFLAGS) -o ./bin/BLS-bench ./obj/BLS-bench.o ./obj/handler-BLS-bench.o \

BMA-bench: ./src/BMA.cpp	
	$(CXX) $(CXXFLAGS) -c -D ALGBMA -D BENCH -o ./obj/handler-BMA-bench.o ./src/QAPhandler.cpp \

	$(CXX) $(CXXFLAGS) -c -o ./obj/BMA-bench.o ./src/BMA.cpp \
		
	$(CXX) $(CXXFLAGS) -o ./bin/BMA-bench ./obj/BMA-bench.o ./obj/handler-BMA-bench.o \


BLS-debug: ./src/BLS_code.cpp	
	$(CXX) $(CXXFLAGS) -c -D ALGBLS -D DEBUG -o ./obj/handler-BLS-debug.o ./src/QAPhandler.cpp \

	$(CXX) $(CXXFLAGS) -c -D DEBUG -o ./obj/BLS-debug.o ./src/BLS_code.cpp \
		
	$(CXX) $(CXXFLAGS) -o ./bin/BLS-debug ./obj/BLS-debug.o ./obj/handler-BLS-debug.o \

BMA-debug: ./src/BMA.cpp	
	$(CXX) $(CXXFLAGS) -c -D ALGBMA -D DEBUG -o ./obj/handler-BMA-debug.o ./src/QAPhandler.cpp \

	$(CXX) $(CXXFLAGS) -c -D DEBUG -o ./obj/BMA-debug.o ./src/BMA.cpp \
		
	$(CXX) $(CXXFLAGS) -o ./bin/BMA-debug ./obj/BMA-debug.o ./obj/handler-BMA-debug.o \
    
MMAS: ./src/ACO/acoqap.c ./src/ACO/aco.c ./src/ACO/QAP.c ./src/ACO/QAP.h ./src/ACO/utilities.c ./src/ACO/utilities.h ./src/ACO/ants.c ./src/ACO/qap-ants.c ./src/ACO/ants.h ./src/ACO/aco-parameters.def ./src/ACO/InOut.c ./src/ACO/InOut.h ./src/ACO/unix_timer.c ./src/ACO/timer.h ./src/ACO/qap-ls.c ./src/ACO/qap-ls.h ./src/ACO/parse.c ./src/ACO/parse.h ./src/ACO/adaptation.h 

	$(CXX) $(CXXFLAGS) -c -D ALGACO -D MODULE -o ./obj/handler-ACO.o ./src/QAPhandler.cpp \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/acoqap.o ./src/ACO/acoqap.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/aco.o ./src/ACO/aco.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/QAP-aco.o ./src/ACO/QAP.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/utilities-aco.o ./src/ACO/utilities.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/ants-aco.o ./src/ACO/ants.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/qap-ants-aco.o ./src/ACO/qap-ants.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/InOut-aco.o ./src/ACO/InOut.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/unix_timer-aco.o ./src/ACO/unix_timer.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/ls-aco.o ./src/ACO/qap-ls.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/parse-aco.o ./src/ACO/parse.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/adaptation-aco.o ./src/ACO/adaptation.c \

	$(CXX) $(ACO_CFLAGS) -o ./bin/MMAS ./obj/handler-ACO.o ./obj/acoqap.o ./obj/aco.o ./obj/QAP-aco.o ./obj/utilities-aco.o ./obj/ants-aco.o ./obj/qap-ants-aco.o ./obj/InOut-aco.o ./obj/unix_timer-aco.o ./obj/ls-aco.o ./obj/parse-aco.o ./obj/adaptation-aco.o \
	
MMAS-bench: ./src/ACO/acoqap.c ./src/ACO/aco.c ./src/ACO/QAP.c ./src/ACO/QAP.h ./src/ACO/utilities.c ./src/ACO/utilities.h ./src/ACO/ants.c ./src/ACO/qap-ants.c ./src/ACO/ants.h ./src/ACO/aco-parameters.def ./src/ACO/InOut.c ./src/ACO/InOut.h ./src/ACO/unix_timer.c ./src/ACO/timer.h ./src/ACO/qap-ls.c ./src/ACO/qap-ls.h ./src/ACO/parse.c ./src/ACO/parse.h ./src/ACO/adaptation.h 

	$(CXX) $(CXXFLAGS) -c -D ALGACO -D BENCH -D MODULE -o ./obj/handler-ACO-bench.o ./src/QAPhandler.cpp \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/acoqap.o ./src/ACO/acoqap.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/aco.o ./src/ACO/aco.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/QAP-aco.o ./src/ACO/QAP.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/utilities-aco.o ./src/ACO/utilities.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/ants-aco.o ./src/ACO/ants.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/qap-ants-aco.o ./src/ACO/qap-ants.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/InOut-aco.o ./src/ACO/InOut.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/unix_timer-aco.o ./src/ACO/unix_timer.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/ls-aco.o ./src/ACO/qap-ls.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/parse-aco.o ./src/ACO/parse.c \

	$(CXX) $(ACO_CFLAGS) -c -D MODULE -o ./obj/adaptation-aco.o ./src/ACO/adaptation.c \

	$(CXX) $(ACO_CFLAGS) -o ./bin/MMAS-bench ./obj/handler-ACO-bench.o ./obj/acoqap.o ./obj/aco.o ./obj/QAP-aco.o ./obj/utilities-aco.o ./obj/ants-aco.o ./obj/qap-ants-aco.o ./obj/InOut-aco.o ./obj/unix_timer-aco.o ./obj/ls-aco.o ./obj/parse-aco.o ./obj/adaptation-aco.o \
	

ACO-MAIN: ./src/ACO/acoqap.c ./src/ACO/aco.c ./src/ACO/QAP.c ./src/ACO/QAP.h ./src/ACO/utilities.c ./src/ACO/utilities.h ./src/ACO/ants.c ./src/ACO/qap-ants.c ./src/ACO/ants.h ./src/ACO/aco-parameters.def ./src/ACO/InOut.c ./src/ACO/InOut.h ./src/ACO/unix_timer.c ./src/ACO/timer.h ./src/ACO/qap-ls.c ./src/ACO/qap-ls.h ./src/ACO/parse.c ./src/ACO/parse.h ./src/ACO/adaptation.h 

	$(CXX) $(ACO_CFLAGS) -c  -o ./obj/acoqap.o ./src/ACO/acoqap.c \

	$(CXX) $(ACO_CFLAGS) -c  -o ./obj/aco.o ./src/ACO/aco.c \

	$(CXX) $(ACO_CFLAGS) -c  -o ./obj/QAP-aco.o ./src/ACO/QAP.c \

	$(CXX) $(ACO_CFLAGS) -c  -o ./obj/utilities-aco.o ./src/ACO/utilities.c \

	$(CXX) $(ACO_CFLAGS) -c  -o ./obj/ants-aco.o ./src/ACO/ants.c \

	$(CXX) $(ACO_CFLAGS) -c  -o ./obj/qap-ants-aco.o ./src/ACO/qap-ants.c \

	$(CXX) $(ACO_CFLAGS) -c  -o ./obj/InOut-aco.o ./src/ACO/InOut.c \

	$(CXX) $(ACO_CFLAGS) -c  -o ./obj/unix_timer-aco.o ./src/ACO/unix_timer.c \

	$(CXX) $(ACO_CFLAGS) -c  -o ./obj/ls-aco.o ./src/ACO/qap-ls.c \

	$(CXX) $(ACO_CFLAGS) -c  -o ./obj/parse-aco.o ./src/ACO/parse.c \

	$(CXX) $(ACO_CFLAGS) -c  -o ./obj/adaptation-aco.o ./src/ACO/adaptation.c \

	$(CXX) $(ACO_CFLAGS) -o ./bin/ACO-main ./obj/acoqap.o ./obj/aco.o ./obj/QAP-aco.o ./obj/utilities-aco.o ./obj/ants-aco.o ./obj/qap-ants-aco.o ./obj/InOut-aco.o ./obj/unix_timer-aco.o ./obj/ls-aco.o ./obj/parse-aco.o ./obj/adaptation-aco.o \

ROTS-MAIN: ./src/rots.c 

	$(CXX) -c -D ROTSMAIN -o ./obj/rots.o ./src/rots.c 
	
	$(CXX) $(CXXFLAGS) -o ./bin/ROTS-main ./obj/rots.o

ROTS: ./src/rots.c 	
	$(CXX) $(CXXFLAGS) -c -D ALGROTS -o ./obj/handler-ROTS.o ./src/QAPhandler.cpp \

	$(CXX) $(CXXFLAGS) -c -o ./obj/ROTS.o ./src/rots.c  \
		
	$(CXX) $(CXXFLAGS) -o ./bin/ROTS ./obj/ROTS.o ./obj/handler-ROTS.o \

ROTS-bench: ./src/rots.c 	
	$(CXX) $(CXXFLAGS) -c -D ALGROTS -D BENCH -o ./obj/handler-ROTS-bench.o ./src/QAPhandler.cpp \

	$(CXX) $(CXXFLAGS) -c -o ./obj/ROTS-bench.o ./src/rots.c  \
		
	$(CXX) $(CXXFLAGS) -o ./bin/ROTS-bench ./obj/ROTS-bench.o ./obj/handler-ROTS-bench.o \

ROTS-debug: ./src/rots.c 	
	$(CXX) $(CXXFLAGS) -c -D ALGROTS -D DEBUG -o ./obj/handler-ROTS-debug.o ./src/QAPhandler.cpp \

	$(CXX) $(CXXFLAGS) -c -D DEBUG -o ./obj/ROTS-debug.o ./src/rots.c  \
		
	$(CXX) $(CXXFLAGS) -o ./bin/ROTS-debug ./obj/ROTS-debug.o ./obj/handler-ROTS-debug.o \

handler: ./src/QAPhandler.cpp
	$(CXX) $(CXXFLAGS) -c -D NONE -o ./obj/handler.o ./src/QAPhandler.cpp \
		
	$(CXX) $(CXXFLAGS) -o ./bin/handler ./obj/handler.o \
	
clean:
	rm -f ./bin/* ./obj/*.o