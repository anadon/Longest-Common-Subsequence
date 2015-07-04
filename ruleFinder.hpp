/*
 * ruleFinder.hpp
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

#ifndef RULE_FINDER_H
#define RULE_FINDER_H


/***********************************************************************
 * STRUCTURES///////////////////////////////////////////////////////////
 * ********************************************************************/
typedef struct point{
	int x, y;
}point;


typedef struct points{
	int numPoints;
	point *array;
}points;

/***********************************************************************
 * FUNCTIONS////////////////////////////////////////////////////////////
 * ********************************************************************/


/***********************************************************************
 * 
 * This function takes 2 sequences and returns a malloc'd array of ints
 * which are indexes of characters in each sequence which are the same
 * such that the indexes tell the longest common subsequence in each of
 * the input sequences in complete detail.
 * 
 * ********************************************************************/
points dynamicSolver(const char* seqOne, const int seqOneLen, 
											const char *seqTwo, const int seqTwoLen);
 
 
#endif