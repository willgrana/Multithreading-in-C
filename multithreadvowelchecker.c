//
// Created by Will Grana on 10/17/20.
//
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#define SEM_NAME "/SEM_wgrana"
#define SEM_NAME_TWO "/SEM_wgrana2"

typedef struct {
    int numPhrases;
    sem_t* sem;
    sem_t* sem1;
    pthread_mutex_t mutex;
    pthread_mutex_t mutex1;
} SyncInfo;

#define BUFLEN 128
char buffer[BUFLEN];
int allVowelsPresent;

char *phrases[] = { "educated", "educated cat", "educated lion", "serious person", \
"serious panda", "curious student", "curious art student", "obnoxious web developer"};

void *checker(void *param) {
    SyncInfo* copy = (SyncInfo*) param;
    char localBuffer[BUFLEN];

    int hasa;
    int hase;
    int hasi;
    int haso;
    int hasu;
    int i, j;
    int n = ((SyncInfo*)param)->numPhrases;

    for (i=0; i<n; ++i) {
        hasa = 0;
        hase = 0;
        hasi = 0;
        haso = 0;
        hasu = 0;

        sem_wait(copy->sem);
        printf("I am runner: starting word test\n");
        strcpy(localBuffer, buffer);

        for (j=0; j<strlen(localBuffer); j++) {
            if (localBuffer[j]=='a') {
                hasa = 1;
            }
            if (localBuffer[j]=='e') {
                hase = 1;
            }
            if (localBuffer[j]=='i') {
                hasi = 1;
            }
            if (localBuffer[j]=='o') {
                haso = 1;
            }
            if (localBuffer[j]=='u') {
                hasu = 1;
            }
        }

        pthread_mutex_lock(&(copy->mutex1));
        if (hasa==1 && hase==1 && hasi==1 && haso==1 && hasu==1) {
            allVowelsPresent = 1;
        }
        else {
            allVowelsPresent = 0;
        }
        pthread_mutex_unlock(&(copy->mutex1));

        sem_post(copy->sem1);
        printf("I am runner: done\n");
    }
    pthread_exit(0);
}

int main() {
    pthread_t tidOne;
    SyncInfo syncInfo;
    syncInfo.numPhrases = 8;

    syncInfo.sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 0);
    if (syncInfo.sem == SEM_FAILED) {
        printf("sem_open() failed\n");
        sem_unlink(SEM_NAME);
        return(8);
    }

    syncInfo.sem1 = sem_open(SEM_NAME_TWO, O_CREAT | O_EXCL, 0666, 0);
    if (syncInfo.sem1 == SEM_FAILED) {
        printf("checker sem_open() failed\n");
        sem_unlink(SEM_NAME_TWO);
        return(8);
    }

    pthread_mutex_init(&(syncInfo.mutex), NULL);
    pthread_mutex_init(&(syncInfo.mutex1), NULL);
    pthread_create(&tidOne,NULL,checker,&syncInfo);

    int i;
    for (i=0; i<syncInfo.numPhrases; ++i) {
        pthread_mutex_lock(&(syncInfo.mutex));
        strcpy(buffer, phrases[i]);
        pthread_mutex_unlock(&(syncInfo.mutex));

        sem_post(syncInfo.sem);
        sem_wait(syncInfo.sem1);

        printf("result of checking ’%s’: %d\n", phrases[i], allVowelsPresent);
    }
    pthread_join(tidOne, NULL);
    sem_close(syncInfo.sem);
    sem_close(syncInfo.sem1);
    sem_unlink(SEM_NAME);
    sem_unlink(SEM_NAME_TWO);
    return 0;
}
