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
#include <thread>
#include <chrono>
#include <deque>
using namespace std;

#define SEM_KEY 0x87 // Change this number as needed

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
void sem_timeout (int id, short unsigned int num, unsigned int const timeout);


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

    Job& operator=(Job const &other) {
        this->id = other.id;
        this->duration = other.duration;

        return *this;
    }

    Job(Job const &other) {
        *this = other;
    }

    unsigned int getDuration() const {
        return duration;
    }

    unsigned int getID() const {
        return this->id;
    }

    void setID(unsigned int id) {
        this->id = id;
    }
};

struct Params {
    int semID;
    unsigned int jobPerPro;

    Params(int semID,  int jobPerPro) {
        this->semID = semID;
        this->jobPerPro = jobPerPro;
    }
};