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


typedef struct intPair intPair;
typedef struct intPairHead intPairHead;


void exitWithMessage(const char* msg);


/***********************************************************************
 * Using the "total" and "iterations" global values, this function runs
 * until total == iterations, updating the percent complete message to 
 * the user in 1 second intervals and provides an estimated time to 
 * completion using the following format:
 * 
 * [<PERCENT>%] Estimated time remaining: <SECONDS> seconds
 * ********************************************************************/
void *reportProgress(void *notUsed);


void recursiveIntPairFree(intPair &toFree);


void *recursiveFormLinksPthreadEntry(void *input);


/***********************************************************************
 * Given an array of pairs, they need to be ordered such that each in 
 * a given level (head) cannot exist as part of another pair.  This
 * means that each pair that is considered a head if it does not have
 * both of its indexes greater than another pair.  each is then loaded 
 * with kinks which is has smaller indexes in both sequences.  When 
 * applied recursively, a subsequence hierarchy relevant graph is made.
 * ********************************************************************/
void recursiveFormLinks(intPair &point);


/***********************************************************************
 * 
 * This function takes 2 sequences and returns a malloc'd array of ints
 * which are indexes of characters in each sequence which are the same
 * such that the indexes tell the longest common subsequence in each of
 * the input sequences in complete detail.
 * 
 * ********************************************************************/
intPair** substringMappings(const char *base, const long long baseLen, 
														const char *other, const long long otherLen);



#endif