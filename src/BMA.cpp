#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <map>
#include<cmath>
#include<cstdint>

#include "structs.h"

typedef int_fast64_t reallng;

typedef int*   type_vector;

typedef reallng** type_matrix;
typedef unsigned short int** type_matrix_s;


const reallng infinite = 999999999;

const int MAX_N = 150;
const int MAX_POP_SIZE = 50;

using namespace std;

unsigned short int all_results[100][230];
unsigned short int best_solution_found[300];

double perturb_str, init_ptr;
clock_t start;
reallng iteration = 0;

unsigned long long resulting_cost = 99999999999ULL;
reallng target;

double global_maxtime;


void output(reallng n, string f_name, reallng all_sol[],  int num_runs, reallng best_cost, type_vector & s, double times[])
{
            char una[70];
            
             cout<<f_name<<endl;
            for(int i=0; i<f_name.size(); i++)
               una[i]=f_name[i];       
            una[f_name.size()]=0;
            cout<<"OUTPUTING"<<endl;
             double sum = 0;    
             double STD_DEV = 0; // returning zero's
             double time=0.0, avg_time=0.0;      
             int c=0;
             for (int i = 0; i < num_runs; i++)
             {
                sum = sum + all_sol[i];
                avg_time+=times[i];
                if(all_sol[i]==best_cost)
                {
                  time+=times[i]; c++;
                }
             }
             time/=static_cast<double>(c);
             avg_time/=static_cast<double>(num_runs);
             int mean = sum/num_runs;
              for (int i = 0; i < num_runs; i++)
                STD_DEV +=((static_cast<double>(all_sol[i])-mean)*(static_cast<double>(all_sol[i])-mean))/static_cast<double>(num_runs-1);           
              double s_dev = sqrt(STD_DEV);
            
             
 
            ofstream out;
            out.open(una);
            out<<best_cost<<"   "<<c<<"   "<<time<<"  "<<avg_time<<" "<<mean<<" "<<s_dev<< endl;
           for(int i=0; i<num_runs; i++)
           {
              out<<all_sol[i]<<" "<<times[i]<<endl;
           }
            out.close();
}

void transpose(int & a, int & b) {reallng temp = a; a = b; b = temp;}





/*--------------------------------------------------------------*/

/*       compute the cost difference if elements i and j        */

/*         are transposed in permutation (solution) p           */

/*--------------------------------------------------------------*/

reallng compute_delta(int n, type_matrix & a, type_matrix & b,

                   type_vector & p, int i, int j)

 {reallng d; int k;

  d = (a[i][i]-a[j][j])*(b[p[j]][p[j]]-b[p[i]][p[i]]) +

      (a[i][j]-a[j][i])*(b[p[j]][p[i]]-b[p[i]][p[j]]);

  for (k = 1; k <= n; k = k + 1) if (k!=i && k!=j)

    d = d + (a[k][i]-a[k][j])*(b[p[k]][p[j]]-b[p[k]][p[i]]) +

            (a[i][k]-a[j][k])*(b[p[j]][p[k]]-b[p[i]][p[k]]);

  return(d);

 }



/*--------------------------------------------------------------*/

/*      Idem, but the value of delta[i][j] is supposed to       */

/*    be known before the transposition of elements r and s     */

/*--------------------------------------------------------------*/

reallng compute_delta_part(type_matrix & a, type_matrix & b,

                        type_vector & p, type_matrix & delta, 

                        int i, int j, int r, int s)

  {return(delta[i][j]+(a[r][i]-a[r][j]+a[s][j]-a[s][i])*

     (b[p[s]][p[i]]-b[p[s]][p[j]]+b[p[r]][p[j]]-b[p[r]][p[i]])+

     (a[i][r]-a[j][r]+a[j][s]-a[i][s])*

     (b[p[i]][p[s]]-b[p[j]][p[s]]+b[p[j]][p[r]]-b[p[i]][p[r]]) );

  }

