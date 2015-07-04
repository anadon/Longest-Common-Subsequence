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

using namespace std;


/***********************************************************************
 * FUNCTIONS////////////////////////////////////////////////////////////
 * ********************************************************************/


char* readFileAsCString(const char* fileName, int &fileLength){
	FILE *fp;
	char *toReturn;
	//printf("Reading in %s\n",  fileName);  fflush(stdout);
																												
	fp = fopen(fileName, "r");
	fseek(fp, 0, SEEK_END);
	fileLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	toReturn = (char*) malloc(sizeof(char) * (fileLength+1));
	if(toReturn != NULL)
		fread(toReturn, sizeof(char), fileLength, fp);
	fclose(fp);
	return toReturn;
}


int main(int argc, char **argv){
//DECLARATIONS//////////////////////////////////////////////////////////	
	char *seqOne, *seqTwo;
	int seqOneLen, seqTwoLen;
	points check;
	//pthread_t completionMonitor;
	
//INITIALIZATION////////////////////////////////////////////////////////
	seqOne = seqTwo = NULL;

	seqOne = readFileAsCString(argv[1], seqOneLen);
	if(seqOneLen == 0){
		fprintf(stderr, "Aborting: first file is empty\n");
		exit(-EIO);
	}
	
	seqTwo = readFileAsCString(argv[2], seqTwoLen);
	if(seqTwoLen == 0){
		fprintf(stderr, "Aborting: second file is empty\n");
		exit(-EIO);
	}
	
//MAIN LOOP/////////////////////////////////////////////////////////////
	
	check = dynamicSolver(seqOne, seqOneLen, seqTwo, seqTwoLen);
	
//PRINT RESULTS/////////////////////////////////////////////////////////
	
	for(long i = 0; i < check.numPoints;  i++){
		printf("{%d, %d}, ", check.array[i].x, check.array[i].y);
	}
	
//CLEANUP///////////////////////////////////////////////////////////////
	
	free(seqOne);
	free(seqTwo);
	free(check.array);
	
	return 0;
}
