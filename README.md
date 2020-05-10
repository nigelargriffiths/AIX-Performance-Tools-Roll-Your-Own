# AIX perfstat Library example C code starter pack

See AIX pert Blog article https://www.ibm.com/support/pages/roll-your-own-aix-performance-monitor-code-samples

## Summary
AIX offers lots of performance stats from the libperfstat library. Here is a collection of C tools that show programmers how to extract the statistics to develop further and some are actually useful in their own right.

## Roll You Own AIX Performance Monitoring code samples
- WARNING: This web page is really for C programmers but if you are not a programmer you can just run the programs and watch the output.
- From Nigel Griffiths, Advanced Technical Support, EMEA, IBM.
- Updated the binary file download for AIX 7.2 Feb 2017.  
- Note the libperfstat.h and its library has more stats available in recent AIX releases, so there may be more interesting data in them than shown in these examples.
 
Writing your own AIX Performance Monitoring commands or tools is really easy to do on AIX but it can be hard work to get started. The below sample source code and precompiled tools should make this much easier. Most of the important performance data can be extracted from the AIX libperfstat API library. This is documented in the /usr/include/libperfstat.h header file and in the AIX documentation. But these two places don't explain all the details and a worked example is worth a 10,000 lines of documentation!! There other places for some more information like the lpar_get_info() function is documented in the /usr/include/sys/dr.h header file and there is lots of process stats in the getentry64() system call which is defined in the /usr/include/procinfo.h header file.

## General Hints on the programs:
- They are compiled only for AIX 5.3 ML03
- Due to their being multiple versions of libperfstat they might not run on older AIX versions or even old Maintenance Levels!
- They all output data once a second to the screen
- To stop all the below programs use: Control+C
- To compile most use: cc -g -l perfstat progname.c -o progname
- To compile perf use: cc -g -l perfstat perf.c -o perf -lcurses
- The program cpus can take two parameters. the first is the seconds between output and the second the number of times to output

## Adapters
This program outputs the adapters stats (actually the added up disk stats as tapes are not included). This explains why the busy percentage can go over 100%. It shows the actual counters and the fact you have to take the differences between data snapshots to workout what happened in the last interval.

## Disks Simple
This program outputs the disk stats for all disks. It will work with early version of libperfstat on AIX 5.1 onwards. It outputs the disk names, descriptions, volume group, block size. Then once a second the disk names, disk size and disk free in MB, transfers, read and write blocks, time busy percent and Qdepth.

## Disk Service
This program outputs the disk stats for all disks. It includes the new in AIX 5.3 service times details that should allow queueing theory analyses. The output is modelled on the iostat -D output from higher Maintenance Levels of AIX 5.3 - I compiled it on ML04. 

## Disk Paths
This program outputs the disk path stats for each disk in turn. This should allow the study of multipath I/O and vpaths but currently only documents the LAST PATH.

## Disk Totals
This program outputs the disk stats for all disks as a total.

## Memory
This program outputs the memory stats (real and virtual) from libperfstat and the perfstat_memory_total() function.

## Memory Virtual Memory Information
This program outputs the memory stats from the vminfo structure and the vmgetinfo() function.


## Network
This program outputs the Network stats for each network. This one just shows the counters . This will demonstrate the counters - although the libperfstat I/F claims 64 but the numbers will over flow at unsigned 32 bit. With a 10 Gbit Network card going full speed this takes on 4 seconds!!

## Network Totals
This program outputs the Network stats overall for the machine.


## NFS - Network File System
This program outputs the NFS stats. This Interface is complex due to all the protocols but this worked example can help you out by showing how to get the basic information out. 

## CPU
This program outputs the CPU stats and details for each online CPU including system calls, read, write, fork, exec and character I/O. 
It can help you check that the workload it evenly spread across CPUs and is multi-threaded.

## CPU Totals
This program outputs details about your hardware and the CPU stats for the machine and is a "faked up" vmstat with physical CPU stats too. You need to run this on a shared processor LPAR to see the fully details of the difference between physical, virtual processors.

## LPAR - Logical Partition IBM speak for a Virtual Machine
This program outputs the logical partition stats. This is all you can find out about your logical partition (LPAR) in full gory detail from the lpar_get_info() system call. The names are inconsistent, the units are often weird (and not documented), some fields are unexplained and some are truly massive numbers like tot_dispatch_time. 

## Perf - a bit like a cut down nmon
This program is a prototype nmon and shows the use of curses and outputs CPU information. 

## Processes from getprocs64() - not perfstat library
This program shows how to start writing the "ps" command. It gets the procentry64 structure for each process and outputs PID, Parent PID, CPU time, Resident Text and Data sizes, threads, maximum open files, WLM class-name and command name. Unfortunately, to get the CPU time per process you need to capture the processes stats twice and take the difference on the CPU counters (pi_ru.ru_utime and ru_stime). This is made difficult as some processes may have started or finished so you have to match the two procentry64 structure arrays by PID and then may be reorder them too so you list this in CPU use order! Also note that on some machines the processes are NOT returned in PID order. Updated Nov 2011.

## Worms
This program is very different. It is an implementation of the classic UNIX kernel testing program from 20 years ago. It outputs wiggly worms via curses on the screen. This was a good test of interrupt handling, terminal output and the Kernel. So what is the point? Now the program is used to demonstrate Dynamic LPAR changes i.e Entitlement changes of Capped LPAR and/or Uncapping the LPAR. As it is single threaded it can't spread across more then one CPU so VP changes do not effect it. Note: this version had to be slowed down by a factor of 1 million to work on POWER5 - the CPUs are a lot faster these days. It takes a single argument, the number of worms (maximum is 50). Use control-C to end the program. The number at the top left is the number of loops per second is is managing to do. worms_aix is for AIX 5L and worms_lop is for Linux on POWER (compiled under SUSE SLES9). To slow the worms down, set the environment variable to numbers much higher than 1024, like: export SPIN=10000; ./worms 10 

## File Systems
This is a simple df command clone and lists the filesystems - it does not use libperfstat just regular system calls. 

## Dedicated Donating LPAR
This program outputs the Dedicated CPU LPAR with Donating activated statistics. This needs to be compiled on a system the its libperfstat includes the donating stats in perfstat_partition_t data structure. It outputs the logical and physical percentages, physical CPU Used and then the donating stats so you get the whole picture. I compiled it on ML04 but with the libperfstat.h header file from AIX6. August 2007.

## CPU Totals
This program outputs Entitlement, VP and physical CPU used (Consumed) - an input parameter is the seconds between output lines (default 1 second. Compiled on AIX 6.1 TL6 sp5. 
