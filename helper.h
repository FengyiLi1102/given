/******************************************************************
 * Header file for the helper functions. This file includes the
 * required header files, as well as the function signatures and
 * the semaphore values (which are to be changed as needed).
 ******************************************************************/


# include <cstdio>
# include <cstdlib>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <ctime>
# include <cmath>
# include <cerrno>
# include <cstring>
# include <pthread.h>
# include <cctype>
# include <iostream>
# include <random>
# include <vector>
using namespace std;

#define SEM_KEY 0x77 // Change this number as needed

union semun {
    int val;               /* used for SETVAL only */
    struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
    ushort *array;         /* used for GETALL and SETALL */
};

int check_arg (char *);
int sem_create (key_t, int);
int sem_init (int, int, int);
void sem_wait (int, short unsigned int);
void sem_signal (int, short unsigned int);
int sem_close (int);
unsigned int randInt(int maxRange);
void checkValidity(int returnValue);


class Job;
struct Params;

typedef Job* JobPtr;
typedef Params* ParamPtr;

class Job {
    unsigned int id;
    unsigned int duration;

public:
    Job(unsigned int id, unsigned int duration) {
        this->id = id;
        this->duration = duration;
    }

    Job() = default;
};

struct Params {
    int semID;
    std::vector<Job*> *jobSet = nullptr;
    int position = 0;
    std::vector<int> producerIDs;
    std::vector<int> consumerIDs;

    Params(int semID, int numPro, int numCon) {
      this->semID = semID;

      for (int i = 0; i < numPro; i++)
        producerIDs.push_back(i);

      for (int i = 0; i < numCon; i++)
        consumerIDs.push_back(i);
    }

    ~Params() {
        for (int i = 0; i < jobSet->size(); i++)
            delete jobSet->front();
    }
};
