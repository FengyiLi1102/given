/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/


#include "helper.h"

void producer (int producerID, int jobsPerProducer, int semID);
void consumer (int consumerID, int semID);

const unsigned int TIME_TO_WAIT = 20;

auto *jobSet = new vector<Job>();
auto *positionQueue = new deque<unsigned int>();
//std::counting_semaphore<100> *space, *job;

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
  thread producerId[numProducer], consumerId[numConsumer];

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
  auto jobsFlag = sem_init(semID, 2, 0);
  checkValidity(jobsFlag);
  cout << "Semaphore set of jobs is initialised successfully.\n";

  cout << "====================== START ==============================\n";

  /* Create the parameter struct */

  for (int i = 0; i < sizeOfQueue; i++)
      positionQueue->push_back(i);

  /* Producer threads */
  for (int i = 0; i < numProducer; i++) {
      producerId[i] = thread(producer, i+1, jobsProducer, semID);
  }

  /* Consumer threads */
  for (int i = 0; i < numConsumer; i++) {
      consumerId[i] = thread(consumer, i+1, semID);
  }

  for (auto& thread : producerId)
      thread.join();

  for (auto& thread : consumerId)
      thread.join();

  cout << "All jobs have been finished." << endl;

  sem_close(semID);

  return 0;
}


void producer (int producerID, int jobsPerProducer, int semID) {
  for (int n = 0; n < jobsPerProducer; n++) {

      /* Create the job with a default ID*/
      Job newJob = Job(-1, randInt(10));

      sem_wait(semID, 1);           // Check if it is empty in the queue

      clock_t begin = clock();

      sem_wait(semID, 0);           // Mutual exclusion
      if ((begin - clock()) / CLOCKS_PER_SEC > TIME_TO_WAIT ) {
          cerr << "ERROR: wait more than 20s.\n";
          return;
      }
      newJob.setID(positionQueue->front());
      jobSet->push_back(newJob);    // Add a new produced job into the queue
      cout << "Producer(" << producerID << "): Job id " << positionQueue->front()
           << " duration " << newJob.getDuration() << endl;
      positionQueue->pop_front();
      sem_signal(semID, 0);         // Release mutual exclusion
      sem_signal(semID, 2);         // Signal the jobs part


      sleep (randInt(5));
  }

  cout << "Producer(" << producerID << "): No more jobs to generate." << endl;
}


void consumer (int consumerID, int semID)
{
    while (true) {
//       clock_t begin = clock();
       if (sem_timeout_wait(semID, 2, TIME_TO_WAIT) != 0) {
           cerr << "ERROR: wait more than 20s.\n";
           return;
       }
//        sem_wait(semID, 2);           // Check if there is an item
//        clock_t end = clock();
//        if ((begin - end) / CLOCKS_PER_SEC > 20 ) {
//            cerr << "ERROR: wait more than 20s.\n";
//            break;
//        }

//        begin = clock();
       if (sem_timeout_wait(semID, 0, TIME_TO_WAIT) != 0) {
           cerr << "ERROR: wait more than 20s.\n";
           return;
       }           // Mutual exclusion
//        if ((begin - clock()) / CLOCKS_PER_SEC > 20 ) {
//            cerr << "ERROR: wait more than 20s.\n";
//            return;
//        }

//        Job thisJob = jobSet->front();
        unsigned int id = jobSet->front().getID();
        unsigned int duration = jobSet->front().getDuration();

        cout << "Consumer(" << consumerID << "): Job id " << id
             << " executing sleep duration " << duration << endl;
        jobSet->erase(jobSet->begin());       // Delete the job in the queue
        positionQueue->push_back(id);
        sem_signal(semID, 0);         // Release mutual exclusion
        sem_signal(semID, 1);         // Signal the space part
        sleep (duration);
    }

    cout << "Consumer(" << consumerID << "): No more jobs left\n";
}
