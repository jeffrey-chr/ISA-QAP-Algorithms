#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <assert.h>

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

#include "structs.h"

int main(int argc, char *argv[])
{
    
    int n;
    long** dist;
    long** flow;
    
    
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
        
        // The next lines should contain the distance matrix.
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
        
        
        
        /* for (int i = 0; i < n; i++)
        {
            std::istringstream iss(line);
            for (int j = 0; j < n; j++)
            {
                iss >> entry;
                dist[i][j] = std::stoi(entry);
            }
            getline (myfile, line);
        } */
        
        while (line.find_first_not_of(' ') == std::string::npos)
        {   
            // This line contains only whitespace. Get the next one.
            getline (myfile, line);
        }
        
        // The next lines should contain the flow matrix.
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
#else

#ifdef BLS
        std::cout << "BLS\n";
        jtc_interface_bls(qinput, qoutput);
#endif

#ifdef BMA
        std::cout << "BMA\n";
        jtc_interface_bma(qinput, qoutput);
#endif

        long bestvalue = 1e9;
        double averagetime = 0;
        double averagesoln = 0;
        int nbest = 0;
        for (int i = 0; i < ntrials; i++)
        {
            std::cout << "Trial " << i << ": " << qoutput[i]->value << "\nTime: " << qoutput[i]->time_for_best << "s\n";
            /* if (qoutput[i]->value < bestvalue) {
                bestvalue = qoutput[i]->value;
                averagetime = 0;
                nbest = 0;
            }
            if (qoutput[i]->value == bestvalue) {
                averagetime += qoutput[i]->time_for_best;
                nbest++;
            }  */
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
        
#endif
        // TODO delete dist and flow.
    
    }
    
    
    
}