/*
# PERTECS (Programmable Environment for Real-Time emulation of Continuous Systems) is a suite of tools. At the core of this suite is an executable UNIX tool, configured using the language of analogue computer, to give flexible real-time data processing capabilities
# Copyright (C) 2008,2009  Author, Mark R. Titchener, email mark@tcode.auckland.ac.nz
#
# This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/* routines for computing Realtime  entropy*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "pertecs.h"


 int	Binary = FALSE;
 

double ei(double x) 
{
        void nrerror(char error_text[]);
        int k;
        double fact,prev,sum,term;
                
        if (x <= 0.0) nrerror("Bad argument in ei");
        if (x < FPMIN) return log(x)+EULER;
        if (x <= -log(EPS)) {
                sum=0.0;
                fact=1.0;
                for (k=1;k<=MAXIT;k++) {
                        fact *= x/k;
                        term=fact/k;
                        sum += term;
                        if (term < EPS*sum) break;
                }
                if (k > MAXIT) nrerror("series failed in ei");
                return sum+log(x)+EULER;
        } else {
                sum=0.0;
                term=1.0;
                for (k=1;k<=MAXIT;k++) {
                        prev=term;
                        term *= k/x;
                        if (term < EPS) break;
                        if (term < prev) sum += term;
                        else {
                                sum -= prev;
                                break;
                        }
                }
                return exp(x)*(1.0+sum)/x;
        }
}

void nrerror(char error_text[])
{
        printf("%s\n", error_text);
        exit(1);
}


double logint(double x) {
        return ei(log(x));
}       

double invlogint(double x) {
        double lb, ub, tmp1, tmp2, g1;
        lb = 1.0+EPS;   
        if (x < logint(lb)) {
                nrerror("argument too small for fast algorithm");
        }
        ub = 2.0;
        while (logint(ub) < x) {
                lb = ub;
                ub *= 2;
        }
        tmp1 = logint(lb);
        tmp2 = logint(ub);
        g1 = 1/log(lb);
        /* x is now between logint(lb) and logint(ub) */
        /* printf("lb:%g ub:%g tmp1:%g tmp2:%g g1:%g\n",lb,ub,tmp1,tmp2,g1); */
        while (tmp2-tmp1 > EPS) {       
                /* printf("lb:%g ub:%g tmp1:%g tmp2:%g g1:%g\n",lb,ub,tmp1,tmp2,g1);
                printf("Iteration\n"); */
                ub = (x - tmp1) * (ub - lb)/(tmp2 - tmp1) + lb;
                tmp2 = logint(ub);
                lb += (x - tmp1)/g1;
                tmp1 = logint(lb);
                g1 = 1/log(lb);
        }
        if (Binary) return (ub-.7582221)/log(2.0);
        else return (ub-.7582221);
}

/**********************************************************************************/
/* New quickcalc routine
/*
/**********************************************************************************/


