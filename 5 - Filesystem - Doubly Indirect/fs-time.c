/*
 Authors : Elisa Idrobo and Warren Seto
 Purpose : To test the performance of a filesystem
 */

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

// Macro for the size of a file
#define FILE_SIZE 0x1<<6  // 64b


// Data structure for test file
typedef struct {
    char* name;
    int size;
} TestFile;


int main(int argc, char *argv[])
{
    char* string = "The Quick Brown Fox Jumped Over the Lazy Dog.";
    
    TestFile files[] = {
        {"file1.txt", FILE_SIZE},
        {"file2.txt", FILE_SIZE},
        {"file3.txt", FILE_SIZE},
        {"file4.txt", FILE_SIZE},
        {"file5.txt", FILE_SIZE},
        {"file6.txt", FILE_SIZE},
        {"file7.txt", FILE_SIZE},
        {"file8.txt", FILE_SIZE},
        {"file9.txt", FILE_SIZE},
        {"file10.txt", FILE_SIZE}
    };
    
    char* dirNames[] = { "blah-1", "blah-2", "blah-3", "blah-4", "blah-5", "blah-6", "blah-7", "blah-8", "blah-9", "blah-10" };
    
    int count, count2, count3, numWrites, fd, unlink_ret, startTime = uptime();
    
    /* Make 10 directories */
    for (count = 0; count < 10; count++)
    {
        /* Make a directory */
        printf(1, "Create > /%s\n", dirNames[count]);
        
        if (mkdir(dirNames[count]) < 0)
        {
            printf(2, "Error > /%s\n", dirNames[count]);
            return 1;
        }
        
        /* Select the directory */
        if (chdir(dirNames[count]) < 0)
        {
            printf(2, "Error > Cannot cd into /%s\n", dirNames[count]);
            return 1;
        }
    
        /* Create 10 files */
        for (count2 = 0; count2 < 10; count2++)
        {
            /* Create a file */
            printf(1, "Create > /%s/%s\n", dirNames[count], files[count2].name);
            
            if ((fd = open(files[count2].name, O_CREATE | O_WRONLY)) > 0)
            {
                /* Write the string into the file */
                numWrites = files[count2].size / sizeof(string);
                
                for (count3 = 0; count3 < numWrites; count3++)
                {
                    if (write(fd, string, sizeof(string)) != sizeof(string))
                    {
                        printf(2, "Error > Cannot write to /%s/%s\n", dirNames[count], files[count2].name);
                        return 1;
                    }
                }
                
                printf(2, "Wrote > /%s/%s = %d bytes\n", dirNames[count], files[count2].name, files[count2].size);
                
                /* Stop writing and delete the file */
                close(fd);
                
                if (unlink(files[count2].name) < 0)
                {
                    printf(2, "Error > Cannot remove file /%s/%s\n", dirNames[count], files[count2].name);
                    return 1;
                }
            }
            
            else
            {
                printf(2, "Error > Cannot create file: /%s/%s, fd: %d\n", dirNames[count], files[count2].name, fd);
                return 1;
            }
        }
        
        /* Moves to root directory */
        if (chdir("/") < 0)
        {
            printf(2, "Error > Cannot cd into /%s\n", dirNames[count]);
            return 1;
        }
        
        /* Remove temp Directory */
        printf(2, "Remove > /%s\n", dirNames[count]);
        
        if ((unlink_ret = unlink(dirNames[count])) != 0)
        {
            printf(2, "Error > Cannot remove directory: /%s, unlink: %d\n", dirNames[count], unlink_ret);
            return 1;
        }
    }
    
    printf(1, "Total Elapsed Time: %d ms\n", uptime() - startTime);
    
    exit();
}
