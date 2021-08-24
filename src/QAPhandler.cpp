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

typedef long*   ptr_long;

#ifdef NONE
    // just testing the file reading code
#endif

#ifdef BLS
#include "BLS_code.h"
#endif

#ifdef BMA
#include "BMA.h"
#endif

#ifdef ACO
#include "ACO/aco.h"
#endif

#include "structs.h"

int main(int argc, char *argv[])
{
    bool reduce = false;
    bool reduceA = false;
    bool reduceB = false;
    
    long additive = 0;
    long additiveADiag = 0;
    long additiveAElse = 0;
    long additiveBDiag = 0;
    long additiveBElse = 0;
    
    long addAdjust = 0;
    
    bool invert = false;
    
    long invertAdjust = 0;
    
    long multiplier = 1;
    long multiplierA = 1;
    long multiplierB = 1;
    
    long multAdjust = 1;
    
    bool forceSwap = false;

    int n;
    long** dist;
    long** flow;
    long maxdist = 0;
    long maxflow = 0;
    
    long mindistdiag = 1e9;
    long minflowdiag = 1e9;
    long mindistelse = 1e9;
    long minflowelse = 1e9;
    long tracedist = 0;
    long traceflow = 0;
    long offsumdist = 0;
    long offsumflow = 0;
    
    if(argc>=2)
    {
        printf("INSTANCENAME:\n%s\n",argv[1]);
    }
    else
    {
        printf("No file name provided");
        throw(-1);
    }
    
    const char* path = argv[1];
    
    // Some quick and dirty command line parameters for modifying the instance.
    
    if (argc >= 3)
    {
        for (int i = 2; i < argc; i++)
        {
            if (std::strcmp(argv[i], "-reduce") == 0) {
                reduce = true;
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
        }
    }
    
    if (reduce) { reduceA = true; reduceB = true; }
    
    additiveADiag += additive;
    additiveAElse += additive;
    additiveBDiag += additive;
    additiveBElse += additive;
    
    multiplierA *= multiplier;
    multiplierB *= multiplier;
    
    //
    
    std::ifstream myfile;
    myfile.open(path);
    
    std::string line;
    std::string entry;
    int ientry;
    
    std::string delimiter = " ";
    
    if (myfile.is_open())
	{    
		getline (myfile, line);
        while (line.find_first_not_of(' ') == std::string::npos)
        {   
            // This line contains only whitespace. Get the next one.
            getline (myfile, line);
        }
        
        // This line should contain one positive integer, n.
        {
            std::istringstream iss(line);
            iss >> entry;
            n = std::stoi(entry);
        }
        
        dist = new ptr_long[n];
        flow = new ptr_long[n];
        for (int i = 0; i < n; i++)
        {
            dist[i] = new long[n];
            flow[i] = new long[n];
        }            
        
        getline (myfile, line);
        while (line.find_first_not_of(' ') == std::string::npos)
        {   
            // This line contains only whitespace. Get the next one.
            getline (myfile, line);
        }
        
        
        // READ THE FIRST MATRIX
        int ii = 0;
        int jj = 0;
        while (line.find_first_not_of(' ') != std::string::npos && (jj != 0 || ii != n))
        {   
            // This line contains non-whitespace - read it.
            std::istringstream iss(line);
            while (iss)
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
                }
            }
            getline (myfile, line);
        }
        if (jj != 0 || ii != n) { throw (-1); }
        
        while (line.find_first_not_of(' ') == std::string::npos)
        {   
            // This line contains only whitespace. Get the next one.
            getline (myfile, line);
        }
        
        // READ THE SECOND MATRIX
        ii = 0;
        jj = 0;
        while (line.find_first_not_of(' ') != std::string::npos && (jj != 0 || ii != n))
        {   
            // This line contains non-whitespace - read it.
            std::istringstream iss(line);
            while (iss)
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
                }
                
            }
            getline (myfile, line);
        }
        if (jj != 0 || ii != n) { throw (-1); }
    
        myfile.close();
    
        int ntrials = 5;
        
#ifdef DEBUG
        ntrials = 1;
#endif
        
        double maxtime = 0;
        
#ifdef BENCH
        maxtime = 8*n + 20;
        if (n <= 30)
            { maxtime = 2*n + 20; }
        else
            { maxtime = 2*30 + 20 + (n-30)*8; }
