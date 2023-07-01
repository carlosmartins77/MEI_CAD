#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef _OPENMP
    #include <omp.h>
    #define getClock() omp_get_wtime()
    #else
    #include <time.h>
    #define getClock() ((double)clock() / CLOCKS_PER_SEC)
#endif

typedef struct
{
    int machineId;
    bool isWorking;
    int endTime;
} Machine;

typedef struct
{
    int jobId;
    int machineId;
    int duration;
} Operation;

typedef struct
{
    Operation operations[100];
} Job;



int main(int argc, char **argv)
{
    
    double tempoinicio = getClock();
    
    FILE *inputFile;
    FILE *outputFile;
    int nJobs;
    int nMachines;

    inputFile = fopen(argv[1], "r");
    fscanf(inputFile, "%d %d", &nJobs, &nMachines);
    outputFile = fopen("output.txt", "w");

    int nThreads = nJobs;
    if (argc >= 2) {
        nThreads = atoi(argv[2]);
        printf("Number of threads: %d\n", nThreads);
    }
    
    Job jobs[100];
    Machine machines[100];

    for (int m = 0; m < nMachines; m++)
    {
        machines[m].isWorking = false;
        machines[m].machineId = m;
    }

    // Preencher operaçoes -> (Maquina, Duracao)
    for (int i = 0; i < nJobs; i++) 
    {
        for (int m = 0; m < nMachines; m++) 
        {
            fscanf(inputFile, "%d  %d", &jobs[i].operations[m].machineId, &jobs[i].operations[m].duration);
            //printf("\nMachine:%d Duration:%d", jobs[i].operations[m].machineId, jobs[i].operations[m].duration);
            jobs[i].operations[m].jobId = i;
        }
    }

    int initialTime[100][100]; // Array para guardar initial time
    int endTime[100][100];     // Array para guardar end time
    int maxEndTime = 0; // Guardar o ultimo tempo

#pragma omp parallel num_threads(nThreads > 0 ? nThreads : nJobs) // Com duplo for não conseguimos utilizar o criticall juntamnete com o barrier.
    {
        int jobThread = omp_get_thread_num(); //Com isto substitui-mos o for de fora. Cada thread é responsavel por uma iteração.

        // Percorrer as linhas
        for (int j = 0; j < nMachines; j++) // Percorre os jobs (combinaçao maquina, operacao)
        {   
        #pragma omp critical 
            if (machines[jobs[jobThread].operations[j].machineId].isWorking == true) 
            {
                // o tempo inicial da operacao 0 do job 1 é igual ao tempo da operacao 0 do job 0 
                initialTime[jobThread][j] = machines[jobs[jobThread].operations[j].machineId].endTime; // Agora vai aceder à maquina que fez a operaçao 1 do job 0, que neste caso foi a maquina 0 que teve o end time definido na iteraçao 0,0      
                endTime[jobThread][j] = initialTime[jobThread][j] + jobs[jobThread].operations[j].duration;

                machines[jobs[jobThread].operations[j].machineId].isWorking = true; //ocupada
                machines[jobs[jobThread].operations[j].machineId].endTime = endTime[jobThread][j];
            }
            else // se a maquina responsavel pela operaçao 0 do job 0 esta livre..
            {
                initialTime[jobThread][j] = maxEndTime;        // a opercao 0 começa aos 0                                        
                endTime[jobThread][j] = initialTime[jobThread][j] + jobs[jobThread].operations[j].duration; // A operaçao 0 do job 0 termina aos 0 + o tempo de duraçao do job 0 da maquina 0

                machines[jobs[jobThread].operations[j].machineId].isWorking = true; // a maquina 0 passou a estar ocupada
                machines[jobs[jobThread].operations[j].machineId].endTime = endTime[jobThread][j]; //O tempo de término da operaçao 0 realizada na maquina 0 é igual ao endtime definido anteriormente   
            }
        }
       
        #pragma omp barrier
        //  resetar pq ja definiu a primeira operaçao de cada job e já definiu os end times de cada uma das operaçoes
        for (int m = 0; m < nMachines; m++)
        {
            machines[m].isWorking = false;
            machines[m].endTime = 0;
        }
        // Atualizar o endtime das operacoes
        
        for (int mAux = 0; mAux < nMachines; mAux++)
        {
            if (maxEndTime < endTime[mAux][jobThread])
                maxEndTime = endTime[mAux][jobThread];
        }
    
    }

    // Print do resultado
    for (int j = 0; j < nJobs; j++) {
        for (int m = 0; m < nMachines; m++) {
            printf("Job %d, Machine %d, Start Time: %d, End Time: %d\n", j, jobs[j].operations[m].machineId, initialTime[j][m], endTime[j][m]);
            fprintf(outputFile, "Job %d, Machine %d, Start Time: %d, End Time: %d\n", j, jobs[j].operations[m].machineId, initialTime[j][m], endTime[j][m]);
        }
    }
    printf("tempo execucao (s)= %.6f\n", getClock() - tempoinicio);


    return 0;
}