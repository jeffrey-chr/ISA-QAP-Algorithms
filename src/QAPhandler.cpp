#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <assert.h>
#include <cstring>
#include <climits>
#include <ctime>
#include <cstdlib>

#if defined(unix) || defined(__unix__) || defined(__unix)
#define PREDEF_PLATFORM_UNIX
#endif


#include <fcntl.h>
#ifdef PREDEF_PLATFORM_UNIX
#include <sys/wait.h>
#include <unistd.h>
#endif
// #include "matrixScrambler.h"

typedef int_fast64_t reallng;
typedef reallng*   ptr_reallng;

#ifdef NONE
    // just testing the file reading code
#endif

#ifdef ALGBLS
#include "BLS_code.h"
#endif

#ifdef ALGBMA
#include "BMA.h"
#endif

#ifdef ALGACO
#include "ACO/aco.h"
#endif

#include "structs.h"

int main(int argc, char* argv[])
{
    int ntrials = 1;
    bool indtrials = false;
    int thisindtrial = -1;
    int maxindtrial = -1;

    bool reduce = false;
    bool reduceA = false;
    bool reduceB = false;

    reallng additive = 0;
    reallng additiveADiag = 0;
    reallng additiveAElse = 0;
    reallng additiveBDiag = 0;
    reallng additiveBElse = 0;

    reallng addAdjust = 0;

    bool invert = false;

    reallng invertAdjust = 0;

    reallng multiplier = 1;
    reallng multiplierA = 1;
    reallng multiplierB = 1;

    reallng multAdjust = 1;

    bool forceSwap = false;

    bool scramble = false;
	
	bool trashStdOut = true;

    int n;
    reallng** dist;
    reallng** flow;
    reallng maxdist = 0;
    reallng maxflow = 0;

    reallng mindistdiag = (reallng)1e9;
    reallng minflowdiag = (reallng)1e9;
    reallng mindistelse = (reallng)1e9;
    reallng minflowelse = (reallng)1e9;
    reallng tracedist = 0;
    reallng traceflow = 0;
    reallng offsumdist = 0;
    reallng offsumflow = 0;

    // printf("argc is %d\n", argc);

    if (argc < 2)
    {
        printf("No file name provided");
        throw(-1);
    }

    const char* path = argv[1];

    std::string algparamstring = "";

    // Some quick and dirty command line parameters for modifying the instance.

    if (argc >= 3)
    {
        for (int i = 2; i < argc; i++)
        {
            if (std::strcmp(argv[i], "-reduce") == 0) {
                reduce = true;
            }
			if (std::strcmp(argv[i], "-donttrash") == 0) {
                trashStdOut = false;
            }
            if (std::strcmp(argv[i], "-addall") == 0) {
                i++;
                if (i < argc)
                {
                    additive = std::stol(argv[i]);
                }
            }
            if (std::strcmp(argv[i], "-addAdiag") == 0) {
                i++;
                if (i < argc)
                {
                    additiveADiag = std::stol(argv[i]);
                }
            }
            if (std::strcmp(argv[i], "-addAelse") == 0) {
                i++;
                if (i < argc)
                {
                    additiveAElse = std::stol(argv[i]);
                }
            }
            if (std::strcmp(argv[i], "-addBdiag") == 0) {
                i++;
                if (i < argc)
                {
                    additiveBDiag = std::stol(argv[i]);
                }
            }
            if (std::strcmp(argv[i], "-addBelse") == 0) {
                i++;
                if (i < argc)
                {
                    additiveBElse = std::stol(argv[i]);
                }
            }
            if (std::strcmp(argv[i], "-multall") == 0) {
                i++;
                if (i < argc)
                {
                    multiplier = std::stoi(argv[i]);
                }
            }
            if (std::strcmp(argv[i], "-multA") == 0) {
                i++;
                if (i < argc)
                {
                    multiplierA = std::stoi(argv[i]);
                }
            }
            if (std::strcmp(argv[i], "-multB") == 0) {
                i++;
                if (i < argc)
                {
                    multiplierB = std::stoi(argv[i]);
                }
            }
            if (std::strcmp(argv[i], "-invert") == 0) {
                invert = true;
            }
            if (std::strcmp(argv[i], "-swap") == 0) {
                forceSwap = true;
            }
            if (std::strcmp(argv[i], "-scramble") == 0) {
                scramble = true;
            }
            if (std::strcmp(argv[i], "-trials") == 0) {
                i++;
                if (i < argc)
                {
                    ntrials = std::stoi(argv[i]);
                }
            }
            if (std::strcmp(argv[i], "-indtrial") == 0) {
                indtrials = true;
                i++;
                if (i < argc)
                {
                    thisindtrial = std::stoi(argv[i]);
                }
                i++;
                if (i < argc)
                {
                    maxindtrial = std::stoi(argv[i]);
                }
            }
            if (std::strcmp(argv[i], "-algparams") == 0) {
                i++;
                if (i < argc)
                {
                    algparamstring.assign(argv[i]);
                }
            }
        }
    }

    if (scramble) { std::srand(unsigned(std::time(0))); }

    if (reduce) { reduceA = true; reduceB = true; }


    additiveADiag += additive;
    additiveAElse += additive;
    additiveBDiag += additive;
    additiveBElse += additive;

    multiplierA *= multiplier;
    multiplierB *= multiplier;

    //
    int nalgpar = 0;
    double* algpars;
    if (algparamstring.length() > 0)
    {
        nalgpar = 1;
        std::size_t found = algparamstring.find_first_of(",");
        while (found != std::string::npos)
        {
            nalgpar++;
            found = algparamstring.find_first_of(",", found + 1);
        }
        //std::cout << "nalgpar: " << nalgpar << "\n";
        algpars = new double[nalgpar];
        for (int j = 0; j < nalgpar; j++) { algpars[j] = -1; }
        std::size_t start = 0;
        found = algparamstring.find_first_of(",");
        int i = 0;
        while (found != std::string::npos && i < nalgpar)
        {
            algpars[i] = std::stod(algparamstring.substr(start, found - start));
            i = i + 1;
            start = found + 1;
            found = algparamstring.find_first_of(",", found + 1);
        }
        algpars[i] = std::stod(algparamstring.substr(start, std::string::npos));
    }
    else
    {
        algpars = new double[1];
    }

    //

    std::ifstream myfile;
    myfile.open(path);

    std::string line;
    std::string entry;
    int ientry;

    std::string delimiter = " ";

    if (myfile.is_open())
    {
        getline(myfile, line);
        while (line.find_first_not_of(' ') == std::string::npos)
        {
            // This line contains only whitespace. Get the next one.
            getline(myfile, line);
        }

        // This line should contain one positive integer, n.
        {
            std::istringstream iss(line);
            iss >> entry;
            n = std::stoi(entry);
        }

        dist = new ptr_reallng[n];
        flow = new ptr_reallng[n];
        for (int i = 0; i < n; i++)
        {
            dist[i] = new reallng[n];
            flow[i] = new reallng[n];
        }

        getline(myfile, line);
        while (line.find_first_not_of(' ') == std::string::npos)
        {
            // This line contains only whitespace. Get the next one.
            getline(myfile, line);
        }


        // READ THE FIRST MATRIX
        int ii = 0;
        int jj = 0;
        while (line.find_first_not_of(' ') != std::string::npos && (jj != 0 || ii != n))
        {
            bool mat1done = false;
            // This line contains non-whitespace - read it.
            std::istringstream iss(line);
            while (!mat1done && iss)
            {
                iss >> ientry;
                if (iss)
                {
                    dist[ii][jj] = ientry;
                    maxdist = (maxdist > ientry ? maxdist : ientry);

                    if (ii == jj) {
                        mindistdiag = (mindistdiag < ientry ? mindistdiag : ientry);
                        tracedist += ientry;
                    }
                    else
                    {
                        mindistelse = (mindistelse < ientry ? mindistelse : ientry);
                        offsumdist += ientry;
                    }

                    jj++;
                    if (jj >= n) {
                        jj = 0;
                        ii++;
                    }
                    if (ii >= n) {
                        mat1done = true;
                    }
                }
            }
            getline(myfile, line);
        }
        if (jj != 0 || ii != n) { throw (-1); }

        while (line.find_first_not_of(' ') == std::string::npos)
        {
            // This line contains only whitespace. Get the next one.
            getline(myfile, line);
        }

        // READ THE SECOND MATRIX
        ii = 0;
        jj = 0;
        while (line.find_first_not_of(' ') != std::string::npos && (jj != 0 || ii != n))
        {
            bool mat2done = false;
            // This line contains non-whitespace - read it.
            std::istringstream iss(line);
            while (!mat2done && iss)
            {
                iss >> ientry;
                if (iss)
                {
                    flow[ii][jj] = ientry;
                    maxflow = (maxflow > ientry ? maxflow : ientry);

                    if (ii == jj) {
                        minflowdiag = (minflowdiag < ientry ? minflowdiag : ientry);
                        traceflow += ientry;
                    }
                    else
                    {
                        minflowelse = (minflowelse < ientry ? minflowelse : ientry);
                        offsumflow += ientry;
                    }

                    jj++;
                    if (jj >= n) {
                        jj = 0;
                        ii++;
                    }
                    if (ii >= n) {
                        mat2done = true;
                    }
                }

            }
            getline(myfile, line);
        }
        if (jj != 0 || ii != n) { throw (-1); }

        myfile.close();

#ifdef DEBUG
        ntrials = 1;
#endif

        double maxtime = 0;

#ifdef BENCH
        maxtime = 8 * n + 20;
        if (n <= 5)
        {
            maxtime = 120;
        }
        else
        {
            maxtime = 2400;
        }
#else
        //maxtime = 0.0228*(n*n) - 1.3722*n + 18.8760;
        //maxtime = 0.001*((n-25)*(n-25)*(n-25)) - 0.085*((n-25)*(n-25)) + 4.45*(n-25) - 14.7;
        maxtime = 0.0445 * ((n - 25) * (n - 25)) + 0.9471 * (n - 25) + 7.6896;
        if (maxtime < 5)
        {
            maxtime = 5;
        }
#endif

#ifdef DEBUG
        maxtime = 10;
#endif

        if (reduceA) {
            std::cout << "**** Reducing first matrix" << "\n";
            additiveADiag -= mindistdiag;
            additiveAElse -= mindistelse;
        }

        if (reduceB) {
            std::cout << "**** Reducing second matrix" << "\n";
            additiveBDiag -= minflowdiag;
            additiveBElse -= minflowelse;
        }

        if (additiveADiag != 0 || additiveAElse != 0 || additiveBDiag != 0 || additiveBElse != 0)
        {
            std::cout << "**** Applying additive changes to problem matrices" << "\n";
            std::cout << "\tadditiveADiag: " << additiveADiag << "\n";
            std::cout << "\tadditiveAElse: " << additiveAElse << "\n";
            std::cout << "\tadditiveBDiag: " << additiveBDiag << "\n";
            std::cout << "\tadditiveBElse: " << additiveBElse << "\n";

            for (int i = 0; i < n; i++)
            {
                for (int j = i + 1; j < n; j++)
                {
                    dist[i][j] += additiveAElse;
                    flow[i][j] += additiveBElse;
                    dist[j][i] += additiveAElse;
                    flow[j][i] += additiveBElse;
                }
                dist[i][i] += additiveADiag;
                flow[i][i] += additiveBDiag;
            }

            addAdjust = (additiveBDiag * tracedist) + (additiveADiag * traceflow) + (n * additiveADiag * additiveBDiag) + (additiveBElse * offsumdist) + (additiveAElse * offsumflow) + (n * (n - 1) * additiveBElse * additiveAElse);
            std::cout << "\tAdditive step adjustment to objective: " << addAdjust << "\n";
        }

        if (invert)
        {
            std::cout << "**** Inverting both data matrices" << "\n";

            // Unfortunately we need to recalculate these after the additive change. 
            // Could do some of them a little more cleverly but this will be fast enough.
            maxdist = -1e9;
            maxflow = -1e9;
            tracedist = 0;
            traceflow = 0;
            offsumdist = 0;
            offsumflow = 0;
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    maxdist = (maxdist > dist[i][j] ? maxdist : dist[i][j]);
                    maxflow = (maxflow > flow[i][j] ? maxflow : flow[i][j]);
                }
                for (int j = i + 1; j < n; j++)
                {
                    offsumdist -= dist[i][j] + dist[j][i];
                    offsumflow -= flow[i][j] + flow[j][i];
                }
                tracedist -= dist[i][i];
                traceflow -= flow[i][i];
            }

            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    dist[i][j] = maxdist - dist[i][j];
                    flow[i][j] = maxflow - flow[i][j];
                }
            }

            //std::cout << reallng_MAX << "\n";
            if (maxdist * maxflow >= ((INT_FAST64_MAX / n) / n) || maxdist * maxflow * n * n <= -((INT_FAST64_MAX / n) / n))
            {
                std::cout << "\tERROR: Invert potentially exceeds INT_FAST64_MAX, terminating" << "\n";
                throw(-1);
            }

            std::cout << "\tCurrent maxdist: " << maxdist << "\n";
            std::cout << "\tCurrent maxflow: " << maxflow << "\n";
            std::cout << "\tCurrent tracedist: " << tracedist << "\n";
            std::cout << "\tCurrent traceflow: " << traceflow << "\n";
            std::cout << "\tCurrent offsumdist: " << offsumdist << "\n";
            std::cout << "\tCurrent offsumflow: " << offsumflow << "\n";

            invertAdjust = (maxflow * tracedist) + (maxdist * traceflow) + (n * maxdist * maxflow) + (maxflow * offsumdist) + (maxdist * offsumflow) + (n * (n - 1) * maxflow * maxdist);
            std::cout << "\tInversion step adjustment to objective: " << invertAdjust << "\n";

        }

        if (multiplierA != 1 || multiplierB != 1)
        {
            std::cout << "**** Multiplying data matrices" << "\n";
            std::cout << "\tMultiplier for first matrix: " << multiplierA << "\n";
            std::cout << "\tMultiplier for second matrix: " << multiplierB << "\n";

            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    dist[i][j] = dist[i][j] * multiplierA;
                    flow[i][j] = flow[i][j] * multiplierB;
                }
            }
            multAdjust = multiplierA * multiplierB;
            std::cout << "\tMultiplier adjustment to objective: " << multAdjust << "\n";
        }

        if (forceSwap)
        {
            ptr_reallng tmpptr;
            for (int i = 0; i < n; i++)
            {
                tmpptr = dist[i];
                dist[i] = flow[i];
                flow[i] = tmpptr;
            }
        }

        QAP_input* qinput = new QAP_input(n, dist, flow, maxtime, ntrials, nalgpar, algpars);

        QAP_output** qoutput = new QAP_output * [ntrials];
        for (int i = 0; i < ntrials; i++)
        {
            qoutput[i] = new QAP_output(1e9, 1e9);
        }

        if (!indtrials || thisindtrial == 1)
        {
            printf("INSTANCENAME:\n%s\n", argv[1]);
            std::cout << "INSTANCESIZE:\n";
            std::cout << n << "\n";
            std::cout << "MAXTIME:\n" << maxtime << "\n";
            std::cout << "ALGPARAMETERS:" << "\n";
            for (int i = 0; i < nalgpar; i++) {
                std::cout << algpars[i] << " ";
            }
            std::cout << '\n';
            std::cout << "ALGORITHMNAME:\n";
        }

