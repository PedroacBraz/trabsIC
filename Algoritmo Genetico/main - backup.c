//
//  main.c
//  Algoritmo Genetico
//
//  Created by Miguel dos Santos Pereira on 18/08/16.
//  Copyright © 2016 Miguel dos Santos Pereira. All rights reserved.
//

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#define CHOICE_MAX 10
#define SIZE_POPULATION 100
#define CROSSOVER 60
#define TX_MUTATION 1
#define REPEAT 50


typedef struct{
    float bestFitAvg[2];
    float lessHundredAvg[2];
    float lessHundredFitAvg[2];
    float fitAvg[2];
    float xMIs0[2];
    float xMIs1[2];
}TStatistics;

void prepareArrayToSelect(int[CHOICE_MAX]);
void prepareStats(TStatistics*);
void swap(int*, int*);
void swapWhitinArray(int[],int,int);
int searchIndex(int[11], int );
void showArray(int[], int);
void copyArray(int[11], int[11]);
void swapArray(int[11], int[11]);
void generatePopulation(int[SIZE_POPULATION][11], int[CHOICE_MAX]);
int calculateFitness(int[]);
void resetChildren(int[CROSSOVER][11]);
int tournament(int[SIZE_POPULATION][11]);
int roulete(int[SIZE_POPULATION][11]);
void cyclicCrossover(int[11], int[11], int[CROSSOVER][11], int, int);
void mutation(int[CROSSOVER][11]);
void procreate(int[SIZE_POPULATION][11],int[CROSSOVER][11]);
void statistics(TStatistics*, int[][11], int[11], int);
void finilizeStatistics(TStatistics* );
void quicksort(int[][11], int, int);
void condense(int parents[][11], int children[][11], int condensedPopulation[][11]);


int main(int argc, const char * argv[]) {
    // insert code here...
    srand(time(NULL));
    TStatistics stats;
    int i, bestSubject[11];
    int possibilities[CHOICE_MAX]; // numeros de 0 a 9
    int population[SIZE_POPULATION][11]; //100 individuos com S=0, E=1, N=2, D=3, M=4, O=5, R=6, Y=7 ,8 e 9 mantidos para o funcionamento do crossover ciclico e index 10 reservado para a fitness
    int children[CROSSOVER][11]; //mesmo formato da population
    int newPopulation[SIZE_POPULATION][11];
    bestSubject[10] = INT_MAX;
    prepareStats(&stats);
    
    for(i=0;i< REPEAT; i++){
        generatePopulation(population, possibilities);
        resetChildren(children);
        procreate(population, children);
        quicksort(population, 0, SIZE_POPULATION); //TODO
        quicksort(children, 0, CROSSOVER); //TODO
        condense(population, children, newPopulation); //TODO
        statistics(&stats, population, bestSubject, 0);
        statistics(&stats, newPopulation, bestSubject, 1);
        //population = newPopulation;
    }
    finilizeStatistics(&stats);
    printf("Estatisticas: \t G0 \t G1\n\n");
    printf("Media do melhor:\t %f\t %f\n",stats.bestFitAvg[0],stats.bestFitAvg[1]);
    printf("Media da populacao:\t %f\t %f\n",stats.fitAvg[0],stats.fitAvg[1]);
    printf("Media qt < 100:\t %f\t %f\n",stats.lessHundredAvg[0],stats.lessHundredAvg[1]);
    printf("Media fit < 100:\t %f\t %f\n",stats.lessHundredFitAvg[0],stats.lessHundredFitAvg[1]);
    printf("Media M eh 0:\t %f\t %f\n",stats.xMIs0[0],stats.xMIs0[1]);
    printf("Media M eh 1:\t %f\t %f\n",stats.xMIs1[0],stats.xMIs1[1]);
    printf("\nMelhor individuo\n");
    showArray(bestSubject, 11);
}

