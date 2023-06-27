#include <stdio.h>
#include <stdbool.h>

typedef struct machine
{
    int machineId;
    bool isWorking;
    int endTime;
} Machine;

typedef struct operation
{
    int jobId;
    int machineId;
    int duration;
} Operation;

typedef struct job
{
    Operation operations[100];
} Job;


int main(int argc, char **argv)
{
    FILE *inputFile;
    int nJobs;
    int nMachines;

    inputFile = fopen("jobshop.txt", "r");
    fscanf(inputFile, "%d %d", &nJobs, &nMachines);

    printf("1: %d\n", nJobs);
    printf("2: %d\n", nMachines);

    Job jobs[100];
    Machine machines[100];

    // Preencher operaçoes -> (Maquina, Duracao)
    for (int i = 0; i < nJobs; i++) {
        for (int m = 0; m < nMachines; m++) {
            fscanf(inputFile, "%d  %d", &jobs[i].operations[m].machineId, &jobs[i].operations[m].duration);
            printf("\nMachine:%d Duration:%d", jobs[i].operations[m].machineId, jobs[i].operations[m].duration);
            jobs[i].operations[m].jobId = i;
        }
    }
    

    return 0;
}