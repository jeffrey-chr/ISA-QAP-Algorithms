#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct QAP_input{
    int n;
    long** dist;
    long** flow;
    double maxtime;
    int ntrials;
	
	int nalgparams;
	double* algparams;
    
    QAP_input(int nn, long** dd, long** ff, double t, int ntry, int npar, double* par) : n(nn), dist(dd), flow(ff), maxtime(t), ntrials(ntry), nalgparams(npar), algparams(par) {;}
    ~QAP_input() {;} // The handler is responsible for deleting dist, flow and algparams.
} QAP_input;

typedef struct QAP_output{
    long value;
    double time_for_best;
    
    QAP_output(long v, double t) : value(v), time_for_best(t) {;}
    ~QAP_output() {;}
} QAP_output;

#endif