#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct process {
    /* Values initialized for each process */
    int id;
    int arrivaltime;  /* Time process arrives and wishes to start */
    int runtime;      /* Time process requires to complete job */

    /* Values algorithm may use to track processes */
    int starttime;
    int endtime;
};

struct process proc[100];

void first_come_first_served(int n) {
    /* Your code for FCFS algorithm here */
    int* servicetime, * waitingtime;
    servicetime = (int*)malloc(n * sizeof(int));
    waitingtime = (int*)malloc(n * sizeof(int));
    servicetime[0] = proc[0].arrivaltime;
    waitingtime[0] = 0;
    proc[0].endtime = proc[0].runtime;
    for (int i = 1; i < n; i++) {
        servicetime[i] = servicetime[i - 1] + proc[i-1].runtime;
        waitingtime[i] = servicetime[i] - proc[i].arrivaltime;
        if (waitingtime[i] < 0)
            waitingtime[i] = 0;
        proc[i].starttime = waitingtime[i] + proc[i].arrivaltime > proc[i-1].endtime ? waitingtime[i] + proc[i].arrivaltime : proc[i - 1].endtime;
        proc[i].endtime = proc[i].starttime + proc[i].runtime;
    }
    free(servicetime);
}

void sort_by(int maxTime, int s, int n) {
    int sort_up_to = s;
    for (int i = s; i < n; i++) {
        if (proc[i].arrivaltime <= maxTime) {
            sort_up_to++;
        }
        else break;
    }
    for (int i = s; i < sort_up_to; i++) {
        for (int j = s; j < sort_up_to - i - 1;j++) {
            if (proc[j].runtime > proc[j + 1].runtime) {
                struct process p = proc[j];
                proc[j] = proc[j + 1];
                proc[j + 1] = p;
            }
        }
    }
}

void shortest_job_first(int n) {
    /* Your code for SJF algorithm here */
    int min, idx, tt;
    int *waitingtime,* turnaroundtime;
    waitingtime = (int*)malloc(n * sizeof(int));
    turnaroundtime = (int*)malloc(n * sizeof(int));
    // arranging by arrival times (already) then by runtime
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (proc[j].arrivaltime == proc[j + 1].arrivaltime) {
                if (proc[j].runtime > proc[j + 1].runtime) {
                    struct process p = proc[j];
                    proc[j] = proc[j + 1];
                    proc[j + 1] = p;
                }
            }
        }
    }
    waitingtime[0] = 0;
    turnaroundtime[0] = proc[0].runtime;
    proc[0].starttime = waitingtime[0] + proc[0].arrivaltime;
    proc[0].endtime = turnaroundtime[0];
    for (int i = 1; i < n;i++) {
        sort_by(turnaroundtime[i-1],i,n);
        if (proc[i].arrivaltime >= waitingtime[i - 1] && proc[i].arrivaltime >= turnaroundtime[i-1])
            waitingtime[i] = 0;
        else waitingtime[i] = turnaroundtime[i - 1] - proc[i].arrivaltime;
        turnaroundtime[i] = waitingtime[i] + proc[i].arrivaltime + proc[i].runtime;
        proc[i].starttime = waitingtime[i] + proc[i].arrivaltime;
        proc[i].endtime = turnaroundtime[i];
    }
    free(waitingtime);
    free(turnaroundtime);
}

void printProcesses(int n) {
    for (int i = 0; i < n; i++) {
        printf("Process %d started at time %d and finished at time %d\n", proc[i].id, proc[i].starttime, proc[i].endtime);
    }
}

int main() {

    FILE* pFile = fopen("Testcase.txt", "r");
    if (pFile == NULL) {
        printf("Error! Could not open file\n");
        exit(-1);
    }
    int i = 0;
    printf("Process\tarrival\truntime\n");
    while (fscanf(pFile, "%d%d%d", &proc[i].id, &proc[i].arrivaltime, &proc[i].runtime) != EOF) {
        printf("%d\t%d\t%d\n", proc[i].id, proc[i].arrivaltime, proc[i].runtime);
        i++;
    }

    /* Run scheduling algorithms */
    printf("First come first served...\n");
    first_come_first_served(i);
    printProcesses(i);

    printf("Shortest job first...\n");
    shortest_job_first(i);
    printProcesses(i);
    return 0;
}