/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/


#include "helper.h"

void producer (int producerID, int jobsPerProducer, int semID);
void consumer (int consumerID, int semID);

const unsigned int TIME_TO_WAIT = 20;  // Max time to wait for a new job

auto *jobSet = new vector<Job>();
auto *positionQueue = new deque<unsigned int>();


int main (int argc, char **argv) {
    /* Check if all 4 parameters are provided */
    if (argc < 5) {
        cerr << "ERROR: fewer parameters provided.\n";
        exit(0);
    }

    auto sizeOfQueue = check_arg(argv[1]);   // size of the queue
    auto jobsProducer = check_arg(argv[2]);  // number of items produced per producer
    auto numProducer = check_arg(argv[3]);   // number of producers
    auto numConsumer = check_arg(argv[4]);   // number of consumers

    /* Each producer and consumer uses one thread */
    thread producerId[numProducer], consumerId[numConsumer];

    /* Create the semaphore set including mutual exclusion, space and jobs */
    auto semID = sem_create(SEM_KEY, 3);
    if (semID == -1) {
        cerr << "ERROR for creating the semaphore set.\n";
        exit(0);
    }

    /* Initialise semaphores for the shared data part */
    auto mutexFlag = sem_init(semID, 0, 1);
    checkValidity(mutexFlag);

    /* Initialised semaphores for the space in the circle queue */
    auto spaceFlag = sem_init(semID, 1, sizeOfQueue);
    checkValidity(spaceFlag);

    /* Initialised semaphores for the jobs in the queue */
    auto jobsFlag = sem_init(semID, 2, 0);
    checkValidity(jobsFlag);

    /* Fill the circular queue with job ids */
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

    /* Wait for each producer thread to finish */
    for (auto& thread : producerId)
        thread.join();

    /* Wait for each consumer thread to finish */
    for (auto& thread : consumerId)
        thread.join();

    sem_close(semID);

    return 0;
}


void producer (int producerID, int jobsPerProducer, int semID) {
    /* Timespec for semtimeop system call */
    auto *timeout = new (nothrow) timespec;
    timeout->tv_sec = TIME_TO_WAIT;

    for (int n = 0; n < jobsPerProducer; n++) {
        /* Create the job with a default ID*/
        Job newJob = Job(-1, randInt(10));

        /* Check if the queue has places */
        sem_wait(semID, 1);

        /* Mutual exclusion for the critical section */
        if (sem_timeout_wait(semID, 0, timeout) != 0) {
          cerr << "Producer(" << producerID << "): Wait more than "
               << TIME_TO_WAIT << " seconds.\n";
          delete timeout;
          exit(0);
        }

        newJob.setID(positionQueue->front());  // Reset the id for the new job
        jobSet->push_back(newJob);             // Add the job into the queue
        cout << "Producer(" << producerID << "): Job id "
             << positionQueue->front()
             << " duration " << newJob.getDuration() << endl;
        positionQueue->pop_front();            // Remove the used id
        sem_signal(semID, 0);
        /* Release mutual exclusion */

        sem_signal(semID, 2);                  // Signal the jobs part

        sleep (randInt(5));   // Sleep for 1 to 5 seconds
  }

  cout << "Producer(" << producerID << "): No more jobs to generate." << endl;

  pthread_exit(nullptr);
}


void consumer (int consumerID, int semID) {
    /* Timespec for semtimeop system call */
    auto *timeout = new (nothrow) timespec;
    timeout->tv_sec = TIME_TO_WAIT;

    while (true) {
        /* Check if there are any jobs */
        if (sem_timeout_wait(semID, 2, timeout) != 0) {
            cerr << "Consumer(" << consumerID << "): No more jobs left\n";
            delete timeout;
            pthread_exit(nullptr);
        }

        /* Mutual exclusion for the critical section */
        sem_wait(semID, 0);
        Job removedJob = jobSet->front();

        cout << "Consumer(" << consumerID << "): Job id "
             << removedJob.getID() << " executing sleep duration "
             << removedJob.getDuration() << endl;
        jobSet->erase(jobSet->begin());  // Delete the job in the queue
        positionQueue->push_back(removedJob.getID());  // Add the id back
        sem_signal(semID, 0);
        /* Release mutual exclusion */

        sem_signal(semID, 1);  // Signal the space part

        sleep (removedJob.getDuration());

        cout << "Consumer(" << consumerID << "): Job id " << removedJob.getID()
             << " completed.\n";
    }
}
