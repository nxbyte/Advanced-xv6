#define NPROC 64                   // maximum number of processes
#define KSTACKSIZE 4096            // size of per-process kernel stack
#define NCPU 8                     // maximum number of CPUs
#define NOFILE 16                  // open files per process
#define NFILE 100                  // open files per system
#define NINODE 50                  // maximum number of active i-nodes
#define NDEV 10                    // maximum major device number
#define ROOTDEV 1                  // device number of file system root disk
#define MAXARG 32                  // max exec arguments
#define MAXOPBLOCKS 10             // max # of blocks any FS op writes
#define LOGSIZE (MAXOPBLOCKS * 3)  // max data blocks in on-disk log
#define NBUF (MAXOPBLOCKS * 3)     // size of disk block cache


// Added for Project 4 (FFS) Extra Credit
#define BG_START (2 + LOGSIZE)     // block group start offset
#define BG_SZ 4096                 // block group size
#define BG_COUNT 40                // block group count
#define INODES_PER_BG 32           // inodes per each block group
#define FSSIZE BG_SZ * BG_COUNT + BG_START // size of file system in blocks
