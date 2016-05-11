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


// Header for Genetic Algorithm module
#ifndef __GA_H__
#define __GA_H__

#define DEF_POPULATION_SIZE		64
#define DEF_VAR_DIMENSION		6

#define EPSILON_AVOID_DIV_0     1e-10

#define GA_OK			   0
#define GA_ERR_OVER_SIZE   1

typedef struct
{
	unsigned int nPopSize;
	unsigned int nDimVar;
	double	fPerCrossOver;
	double  fEpsilonStopVar;
	double  fMutionPercent;
}GA_CONFIG;

typedef struct
{
	double fKey;
	int idx;
}ELEMENT_SORT;


extern void sort_double_by_insert(double *afInput, unsigned int nSize, double *afOutput, unsigned int *aIdxSort);
extern void sort_insert(ELEMENT_SORT *pInput, ELEMENT_SORT *pOutput, unsigned int nSize);

typedef struct
{
	double *afMinX;
	double *afMaxX;
}GA_INPUT_BOUND_DBL;

typedef struct
{
	double matVolPopulationCurrGen[DEF_POPULATION_SIZE][DEF_VAR_DIMENSION];
	double afBestVar[DEF_VAR_DIMENSION];
	double aFitnessCurrGen[DEF_POPULATION_SIZE];
	double aSortedFitnessCurrGen[DEF_POPULATION_SIZE];
	unsigned int aidxSortedFitnessCurrGen[DEF_POPULATION_SIZE];
}GA_GEN_CURR_POP_INFO;

// 	double matVolPopulationNextGen[DEF_POPULATION_SIZE][DEF_VAR_DIMENSION];

// extern short ga_init_cfg_bound(GA_CONFIG *stpGA_Config);
void ga_transfer_minobj_fitness(double *afObjToMin, GA_CONFIG *stpGA_Config);

class GA_FloatingPoint :  public CObject
{
	DECLARE_DYNAMIC(GA_FloatingPoint)

public:
	short ga_init_cfg_bound(GA_CONFIG *stpGA_Config, GA_INPUT_BOUND_DBL *stpGA_Input);
	void ge_init_popu_1st_gen(double *afInitVariable);
	void ga_transfer_minobj_fitness(double *afObjToMin);
	void ga_prod_next_gen_float();

protected:

	GA_CONFIG stGA_Config;
	GA_INPUT_BOUND_DBL stGA_Input;
	double matVolPopulationCurrGen[DEF_POPULATION_SIZE][DEF_VAR_DIMENSION];
	double matVolPopulationNextGen[DEF_POPULATION_SIZE][DEF_VAR_DIMENSION];
	double aFitnessCurrGen[DEF_POPULATION_SIZE];
	double aSortedFitnessCurrGen[DEF_POPULATION_SIZE];
	unsigned int aidxSortedFitnessCurrGen[DEF_POPULATION_SIZE];

	double afBestVar[DEF_VAR_DIMENSION];
//	double afMinX[DEF_VAR_DIMENSION];
//	double afMaxX[DEF_VAR_DIMENSION];

};


#endif // __GA_H__