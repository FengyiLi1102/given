/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"

void *producer (void *id);
void *consumer (void *id);

struct job {
    unsigned int id;
    unsigned int duration;
};


int main (int argc, char **argv)
{

  // TODO

  pthread_t producerid;
  int parameter = 5;        // queue size
  int jobsProducer = 6;     // number of items produced per producer
  int numProducer = 2;      // number of producers
  int numConsumer = 3;      // number of consumers
  int totalNumJobs = jobsProducer * numProducer;



  pthread_create (&producerid, NULL, producer, (void *) &parameter);

  pthread_join (producerid, NULL);

  cout << "Doing some work after the join" << endl;

  return 0;
}


void *producer (void *parameter) 
{

  // TODO

  int *param = (int *) parameter;

  cout << "Parameter = " << *param << endl;

  sleep (5);

  cout << "\nThat was a good sleep - thank you \n" << endl;

  pthread_exit(0);
}


void *consumer (void *id) 
{
    // TODO 

  pthread_exit (0);

}