#ifdef NONE
    if (!indtrials || thisindtrial == 1) {
        std::cout << "HANDLERTEST\n";
    }
    int mxline = 25;

    std::cout << n << "\n";
    if (n < mxline)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                std::cout << dist[i][j] << " ";
            }
            std::cout << '\n';
        }
        std::cout << '\n';
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                std::cout << flow[i][j] << " ";
            }
            std::cout << '\n';
        }
    }
    else
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < mxline - 7; j++)
            {
                std::cout << dist[i][j] << " ";
            }
            std::cout << "... ";
            for (int j = n - 5; j < n; j++)
            {
                std::cout << dist[i][j] << " ";
            }
            std::cout << '\n';
        }
        std::cout << '\n';
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < mxline - 7; j++)
            {
                std::cout << flow[i][j] << " ";
            }
            std::cout << "... ";
            for (int j = n - 5; j < n; j++)
            {
                std::cout << flow[i][j] << " ";
            }
            std::cout << '\n';
        }
    }

    return 0;
#endif

    if (maxdist == 0) {
        std::cout << "ABORTING: Distance matrix is all zeros";
        return 0;
    }
    if (maxflow == 0) {
        std::cout << "ABORTING: Flow matrix is all zeros";
        return 0;
    }



#ifdef ALGBLS
    if (!indtrials || thisindtrial == 1) {
        std::cout << "BLS\n";
    }
