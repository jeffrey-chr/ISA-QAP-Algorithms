#ifndef _ACO_H_
#define _ACO_H_

#include "../structs.h"

/* Problem specific */
void construct_solutions( void );
void mmas_update( void );
long int compute_tour_length(const long int *t );

#ifdef MODULE
int jtc_interface_aco(QAP_input* qinput, QAP_output** qoutput);
#endif

#endif
