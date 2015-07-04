#ifndef LCS_HPP
#define LCS_HPP

/***********************************************************************
 * STRUCTURES///////////////////////////////////////////////////////////
 * ********************************************************************/
 
typedef struct intPair intPair;

typedef struct LCSGlobal LCSGlobal;

typedef struct pointWorkerArgs pointWorkerArgs;

/***********************************************************************
 * FUNCTIONS////////////////////////////////////////////////////////////
 * ********************************************************************/

/***********************************************************************
 * Pack arguments so that they are easier to pass when creating a 
 * pthread to run pointWorkerWrapper()
 * ********************************************************************/
void* pointWorkerArgPacker(intPair **inGrid, int x, int y, 
																				const LCSGlobal *pseudoGlobal);


/***********************************************************************
 * A wrapper around pointWorker() made to make pthreads easier to work.
 * ********************************************************************/
void *pointWorkerWrapper(void *args);


/***********************************************************************
 * Solve for the intPair self.
 * ********************************************************************/
void pointWorker(intPair **grid, int x, int y, 
																				const LCSGlobal *pseudoGlobal);


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