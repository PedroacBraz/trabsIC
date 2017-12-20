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
#include <time.h>
#define CHOICE_MAX 10
#define SIZE_POPULATION 100
#define M1 5
#define M2 10
#define txMutation 5
#define C1 60
#define C2 80
#define SIZE_CROSSOVER 60
#define GENERATIONS 50
#define REPEAT 1000


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
int searchIndex(int[12], int );
void showArray(int[], int);
void copyArray(int[12], int[12]);
void swapArray(int[12], int[12]);
void overridePopulation(int[][12], int[][12]);
void generatePopulation(int[SIZE_POPULATION][12], int[CHOICE_MAX]);
int calculateFitness(int[]);
void resetChildren(int[][12]);
int tournament(int[SIZE_POPULATION][12]);
int roulete(int[SIZE_POPULATION][12]);
void cyclicCrossover(int[12], int[12], int[][12], int, int);
void pmxCrossover(int[12], int[12], int[][12], int , int );
void pmx(int[12], int[12], int[][12], int );
void mutation(int[][12], int, int);
void procreate(int[SIZE_POPULATION][12],int[][12],int , int , int , int );
void statistics(TStatistics*, int[][12], int[12], int);
void finilizeStatistics(TStatistics* );
void quicksort(int[][12], int, int);
void ordenedReinsertion(int[][12], int[][12], int[][12], int);
void pureReinsertion(int[][12], int[][12], int[][12]);
void condense(int[][12], int[][12], int[][12], int , int );
int main(int argc, const char * argv[]) {
    // insert code here...
    srand(time(NULL));
    TStatistics stats;
    int rep,gen, bestSubject[11];
    int possibilities[CHOICE_MAX]; // numeros de 0 a 9
    int population[SIZE_POPULATION][12]; //100 individuos com S=0, E=1, N=2, D=3, M=4, O=5, R=6, Y=7 ,8 e 9 mantidos para o funcionamento do Crossover ciclico e index 10 reservado para a fitness
    int children[SIZE_CROSSOVER][12]; //mesmo formato da population tirando os tickets da roleta
    int newPopulation[SIZE_POPULATION][12];
    bestSubject[10] = INT_MAX;
    prepareStats(&stats);
	
	int selection, typecross, sizechild,i, mut,j, condensetype; //selection 0:torneio 1:roleta , typecross 0:ciclico 1:pmx, sizechild C1:60 C2:80, mut M1:5 M2:10, condensetype 0:ordenada 1:pura 80% (sempre acompanha com C2)
	
	for(selection = 0; selection < 2; selection++){
		for(typecross = 0; typecross < 2; typecross++){
			for(condensetype = 0; condensetype < 2; condensetype++){
				for(i=0; i<2;i++){ // sizechild
                    if(i == 0){
						if(condensetype == 1)
                            continue;
						else{
							sizechild = C1;
						}
                    }else{
						sizechild = C2;
                    }
					for(j=0;j<2;j++){
						if(j == 0)
							mut = M1;
						else
							mut = M2;
						// negocio tudo aqui
						
						clock_t start = clock();
						for(rep=0;rep< REPEAT; rep++){
							generatePopulation(population, possibilities);
							for(gen=0; gen< GENERATIONS; gen++){
								//printf("GERACAO DE NUMERO %d\n",j+1);
								resetChildren(children);
								procreate(population, children, selection, typecross, sizechild, mut);
								quicksort(population, 0, SIZE_POPULATION);
								quicksort(children, 0, sizechild);
								condense(population, children, newPopulation, condensetype, sizechild);
								overridePopulation(population, newPopulation);
							}
							//statistics(&stats, population, bestSubject, 0);
							//statistics(&stats, newPopulation, bestSubject, 1);
							//population = newPopulation;
						}
						clock_t end = clock();
						float seconds = (float)(end - start) / CLOCKS_PER_SEC;
						printf("S%d + C%d + TC%d + TM%d + R%d\n",selection, typecross,sizechild, mut, condensetype);
						printf("tempo: %.3f segundos\n\n",seconds);
						
					}
				}
			}
		}
	}
    /*finilizeStatistics(&stats);
    printf("Estatisticas: \t G0 \t G1\n\n");
    printf("Media do melhor:\t %f\t %f\n",stats.bestFitAvg[0],stats.bestFitAvg[1]);
    printf("Media da populacao:\t %f\t %f\n",stats.fitAvg[0],stats.fitAvg[1]);
    printf("Media qt < 100:\t %f\t %f\n",stats.lessHundredAvg[0],stats.lessHundredAvg[1]);
    printf("Media fit < 100:\t %f\t %f\n",stats.lessHundredFitAvg[0],stats.lessHundredFitAvg[1]);
    printf("Media M eh 0:\t %f\t %f\n",stats.xMIs0[0],stats.xMIs0[1]);
    printf("Media M eh 1:\t %f\t %f\n",stats.xMIs1[0],stats.xMIs1[1]);
    printf("\nMelhor individuo\n");
    showArray(bestSubject, 11);*/
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
int searchIndex(int array[12], int value){
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
void copyArray(int origin[12], int destiny[12]){
    int i;
    for(i=0; i<12; i++){
        destiny[i] = origin[i];
    }
}
void swapArray(int a[12], int b[12]){
    int i, temp;
    for(i=0; i<12; i++){
        temp = a[i];
        a[i] = b[i];
        b[i] = temp;
    }
}
void overridePopulation(int old[][12], int new[][12]){
    int i;
    for(i = 0; i < SIZE_POPULATION; i++)
        copyArray(new[i], old[i]);
}
void generatePopulation(int population[SIZE_POPULATION][12], int possibilities[CHOICE_MAX]){
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
        //printf("pai de numero %d\n",i);
        //showArray(population[i], 11);
    }
}
int calculateFitness(int array[]){ //S=0, E=1, N=2, D=3, M=4, O=5, R=6, Y=7 => send = 0123 more = 4561 money = 45217
    int fitness;
    fitness = abs((array[0]*1000+array[1]*100+array[2]*10+array[3])+(array[4]*1000+array[5]*100+array[6]*10+array[1])-(array[4]*10000+array[5]*1000+array[2]*100+array[1]*10+array[7]));
    return fitness;
}
void resetChildren(int children[][12]){
    int i,j;
    for(i=0; i<SIZE_CROSSOVER; i++)
        for(j=0; j<12; j++)
            children[i][j]= -1;
}
int tournament(int parents[SIZE_POPULATION][12]){
    int choosen[3],i;
    do{
        for(i=0; i< 3; i++){
            choosen[i] = rand()%SIZE_POPULATION;
        }
    }while(choosen[0] == choosen[1] ||choosen[0] == choosen[2] || choosen[1] == choosen[2]);
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
int roulete(int parents[SIZE_POPULATION][12]){
    int tickets=0, lucky, i, accum=0;
    for(i=0; i< SIZE_POPULATION; i++){
        parents[i][11] = 100000 - parents[i][10];
        tickets += parents[i][11];
    }
    lucky = rand()%tickets;
    lucky++;
    i=0;
    do{
        accum += parents[i++][11];
    }while(accum < lucky && i< SIZE_POPULATION);
    return i-1;
}
void cyclicCrossover(int parent1[12], int parent2[12], int children[][12], int idxChild1, int idxChild2){
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
    //printf("filho de numero %d\n",idxChild1);
    //showArray(children[idxChild1], 11);
    children[idxChild2][10] = calculateFitness(children[idxChild2]);
    //printf("filho de numero %d\n",idxChild2);
    //showArray(children[idxChild2], 11);
}
void pmxCrossover(int parent1[12], int parent2[12], int children[][12], int idxChild1, int idxChild2){
	pmx(parent1, parent2, children, idxChild1);
	pmx(parent2, parent1, children, idxChild2);
}
void pmx(int parent1[12], int parent2[12], int children[][12], int idxChild){
	int alele1, alele2, i,j,pinnedValue, idxPinned, flag=0, auxValue, auxIdx;
	alele1 = rand()%10;
	alele2 = rand()%10;
	if(alele1 > alele2)
		swap(&alele1, &alele2);
    for(i = alele1; i <= alele2; i++)
		children[idxChild][i] = parent1[i];
    for(i = alele1; i <= alele2; i++){
		pinnedValue = parent2[i];
		flag = 0;
        for(j = alele1; j<= alele2; j++){
			if(parent1[j] == pinnedValue){
				flag = 1;
				break;
			}
		}
		idxPinned = i; //i
		while(flag == 0){
			auxValue = parent1[idxPinned];//i
            auxIdx = searchIndex(parent2, auxValue); //ii
			if(auxIdx >= alele1 && auxIdx <= alele2) //iii
				idxPinned = auxIdx;
			else{
				children[idxChild][auxIdx] = pinnedValue; //iv
				break;
			}
		}
	}
	for(i = 0; i < 10; i++){
		if(children[idxChild][i] == -1){
			children[idxChild][i] = parent2[i];
		}
	}
	children[idxChild][10] = calculateFitness(children[idxChild]);
    //printf("filho de numero %d\n",idxChild);
    //showArray(children[idxChild], 11);
}
void mutation(int children[][12], int size, int mut){
    int lucky[mut][3], i;
    for(i=0; i< mut; i++){
        lucky[i][0] = rand()%size;
        do{
            lucky[i][1] = rand()%8;
            lucky[i][2] = rand()%8;
        }while(lucky[i][1] == lucky[i][2]);
        swapWhitinArray(children[lucky[i][0]], lucky[i][1], lucky[i][2]);
        children[lucky[i][0]][10] = calculateFitness(children[lucky[i][0]]);
        //printf("filho de numero %d que sofreu mutacao trocando indices %d e %d\n",lucky[i][0],lucky[i][1],lucky[i][2]);
        //showArray(children[lucky[i][0]], 11);
    }
}
void procreate(int parents[SIZE_POPULATION][12],int children[SIZE_CROSSOVER][12], int selection, int type, int size, int mut){ // type: 0 ciclico, 1 pmx selection: 0 torneio, 1 roleta
    int i, parent1, parent2;
    //printf("Filhos gerados: \n\n");
    for(i = 0;i < (size/2); i++){
		if(selection == 0)
        {parent1 = tournament(parents); parent2 = tournament(parents);}
		else
        {parent1 = roulete(parents); parent2 = roulete(parents);}
		if(type == 0)
			cyclicCrossover(parents[parent1], parents[parent2], children, i, i+size/2);
		else 
			pmxCrossover(parents[parent1], parents[parent2], children, i, i+size/2);
    }
    //sorteando mutacao
    mutation(children, size, mut);
}
void statistics(TStatistics* stats, int population[][12], int bestSubject[12], int generation){
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
void quicksort(int array[][12], int start, int end){
    
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
void ordenedReinsertion(int parents[][12], int children[][12], int condensedPopulation[][12], int sizechild){
    int i, j, k;
    i = j = k = 0;
    for(;k<SIZE_POPULATION;k++){
		if(j < sizechild)
			if(parents[i][10]<children[i][10]){
				copyArray(parents[i],condensedPopulation[k]);
				i++;
			}else{
				copyArray(children[j], condensedPopulation[k]);
				j++;
			}
		else{
			copyArray(parents[i],condensedPopulation[k]);
			i++;
		}
    }

}
void pureReinsertion(int parents[][12], int children[][12], int condensedPopulation[][12]){
	int i, j, k;
    i = j = k = 0;
    for(;k<SIZE_POPULATION;k++){
        if(k < 20){
            copyArray(parents[i],condensedPopulation[k]);
            i++;
        }else{
            copyArray(children[j], condensedPopulation[k]); //ter certeza de children ter 80 individuos
            j++;
        }
    }
}
void condense(int parents[][12], int children[][12], int condensedPopulation[][12], int condensetype, int sizechild){ // condensetype : 0 ordenada, 1 pura 80%
    if(condensetype == 0)
		ordenedReinsertion(parents,children,condensedPopulation, sizechild);
	else
        pureReinsertion(parents, children, condensedPopulation);
}