void update_matrix_of_move_cost(int i_retained, int j_retained,reallng n, type_matrix & delta, type_vector & p, type_matrix & a, type_matrix & b)
{
     int i, j;
     for (i = 1; i < n; i = i+1) for (j = i+1; j <= n; j = j+1)

        if (i != i_retained && i != j_retained && 

            j != i_retained && j != j_retained)

         {delta[i][j] = 

            compute_delta_part(a, b, p, delta, 

                               i, j, i_retained, j_retained);}

        else

         {delta[i][j] = compute_delta(n, a, b, p, i, j);};
}
void perturbe(type_vector & p,reallng n, type_matrix & delta, reallng & current_cost, type_matrix & a, type_matrix & b,
             type_matrix & last_swaped, type_matrix & frequency, int iter_without_improvement, reallng & best_best_cost)
{
   int i_retained=-1, j_retained=-1, k, i, j, min_delta, min;
   reallng cost = current_cost; int bit;
   bit=0;
   double d = static_cast<double>(iter_without_improvement)/249;
   double e = pow(2.718, -d);
   if(e<0.75) e=0.75;
   if(e>(static_cast<double>(rand()%101)/100.0))
        bit=1;
   for(k =0; k<(perturb_str); k++)
   {// cout<<e<<endl;
      if(bit==1)
      {//cout<<"here "<<static_cast<double>(iter_without_improvement)/2499<<endl;
         min_delta = 999999999;
         for (i = 1; i < n; i = i + 1) 
           for (j = i+1; j <= n; j = j+1)
           {
             if((current_cost + delta[i][j])!=cost && delta[i][j] < min_delta && 
               ((last_swaped[i][j]+ n*0.9 + rand()%(static_cast<int>(n*0.2)))<iteration or (current_cost + delta[i][j])<best_best_cost))
             {
                i_retained = i; j_retained = j;
                min_delta = delta[i][j];
                if((current_cost + delta[i][j])<best_best_cost)
                {
                     best_best_cost = current_cost + delta[i][j];
                }
             };
           };// cout<<min_delta<<endl;
       }
       else
       {  
        
            i_retained = 1+rand()%n;
            j_retained = 1+rand()%n;
            if(i_retained>j_retained)
               swap(i_retained, j_retained);
            while(i_retained==j_retained || (current_cost + delta[i_retained][j_retained])==cost)
            {
               j_retained = 1+rand()%n;
               if(i_retained>j_retained)
                  swap(i_retained, j_retained);
            }
        
       }
      if(i_retained!=-1 and j_retained!=-1)
      {
         last_swaped[i_retained][j_retained] = iteration;
         frequency[i_retained][j_retained] = frequency[i_retained][j_retained] + 1;
         transpose(p[i_retained], p[j_retained]);
          current_cost = current_cost + delta[i_retained][j_retained];
          update_matrix_of_move_cost(i_retained, j_retained, n, delta, p, a, b);
       }
      iteration++;
   }
}

