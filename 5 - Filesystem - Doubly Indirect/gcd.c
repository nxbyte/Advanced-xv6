//
//  main.c
//  Labs
//
//  Created by Warren Seto on 1/23/17.
//  Copyright © 2017 Warren Seto. All rights reserved.
//

#include "types.h"
#include "stat.h"
#include "user.h"

void outputPrimeFactor(int* input);
int pid;
int main(int argc, const char * argv[])
{
    // Declare an integer to find its prime factors
	int numChildren =3;

	int forkReturn;
	for(int i = 1; i <=numChildren; i++){
		//create children
			forkReturn = fork();
			if(forkReturn == 0){//child
				break;//do not create more children
			}
	}
	int startTime = uptime();
	pid = getpid();
	int num = 30000000;//suitably long enough to see effects of round robin scheduler
    printf(2, "\nGCD(%d) The prime factorization of %d = \n", pid,num);

    // Call the outputPrimeFactor procedure with the address of the desired integer
    outputPrimeFactor(&num);
	if(forkReturn > 0){
		for(int i = 1; i <=numChildren;i++)
			wait();
	}
    printf(2, "\nGCD(%d) finished.\n",pid);
	int endTime = uptime();
	printf(2, "\nGCD(%d) time: %d.\n",pid, endTime-startTime);

    exit();
}

/** Displays the prime factors for a given integer input */
void outputPrimeFactor(int* input)
{
    // Counter variable to conrol the loops
    int count;

    //"For" loop for finding the prime factors
    for (count = 2; count <= (*input); count++) //This will count from 2 to the inputted number
    {
        while ((*input) % count == 0) //It will check if the inputted number and count is divisible, then loop the statement if is true
        {
            (*input) = (*input) / count; //This will divide the number by count

            if ((*input) / count >= 1) //This will add the time symbol, when the expression is greater to or equal to one
            {
                printf(2, "GCD(%d): %d x ", pid, count);
            }

            else //If (num / count < 0) is true, then show the last number in the statement
            {
                printf(2, "GCD(%d): %d x ", pid, count);
            }
        }
    }
}
