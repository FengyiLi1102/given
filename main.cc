/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"

void *producer (void *id);
void *consumer (void *id);

struct Job;
struct Params;

typedef Job* JobPtr;
typedef Params* ParamPtr;

struct Job {
    unsigned int id;
    unsigned int duration;

    Job(unsigned int id, unsigned int duration) {
        this->id = id;
        this->duration = duration;
    }
};

struct Params {
    int semID;

};

int main (int argc, char **argv)
{

  // TODO

  pthread_t producerid;
  auto parameter = 5;

  auto sizeOfQueue = check_arg(argv[1]);             // size of the queue
  auto jobsProducer = check_arg(argv[2]);            // number of items produced per producer
  auto numProducer = check_arg(argv[3]);             // number of producers
  auto numConsumer = check_arg(argv[4]);             // number of consumers
  auto totalNumJobs = jobsProducer * numProducer;    // total number of jobs
//  int totalNumThreads = numProducer + numConsumer;  // total number of threads


  /* Each producer and consumer uses one thread */
  pthread_t producerId[numProducer], consumerId[numConsumer];

  /* Create the semaphore set including mutual exclusion, space and jobs */
  auto semID = sem_create(SEM_KEY, 3);
  if (semID == -1) {
      cerr << "ERROR for creating the semaphore set.\n";
      exit(0);
  } else
      cout << "Semaphore set for three parts is created successfully.\n";

  /* Initialise semaphores for the shared data part */
  auto mutexFlag = sem_init(semID, 0, 1);
  checkValidity(mutexFlag);
  cout << "Semaphore set of mutual exclusion is initialised successfully.\n";

  /* Initialised semaphores for the space in the circle queue */
  auto spaceFlag = sem_init(semID, 1, sizeOfQueue);
  checkValidity(spaceFlag);
  cout << "Semaphore set of space is initialised successfully.\n";

  /* Initialised semaphores for the jobs in the queue */
  auto jobsFlag = sem_init(semID, 3, 0);
  checkValidity(jobsFlag);
  cout << "Semaphore set of jobs is initialised successfully.\n";





  pthread_create (&producerid, NULL, producer, (void *) &parameter);

  pthread_join (producerid, NULL);

  cout << "Doing some work after the join" << endl;

  return 0;
}


void *producer (void *parameter) 
{

  // TODO

  /* Create the job */
  Job* newJob = new (nothrow) Job(randInt());

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
