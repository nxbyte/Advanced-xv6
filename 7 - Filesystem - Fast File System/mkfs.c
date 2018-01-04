#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#define stat xv6_stat  // avoid clash with host struct stat
#include "types.h"
#include "fs.h"
#include "stat.h"
#include "param.h"

#ifndef static_assert
#define static_assert(a, b) do {switch (0) case 0 : case (a) :; } while (0)
#endif

#define min(a, b) ((a) < (b) ? (a) : (b))

int nbitmap = DATA_BM_BLOCKS * BG_COUNT;
int ninodeblocks = INODES_PER_BG * BG_COUNT;

// Number of meta blocks (boot, sb, nlog, inode, bitmap)
int nmeta = BG_START;

// Extra fields for FFS Extra Credit
int fsfd;
struct superblock sb;
char zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;
uint last_inum = 0;

// Function Prototypes for FFS Extra Credit
uint balloc();
void wsect(uint, void *);
void winode(uint, struct dinode *);
void rinode(uint inum, struct dinode *ip);
void rsect(uint sec, void *buf);
uint ialloc(ushort type);
void iappend(uint inum, void *p, int n);

// convert to intel byte order
ushort
xshort(ushort x) {
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  return y;
}

uint
xint(uint x) {
  uint y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  a[2] = x >> 16;
  a[3] = x >> 24;
  return y;
}

// Calculate the location for each consecutive block after th superblock
inline
uint IBLOCK(uint current) {
  uint start = ((current-1) / INODES_PER_BG) * BG_SZ;
  uint inode_in_bg = (current-1) % INODES_PER_BG;
  uint block_in_bg = inode_in_bg / INODES_PER_BLOCK;
  return BG_START + start + INODE_BM_BLOCKS + block_in_bg;
}

inline
uint BBLOCK(uint current) {
  uint bg_index = BG_IDX(current);
  uint block_in_bg = current - ((bg_index * BG_SZ) + BG_START + METABLOCKS);
  uint bm_start = (bg_index * BG_SZ) + BG_START + INODE_BM_BLOCKS + INODE_BLOCKS_PER_BG;
  return bm_start + (block_in_bg) / BSIZE;
}

inline
uint PREV_DATA_BLOCK(uint current) {
  current -= BG_START;
    
  if(((current - METABLOCKS) % BG_SZ) == 0)
    current -= (METABLOCKS - 1);
  else
    current--;

  current += BG_START;

  return current;
}

inline
uint NEXT_DATA_BLOCK(uint current) {
  current -= BG_START;
  current++;

  if((current % BG_SZ) == 0)
    current += METABLOCKS;

  current += BG_START;

  return current;
}