void local_search(reallng n,                  // problem size

                 type_matrix & a,         // flows matrix
                 type_matrix & b,         // distance matrix
                 type_vector & best_sol,  // best solution found
                 reallng & best_best_cost,        // cost of best solution
                 reallng nr_iterations,
                 clock_t & time)

 {type_vector p;                        // current solution

  type_matrix delta;                    // store move costs
  type_matrix last_swaped;
  type_matrix frequency;
  reallng current_iteration;               // current iteration
  reallng current_cost, best_cost;                    // current sol. value

  int i, j, k, i_retained, j_retained, bit;  // indices
  int iter_without_improvement = 0;
  reallng  iter_last_improvement=0; 
  perturb_str = init_ptr*n;

  bool perturbed_once = false;
  reallng previous_cost=1;
  iteration = 0;
 
  
  /***************** dynamic memory allocation *******************/

  p = new int[n+1];

  delta = new reallng* [n+1];
  last_swaped = new reallng* [n+1];
  frequency = new reallng* [n+1];
  for (i = 1; i <= n; i = i+1)
  { 
     delta[i] = new reallng[n+1]; 
     last_swaped[i] = new reallng [n+1];
     frequency[i] = new reallng [n+1];
   }

  /************** current solution initialization ****************/

  for (i = 1; i <= n; i = i + 1) p[i] = best_sol[i];
   for (i = 1; i <= n; i = i+1){
     for(j=1; j<=n; j++){
       last_swaped[i][j] = 0;
       frequency[i][j] = 0;
     }
   }

  /********** initialization of current solution value ***********/

  /**************** and matrix of cost of moves  *****************/

  current_cost = 0;
  for (i = 1; i <= n; i = i + 1) for (j = 1; j <= n; j = j + 1)
   {current_cost = current_cost + a[i][j] * b[p[i]][p[j]];
    if (i < j) {delta[i][j] = compute_delta(n, a, b, p, i, j);};
   };
  best_best_cost = current_cost;

   /******************** main tabu search loop ********************/

  for (current_iteration = 1; current_iteration <= nr_iterations and target!=best_best_cost;
      current_iteration = current_iteration + 1)
   {/** find best move (i_retained, j_retained) **/

    i_retained = infinite;       // in case all moves are tabu

    reallng min_delta = infinite;   // retained move cost


    for (i = 1; i < n; i = i + 1) 
      for (j = i+1; j <= n; j = j+1)
       {
          if(delta[i][j] < min_delta)
          {
             i_retained = i; j_retained = j;
             min_delta = delta[i][j];
          };
       };
      
       if((current_cost + delta[i_retained][j_retained])<current_cost)
       {
        //  cout<<"DO something "<<current_iteration<<endl;
          transpose(p[i_retained], p[j_retained]);
          last_swaped[i_retained][j_retained] = iteration;
          frequency[i_retained][j_retained] =  frequency[i_retained][j_retained] + 1;
          iteration++;
          current_cost = current_cost + delta[i_retained][j_retained];
         // best solution improved ?

         if (current_cost < best_best_cost)

         {
            iter_without_improvement = 0;
            iter_last_improvement = current_iteration;
            best_best_cost = current_cost;
            time = clock()-start;

            for (k = 1; k <= n; k = k+1) best_sol[k] = p[k];

         }
          update_matrix_of_move_cost(i_retained, j_retained, n, delta, p, a, b);
          perturbed_once = false;
       }
       else
       {
           if(iter_without_improvement>250)
           {
              iter_without_improvement = 0;
              perturb_str = n*(0.2 + (static_cast<double>(rand()%20)/100.0));
           }
          else if((perturbed_once==false and previous_cost != current_cost) or (perturbed_once and previous_cost != current_cost)) // Escaped from the previous local optimum. New local optimum reached
          {
               iter_without_improvement++;
               perturb_str=ceil(n*init_ptr); if(perturb_str<2) perturb_str = 2;
          }
          else if(previous_cost == current_cost)//  or (perturbed_once and previous_cost != current_cost))
             perturb_str+= 1;
         

          
          previous_cost = current_cost;
          perturbe(p,n, delta, current_cost, a, b, last_swaped, frequency, iter_without_improvement, best_best_cost);
           perturbed_once = true;
         
       } 

     };
  // free memory

  delete[] p;

  for (i=1; i <= n; i = i+1)
  { 
      delete [] frequency[i];
      delete[] delta[i];
      delete [] last_swaped[i];
  }
    delete[] delta; delete[] frequency; delete [] last_swaped;

  

} // tabu

void generate_random_solution(reallng n, type_vector  & p)

 {int i;

  for (i = 0; i <= n; i = i+1) p[i] = i;

  for (i = 1; i <  n; i = i+1) transpose(p[i], p[i + rand()%(n-i+1)]);

 }

void generate_random_population(reallng n, int pop_size, type_matrix_s & pop)
{
    int i, j;
    for(i=0; i<pop_size; i++)
    {
       for (j = 0; j <= n; j = j+1) pop[i][j] = j;  
       for (j = 1; j <  n; j = j+1) swap(pop[i][j], pop[i][j + rand()%(n-j+1)]);
    }
}

bool not_assigned_in_child(reallng n, type_vector child_sol, int vertex)
{
   int j; bool result = false;
   for(j=1; j<=n; j++)
   {
     if(child_sol[j]==vertex)
      result = true;
   }
   return result;
}


