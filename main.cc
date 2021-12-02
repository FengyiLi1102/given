/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"

void *producer (void *parameter);
void *consumer (void *parameters);

auto *jobSet = new vector<Job*>();
auto position = 0;

int main (int argc, char **argv) {
  if (argc < 5) {
      cerr << "ERROR: fewer parameters provided.\n";
      exit(0);
  }

  auto sizeOfQueue = check_arg(argv[1]);             // size of the queue
  auto jobsProducer = check_arg(argv[2]);            // number of items produced per producer
  auto numProducer = check_arg(argv[3]);             // number of producers
  auto numConsumer = check_arg(argv[4]);             // number of consumers

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

  cout << "====================== START ==============================\n";

  /* Create the parameter struct */


  /* Producer threads */
  for (int i = 0; i < numProducer; i++) {
      auto params = Params(semID, sizeOfQueue, jobsProducer, i+1);
      pthread_create(&producerId[i], NULL, producer, (void *) &params);
  }

  /* Consumer threads */
  for (int i = 0; i < numConsumer; i++) {
      auto params = Params(semID, sizeOfQueue, jobsProducer, i+1);
      pthread_create(&consumerId[i], NULL, consumer, (void *) &params);
  }

  for (auto& thread : producerId)
      pthread_join(thread, NULL);

  for (auto& thread : consumerId)
      pthread_join(thread, NULL);

  cout << "Doing some work after the join" << endl;

  pthread_exit(NULL);

  return 0;
}


void *producer (void *parameters) {
  auto *params = (Params *) parameters;

  for (unsigned int n = 0; n < params->jobPerPro; n++) {

      /* Create the job with a default ID*/
      Job* newJob = new (nothrow) Job(position, randInt(10));

      sem_wait(params->semID, 1);           // Check if it is empty in the queue

      clock_t begin = clock();

      sem_wait(params->semID, 0);           // Mutual exclusion
      if ((begin - clock()) / CLOCKS_PER_SEC > 20 ) {
          cerr << "ERROR: wait more than 20s.\n";
          pthread_exit(0);
      }

      jobSet->push_back(newJob);    // Add a new produced job into the queue
      sem_signal(params->semID, 0);         // Release mutual exclusion
      sem_signal(params->semID, 2);         // Signal the jobs part

      if (position == (params->queueSize - 1))
          position = 0;
      else
          position++;

      cout << "Producer(" << params->ID << "): Job id " << position
           << " duration " << randInt(10) << endl;

      sleep (randInt(5));
  }

  pthread_exit(0);
}


void *consumer (void *parameters)
{
    auto *params = (Params *) parameters;

    while (true) {
        sem_wait(params->semID, 2);           // Check if there is an item
        clock_t begin = clock();

        sem_wait(params->semID, 0);           // Mutual exclusion
        if ((begin - clock()) / CLOCKS_PER_SEC > 20 ) {
            cerr << "ERROR: wait more than 20s.\n";
            pthread_exit(0);
        }
        unsigned int duration = jobSet->front()->getDuration();
        unsigned int newPosition = jobSet->front()->getID();

        delete jobSet->front();               // Remove the first job in the queue
        jobSet->erase(jobSet->begin());       // Delete the job in the queue
        sem_signal(params->semID, 0);         // Release mutual exclusion
        sem_signal(params->semID, 1);         // Signal the space part

        position = newPosition;

        cout << "Consumer(" << params->ID << "): Job id " << newPosition
             << " executing sleep duration " << duration << endl;

        sleep (duration);

        begin = clock();
        sem_wait(params->semID, 2);
        if ((begin - clock()) / CLOCKS_PER_SEC > 20) {
            cerr << "NO JOBS IN THE QUEUE.\n";
            pthread_exit(0);
        }
    }

    pthread_exit (0);

}