int
main(int argc, char *argv[])
{

  int i, bytes_read, fd;
  uint rootino, inum, off;
  struct dirent de;
  char buf[BSIZE];
  struct dinode din;

  static_assert(sizeof(int) == 4, "Integers must be 4 bytes!");

  if (argc < 2) {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  assert((BSIZE % sizeof(struct dinode)) == 0);
  assert((BSIZE % sizeof(struct dirent)) == 0);

  fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  if (fsfd < 0) {
    perror(argv[1]);
    exit(1);
  }

  sb.size = xint(FSSIZE);
  sb.nblocks = xint(NDATA);
  sb.ninodes = xint(NINODES);
  sb.nlog = xint(LOGSIZE);
  sb.logstart = xint(2);

  sb.bg_start = xint(BG_START);
  sb.bg_metablocks = xint(METABLOCKS);
  sb.bg_datablocks = xint(DATA_BLOCKS_PER_BG);
  sb.bg_inodes = xint(INODE_BLOCKS_PER_BG/INODES_PER_BLOCK);
  sb.bg_size = xint(BG_SZ);
  sb.bg_count = xint(BG_COUNT);
  sb.bg_inodeblocks = xint(INODE_BLOCKS_PER_BG);

  freeblock = BG_START + METABLOCKS;  // the first free block that we can allocate

  for (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  // ZERO OUT INFO BLOCKS
  char buffer[BSIZE];
  int h;
  for(h = BG_START; h < FSSIZE; h += BG_SZ){
    rsect(h, buffer);

    ((bg_t*)buffer)->bg_index = (h - BG_START) / BG_SZ;
    ((bg_t*)buffer)->data_blocks_in_use = 0;
    ((bg_t*)buffer)->inodes_in_use = 0;

    wsect(h, buffer);
  }
    
  // ZERO OUT INFO BLOCKS

  memset(buf, 0, sizeof(buf));
  memmove(buf, &sb, sizeof(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  assert(rootino == ROOTINO);

  bzero(&de, sizeof(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, sizeof(de));

  bzero(&de, sizeof(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, sizeof(de));

  for (i = 2; i < argc; i++) {
    assert(index(argv[i], '/') == 0);

    if ((fd = open(argv[i], 0)) < 0) {
      perror(argv[i]);
      exit(1);
    }

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    if (argv[i][0] == '_') ++argv[i];

    inum = ialloc(T_FILE);

    bzero(&de, sizeof(de));
    de.inum = xshort(inum);
    strncpy(de.name, argv[i], DIRSIZ);
    iappend(rootino, &de, sizeof(de));

    while ((bytes_read = read(fd, buf, sizeof(buf))) > 0)
      iappend(inum, buf, bytes_read);

    close(fd);
  }

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.size);
  off = ((off / BSIZE) + 1) * BSIZE;
  din.size = xint(off);
  winode(rootino, &din);

  exit(0);

}

void
wsect(uint sec, void *buf)
{
  if (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE) {
    perror("lseek");
    exit(1);
  }
  if (write(fsfd, buf, BSIZE) != BSIZE) {
    perror("write");
    exit(1);
  }
}

void
winode(uint inum, struct dinode *ip)
{
  char buf[BSIZE];
  uint bn;
  struct dinode *dip;

  bn = IBLOCK(inum);
  rsect(bn, buf);
  dip = ((struct dinode *)buf) + (inum % INODES_PER_BLOCK);
  *dip = *ip;
  wsect(bn, buf);
}

void
rinode(uint inum, struct dinode *ip)
{
  char buf[BSIZE];
  uint bn;
  struct dinode *dip;

  bn = IBLOCK(inum);
  rsect(bn, buf);
  dip = ((struct dinode *)buf) + (inum % INODES_PER_BLOCK);
  *ip = *dip;
}

void
rsect(uint sec, void *buf)
{
  if (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE) {
    perror("lseek");
    exit(1);
  }
  int bytes = read(fsfd, buf, BSIZE);
  if (bytes != BSIZE) {
    fprintf(stderr,"I Didn't read in a full block: %d\n", bytes);
    perror("read");
    exit(1);
  }
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  struct dinode din;

  bzero(&din, sizeof(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  return inum;
}


uint
balloc()
{
  // Temp variables for block information and the index in its blockgroup
  char bg_info[BSIZE];
  uchar buf[BSIZE];
    
  // Get the ordinal number (0 - FSSIZE) of the next free data block.
  freeblock = NEXT_DATA_BLOCK(freeblock);
  
  uint block = freeblock;
    
  uint data_idx = DATA_IDX(block);

  // Determine which block contains the allocation bitmap for the new block
  uint block_bm = BBLOCK(block);

  // Mark allocated block
  rsect(block_bm, buf);
  buf[data_idx / 8] = buf[data_idx / 8] | (0x1 << (data_idx % 8));
  wsect(block_bm, buf);

  // Get local group information
  rsect(GET_INFO_BLOCK(block), bg_info);
  ((bg_t*)bg_info)->data_blocks_in_use++;
  wsect(GET_INFO_BLOCK(block), bg_info);

  return block;
}

void
iappend(uint inum, void *xp, int bytes_to_append)
{
  char *data_ptr = (char *)xp;
  uint block_index, indirect_block_index;
  uint current_file_bytes, bytes_to_write;
  struct dinode din;
  char block_buffer[BSIZE];
  char bg_info[BSIZE];
  uint indirect[NINDIRECT];
  uint new_block_num;

  if(last_inum < inum){
    last_inum = inum;
    uint info_block_num = ((inum-1)/(INODES_PER_BLOCK*INODE_BLOCKS_PER_BG))*BG_SZ + BG_START;
    rsect(info_block_num, bg_info);
    ((bg_t*)bg_info)->inodes_in_use++;
    wsect(info_block_num, bg_info);
  }

  rinode(inum, &din);
  current_file_bytes = xint(din.size);

  while (bytes_to_append > 0) {
    block_index = current_file_bytes / BSIZE;

    assert(block_index < MAXFILE);

    if (block_index < NDIRECT) {

      if (xint(din.addrs[block_index]) == 0) {
        din.addrs[block_index] = xint(balloc());
      }

      new_block_num = xint(din.addrs[block_index]);

    } else {

      if (xint(din.addrs[NDIRECT]) == 0)
        din.addrs[NDIRECT] = xint(balloc());
      
      rsect(xint(din.addrs[NDIRECT]), (char *)indirect);
      indirect_block_index = block_index - NDIRECT;
        
      if (indirect[indirect_block_index] == 0) {
        indirect[indirect_block_index] = xint(balloc());
        wsect(xint(din.addrs[NDIRECT]), (char *)indirect);
      }

      new_block_num = xint(indirect[indirect_block_index]);
    }

    bytes_to_write = min(bytes_to_append, (block_index + 1) * BSIZE - current_file_bytes);
    rsect(new_block_num, block_buffer);
    bcopy(data_ptr, block_buffer + (current_file_bytes - (block_index * BSIZE)), bytes_to_write);
    wsect(new_block_num, block_buffer);
    bytes_to_append -= bytes_to_write;
    current_file_bytes += bytes_to_write;
    data_ptr += bytes_to_write;
  }

  din.size = xint(current_file_bytes);
  winode(inum, &din);
}
