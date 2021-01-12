//============================================================================
// Name        : knn.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <fstream>
using namespace std;


#define  ATTR_NUM  4                        //Numero di attributi
#define  MAX_SIZE_OF_TRAINING_SET  1000      //La dimensione massima del set di dati di addestramento
#define  MAX_SIZE_OF_TEST_SET      100       //Dimensione massima del set di dati di prova
#define  MAX_VALUE  10000.0
#define  K  5

//
struct dataVector {
	int ID;
	char classLabel[15];             //Etichetta di classificazione
	double attributes[ATTR_NUM]; 	//Attributi
};
struct distanceStruct {
	int ID;
	double distance;             //Distanza
	char classLabel[15];             //Etichetta di classificazione
};


struct dataVector gTrainingSet[MAX_SIZE_OF_TRAINING_SET]; //Set di dati di allenamento
struct dataVector gTestSet[MAX_SIZE_OF_TEST_SET];         //Test set di dati
struct distanceStruct gNearestDistance[K];                //K distanze dei vicini più vicini
int curTrainingSetSize=0;                                 //La dimensione del set di dati di addestramento
int curTestSetSize=0;                                     //La dimensione del set di dati di test

//Distanza Euclidea tra vector1=(x1,x2,...,xn) e vector2=(y1,y2,...,yn)
double Distance(struct dataVector vector1,struct dataVector vector2)
{
	double dist,sum=0.0;
	for(int i=0;i<ATTR_NUM;i++)
	{
		sum+=(vector1.attributes[i]-vector2.attributes[i])*(vector1.attributes[i]-vector2.attributes[i]);
	}
	dist=sqrt(sum);
	return dist;
}

//Ottenere la distanza massima in gNearestDistance
int GetMaxDistance()
{
	int maxNo=0;
	for(int i=1;i<K;i++)
	{
		if(gNearestDistance[i].distance>gNearestDistance[maxNo].distance)	maxNo = i;
	}
    return maxNo;
}

//Classificazione per campioni sconosciuti
char* Classify(struct dataVector Sample)
{
	double dist=0;
	int maxid=0,freq[K],i,tmpfreq=1;;
	char *curClassLable=gNearestDistance[0].classLabel;
	memset(freq,1,sizeof(freq));
	//step.1---La distanza iniziale è la massima
	for(i=0;i<K;i++)
	{
		gNearestDistance[i].distance=MAX_VALUE;
	}
	//step.2---Calcola K-distanze del vicino più vicino
	for(i=0;i<curTrainingSetSize;i++)
	{
		//step.2.1--Calcola la distanza tra il campione sconosciuto e ogni campione di addestramento
		dist=Distance(gTrainingSet[i],Sample);
		//step.2.2---Ottiene la distanza massima in gNearestDistance
		maxid=GetMaxDistance();
		//step.2.3---Se la distanza è inferiore alla distanza massima in gNearestDistance, il campione è considerato come il campione vicino più vicino K
		if(dist<gNearestDistance[maxid].distance)
		{
			gNearestDistance[maxid].ID=gTrainingSet[i].ID;
			gNearestDistance[maxid].distance=dist;
			strcpy(gNearestDistance[maxid].classLabel,gTrainingSet[i].classLabel);
		}
	}
	//step.3---Contare il numero di occorrenze di ciascuna categoria
	for(i=0;i<K;i++)
	{
		for(int j=0;j<K;j++)
		{
			if((i!=j)&&(strcmp(gNearestDistance[i].classLabel,gNearestDistance[j].classLabel)==0))
			{
				freq[i]+=1;
			}
		}
	}
	//step.4---Seleziona l'etichetta della classe che appare più frequentemente
	for(i=0;i<K;i++)
	{
		if(freq[i]>tmpfreq)
		{
			tmpfreq=freq[i];
 			curClassLable=gNearestDistance[i].classLabel;
		}
	}
	return curClassLable;
}


