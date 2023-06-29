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

    inputFile = fopen("jobshop1.txt", "r");
    fscanf(inputFile, "%d %d", &nJobs, &nMachines);

    printf("1: %d\n", nJobs);
    printf("2: %d\n", nMachines);

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
            printf("\nMachine:%d Duration:%d", jobs[i].operations[m].machineId, jobs[i].operations[m].duration);
            jobs[i].operations[m].jobId = i;
        }
    }

    int initialTime[100][100]; // Array para guardar initial time
    int endTime[100][100];     // Array para guardar end time
    int maxEndTime = 0; // Guardar o ultimo tempo
    
    for (int m = 0; m < nMachines; m++) // percore as operaçoes
    {
        // Percorrer as linhas
        for (int j = 0; j < nJobs; j++) // Traverse the jobs
        {
            
            if (machines[jobs[j].operations[m].machineId].isWorking == true) 
            {
                // o tempo inicial da operacao 0 do job 1 é igual ao tempo da operacao 0 do job 0 
                initialTime[j][m] = machines[jobs[j].operations[m].machineId].endTime; // Agora vai aceder à maquina que fez a operaçao 1 do job 0, que neste caso foi a maquina 0 que teve o end time definido na iteraçao 0,0      
                endTime[j][m] = initialTime[j][m] + jobs[j].operations[m].duration;

                // Block the machine
                machines[jobs[j].operations[m].machineId].isWorking = true;
                machines[jobs[j].operations[m].machineId].endTime = endTime[j][m];
                
                printf("\naqui aqui %d", machines[jobs[j].operations[m].machineId].endTime);
            }
            else // se a maquina responsavel pela operaçao 0 do job 0 esta livre..
            {
                initialTime[j][m] = maxEndTime;        // a opercao 0 começa aos 0                                        
                endTime[j][m] = initialTime[j][m] + jobs[j].operations[m].duration; // A operaçao 0 do job 0 termina aos 0 + o tempo de duraçao do job 0 da maquina 0

                // Mark machine as occupied
                machines[jobs[j].operations[m].machineId].isWorking = true; // a maquina 0 passou a estar ocupada
                machines[jobs[j].operations[m].machineId].endTime = endTime[j][m]; //O tempo de término da operaçao 0 realizada na maquina 0 é igual ao endtime definido anteriormente
                
                printf("\naqui %d", machines[jobs[j].operations[m].machineId].endTime);
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
    return 0;
}