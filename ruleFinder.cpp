/*
 * ruleFinder.c
 * 
 * Copyright 2015 Unknown <anadon@localhost>
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
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "ruleFinder.hpp"

using namespace std;


typedef struct intPair{
	long first;
	long second;
	long depth;
	long numPoints;
	intPair *array;
}intPair;


typedef struct intPairHead{
	long numPoints;
	intPair *array;
	intPair  max;
}intPairHead;


//Used to tell progress
long total, iteration;


void exitWithMessage(const char* msg){
	fprintf(stderr, "%s\n", msg);
	fflush(stderr);
	exit(-1);
}


/***********************************************************************
 * Using the "total" and "iterations" global values, this function runs
 * until total == iterations, updating the percent complete message to 
 * the user in 1 second intervals and provides an estimated time to 
 * completion using the following format:
 * 
 * [<PERCENT>%] Estimated time remaining: <SECONDS> seconds
 * ********************************************************************/
void *reportProgress(void *notUsed){
	
	do{
		fprintf(stderr, "\33[2K\r%lld", iteration);
		fflush(stderr);
	}while(total > 0);
	fprintf(stderr, "\n%lld", iteration);
	fflush(stderr);
	
	pthread_exit(NULL);
}


void recursiveIntPairFree(intPair &toFree){
	for(long i = 0; i < toFree.numPoints; i++){
		recursiveIntPairFree(toFree.array[i]);
		toFree.array[i].array = NULL;
		toFree.array[i].numPoints = 0;
		toFree.array[i].depth = 0;
	}
	free(toFree.array);
	toFree.array = NULL;
}




/***********************************************************************
 * The following section is intended to speed up generation of a list
 * of points for indexes along an axis at a certain other axis value.
 * ********************************************************************/
typedef struct pointListLoopArgs{
	intPair *array;
	long start, end, otherIndex, arrayOffset;
	bool loopOverX;
}pointListLoopArgs;

pointListLoopArgs PointListLoopPThreadPacker(intPair *array, 
								const long start, const long end, const long otherIndex, 
													const long arrayOffset, const bool loopOverX){
	pointListLoopArgs toReturn;
	toReturn->array = array;
	toReturn->start = start;
	toReturn->snd = end;
	toReturn->otherIndex = otherIndex;
	toReturn->arrayOffset = arrayOffset;
	toReturn->loopOverX = loopOverX;
	
	return toReturn;
}

void *PointListLoopPThreadUnpacker(void *args){
	pointListLoopArgs *toSend = (pointListLoopArgs*) args;
	PointListLoop(toSend->array, toSend->start, toSend->end, 
						toSend->otherIndex, toSend->arrayOffset, toSend->loopOverX);
}

void PointListLoop(intPair *array, long start, long end, long otherIndex, 
																			long arrayOffset, bool loopOverX){
	if(loopOverX){
		for(long i = start; i < end; i++){
			toReturn.array[i+arrayOffset].x = i;
			toReturn.array[i+arrayOffset].y = otherIndex;
		}
	}else{
		for(long i = xStart; i < xEnd; i++){
			toReturn.array[i+arrayOffset].x = otherIndex;
			toReturn.array[i+arrayOffset].y = i;
		}
	}
}


/***********************************************************************
 * TODO: document as opposite of topToBottomPointList
 * 
 * NOTE: likely extremely optimisable.
 * ********************************************************************/
intPairHead bottomToTopPointList(const intPair start, const long steps){
	intPairHead toReturn;
	long pointIndex;
	pThread_t xThread, yThread;
	PointListLoopArgs xArgs, yArgs;
	
	if(start.x - steps < 1 || start.y - steps < 1){
		toReturn.numPoints = 0;
		toReturn.array = NULL;
		return toReturn;
	}
	
	pointIndex = 0;
	toReturn.numPoints = (start.x-steps) + (start.y-steps) - 1;
	toReturn.array = (intPair*) malloc(sizeof(intPair) * toReturn.numPoints);
	
	
	xArgs = PointListLoopPThreadPacker(toReturn.array, 0, start.x-steps, 
																								start.y-steps, 0, true);
	pthread_create(&xThread, NULL, PointListLoopPThreadUnpacker, 
																												(void*) &xArgs);
																												
	yArgs = PointListLoopPThreadPacker(toReturn.array, 0, start.y-steps, 
																		start.x-steps, start.x-steps, true);
	pthread_create(&yThread, NULL, PointListLoopPThreadUnpacker, 
																												(void*) &yArgs);
	
	pthread_join(xThread, NULL);
	pthread_join(yThread, NULL);
	
	return toReturn;
}