jtc_interface_bls(qinput, qoutput);
#endif

#ifdef ALGBMA
    if (!indtrials || thisindtrial == 1) {
        std::cout << "BMA\n";
    }
        jtc_interface_bma(qinput, qoutput);
#endif

#ifdef ALGACO
        if (!indtrials || thisindtrial == 1) {
            std::cout << "MMAS\n";
        }
		fflush(stdout);
		
#ifdef PREDEF_PLATFORM_UNIX
		int save_out = -99;
		int devNull = -99;
		int dup2Result = -99;
		if (trashStdOut) {
			save_out = dup(STDOUT_FILENO);
			if(save_out == -1){
				fprintf(stderr,"Error in dup(STDOUT_FILENO)\n");
				exit(EXIT_FAILURE);
			}
			
			devNull = open("/dev/null",O_WRONLY);
			if(devNull == -1){
				fprintf(stderr,"Error in open('/dev/null',0)\n");
				exit(EXIT_FAILURE);
			}
			
			dup2Result = dup2(devNull, STDOUT_FILENO);
			if(dup2Result == -1) {
				fprintf(stderr,"Error in dup2(devNull, STDOUT_FILENO)\n");
				exit(EXIT_FAILURE);
			}
		}
#endif
		
		jtc_interface_aco(qinput, qoutput);
		