void crossos_uni(reallng n, type_vector & child_sol, type_matrix_s & pop, int pop_size, double *dist)
{
   int random;
   int unassigned[MAX_N+1]; // changed this to fixed size array

   for(int i=1; i<=n; i++) 
   {
      child_sol[i]=0;
      unassigned[i] =0;
   }
   for (int i=1; i<=n ; i++)
   {
       random = rand()%101;
       if(random>50 and not_assigned_in_child(n, child_sol, pop[0][i])==false)
          child_sol[i] = pop[0][i];
       else if(not_assigned_in_child(n, child_sol, pop[1][i])==false)
          child_sol[i] = pop[1][i]; 
       unassigned[child_sol[i]]++; 
   } 
   
   for(int i=1; i<=n; i++)
   {
      if(unassigned[i]==0)
      {
         random = 1+ rand()%n;
         while(child_sol[random]!=0)
           random = 1+ rand()%n;
        child_sol[random] = i;  
      }
   }
   int d1=0, d2=0;
  for(int i=1; i<=n; i++)
 {
    if(pop[0][i] != child_sol[i]) d1++;
    if(pop[1][i] != child_sol[i]) d2++;
 } 
#ifdef DEBUG
 cout<<d1<<"  "<<d2<<endl;
#endif
 // TODO cout above is real
   if(d1<=d2)
    *dist = d1/static_cast<double>(n);
  else
     *dist = d2/static_cast<double>(n);


}

reallng avg_pop_cost(reallng *pop_costs, int pop_size)
{
   reallng min1 = 0, min2=0, c=0;
   int i;
   for(i=0; i<pop_size; i++)
   {
      // if(pop_costs[i]<min)
        min1 += pop_costs[i];
   }
   min1=min1/pop_size;
   for(i=0; i<pop_size; i++)
   {
       if(pop_costs[i]<min1)
       {
          min2+=pop_costs[i]; c++;
       }
   } 
   return min2/c;
}

reallng worst_pop_cost(reallng *pop_costs, int pop_size)
{
   long long min = 0;
   int i;
   for(i=0; i<pop_size; i++)
   {
       if(pop_costs[i]>min)
        min = pop_costs[i];
   }
   return min;
}
reallng best_pop_cost(reallng *pop_costs, int pop_size)
{
   long long min = 99999999999LL;
   int i;
   for(i=0; i<pop_size; i++)
   {
       if(pop_costs[i]<min)
        min = pop_costs[i];
   }
   return min;
}
// Returns 1 if all individuals are unique, 0 otherwise?
bool unique_individuals(reallng n, type_matrix_s & pop, int pop_size)
{
   int min = 999999;
   int k, i, j, count=0;
   for(k=0; k<pop_size; k++)
   { 
      min = 999999;
      for(i=0; i<pop_size; i++)
      {
          count=0; 
          if(k!=i)
          {    
             for(j=1; j<=n; j++)
             {
                if(pop[i][j]!=pop[k][j])
                   count++;
             }
          if(count<min)
            min = count;
          }
       }  
       if(min==0)
          return 1;
   } 
   return 0;
}
void replacement_other(reallng n, type_vector & child_sol, reallng  child_cost, type_matrix_s & pop,  reallng *pop_costs, int pop_size)
{
   int i, j, k, replace_with;
   int min = 999999, sum=0, count;
   int min_dist=99999;
   int dist[MAX_POP_SIZE+2]; // changed this to fixed size array
   double score[MAX_POP_SIZE+2]; // changed this to fixed size array
   long long mx=0;
   
   for(j=1; j<=n; j++)
    pop[pop_size][j] = child_sol[j];
   pop_costs[pop_size] = child_cost; 

    min = 999999;
    for(k=0; k<pop_size; k++)
    {
       count=0;
       for(j=1; j<=n; j++)
       {
                 if(pop[pop_size][j]!=pop[k][j])
                   count++;
        }
          if(count<min)
            min = count;
     }
     dist[pop_size] = min;

   for(i=0; i<pop_size; i++)
   {
      if(pop_costs[i]>mx)
      {
         mx=pop_costs[i];
         replace_with = i;
      }
    }

 //  cout<<"WPC "<<worst_pop_cost(pop_costs, pop_size)<<endl;
   if(dist[pop_size]!=0 and  child_cost<worst_pop_cost(pop_costs, pop_size))// and (min_dist<=dist[pop_size] or child_cost<best_pop_cost(pop_costs, pop_size)))
   {
      // cout<<"REPLACED "<<endl;
       for(i=1; i<=n; i++)
         pop[replace_with][i] = child_sol[i];
      pop_costs[replace_with] = child_cost;
   }//else cout<<"NO REPLACEMENT "<<endl;
   if(child_cost < resulting_cost)
   {
      resulting_cost = child_cost;
      for(i=1; i<=n; i++)
       best_solution_found[i] = child_sol[i];
   }
}