double ftdSelAugment ( char *buffer, long bufflen, MATCHLISTENTRY *EntryList, CODEWORD *PrimeList)
{  

    long Loop, StepCount, HashCodeTemp, Length_SuffixString_ParsedIntoOneCodeword, StepInfoPrintCount;
	long TprefixLength, HashCode_Tprefix, Count_TprefixMatch, TempID, TempEntry;

	char *pc_TprefixAddress;
	CODEWORD *CodeWordBegin, *PrimeListTemp, *RealEndOfPrimeList, *Node_Tprefix, *HeadOfCurrentMatchList;
	CODEWORD *CodeWordInMatchList;
	CODEWORD *PrimeListEnd;
	
	double Complexity;
    long Tprefix_N_Indicate;  
    long NextAvailableEntry; 
    long FirstNewEntry_PerPass;  
	int	K;
	
    Complexity = 0;
    Tprefix_N_Indicate = 256;												// entry reserved for p^k_n
    NextAvailableEntry = 257; 												// points to next available entry
    FirstNewEntry_PerPass = NextAvailableEntry;  							// entry for first new codeword in each pass
    EntryList[Tprefix_N_Indicate].CodeWordID = 0;

    PrimeListEnd = NULL; 


/********* Initialize head node *********/
    PrimeList[0].NextCodeWordInStringList = PrimeList + 1;   
    PrimeList[0].PreviousCodeWordInStringList = NULL;
    
/********* Initialize end node *********/    
    PrimeList[bufflen].NextCodeWordInStringList = NULL;
    PrimeList[bufflen].PreviousCodeWordInStringList = PrimeList + bufflen - 1; 
    
    for (Loop = 0; Loop < 256; Loop++)  									// initialize first 256 entries (0--255)
    {
        EntryList[Loop].PreviousCodeWordInMatchList = (CODEWORD*)(EntryList + Loop); 
        EntryList[Loop].CodeWordID = 0; 
    }
    
    for (Loop= 1; Loop < bufflen ; Loop++)   							// create lists. Pointers in NextCodeWordInMatchList and NextCodeWordInStringList should have same values
    {
        PrimeList[Loop].NextCodeWordInStringList = PrimeList + Loop + 1;
        PrimeList[Loop].PreviousCodeWordInStringList = PrimeList + Loop - 1;
    }
    PrimeListEnd = PrimeList + bufflen;

    StepCount = 1;   													// includes literal character

    PrimeListTemp = PrimeList;
    for (Loop= 0; Loop < bufflen - 1; Loop++)  
    {
        HashCodeTemp = (unsigned char)(buffer[Loop]);  
        PrimeListTemp->NextCodeWordInStringList->CodeWordID = HashCodeTemp;
        
																			/*********** Add to the list **********/
        PrimeListTemp->NextCodeWordInStringList->PreviousCodeWordInMatchList =EntryList[HashCodeTemp].PreviousCodeWordInMatchList;
        PrimeListTemp->NextCodeWordInStringList->NextCodeWordInMatchList = (CODEWORD*)&(EntryList[HashCodeTemp]);
        EntryList[HashCodeTemp].PreviousCodeWordInMatchList->NextCodeWordInMatchList = PrimeListTemp->NextCodeWordInStringList;
        EntryList[HashCodeTemp].PreviousCodeWordInMatchList = PrimeListTemp->NextCodeWordInStringList;
																			/*************************************/

        PrimeListTemp++;
    }
    PrimeListTemp = PrimeList;

    Length_SuffixString_ParsedIntoOneCodeword = 0;   					// compute length of substring (suffix of string) that has been parsed as one codeword after each run
    RealEndOfPrimeList = PrimeListEnd;

    StepInfoPrintCount = 0;   											// To work with StepInfoDisplayInterval

    				/*********** start T-decomposition ***********/

    for (;;)  
    {
        if (PrimeList->NextCodeWordInStringList->NextCodeWordInStringList == NULL) // Check if process has finished (i.e. if these is only one codeword in string)
            break;

        K = 1;   														// count the number of the adjacent codewords that match the T-prerix.
        StepCount++;

         			/*********** (1)Get the T-prefix ********** */
        Node_Tprefix = PrimeListEnd->PreviousCodeWordInStringList;
        pc_TprefixAddress = buffer + (Node_Tprefix - PrimeList) - 1;  
        TprefixLength = RealEndOfPrimeList - Node_Tprefix;
        HashCode_Tprefix = (Node_Tprefix->CodeWordID);
        HeadOfCurrentMatchList = Node_Tprefix->NextCodeWordInMatchList;  /*Point to the dummy head of the hash list.*/

        			/*********** (2)Find how many adjacent copies preceding the T-prefix at the end of the string. So we can got the T-expansion parameter ***********/
        while (Node_Tprefix->PreviousCodeWordInStringList->PreviousCodeWordInStringList != NULL)
        {
            if (Node_Tprefix->PreviousCodeWordInMatchList == Node_Tprefix->PreviousCodeWordInStringList)
            {  
                Node_Tprefix = Node_Tprefix->PreviousCodeWordInStringList;
                K++;
            }
            else
                break;
        }
        RealEndOfPrimeList = Node_Tprefix; 
  
        Node_Tprefix->PreviousCodeWordInMatchList->NextCodeWordInMatchList = HeadOfCurrentMatchList;
        HeadOfCurrentMatchList->PreviousCodeWordInMatchList = Node_Tprefix->PreviousCodeWordInMatchList;
        Node_Tprefix->PreviousCodeWordInStringList->NextCodeWordInStringList = PrimeListEnd;
        PrimeListEnd->PreviousCodeWordInStringList = Node_Tprefix->PreviousCodeWordInStringList;

        Complexity += log((double)(K + 1)); 
        
        			/*********** (3)Search from the beginning of the match list to find the codeword that matches the T-prefix ***********/
        CodeWordInMatchList = HeadOfCurrentMatchList;  /*The head of the match list.*/

        if (CodeWordInMatchList ->NextCodeWordInMatchList == HeadOfCurrentMatchList) 	// if list does not exist
          	continue;
        CodeWordInMatchList = CodeWordInMatchList->NextCodeWordInMatchList;
        Count_TprefixMatch = 0;  													// The number of the adjacent copies of T-prefix found. It will be set to 0 again
                                                                             			// once the new codeword does not mathc the T-prefix
        																				// Now search really begins from begining of corresponding match list
        																				// The match list helps to locate the codeword in the string list
        while (CodeWordInMatchList != HeadOfCurrentMatchList)  
        {
            PrimeListTemp = CodeWordInMatchList;
            Count_TprefixMatch = 1;  													// number of adjacent codewords matching the T-prefix
            CodeWordBegin = PrimeListTemp; 									// The node that pointing to the possible beginning of a new codeword
            PrimeListTemp = PrimeListTemp->NextCodeWordInStringList;

            for (;;)  																	// matched codeword found. Here we need remove some codewords from the match list and string list
                        																// New merged codeword will be added to the corresponding match list. Check in the string list
            {    
                if (HashCode_Tprefix == PrimeListTemp->CodeWordID)
                { 																		// The current codeword DOES match the T-prefix
                    Count_TprefixMatch++;  
                    if (Count_TprefixMatch <= K)
                    {
                        PrimeListTemp = PrimeListTemp->NextCodeWordInStringList;
                    }
                    else  																// number of the adjacent matching codewords exceeds the T-expansion parameter
                    {
                        																// move the pointer to the next code in the match list
                        CodeWordInMatchList = PrimeListTemp->NextCodeWordInMatchList;
                        																// delete the node in the string list
                        CodeWordBegin->NextCodeWordInStringList = PrimeListTemp->NextCodeWordInStringList;
                        PrimeListTemp->NextCodeWordInStringList->PreviousCodeWordInStringList = CodeWordBegin;
                        if ( EntryList[Tprefix_N_Indicate].CodeWordID < FirstNewEntry_PerPass) // no entry for this new codeword
                        {
                            EntryList[Tprefix_N_Indicate].CodeWordID = NextAvailableEntry;
                            TempID = NextAvailableEntry;
                            EntryList[NextAvailableEntry].PreviousCodeWordInMatchList = (CODEWORD*)(EntryList + NextAvailableEntry);
                            EntryList[NextAvailableEntry].CodeWordID= 0;
                            NextAvailableEntry++;
                        }
                        else
                        {
                            TempID = EntryList[Tprefix_N_Indicate].CodeWordID;
                        }
                        CodeWordBegin->CodeWordID = TempID;

																						/*********** Add to the lists ***********/
                        CodeWordBegin->PreviousCodeWordInMatchList =EntryList[TempID].PreviousCodeWordInMatchList;
                        CodeWordBegin->NextCodeWordInMatchList = (CODEWORD*)(&(EntryList[TempID]));
                        EntryList[TempID].PreviousCodeWordInMatchList->NextCodeWordInMatchList = CodeWordBegin;
                        EntryList[TempID].PreviousCodeWordInMatchList = CodeWordBegin;

                        break;
                    }
                }
                else   																	// current codeword does not match the T-prefix
                {
                    TempEntry = PrimeListTemp->CodeWordID;
                    CodeWordInMatchList = PrimeListTemp->PreviousCodeWordInStringList->NextCodeWordInMatchList;
                    for (Loop = 0; Loop < Count_TprefixMatch; Loop++)  					// Remove codewords from string list, the first matched codeword remains
                    {
                        if (EntryList[TempEntry].CodeWordID < FirstNewEntry_PerPass) 	// entry for the new codeword
                        {
                            EntryList[TempEntry].CodeWordID = NextAvailableEntry;
                            TempEntry = NextAvailableEntry;
                            EntryList[TempEntry].PreviousCodeWordInMatchList = (CODEWORD*)(EntryList + TempEntry);
                            EntryList[TempEntry].CodeWordID= 0;
                                
                            NextAvailableEntry++;
                        }
                        else
                        {
                            TempEntry = EntryList[TempEntry].CodeWordID;
                        }
                    }
                    CodeWordBegin->CodeWordID = TempEntry;  							// new ID
                    { 																	// if not the last node in the match list
                        PrimeListTemp->PreviousCodeWordInMatchList->NextCodeWordInMatchList = PrimeListTemp->NextCodeWordInMatchList; 
                        PrimeListTemp->NextCodeWordInMatchList->PreviousCodeWordInMatchList = PrimeListTemp->PreviousCodeWordInMatchList;
                    }
                    
																						/*********** Add to the lists ***********/
                    CodeWordBegin->PreviousCodeWordInMatchList =EntryList[TempEntry].PreviousCodeWordInMatchList;
                    CodeWordBegin->NextCodeWordInMatchList = (CODEWORD*)(&(EntryList[TempEntry]));
                    EntryList[TempEntry].PreviousCodeWordInMatchList->NextCodeWordInMatchList = CodeWordBegin;
                    EntryList[TempEntry].PreviousCodeWordInMatchList = CodeWordBegin;
																						/****************************************/
                    CodeWordBegin->NextCodeWordInStringList = PrimeListTemp->NextCodeWordInStringList;
                    PrimeListTemp->NextCodeWordInStringList->PreviousCodeWordInStringList = CodeWordBegin;
                
                    break;
                }
            }																			// end of for (;;)
        }
        FirstNewEntry_PerPass = NextAvailableEntry;
    }

//	fprintf(stderr," complexity %g\n", Complexity/log(2.0)); fflush(stderr);
    return Complexity/log(2.0);
}


