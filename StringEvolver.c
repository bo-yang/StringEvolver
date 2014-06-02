#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>       /* time */

#define POP 40 
#define TNMT 4	    // Total number of tournaments
#define MAXSTR 80
char strPop[POP][MAXSTR]; // population of strings
char* mostFit[TNMT]; 	  // the most fit string of each tournament
char* leastFit[TNMT]; 	// the least fit string of each tournament

const char* REF="Hello,world!"; // Reference string

char GenRandChar()
{
	return (char)(32+rand()%96);
}

char * GenRandStr(char* str, unsigned int len)
{
	int i;
	for(i=0;i<len;++i)
	{
		str[i]=GenRandChar();
	}
	str[i]='\0';
	return str;
}

// Copy the most fit string(break ties by picking at 
// random among the most fit strings) over the least 
// fit string in each tournament.
unsigned int CrossoverM2L(char* strMostFit, char* strLeastFit, unsigned int len)
{
	int i,p;
	char tmp;

	// Generate random position. Make sure the string
	// won't be broken at the first or last character.
	p=rand()%(len-2)+1;
	for(i=0;i<p;++i)
	{
		tmp=strMostFit[i];
		strMostFit[i]=strLeastFit[i];
		strLeastFit[i]=tmp;
	}
	return p;
}

// Change one randomly chosen character in each copy.
unsigned int Mutate(char* str,unsigned int len)
{
	int p;

	p=rand()%len;
	str[p]=GenRandChar();
	return p;
}

// Count the number of identical chars between ref and str.
unsigned int GetFitness(char* str,unsigned int len)
{
	int i;
	int cnt=0;
	for(i=0;i<len;++i)
	{
		if(REF[i]==str[i])
			cnt++;
	}
	
	return cnt;
}

// Find out the most and least fit strings in a tournament.
// 	tourn -- the number of tourn, starting from 0.
void GetFitOfTnmt(char* mostFit[],char* leastFit[],unsigned int tourn)
{
	int i,fit;
	char* str;
	int most_fit,least_fit;

	// Make sure the most and least fit strings are different
	mostFit[tourn]=strPop[tourn*POP/TNMT];
	most_fit=GetFitness(mostFit[tourn],strlen(REF));
	leastFit[tourn]=strPop[tourn*POP/TNMT+1];
	least_fit=GetFitness(leastFit[tourn],strlen(REF));
	for(i=1;i<POP/TNMT;++i)
	{
		str=strPop[tourn*POP/TNMT+i];
		fit=GetFitness(str,strlen(REF));
		if(most_fit)
		{
			least_fit=fit;
			leastFit[tourn]=str;
		}
	}
}

// Get the most fit string of the population
char* GetMostFit(char* mostFit[])
{
	int i,fit;
	char* strFit=mostFit[0];
	int most_fit=GetFitness(strFit,strlen(REF));
	for(i=1;i<TNMT;++i)
	{
		fit=GetFitness(mostFit[i],strlen(REF));
		if(most_fit<fit)
		{
			most_fit=fit;
			strFit=mostFit[i];
		}
	}
	
	return strFit;
}

// Sort the strings in a tournament by fitness.
void SortTournByFit(unsigned int tourn)
{
	int tmpInt;
	unsigned int fit[POP/TNMT];
	int i,j;
	int len=strlen(REF);
	char tmpStr[MAXSTR];

	// Get the fitness of each string in this tournament
	for(i=0;i<POP/TNMT;i++)
	{
		fit[i]=GetFitness(strPop[tourn*POP/TNMT+i],len);
	}

	// Sort strings by fitness
	for(i=0;i<POP/TNMT;i++)
	{
		for(j=i+1;j<POP/TNMT;j++)
		{
			if(fit[i]<fit[j])
			{
				tmpInt=fit[i];
				fit[i]=fit[j];
				fit[j]=tmpInt;

				strcpy(tmpStr,strPop[tourn*POP/TNMT+i]);
				strcpy(strPop[tourn*POP/TNMT+i],strPop[tourn*POP/TNMT+j]);
				strcpy(strPop[tourn*POP/TNMT+j],tmpStr);
			}
		} // end of for j
	} // end of for i
}

// Crossover the top half strings.
// Remove the second half strings after sorting, keep the top half, 
// and generate children by crossing over top half strings.
void CrossoverByFit(unsigned int tourn)
{
	int i,j,p,len;
	char* strTmp;

	SortTournByFit(tourn);

	// Generate random position. Make sure the string
	// won't be broken at the first or last character.
	len=strlen(REF);
	p=rand()%(len-2)+1;
	
	for(i=0;i<(int)POP/TNMT/2;i+=2)
	{
		for(j=0;j<p;++j)
		{
			strPop[(tourn+1)*POP/TNMT-1-i][j]=strPop[tourn*POP/TNMT+i+1][j];
			strPop[(tourn+1)*POP/TNMT-2-i][j]=strPop[tourn*POP/TNMT+i][j];
		}
	}
}


// Print population by tournaments.
void DumpPop()
{
	int i,j;
	char* curStr;
	for(i=0;i<TNMT;i++)
	{
		printf("Tournament %d:\n",i);
		for(j=0;j<POP/TNMT;j++)
		{
			curStr=strPop[i*POP/TNMT+j];
			printf("\t%s\tFitness: %d\n",curStr,GetFitness(curStr,strlen(curStr)));
		}
		printf("\n");
	}
}

//
// MAIN START HERE
//
int main()
{
	int LEN=strlen(REF);

	int i,j,p;
	long cnt=0;
	char * strFit=NULL;

	/* initialize random seed: */
  	srand (time(NULL));
	
	// Initionlization
	for(i=0;i<POP;++i)
	{
		GenRandStr(strPop[i],LEN);
	}
	
	printf("---------------- Original Population ---------------\n");
	DumpPop();
	
	for(i=0;i<TNMT;++i)
		GetFitOfTnmt(mostFit,leastFit,i);
		
	strFit=GetMostFit(mostFit);
	while(GetFitness(strFit,LEN) != LEN)
	//while(GetFitness(strFit,LEN)<5)
	{
		printf("\n---------------- Generation %d ----------------\n",cnt);
		DumpPop();

		printf("%s\tReference\n",REF);
		printf("%s\tGen: %d\tFitness: %d\n",strFit,cnt++,GetFitness(strFit,LEN));
		for(i=0;i<TNMT;++i)
		{
			// Crossover
			//CrossoverM2L(mostFit[i],leastFit[i],LEN);
			CrossoverByFit(i);
			
			// Mutation
			p=rand()%(POP/TNMT);
			Mutate(strPop[i*POP/TNMT+p],LEN);

			// Re-calculate the fitness
			GetFitOfTnmt(mostFit,leastFit,i);
		}
		strFit=GetMostFit(mostFit);
	}

	strFit=GetMostFit(mostFit);
	printf("\n---------------- Generation %d ----------------\n",cnt);
	DumpPop();
	printf("%s\tReference\n",REF);
	printf("%s\tGen: %d\tFitness: %d\n",strFit,cnt++,GetFitness(strFit,LEN));

	return 0;
}
