# 7 - Filesystem - Fast File System

### On disk, the Berkley Fast File System (FFS) is laid out with the: 

- Boot Block
- Super Block
- Log
- Chunks of Block groups

Each block group consist of the inode blocks, free bit map and the data blocks which contain the actual data. 

### Many different source files had to be modified in order to support the file system organization techniques in FFS. 

- `balloc` in `fs.c` had to be modified in order to allow data blocks to be allocated close to each other rather than putting new data naively at the next free chunk on disk.

- `mkfs.c` had to be changed in order to accommodate creating a FFS specific disk image for xv6 to run.

In order to test the performance difference between the [default xv6 file system](https://github.com/mit-pdos/xv6-public) and the FFS implementation there is a simple program called `fs-time`. The custom program runs through and tests many common file system operations such as: creating directories, creating files, changing directories, writing info directories, writing into files, removing files and removing directories. 

### `fs-time`

- Creates ten directories

- Creates ten text files

- Writes a string into each text file

- Deletes the files

- Deletes the directories

## License

All **source code** in this repository is released under the MIT license. See LICENSE for details.