//The MIT License (MIT)
//
//Copyright (c) 2016 ZHAOZhengyi-tgx
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
// (c)All right reserved, sg.LongRenE@gmail.com


#include "stdafx.h"

#include "stdlib.h"
#include "stdio.h"
//#include "WinBase.h"
#include "math.h"
#include "GA.h"

// to resolved externals
IMPLEMENT_DYNAMIC(GA_FloatingPoint, CObject)

short GA_FloatingPoint::ga_init_cfg_bound(GA_CONFIG *stpGA_Config, GA_INPUT_BOUND_DBL *stpGA_Input)
{
	short sRet = GA_OK;
	if(stpGA_Config->nPopSize > DEF_POPULATION_SIZE || stpGA_Config->nDimVar > DEF_VAR_DIMENSION)
	{
		sRet = GA_ERR_OVER_SIZE;
	}

	stGA_Config.fPerCrossOver = stpGA_Config->fPerCrossOver;
	stGA_Config.nDimVar = stpGA_Config->nDimVar;
	stGA_Config.nPopSize = stpGA_Config->nPopSize;
	stGA_Config.fEpsilonStopVar = stpGA_Config->fEpsilonStopVar;

	for(unsigned int ii = 0; ii<stGA_Config.nPopSize; ii++)
	{
		stGA_Input.afMaxX[ii] = stpGA_Input->afMaxX[ii];
		stGA_Input.afMinX[ii] = stpGA_Input->afMinX[ii];
	}

	return sRet;
}


//// function [aFitnessCurrGen, Oderfi,Indexfi] = ga_transfer_minobj_fitness(afObjToMin, stGA_Config);
//// Transform the list of obj (to be minimized) to a list of GA-fitness
// afObjToMin: an array of nPopSize, obj value list to be minimized
// stGA_Config, stGA_Config.nPopSize
void GA_FloatingPoint::ga_transfer_minobj_fitness(double *afObjToMin)
{
	for(unsigned int ii = 1; ii< stGA_Config.nPopSize; ii++)
	{
	   aFitnessCurrGen[ii]= 1.0/(afObjToMin[ii] + EPSILON_AVOID_DIV_0);   //// change minimizing to maximizing
	}

//	[Oderfi,Indexfi]=sort(fi);    //Arranging fi small to bigger
	sort_double_by_insert(aFitnessCurrGen, stGA_Config.nPopSize, aSortedFitnessCurrGen, aidxSortedFitnessCurrGen);
}


void sort_insert(ELEMENT_SORT *pInput, ELEMENT_SORT *pOutput, unsigned int nSize)
{
	ELEMENT_SORT stTemp;
	unsigned ii, jj;
	for(ii = 0; ii < nSize; ii++)
	{
		pOutput[ii] = pInput[ii];
	}
	for(ii = 2; ii < nSize; ++ii)
	{
		stTemp = pOutput[ii]; // pOutput[0]
		jj = ii - 1;
		while(stTemp.fKey < pOutput[jj].fKey)
		{
			pOutput[jj + 1] = pOutput[jj];
			jj --;
		}
		pOutput[jj + 1] = stTemp;
	}
}

ELEMENT_SORT astSortInput[DEF_POPULATION_SIZE];
ELEMENT_SORT astSortOutput[DEF_POPULATION_SIZE];
void sort_double_by_insert(double *afInput, unsigned int nSize, double *afOutput, unsigned int *aIdxSort)
{
	ELEMENT_SORT *pstSortInput, *pstSortOutput;
	static unsigned int ii;
	if(nSize > DEF_POPULATION_SIZE)
	{
		pstSortInput = (ELEMENT_SORT * )calloc(nSize, sizeof(ELEMENT_SORT));
		pstSortOutput = (ELEMENT_SORT * )calloc(nSize, sizeof(ELEMENT_SORT));
	}
	else
	{
		pstSortInput = &astSortInput[0];
		pstSortOutput = &astSortOutput[0];
	}

	for(ii = 0; ii<nSize; ii++)
	{
		pstSortInput[ii].fKey = afInput[ii];
		pstSortInput[ii].idx = ii;
	}

	sort_insert(pstSortInput, pstSortOutput, nSize);

	for(ii = 0; ii<nSize; ii++)
	{
		afOutput[ii] = pstSortOutput[ii].fKey;
		aIdxSort[ii] = pstSortOutput[ii].idx;
	}

	if(nSize > DEF_POPULATION_SIZE)
	{
		delete(pstSortInput);   //  = (ELEMENT_SORT * )calloc(nSize, sizeof(ELEMENT_SORT));
		delete(pstSortOutput);  // = (ELEMENT_SORT * )calloc(nSize, sizeof(ELEMENT_SORT));
	}

}