void check_validity(reallng n, type_vector & child_sol)
{
   int array[MAX_N+1]; // changed this to fixed size array
   for(int i=1; i<=n; i++)
      array[i] = 0;
   for(int i=1; i<=n; i++)
     array[child_sol[i]]++;
   for(int i=1; i<=n; i++)
   {
     if(array[i]!=1) cout<<"PROBLEM IN CROSSOVER "<< array[i]<<"  "<<endl;
   }
}

void best_individual(int n, type_matrix_s pop, reallng* pop_costs, int pop_size)
{
    long long best = 99999999999ll;
    int b;
    for(int i=0; i<pop_size; i++)
    {
       if(pop_costs[i]<best)
       {
           best = pop_costs[i];
           b=i;
       }
    }
    if(best<resulting_cost)
    {
       for(int j=1; j<=n; j++)
         best_solution_found[j] = pop[b][j];
       resulting_cost= best;
    }
}
int best_index(int pop_size,reallng* pop_costs)
{
   int b;
   long long  min = 100000000000ll; 
   for(int i=0; i<pop_size; i++)
   {
     if(pop_costs[i]<min)
     {
        min = pop_costs[i];
        b = i;
     }
   }
    return b;
}
void parent_selection(int n, type_matrix_s pop, int pop_size, reallng* pop_cost, type_matrix_s & parent_pool,  int num_of_parents, int *parents)
{
   int random;
   int a =0;
   bool insert = true;
   short int tournament_pool[100];
   int t_size = 4;
   int winner;
   int tour=0;
   long long  min=100000000000ll;

   while(a<num_of_parents)
   {
       //cout<<a<<"  "<<num_of_parents<<endl;
       for(int i=0; i<t_size; i++)
       {
          tournament_pool[i] = rand()%pop_size;
       //   cout<<tournament_pool[i]<<"  ";
       }//cout<<endl;
       for(int i=0; i<t_size; i++)
       {
           if(pop_cost[tournament_pool[i]]<min)
           {
               min = pop_cost[tournament_pool[i]];
               winner =  tournament_pool[i];
           }
       }
     //  cout<<"Winner "<<winner<<endl;
       for(int i=0; i<a; i++)
       {
          if(parents[i]==winner)
            insert=false;
       }
       if(insert)
       {
          parents[a] = winner;
          a++;
       } insert = true; tour=0; min=100000000000ll; 
   }
  // cout<<"Parents selected "<<pop_cost[parents[0]]<<"  "<<pop_cost[parents[1]]<<endl;
   for(int i=0; i<num_of_parents; i++) 
   {
       for(int j=1; j<=n; j++)
          parent_pool[i][j] = pop[parents[i]][j];
   
   } //cout<<"EXITING"<<endl;
}