/***********************************************************************
 * TODO: document as opposite of bottomToTopPointList
 * ********************************************************************/
intPairHead topToBottomPointList(const intPair start, const long steps){
	intPairHead toReturn;
	long pointIndex;
	pThread_t xThread, yThread;
	PointListLoopArgs xArgs, yArgs;
	
	if(start.x + steps > max.x || start.y + steps > max.y){
		toReturn.numPoints = 0;
		toReturn.array = NULL;
		return toReturn;
	}
	
	pointIndex = 0;
	toReturn.numPoints = max.x - (start.x+steps) + max.y - (start.y+steps)
																																		- 1;
	toReturn.array = (intPair*) malloc(sizeof(intPair) * toReturn.numPoints);
	
	
	xArgs = PointListLoopPThreadPacker(toReturn.array, start.x + steps, 
																			max.x+1, start.y+steps, 0, true);
	pthread_create(&xThread, NULL, PointListLoopPThreadUnpacker, 
																												(void*) &xArgs);
																												
	yArgs = PointListLoopPThreadPacker(toReturn.array, start.y+steps, 
								max.y+1, start.x+steps, max.x - (start.x+steps), true);
	pthread_create(&yThread, NULL, PointListLoopPThreadUnpacker, 
																												(void*) &yArgs);
	
	pthread_join(xThread, NULL);
	pthread_join(yThread, NULL);
	
	return toReturn;
}





inline long min(long a, long b) return ( a < b ? a : b );

inline bool canPointXContainPointY(intPair X, intPair Y) return 
																							(X.x < Y.x && X.y < Y.y);




/***********************************************************************
 * 
 * This function takes 2 sequences and returns a malloc'd array of ints
 * which are indexes of characters in each sequence which are the same
 * such that the indexes tell the longest common subsequence in each of
 * the input sequences in complete detail.
 * 
 * ********************************************************************/
intPair* dynamicSolver(const char *base, const long baseLen, 
														const char *other, const long otherLen){
//DECLARATIONS//////////////////////////////////////////////////////////

	
//INITALIZATION/////////////////////////////////////////////////////////


//SETUP PAIRING GRAPH AND SOLVE/////////////////////////////////////////



//FORMAT RETURN/////////////////////////////////////////////////////////


	
	return toReturn;
}


int main(int argc, char **argv){
	
//DECLARATIONS//////////////////////////////////////////////////////////
	
	fprintf(stderr, "Starting substring search\n");
	fflush(stderr);
	
	char *seqOne, *seqTwo;
	FILE *fp;
	long sizeSeqOne, sizeSeqTwo;
	intPair check;
	//pthread_t completionMonitor;
	
//INITIALIZATION////////////////////////////////////////////////////////
	
	fp = fopen(argv[1], "r");
	fseek(fp, 0, SEEK_END);
	sizeSeqOne = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	seqOne = (char*) malloc(sizeof(char) * (sizeSeqOne+1));
	if(seqOne == NULL) exitWithMessage("Failed to allocate \"seqOne\"");
	fread(seqOne, sizeof(char), sizeSeqOne, fp);
	fclose(fp);
	fprintf(stderr, "Read in first sequence\n");
	fflush(stderr);
	
	fp = fopen(argv[2], "r");
	fseek(fp, 0, SEEK_END);
	sizeSeqTwo = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	seqTwo = (char*) malloc(sizeof(char) * (sizeSeqTwo+1));
	if(seqTwo == NULL) exitWithMessage("Failed to allocate \"seqTwo\"");
	fread(seqTwo, sizeof(char), sizeSeqTwo, fp);
	fclose(fp);
	fprintf(stderr, "Read in second sequence\n");
	fflush(stderr);
	
	iteration = 0;
	//pthread_create(&completionMonitor, NULL, reportProgress, NULL);
	
//MAIN LOOP/////////////////////////////////////////////////////////////
	
	check = dynamicSolver(seqOne, sizeSeqOne, seqTwo, sizeSeqTwo);
	
//PRINT RESULTS/////////////////////////////////////////////////////////
	
	//pthread_join(completionMonitor, NULL);
	
	fprintf(stderr, "Search complete\n");
	fflush(stderr);
	
	for(long i = 0; i < check.numPoints;  i++){
		printf("{%lld, %lld}, ", check.array[i].first, check.array[i].second);
	}
	printf("\n");
	
//CLEANUP///////////////////////////////////////////////////////////////
	
	free(seqOne); seqOne = NULL;
	free(seqTwo); seqTwo = NULL;
	free(check.array); check = NULL;
	
	return 0;
}

