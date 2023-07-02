#include <stdio.h>
#include <stdbool.h>

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

    outputFile = fopen(argv[2], "w");

    Job jobs[100];
    Machine machines[100];

    for (int m = 0; m < nMachines; m++)
    {
        machines[m].isWorking = false;
        machines[m].machineId = m;
    }

    // Preencher operaçoes -> (Maquina, Duracao)
    for (int i = 0; i < nJobs; i++) {
        for (int m = 0; m < nMachines; m++) {
            fscanf(inputFile, "%d  %d", &jobs[i].operations[m].machineId, &jobs[i].operations[m].duration);
            //printf("\nMachine:%d Duration:%d", jobs[i].operations[m].machineId, jobs[i].operations[m].duration);
            jobs[i].operations[m].jobId = i;
        }
    }

    int initialTime[100][100]; // Array para guardar initial time
    int endTime[100][100];     // Array para guardar end time
    int maxEndTime = 0; // Guardar o ultimo tempo
    
    for (int m = 0; m < nMachines; m++) // percore as jobs (Linha de operacoes)
    {
        // Percorrer as linhas
        for (int j = 0; j < nJobs; j++) // Percorre as operacoes (combinaçao maquina, duraçao)
        {   
            if (machines[jobs[j].operations[m].machineId].isWorking == true) 
            {
                // o tempo inicial da operacao 0 do job 1 é igual ao tempo da operacao 0 do job 0 
                initialTime[j][m] = machines[jobs[j].operations[m].machineId].endTime; // Agora vai aceder à maquina que fez a operaçao 1 do job 0, que neste caso foi a maquina 0 que teve o end time definido na iteraçao 0,0      
                endTime[j][m] = initialTime[j][m] + jobs[j].operations[m].duration;

                machines[jobs[j].operations[m].machineId].isWorking = true; //ocupada
                machines[jobs[j].operations[m].machineId].endTime = endTime[j][m];  
            }
            else // se a maquina responsavel pela operaçao 0 do job 0 esta livre..
            {
                initialTime[j][m] = maxEndTime;        // a opercao 0 começa aos 0                                        
                endTime[j][m] = initialTime[j][m] + jobs[j].operations[m].duration; // A operaçao 0 do job 0 termina aos 0 + o tempo de duraçao do job 0 da maquina 0

                machines[jobs[j].operations[m].machineId].isWorking = true; // a maquina 0 passou a estar ocupada
                machines[jobs[j].operations[m].machineId].endTime = endTime[j][m]; //O tempo de término da operaçao 0 realizada na maquina 0 é igual ao endtime definido anteriormente   
            }
        }

        //  resetar pq ja definiu a primeira operaçao de cada job e já definiu os end times de cada uma das operaçoes
        for (int m = 0; m < nMachines; m++)
        {
            machines[m].isWorking = false;
            machines[m].endTime = 0;
        }

        // Atualizar o endtime das operacoes
        for (int mAux = 0; mAux < nJobs; mAux++)
        {
            for (int jAux = 0; jAux < nMachines; jAux++)
            {
                if (maxEndTime < endTime[mAux][jAux])
                    maxEndTime = endTime[mAux][jAux];
            }
        }
    
    }

    printf("Tempo execucao (s)= %.6f\n", getClock() - tempoinicio);

    // Print do resultado
    for (int j = 0; j < nJobs; j++) {
        for (int m = 0; m < nMachines; m++) {
            printf("Job %d, Machine %d, Start Time: %d, End Time: %d\n", j, jobs[j].operations[m].machineId, initialTime[j][m], endTime[j][m]);
            fprintf(outputFile, "Job %d, Machine %d, Start Time: %d, End Time: %d\n", j, jobs[j].operations[m].machineId, initialTime[j][m], endTime[j][m]);
        }
    }
    


    return 0;
}