#else
        maxtime = 0.0228*(n*n) - 1.3722*n + 18.8760;
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
                for (int j = i+1; j < n; j++)
                {
                    dist[i][j] += additiveAElse;
                    flow[i][j] += additiveBElse;
                    dist[j][i] += additiveAElse;
                    flow[j][i] += additiveBElse;
                }
                dist[i][i] += additiveADiag;
                flow[i][i] += additiveBDiag;
            }
            
            addAdjust = (additiveBDiag * tracedist) + (additiveADiag * traceflow) + (n * additiveADiag * additiveBDiag) + (additiveBElse * offsumdist) + (additiveAElse * offsumflow) + (n * (n-1) * additiveBElse * additiveAElse);
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
                for (int j = i+1; j < n; j++)
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
            
            //std::cout << LONG_MAX << "\n";
            if (maxdist * maxflow >= ((LONG_MAX / n) / n) || maxdist * maxflow * n * n <= -((LONG_MAX / n) / n))
            {
                std::cout << "\tERROR: Invert potentially exceeds LONG_MAX, terminating" << "\n";
                throw(-1);
            }
            
            std::cout << "\tCurrent maxdist: " << maxdist << "\n";
            std::cout << "\tCurrent maxflow: " << maxflow << "\n";
            std::cout << "\tCurrent tracedist: " << tracedist << "\n";
            std::cout << "\tCurrent traceflow: " << traceflow << "\n";
            std::cout << "\tCurrent offsumdist: " << offsumdist << "\n";
            std::cout << "\tCurrent offsumflow: " << offsumflow << "\n";
            
            invertAdjust = (maxflow * tracedist) + (maxdist * traceflow) + (n * maxdist * maxflow) + (maxflow * offsumdist) + (maxdist * offsumflow) + (n * (n-1) * maxflow * maxdist);
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
            ptr_long tmpptr;
            for (int i = 0; i < n; i++)
            {
                tmpptr = dist[i];
                dist[i] = flow[i];
                flow[i] = tmpptr;
            } 
        }
        
        QAP_input* qinput = new QAP_input(n, dist, flow, maxtime, ntrials);
    
        QAP_output** qoutput = new QAP_output*[ntrials];
        for (int i = 0; i < ntrials; i++)
        {
            qoutput[i] = new QAP_output(1e9,1e9);
        }
    
        std::cout << "INSTANCESIZE:\n";
        std::cout << n << "\n";
        std::cout << "MAXTIME:\n" << maxtime << "\n";
    
        std::cout << "ALGORITHMNAME:\n";
    
#ifdef NONE
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
                for (int j = 0; j < mxline-7; j++)
                {
                    std::cout << dist[i][j] << " ";
                }
                std::cout << "... ";
                for (int j = n-5; j < n; j++)
                {
                    std::cout << dist[i][j] << " ";
                }
                std::cout << '\n';
            }
            std::cout << '\n';
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < mxline-7; j++)
                {
                    std::cout << flow[i][j] << " ";
                }
                std::cout << "... ";
                for (int j = n-5; j < n; j++)
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
        
#ifdef BLS
        std::cout << "BLS\n";
        jtc_interface_bls(qinput, qoutput);
#endif

#ifdef BMA
        std::cout << "BMA\n";
        jtc_interface_bma(qinput, qoutput);
#endif

#ifdef ACO
        std::cout << "ACO\n";
        jtc_interface_aco(qinput, qoutput);
#endif

        long bestvalue = 1e9;
        double averagetime = 0;
        double averagesoln = 0;
        int nbest = 0;
        for (int i = 0; i < ntrials; i++)
        {
            std::cout << "Trial " << i << ": " << qoutput[i]->value << "\nTime: " << qoutput[i]->time_for_best << "s\n";
            averagetime += qoutput[i]->time_for_best;
            averagesoln += qoutput[i]->value;
            nbest++;
#ifdef BENCH
            if (qoutput[i]->time_for_best > 0.5*maxtime) {std::cout << "WARNING: may not have given enough time\n";}
#endif
        }
        assert(nbest > 0);
        averagetime = averagetime / nbest;
        averagesoln = averagesoln / nbest;
        std::cout << "AVERAGETIMEFORBEST:\n" << averagetime << "\n";
        
        printf("AVERAGESOLN:\n%.6f\n",averagesoln);
        
        printf("ADJUSTEDSOLN:\n%.6f\n",(averagesoln / multAdjust) - invertAdjust - addAdjust);
        
        // TODO delete dist and flow.
    
    }
    
    return 0;
    
}