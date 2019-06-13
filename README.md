# Advanced-xv6

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/nextseto/Advanced-xv6/master/LICENSE)

**xv6** is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix Version 6. **Advanced-xv6** is a repository that contains some modernizations to improve upon the [base implementation](https://github.com/mit-pdos/xv6-public). There are seven versions of xv6 detailed below:

1. Round-Robin (RR) scheduler in xv6 

2. First-Come-First-Serve (FCFS) scheduler in xv6

3. Priority scheduler in xv6

4. Completely-Fair-Scheduler (CFS) in xv6

5. Doubly-Indirect-Block filesystem in xv6

6. Triply-Indirect-Block filesystem in xv6

7. Berkley Fast Filesystem (FFS) in xv6

## Installation

### Requirements

- An x86 Linux ELF machine
 - Tested on Ubuntu 16.04 with QEMU

### How to use

1. Clone the directory: `git clone `
2. Navigate to the source directory: `cd Advanced-xv6/<version>`
3. Compile the operating system: `make`
4. Run: `make qemu`

## Modernization

The base xv6 implementation provides a great foundation for a simple Operating System. However, there are two areas of improvement that would make it a tad more modern...

### Completely Fair Scheduler (CFS)

The Linux Completely Fair Scheduler (CFS) is based around the idea that every process should be given even amounts of CPU power. For example, if there are four processes each should get a quarter of the total available CPU time.

This repository contains a partially implemented CFS as it lacks the implementation for NICE priority values with a Red-Black Tree.

[You can read more about the Linux CFS here.](https://www.ibm.com/developerworks/library/l-completely-fair-scheduler/)

### Berkley Fast Filesystem (FFS)

The Berkley Fast Filesystem (FFS) uses the notion of cylinder/block groups which places multiple similar files together in order to ensure short disk seek time on disk. This implementation followed three heuristics for locality maintenance:

- Placing new directories in one of the least-used block groups.

- Placing files in the same directory in the same block group whenever possible.

- Chunking and spreading large files across multiple block groups.

More details can be found in the `7 - Filesystem - Fast File System/README.md` and [here](https://people.eecs.berkeley.edu/~brewer/cs262/FFS.pdf).

## Credit

- [Elisa Idrobo](https://github.com/ElisaIdrobo)

## License

All **source code** in this repository is released under the MIT license. See LICENSE for details.
