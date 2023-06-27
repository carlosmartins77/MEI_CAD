#include <stdio.h>

int main(int argc, char **argv)
{
    FILE *inputFile;
    int nJobs;
    int nMachines;

    inputFile = fopen("jobshop.txt", "r");
    fscanf(inputFile, "%d%d", &nJobs, &nMachines);

    printf("1: %d\n", nJobs);
    printf("2: %d\n", nMachines);

    return 0;
}