void mutate_population(reallng n, int pop_size, type_matrix_s & pop, int strength, int best)
{
   int r1, r2;
   bool mutated[MAX_N+1]; // changed this to fixed size array
 
   for(int i=0; i<pop_size; i++)
   {
 //    if(i!=best)
   //  {
      for(int a=0; a<=n; a++)
          mutated[a]=0;
      r1 = 1 + rand()%n;
      mutated[r1] = 1;
      for(int j=0; j<strength; j++)
      {
         r2= 1+rand()%n;
         while(r1==r2 and mutated[r2]==1)
          r2= 1+rand()%n;
         swap(pop[i][r1], pop[i][r2]);
         //cout<<r1<<"  "<<r2<<endl;
         r1 = r2;
         mutated[r2] = 1;
      }
    // } 
   }
}
void short_improvement_of_individuals(reallng n,  int pop_size, type_matrix_s & pop, reallng* pop_cost, type_matrix & a, type_matrix & b, clock_t & time)
{
   type_vector p = new int[n+1];
   int i, j;
   reallng p_cost;
   
   for(i=0; i<pop_size; i++)
   {
       for(j=1; j<=n; j++)
         p[j]=pop[i][j];
      local_search(n, a, b, p, p_cost, 500, time);  // 5000 -> 500 in second last argument
      pop_cost[i] = p_cost;
      for(j=1; j<=n; j++)
         pop[i][j]=p[j];
   }
   delete [] p;
}


int n;                    // problem size
type_matrix a, b;         // flows and distances matrices


type_matrix_s pop;  //Population of individuals
type_matrix_s parent_pool;

int pop_size, num_generations; //size of population and number of iterations
int num_of_parents;

type_vector child_sol;     // solution (permutation) 

reallng child_cost;                // solution cost
reallng pop_costs[50];


ifstream data_file;       

char file_name[30];