//Funcao que reseta o vetor de possibilidades
void prepareArrayToSelect(int select[CHOICE_MAX]){
    int i;
    for (i = 0; i< CHOICE_MAX; i++){
        select[i] = i;
    }
}
void prepareStats(TStatistics* a){
    a->bestFitAvg[0] = a->bestFitAvg[1]= 0;
    a->fitAvg[0] = a->fitAvg[1] = 0;
    a->lessHundredAvg[0] = a->lessHundredAvg[1] = 0;
    a->lessHundredFitAvg[0] = a->lessHundredFitAvg[1] = 0;
    a->xMIs0[0] = a->xMIs0[1] = 0;
    a->xMIs1[0] = a->xMIs1[1] = 0;
}
void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
void swapWhitinArray(int array[], int index1, int index2)
{
    int temp = array[index1];
    array[index1] = array[index2];
    array[index2] = temp;
}
int searchIndex(int array[11], int value){
    int i=0;
    for(i=0; i<10; i++){
        if(array[i] == value)
            return i;
    }
    return -1;
}
void showArray(int array[], int range){
    int i;
    for (i = 0; i< range; i++){
        printf("array[%d]: %d\n",i,array[i]);
    }
}
void copyArray(int origin[11], int destiny[11]){
    int i;
    for(i=0; i<11; i++){
        destiny[i] = origin[i];
    }
}
void swapArray(int a[11], int b[11]){
    int i, temp;
    for(i=0; i<11; i++){
        temp = a[i];
        a[i] = b[i];
        b[i] = temp;
    }
}
void generatePopulation(int population[SIZE_POPULATION][11], int possibilities[CHOICE_MAX]){
    int i, j, aux, choiceBoundary = CHOICE_MAX;
    for(i=0; i< SIZE_POPULATION; i++){
        choiceBoundary = CHOICE_MAX;
        prepareArrayToSelect(possibilities);
        for(j=0; j<10; j++){
            int r = rand()%choiceBoundary;
            aux = possibilities[r];
            choiceBoundary--;
            swapWhitinArray(possibilities, r, choiceBoundary);
            population[i][j] = aux;
        }
        population[i][10] = calculateFitness(population[i]);
        printf("pai de numero %d\n",i);
        showArray(population[i], 11);
    }
}
int calculateFitness(int array[]){ //S=0, E=1, N=2, D=3, M=4, O=5, R=6, Y=7 => send = 0123 more = 4561 money = 45217
    int fitness;
    fitness = abs((array[0]*1000+array[1]*100+array[2]*10+array[3])+(array[4]*1000+array[5]*100+array[6]*10+array[1])-(array[4]*10000+array[5]*1000+array[2]*100+array[1]*10+array[7]));
    return fitness;
}
void resetChildren(int children[CROSSOVER][11]){
    int i,j;
    for(i=0; i<CROSSOVER; i++)
        for(j=0; j<11; j++)
            children[i][j]= -1;
}
int tournament(int parents[SIZE_POPULATION][11]){
    int choosen[3],i;
    for(i=0; i< 3; i++){
        choosen[i] = rand()%SIZE_POPULATION;
    }
    if(parents[choosen[0]][10] > parents[choosen[1]][10]){
        if(parents[choosen[1]][10] > parents[choosen[2]][10])
            return choosen[2];
        else
            return choosen[1];
    }else{
        if(parents[choosen[0]][10] > parents[choosen[2]][10])
            return choosen[2];
        else
            return choosen[0];
    }
}
int roulete(int parents[SIZE_POPULATION][11]){ //ta configurado para maximacao de fitness. tem que configurar para minimizar fitness
    int tickets=0, lucky, i, accum=0;
    for(i=0; i< SIZE_POPULATION; i++){
        tickets += parents[i][10];
    }
    lucky = rand()%tickets;
    lucky++;
    i=0;
    do{
        accum += parents[i++][10];
    }while(accum < lucky || i< SIZE_POPULATION);
    return i-1;
}
void cyclicCrossover(int parent1[11], int parent2[11], int children[CROSSOVER][11], int idxChild1, int idxChild2){
    //TODO CROSSOVER
    int i, j, first, next;
    for(i=0; i<10; i++){
        if(children[idxChild1][i] == -1){
            first = parent1[i];
            children[idxChild1][i] = first;
            next = parent2[i];
            children[idxChild2][i] = next;
            
            while(first != next){
                j = searchIndex(parent1, next);
                children[idxChild1][j] = parent1[j];
                children[idxChild2][j] = parent2[j];
                next = parent2[j];
            }
            swap(&idxChild1, &idxChild2);
        }
    }
    children[idxChild1][10] = calculateFitness(children[idxChild1]);
    printf("filho de numero %d\n",idxChild1);
    showArray(children[idxChild1], 11);
    children[idxChild2][10] = calculateFitness(children[idxChild2]);
    printf("filho de numero %d\n",idxChild2);
    showArray(children[idxChild2], 11);
}
void mutation(int children[CROSSOVER][11]){
    int lucky[TX_MUTATION][3], i;
    for(i=0; i< TX_MUTATION; i++){
        lucky[i][0] = rand()%CROSSOVER;
        do{
            lucky[i][1] = rand()%8;
            lucky[i][2] = rand()%8;
        }while(lucky[i][1] == lucky[i][2]);
        swapWhitinArray(children[lucky[i][0]], lucky[i][1], lucky[i][2]);
        children[lucky[i][0]][10] = calculateFitness(children[lucky[i][0]]);
        printf("filho de numero %d que sofreu mutacao trocando indices %d e %d\n",lucky[i][0],lucky[i][1],lucky[i][2]);
        showArray(children[lucky[i][0]], 11);
    }
}
void procreate(int parents[SIZE_POPULATION][11],int children[CROSSOVER][11]){
    int i;
    printf("Filhos gerados: \n\n");
    for(i = 0;i < (CROSSOVER/2); i++){
        int parent1 = tournament(parents), parent2 = tournament(parents);
        cyclicCrossover(parents[parent1], parents[parent2], children, i, i+CROSSOVER/2);
    }
    //sorteando mutacao
    mutation(children);
}
void statistics(TStatistics* stats, int population[][11], int bestSubject[11], int generation){
    int i, bestGeneration = INT_MAX, best=0;
    for(i=0; i< SIZE_POPULATION; i++){
        if (bestGeneration > population[i][10]) {
            best = i;
            bestGeneration = population[i][10];
            if(bestSubject[10] > population[i][10])
                copyArray(population[i], bestSubject);
        }
         stats->fitAvg[generation]+= population[i][10];
        if(population[i][10] <= 100){
            stats->lessHundredFitAvg[generation] = population[i][10];
            stats->lessHundredAvg[generation]++;
        }
        if(population[i][4] == 0)
            stats->xMIs0[generation]++;
        if(population[i][4] == 1)
            stats->xMIs1[generation]++;
    }
    stats->bestFitAvg[generation] += population[best][10];
}
void finilizeStatistics(TStatistics* stats){
    stats->bestFitAvg[0] = stats->bestFitAvg[0]/REPEAT;
    stats->bestFitAvg[1] = stats->bestFitAvg[1]/REPEAT;
    
    stats->fitAvg[0] = stats->fitAvg[0]/(REPEAT*SIZE_POPULATION);
    stats->fitAvg[1] = stats->fitAvg[1]/(REPEAT*SIZE_POPULATION);
    
    stats->lessHundredFitAvg[0] = stats->lessHundredFitAvg[0]/(stats->lessHundredAvg[0]);
    stats->lessHundredFitAvg[1] = stats->lessHundredFitAvg[1]/(stats->lessHundredAvg[1]);
    
    stats->lessHundredAvg[0] = stats->lessHundredAvg[0]/REPEAT;
    stats->lessHundredAvg[1] = stats->lessHundredAvg[1]/REPEAT;
    
    stats->xMIs0[0] = stats->xMIs0[0]/REPEAT;
    stats->xMIs0[1] = stats->xMIs0[1]/REPEAT;
    
    stats->xMIs1[0] = stats->xMIs1[0]/REPEAT;
    stats->xMIs1[1] = stats->xMIs1[1]/REPEAT;
}
void quicksort(int array[][11], int start, int end){
    
    int pivot, i, j, middle;
    
    i = start;
    j = end;
    
    middle = (int) ((i + j) / 2);
    pivot = array[middle][10];
    
    do{
        while (array[i][10] < pivot) i = i + 1;
        while (array[j][10] > pivot) j = j - 1;
        
        if(i <= j){
            swapArray(array[i], array[j]);
            i = i + 1;
            j = j - 1;
        }
    }while(j > i);
    
    if(start < j) quicksort(array, start, j);
    if(i < end) quicksort(array, i, end);
    
}
void condense(int parents[][11], int children[][11], int condensedPopulation[][11]){
    int i, j, k;
    i = j = k = 0;
    for(;k<SIZE_POPULATION;k++){
        if(parents[i][10]<children[i][10]){
            copyArray(parents[i],condensedPopulation[k]);
            i++;
        }else{
            copyArray(children[j], condensedPopulation[k]);
            j++;
        }
    }
    
}