/* 
 Authors : Elisa Idrobo and Warren Seto
 Project : 3
 Purpose : To test the functionality of the scheduler
 */

#include "types.h"
#include "stat.h"
#include "user.h"

#define BIG_NUMBER 0xFFFF

/*  */
int main()
{
    int count, count2, processID, startTime = uptime();

    for (count = 0; count < 20; count++)
    {
        processID = fork();

        if (!processID)
        {
            break;
        }
    }

    int primeCount = 0;
    
    for (count = 1; count <= BIG_NUMBER; count++)
    {
        for (count2 = 2; count2 < count; count2++)
        {
            if (!(count % count2))
            {
                break;
            }
        }
        
        if (count2 == count)
        {
            primeCount++;
        }
    }
    
    printf(1, "END > experiment(%d) > %d primes from [1, %d]\n", getpid(), primeCount, BIG_NUMBER);

    if (processID)
    {
        for (count = 0; count < 20; count++)
        {
            wait();
        }

        printf(1, "Total Elapsed Time: %d ms\n", uptime() - startTime);
    }

    exit();
}