int i, j;
 
 int jtc_interface_bma(QAP_input* qinput, QAP_output** qoutput) {
     
  srand ( time(NULL) );

  n = qinput->n;
  global_maxtime = qinput->maxtime;

  unsigned long long best_cost_ever = 999999999999ULL;
  double avg_cost=0;
  double dist, times[50];
  int num_mutations = 0;
  type_vector best_solution_ever;
  best_solution_ever = new int[n+1];
  
  
  init_ptr=0.15; //JUMP MAGNITUDE
  num_generations = 50000;
  pop_size = 15;
  
	if (qinput->nalgparams > 3 || qinput->nalgparams < 0 )
	{
		cout << "Invalid number of algorithm parameters, aborting" << endl;
		throw(-1);
	}

	// might not be a good idea to modify pop_size? no idea.
	switch(qinput->nalgparams) { // note where the break is.
		case 3:
			pop_size = (int) (qinput->algparams[2]+0.1);
		case 2:
			num_generations = (int) (qinput->algparams[1]+0.1);
		case 1: 
			init_ptr = qinput->algparams[0];
		case 0:
			break;
		default:
			cout << "Algorithm parameter switch broke" << endl;
			throw(-1);
	}
	
    if (n > MAX_N) { cout << "n exceeded MAX_N in BMA.cpp" << endl; throw(-1); }
    if (pop_size > MAX_POP_SIZE) { cout << "pop_size exceeded MAX_POP_SIZE in BMA.cpp" << endl; throw(-1); }
  
  int gener;
  
  a = new reallng* [n+1];
  b = new reallng* [n+1];
  pop = new unsigned short int *[pop_size+2];
  parent_pool = new unsigned short int *[50]; 
  int parents[30];


  for (i = 1; i <= n; i = i+1)
  {   
      a[i] = new reallng[n+1];
      b[i] = new reallng[n+1];
      child_sol = new int[n+1];

  }

   for (i = 0; i <=pop_size; i = i+1)
     pop[i] = new unsigned short int[n+1];
   for (i = 0; i <=50; i = i+1)
     parent_pool[i] = new unsigned short int[n+1];
     
  for (i = 1; i <= n; i = i+1) for (j = 1; j <= n; j = j+1)

    a[i][j] = qinput->dist[i-1][j-1];

  for (i = 1; i <= n; i = i+1) for (j = 1; j <= n; j = j+1)

    b[i][j] = qinput->flow[i-1][j-1];

   reallng all_solutions[101];

   int num_runs = qinput->ntrials; // TODO was 5
   
   int num_counter;
   clock_t timet;
   clock_t time;
   double time_best=0.0;
   double best_gen;
   double vrijeme_p, vrijeme;
   double b_time = 10000000.0, avg_time = 0.0, worst_time = 0.0;


   for(int p=0; p<num_runs; p++)
   {
     timet = clock();
     time = clock();
     start = clock();

     num_mutations = 0;
     num_counter = 0;

     vrijeme=0.0; vrijeme_p=0.0;
     resulting_cost = 999999999999ULL;
     generate_random_population(n, pop_size, pop);
     short_improvement_of_individuals(n, pop_size, pop, pop_costs, a, b, time);
     best_individual(n, pop, pop_costs, pop_size);

     int insanity = 0;

     for (gener = 0; gener <num_generations; gener = gener+1)
     {
       //cout << "vrijeme = " << vrijeme << "\n";
       if(vrijeme>global_maxtime) // TODO max time set here?
           goto end;
       if((clock()-timet)/static_cast<double>(CLOCKS_PER_SEC)>0 and vrijeme_p<=0)
          vrijeme+=abs(vrijeme_p) + (clock()-timet)/static_cast<double>(CLOCKS_PER_SEC);
       else if((clock()-timet)/static_cast<double>(CLOCKS_PER_SEC)>0 and vrijeme_p>=0)
          vrijeme+= (clock()-timet)/static_cast<double>(CLOCKS_PER_SEC) - vrijeme_p;
       else if((clock()-timet)/static_cast<double>(CLOCKS_PER_SEC)<0 and vrijeme_p<0)
          vrijeme+= abs(vrijeme_p) + (clock()-timet)/static_cast<double>(CLOCKS_PER_SEC);
   
        vrijeme_p = (clock()-timet)/static_cast<double>(CLOCKS_PER_SEC);
      
       num_of_parents = 2; 
       parent_selection(n, pop, pop_size, pop_costs, parent_pool, num_of_parents, parents);
       crossos_uni(n, child_sol, parent_pool, num_of_parents, &dist);
    
     //  check_validity(n, child_sol); 
 
       local_search(n, a, b, child_sol, child_cost, 1000, time); // second last argument 10000 -> 1000
#ifdef DEBUG
       cout<<"  Best cost "<<resulting_cost<<"  "<<time_best <<endl;
#endif
       // TODO cout above is real
       
       if(resulting_cost>child_cost)
       {
          num_mutations = 0;
          num_counter=0; 
          time_best = time/static_cast<double>(CLOCKS_PER_SEC);
       }else num_mutations++;
                                
       if(num_mutations==pop_size)
       {
           int bi = best_index(pop_size, pop_costs); 
           insanity = 0;
          do{ // TODO modify this so it actually terminates...
             mutate_population(n, pop_size, pop, static_cast<int>(n*(0.5 + static_cast<double>(num_counter)/10)), bi);
             short_improvement_of_individuals(n, pop_size, pop, pop_costs, a, b, time);
          //   evaluate_population_costs(n, pop_size, pop, pop_costs, a, b);
            insanity++;
           }while(unique_individuals(n, pop, pop_size) && (insanity < 10));
           best_individual(n, pop, pop_costs, pop_size);
           num_mutations = 0;
           num_counter ++;      
           //if (insanity >= 10) {cout << "Insanity reached\n"; goto end;}
       }
        if(num_counter>5) num_counter = 0;
        
       //replacement_strategy(n, child_sol, child_cost, pop, pop_costs, pop_size, &inserted, &dist, &avg_dist);
       replacement_other(n, child_sol, child_cost, pop, pop_costs, pop_size);
      // out<<dist<<"   "<<avg_dist<<endl;
     
     }end:
    
      //cout<<"Res cost "<<resulting_cost<<"  "<<time_best<<endl;
     
      qoutput[p]->value = resulting_cost;
      qoutput[p]->time_for_best = time_best;
     
      all_solutions[p] = resulting_cost;
      times[p] = time_best;
      if(resulting_cost<best_cost_ever)
      {
         best_cost_ever = resulting_cost;
         for (i = 1; i <= n; i = i+1)
            best_solution_ever[i] = best_solution_found[i];
      }
      
      
   }

  //out.close();
  //output(n, file_name, all_solutions, num_runs, best_cost_ever, best_solution_ever,  times);
  
   
  
   delete [] best_solution_ever;
   
   
   
   return 0;
 }

