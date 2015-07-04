/*
 * ruleFinder.cpp
 * 
 * Copyright 2015 Josh Marshall <jrmarsha@mtu.edu>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * This program is a solution to the Longest Common Substring problem
 * using a dynamic approach which has been parallelized with pthreads.
 */

/***********************************************************************
 * INCLUDES/////////////////////////////////////////////////////////////
 * ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "ruleFinder.hpp"
#include "lcs.hpp"

using namespace std;


/***********************************************************************
 * STRUCTURES///////////////////////////////////////////////////////////
 * ********************************************************************/
 
typedef struct intPair{
	int first;
	int second;
	int depth;
	pthread_mutex_t initializedMutex;
	intPair *array;
	pthread_t *myThread;
}intPair;


typedef struct LCSGlobal{
	const char *seqOne, *seqTwo;
	const int seqOneLen, seqTwoLen;
}LCSGlobal;


typedef struct pointWorkerArgs{
	intPair **grid;
	int x, y;
	const LCSGlobal *pseudoGlobal;
}pointWorkerArgs;


/***********************************************************************
 * FUNCTIONS////////////////////////////////////////////////////////////
 * ********************************************************************/


void* pointWorkerArgPacker(intPair **inGrid, int x, int y, 
																				const LCSGlobal *pseudoGlobal){
	pointWorkerArgs *toReturn, tmp;
	
	//The following is safely handled by pointWorkerWrapper IF AND ONLY IF
	//the allocated memory is sent directly to pointWorkerWrapper, else
	//MEMORY LEAKS AND ERRORS WILL OCCUR.
	toReturn = (pointWorkerArgs*) malloc(sizeof(pointWorkerArgs));
	
	tmp = {inGrid, x, y, pseudoGlobal};
	memcpy(toReturn, &tmp, sizeof(pointWorkerArgs));
	
	return toReturn;
}


void *pointWorkerWrapper(void *args){
	pointWorkerArgs *toSend;
	
	toSend = (pointWorkerArgs*) args;
	
	pointWorker(toSend->grid, toSend->x, toSend->y, toSend->pseudoGlobal);
	free(args);
	pthread_exit(NULL);
}


void pointWorker(intPair **grid, int x, int y, 
																				const LCSGlobal *pseudoGlobal){

//SATISFY DEPENDANCIES//////////////////////////////////////////////////
	if(y - 1 >= 0){
		pthread_mutex_lock(&grid[x][y - 1].initializedMutex);
		pthread_mutex_unlock(&grid[x][y - 1].initializedMutex);
	}
	
//MODIFY LOCAL//////////////////////////////////////////////////////////
	if(pseudoGlobal->seqOne[x] == pseudoGlobal->seqTwo[y]){
		if(x - 1 < 0 || y - 1 < 0){
			grid[x][y].depth = 1;
		}else{
			grid[x][y].depth = grid[x - 1][y - 1].depth + 1;
			grid[x][y].array = &grid[x - 1][y - 1];
		}
	}else{
		if(x - 1 >= 0){
			grid[x][y].depth = grid[x - 1][y].depth;
			grid[x][y].array = grid[x - 1][y].array;
		}
		if(y - 1 >= 0 && grid[x][y - 1].depth > grid[x][y].depth){
			grid[x][y].depth = grid[x][y - 1].depth;
			grid[x][y].array = grid[x][y - 1].array;				
		}
	}
	
	pthread_mutex_unlock(&grid[x][y].initializedMutex);
//START OTHERS//////////////////////////////////////////////////////////

	if(x == 0 && y + 1 < pseudoGlobal->seqTwoLen){
		grid[x][y+1].myThread = (pthread_t*) malloc(sizeof(pthread_t));
		pthread_create(grid[x][y+1].myThread, NULL, pointWorkerWrapper, 
					pointWorkerArgPacker(grid, x, y+1, pseudoGlobal));
	}
	
	if(x + 1 < pseudoGlobal->seqOneLen){
		pointWorker(grid, x+1, y, pseudoGlobal);
	}	
}


points dynamicSolver(const char* seqOne, const int seqOneLen, 
											const char *seqTwo, const int seqTwoLen){
//DECLARATIONS//////////////////////////////////////////////////////////
	intPair **grid;
	points toReturn;
	//LCSGlobal pseudoGlobal;
	
	int i, j;
	
//INITALIZATION/////////////////////////////////////////////////////////

	LCSGlobal pseudoGlobal = {seqOne, seqTwo, seqOneLen, seqTwoLen};


	grid = (intPair**) malloc(sizeof(intPair*) * seqOneLen);
	for(i = 0; i < seqOneLen; i++){
		grid[i] = (intPair*) malloc(sizeof(intPair) * seqTwoLen);
		for(j = 0; j < seqTwoLen; j++){
			grid[i][j].first = i;
			grid[i][j].second = j;
			grid[i][j].depth = 0;
			pthread_mutex_init(&grid[i][j].initializedMutex, NULL);
			pthread_mutex_lock(&grid[i][j].initializedMutex);
			grid[i][j].array = NULL;
			grid[i][j].myThread = NULL;
		}
	}

//SOLVE/////////////////////////////////////////////////////////////////

	pointWorker(grid, 0, 0, &pseudoGlobal);
	
	for(i = 0; i < seqOneLen; i++)
		for(j = 0; j < seqTwoLen; j++)
			if(grid[i][j].myThread != NULL)
				pthread_join(*grid[i][j].myThread, NULL);
	
//FORMAT RETURN/////////////////////////////////////////////////////////
	//At this point, a solution is in grid[sizeSeqOne-1][sizeSeqTwo-1]

	toReturn.numPoints = grid[seqOneLen - 1][seqTwoLen - 1].depth;
	toReturn.array = (point*) malloc(sizeof(point) * toReturn.numPoints);
	i = toReturn.numPoints-1;
	for (intPair *itr = &(grid[seqOneLen - 1][seqTwoLen - 1]); 
																				itr != NULL; itr = itr->array){
		toReturn.array[i].x = itr->first;
		toReturn.array[i].y = itr->second;
		i--;
	}

//CLEANUP///////////////////////////////////////////////////////////////
	
	for(i = 0; i < seqOneLen; i++){
		for(j = 0; j < seqTwoLen; j++){
			if(grid[i][j].myThread != NULL){
				free(grid[i][j].myThread);
			}
		}
		free(grid[i]);
	}
	free(grid);

	
	return toReturn;
}