double fi_Size[DEF_POPULATION_SIZE];
double fi_S[DEF_POPULATION_SIZE];
double Rest[DEF_POPULATION_SIZE];
double fSortRestValue[DEF_POPULATION_SIZE];
unsigned int idxRestSort[DEF_POPULATION_SIZE];
double Pm[DEF_POPULATION_SIZE];
double Pm_rand[DEF_POPULATION_SIZE][DEF_VAR_DIMENSION];
double Mean[DEF_VAR_DIMENSION];
double Dif[DEF_VAR_DIMENSION];

// Generate next population by genetic algorithm
void GA_FloatingPoint::ga_prod_next_gen_float()//GA_GEN_CURR_POP_INFO *stpGA_GenPopInfo)// double **afPopulationNextGen
{
unsigned int ii, jj;

/****** Step 2 : Select and Reproduct Operation******/
	double fi_sum = 0, fi_S_sum = 0, r;
   //fi_sum=sum(aFitnessCurrGen);
	for(ii=0; ii<stGA_Config.nPopSize; ii++)
	{
		fi_sum += aFitnessCurrGen[ii];
	}

	for(ii=0; ii<stGA_Config.nPopSize; ii++)
	{
		fi_Size[ii] = (aSortedFitnessCurrGen[ii]/fi_sum) * stGA_Config.nPopSize;
	    fi_S[ii] = floor(fi_Size[ii]);                    // Selecting Bigger aFitnessCurrGen value
		fi_S_sum += fi_S[ii];
		Rest[ii] = fi_Size[ii] - fi_S[ii]; // 	Rest = fi_Size - fi_S;
	}
	
	r = stGA_Config.nPopSize - fi_S_sum;
//	[fSortRestValue,idxRestSort]=sort(Rest);
	sort_double_by_insert(Rest, stGA_Config.nPopSize, fSortRestValue, idxRestSort);
	
	for(ii =stGA_Config.nPopSize - 1; ii >= stGA_Config.nPopSize-r; ii--) // 1:+1
	{
		fi_S[idxRestSort[ii]]= fi_S[idxRestSort[ii]] + 1;     // Adding rest to equal stGA_Config.nPopSize
	}

unsigned int   kk=0, dd;
   for(ii = stGA_Config.nPopSize -1; ii >=0; ii--) // i=stGA_Config.nPopSize:-1:1       // Select the Sizeth and Reproduce firstly  
   {
      for(jj = 0; jj<fi_S[ii]; jj++ ) //jj = 1:1:fi_S(i)  
	  {
		  for(dd=0; dd<stGA_Config.nDimVar; dd++)
		  {
			  matVolPopulationNextGen[kk][dd] = matVolPopulationCurrGen[aidxSortedFitnessCurrGen[ii]][dd];      // Select and Reproduce 
			  kk = kk+1;                            // k is used to reproduce
		  }
      }
   }
double dTemp, alfa;
//************ Step 3 : Crossover Operation ************, fPerCrossOver;
    for(ii=0; ii<stGA_Config.nPopSize-1; ii= ii+2)  //i=1:2:(stGA_Config.nPopSize-1)
	{
		dTemp = ((double)rand())/ RAND_MAX;
		if( stGA_Config.fPerCrossOver > dTemp )                     //Crossover Condition
		{
			alfa = (double)rand()/ RAND_MAX;
			for(dd = 0; dd<stGA_Config.nDimVar; dd++)
			{
				matVolPopulationNextGen[ii][dd] = alfa * matVolPopulationCurrGen[ii+1][ dd] + (1-alfa) * matVolPopulationCurrGen[ii][dd];  
				matVolPopulationNextGen[ii+1][dd] = alfa * matVolPopulationCurrGen[ii][dd] + (1-alfa) * matVolPopulationCurrGen[ii+1][dd];
			}
		}
    }
	for(dd = 0; dd<stGA_Config.nDimVar; dd++)
	{
	    matVolPopulationNextGen[stGA_Config.nPopSize][dd] = afBestVar[dd];
	}
	for(ii = 0; ii<stGA_Config.nPopSize-1; ii++)
	{
		for(dd = 0; dd<stGA_Config.nDimVar; dd++)
		{
			matVolPopulationCurrGen[ii][dd] = matVolPopulationNextGen[ii][dd];
		}
	}
//************ Step 4: Mutation Operation **************
//	double Pm;
	for(ii = 0; ii<stGA_Config.nPopSize-1; ii++)
	{
		Pm[ii] = 0.10 - (ii *  0.01)/stGA_Config.nPopSize;       //Bigger aFitnessCurrGen,smaller Pm
	}

	for(ii = 0; ii<stGA_Config.nPopSize-1; ii++)
	{
		for(dd=0; dd<stGA_Config.nDimVar; dd++)
		{
			Pm_rand[ii][dd] = (double)rand()/ RAND_MAX; //stGA_Config.nPopSize, stGA_Config.nDimVar);
		}
	}
	
	for(dd = 0; dd<stGA_Config.nDimVar; dd++)
	{
		Mean[dd] = (stGA_Input.afMaxX[dd] + stGA_Input.afMinX[dd])/2; 
		Dif[dd] = (stGA_Input.afMinX[dd] - stGA_Input.afMinX[dd]);
	}
	
	for(ii = 0; ii < stGA_Config.nPopSize; ii++) // i=1:1:stGA_Config.nPopSize
	{
		for(jj = 1; jj<stGA_Config.nDimVar; jj++) // j=1:1:nDimVar
		{
			if( Pm[ii] > Pm_rand[ii][jj] )        //Mutation Condition
			{
				matVolPopulationNextGen[ii][jj] = Mean[jj] + Dif[jj] * ( (double)rand()/ RAND_MAX - 0.5);
			}
		}
	}

//Guarantee TempE(stGA_Config.nPopSize,:) belong to the best individual

	for(dd = 0; dd < stGA_Config.nDimVar; dd++)
	{
		matVolPopulationNextGen[stGA_Config.nPopSize][dd]=afBestVar[dd];      
	}
}
// #define __TEST_MAIN__

