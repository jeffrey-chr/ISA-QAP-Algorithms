#ifndef STRUCTS_H
#define STRUCTS_H

#include <cstdint>

typedef int_fast64_t reallng;
typedef reallng* ptr_reallng;

struct QAP_input {
    int n;
    reallng** dist;
    reallng** flow;
    double maxtime;
    int ntrials;

    int nalgparams;
    double* algparams;

    QAP_input(int nn, reallng** dd, reallng** ff, double t, int ntry, int npar, double* par) : n(nn), dist(dd), flow(ff), maxtime(t), ntrials(ntry), nalgparams(npar), algparams(par) { ; }
    ~QAP_input() { ; } // The handler is responsible for deleting dist, flow and algparams.
};
typedef struct QAP_input QAP_input;

typedef struct QAP_output{
    reallng value;
    double time_for_best;
    
    QAP_output(reallng v, double t) : value(v), time_for_best(t) {;}
    ~QAP_output() {;}
} QAP_output;

#endif