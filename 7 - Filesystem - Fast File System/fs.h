#include "param.h"

// On-disk file system format.
// Both the kernel and user programs use this header file.

#define ROOTINO 1  // root i-number
#define BSIZE 512  // block size
#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)

// Project 4 Extra Credit (FFS)
#define INODES_PER_BLOCK CEIL(BSIZE, sizeof(struct dinode))
#define INODE_BLOCKS_PER_BG CEIL(INODES_PER_BG, INODES_PER_BLOCK)
#define INODE_BM_BLOCKS CEIL(INODES_PER_BG / 8, BSIZE)
#define DATA_BM_BLOCKS (1)
#define METABLOCKS (INODE_BLOCKS_PER_BG + INODE_BM_BLOCKS + DATA_BM_BLOCKS)
#define DATA_BLOCKS_PER_BG (BG_SZ - METABLOCKS)
#define DATA_BLOCK_START (BG_START + METABLOCKS)
#define NINODES (INODES_PER_BG * BG_COUNT)
#define NDATA (DATA_BLOCKS_PER_BG * BG_COUNT)
#define BG_IDX(b) (((b)-BG_START)/BG_SZ)
#define DATA_IDX(b) ((b) - ((BG_IDX(b) * BG_SZ) + METABLOCKS + BG_START))
#define GET_INFO_BLOCK(n) ((BG_IDX(n)) * BG_SZ) + BG_START

typedef struct blockgroup {
  uint bg_index;
  uint data_blocks_in_use;
  uint inodes_in_use;
}bg_t;

// Disk layout:
// [ boot block | super block | log | inode blocks | free bit map | data blocks ]
//                                    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ...
//                                                      Block (n)

struct superblock {
  uint size;        // Size of file system image (blocks)
  uint nblocks;     // Number of data blocks
  uint ninodes;     // Number of inodes.
  uint nlog;        // Number of log blocks
  uint logstart;    // Block number of first log block
  uint bg_start;
  uint bg_count;
  uint bg_size;
  uint bg_inodes;
  uint bg_metablocks;
  uint bg_inodeblocks;
  uint bg_datablocks;
};

// On-disk inode structure
struct dinode {
  short type;               // File type
  short major;              // Major device number (T_DEV only)
  short minor;              // Minor device number (T_DEV only)
  short nlink;              // Number of links to inode in file system
  uint size;                // Size of file (bytes)
  uint addrs[NDIRECT + 1];  // Data block addresses
};

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

struct dirent {
  ushort inum;
  char name[DIRSIZ];
};

// Bitmap bits per block
#define BPB (BSIZE * 8)

#define CEIL(a,b) (((a)+(b)-1)/(b))

// Functions that acts like a macros due to code complexity
uint IBLOCK(uint current);
uint BBLOCK(uint current);
uint PREV_DATA_BLOCK(uint current);
uint NEXT_DATA_BLOCK(uint current);