#ifdef __TEST_MAIN__ 

#define POP_SIZE   20
#define FILE_OUT   "DebugSort.txt"
ELEMENT_SORT stPopSortIn[POP_SIZE], stPopSortOut[POP_SIZE];
double fSortIn[POP_SIZE], fSortOut[POP_SIZE];
unsigned int aidxSort[POP_SIZE];
void main()
{
unsigned int ii, nPopSize = POP_SIZE;


	srand(__TIME__[0]);
	for(ii = 0; ii<nPopSize; ii++)
	{
		fSortIn[ii] = (double)rand();
		stPopSortIn[ii].fKey = fSortIn[ii];
		stPopSortIn[ii].idx = ii;
	}

	sort_insert(&stPopSortIn[0], &stPopSortOut[0], nPopSize);

	sort_double_by_insert(fSortIn, nPopSize, fSortOut, aidxSort);

FILE *fptr;
	fopen_s(&fptr, FILE_OUT, "w");

	fprintf(fptr, "Before Sorting: \n");

	for(ii = 0; ii<nPopSize; ii++)
	{
		fprintf(fptr, "%5.1f\n", stPopSortIn[ii].fKey);
	}

	fprintf(fptr, "\n\nAfter Sorting: \nKeyVal, Idx\n");

	for(ii = 0; ii<nPopSize; ii++)
	{
		fprintf(fptr, "%5.1f, %d\n", stPopSortOut[ii].fKey, stPopSortOut[ii].idx);
	}

	fprintf(fptr, "\n\nDirect Input Double AfterSorting: \nKeyVal, Idx\n");

	for(ii = 0; ii<nPopSize; ii++)
	{
		fprintf(fptr, "%5.1f, %d\n", fSortOut[ii], aidxSort[ii]);
	}
	fclose(fptr);

}
#endif // __TEST_MAIN__