int main()
{
	char c;
    char *classLabel="";
	int i,j, rowNo=0,TruePositive=0,FalsePositive=0;
	ifstream filein("iris.data");
	FILE *fp;
	if(filein.fail()){cout<<"Can't open data.txt"<<endl; return 0;}
	//step.1--Leggi il file
	while(!filein.eof())
	{
		rowNo++;//La prima serie di dati rowNo = 1
		if(curTrainingSetSize>=MAX_SIZE_OF_TRAINING_SET)
		{
			cout<<"The training set has "<<MAX_SIZE_OF_TRAINING_SET<<" examples!"<<endl<<endl;
			break ;
		}
		//100 set di dati con rowNo NON divisibile per 3 vengono utilizzati come set di dati di addestramento
		if(rowNo%3!=0)
		{
			gTrainingSet[curTrainingSetSize].ID=rowNo;
			for(int i = 0;i < ATTR_NUM;i++)
			{
				filein>>gTrainingSet[curTrainingSetSize].attributes[i];
				filein>>c;
			}
			filein>>gTrainingSet[curTrainingSetSize].classLabel;
			curTrainingSetSize++;

		}
		//I 50 gruppi con la rowNo divisibile per 3 vengono lasciati come set di dati di test
		else if(rowNo%3==0)
		{
			gTestSet[curTestSetSize].ID=rowNo;
			for(int i = 0;i < ATTR_NUM;i++)
			{
				filein>>gTestSet[curTestSetSize].attributes[i];
				filein>>c;
			}
			filein>>gTestSet[curTestSetSize].classLabel;
			curTestSetSize++;
		}
	}
	filein.close();
	//step.2---L'algoritmo KNN classifica e scrive il risultato nel file iris_result.txt
	fp=fopen("iris_result.txt","w+t");

	fprintf(fp,"*********************************Descrizione della procedura******************************************\n");
	fprintf(fp,"** Uso l'algoritmo KNN per classificare iris.data. Per un facile utilizzo, aggiungo l'attributo rowNo a ciascun gruppo di dati\n");
	fprintf(fp,"*Sono disponibili 150 set di dati, 100 set (la cui rowNo NON è divisibile per 3) vengono selezionati come set di dati di addestramento e i restanti 50 set vengono utilizzati come set di dati di prova* \n");
	fprintf(fp,"***********************************************************************************\n\n");
	fprintf(fp,"*********************************Risultati sperimentali******************************************\n\n");
	printf("Numeri di vicini piu vicini: %d\n",K);
	for(i=0;i<curTestSetSize;i++)
	{
        fprintf(fp,"********************************Dati del gruppo %d******************************************\n",i+1);
		classLabel =Classify(gTestSet[i]);
	    if(strcmp(classLabel,gTestSet[i].classLabel)==0)//Quando è uguale, la classificazione è corretta
		{
			TruePositive++;
		}
		cout<<"rowNo:";
		cout<<gTestSet[i].ID<<"  \t";
		cout<<"Risultati della classificazione KNN: ";

		cout<<classLabel<<" Categoria corretta: ";
		cout<<gTestSet[i].classLabel<<")";
		fprintf(fp,"rowNo:  %3d   \t  Risultato della classificazione KNN:  %s (categoria corretta:  %s )\n",gTestSet[i].ID,classLabel,gTestSet[i].classLabel);
		if(strcmp(classLabel,gTestSet[i].classLabel)!=0)//Classificazione errata quando non uguale
		{
			cout<<"   ***Errata classificazione***\n";
			fprintf(fp,"                                                Errata classificazione******\n");
		}
		else
			cout<<"\n";
		fprintf(fp,"Numero di vicini più vicini: %d\n",K);
		for(j=0;j<K;j++)
		{
		//	cout<<gNearestDistance[j].ID<<"\t"<<gNearestDistance[j].distance<<"\t"<<gNearestDistance[j].classLabel[15]<<endl;
			fprintf(fp,"rowNo:	 %3d   \t   Distance:  %f   \tClassLable:    %s\n",gNearestDistance[j].ID,gNearestDistance[j].distance,gNearestDistance[j].classLabel);
		}
		fprintf(fp,"\n");
	}
    FalsePositive=curTestSetSize-TruePositive;
	fprintf(fp,"*******************Analisi dei risultati******************************************************\n",i);
	fprintf(fp,"TP(True positive): %d\nFP(False positive): %d\n Precisione: %f\n",TruePositive,FalsePositive,double(TruePositive)/(curTestSetSize-1));
	fclose(fp);
    return 0;
}