#ifdef PREDEF_PLATFORM_UNIX
		if (trashStdOut) {
			dup2(save_out, STDOUT_FILENO);
		}
#endif
		
		
#endif

        reallng bestvalue = 1e9;
        double averagetime = 0;
        double averagesoln = 0;
        int nbest = 0;
        if (!indtrials || thisindtrial == 1) {
            std::cout << "TRIALS:\n";
        }
        for (int i = 0; i < ntrials; i++)
        {
            if (!indtrials || thisindtrial == 1)
            {
                std::cout << "Trial" << i << "," << qoutput[i]->value << "," << ((qoutput[i]->value / multAdjust) - invertAdjust - addAdjust) << "," << qoutput[i]->time_for_best << "s\n";
            }
            else
            {
                std::cout << "Trial" << thisindtrial-1 << "," << qoutput[i]->value << "," << ((qoutput[i]->value / multAdjust) - invertAdjust - addAdjust) << "," << qoutput[i]->time_for_best << "s\n";
            }
            averagetime += qoutput[i]->time_for_best;
            averagesoln += qoutput[i]->value;
            nbest++;
        }
        if (!indtrials || thisindtrial == maxindtrial) {
            std::cout << "TRIALSEND\n";
        }
        assert(nbest > 0);
        averagetime = averagetime / nbest;
        averagesoln = averagesoln / nbest;
        if (!indtrials)
        {
            std::cout << "AVERAGETIMEFORBEST:\n" << averagetime << "\n";
#ifdef BENCH
            //if (averagetime > 0.75 * maxtime) { std::cout << "WARNING: may not have given enough time\n"; }
#endif

            printf("AVERAGESOLN:\n%.6f\n", averagesoln);

            printf("ADJUSTEDSOLN:\n%.6f\n", (averagesoln / multAdjust) - invertAdjust - addAdjust);
        }
        else
        {
            if (thisindtrial == maxindtrial) {
                printf("MULTADJUST:\n%d\n", multAdjust);
                printf("INVERTADJUST:\n%d\n", invertAdjust);
                printf("ADDADJUST:\n%d\n", addAdjust);
            }
        }
        
        // TODO delete dist and flow.

        delete qinput;

        for (int i = 0; i < ntrials; i++)
        {
            delete qoutput[i];
        }
        delete[] qoutput;

        
        for (int i = 0; i < n; i++)
        {
            delete[] dist[i];
            delete[] flow[i];
        }
        delete[] dist;
        delete[] flow;
    
    }

    delete[] algpars;
    
    return 0;
    
}