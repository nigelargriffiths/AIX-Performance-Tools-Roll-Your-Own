CFLAGS=-O3 -lperfstat -lm -lcurses

all: adapt disks_simple disks_service diskpath disktot memory memory_vminfo net nettot nfs cpu cpus lpar perf getprocs64 worms filesystems donate evp

test: adapt cmd cpu cpu_pools cpu_total cpu_total2 cpus curses diskpath disks disks2 disks_service disks_simple diskserve disktot donate ds evp filesystems fold getprocs64 junk libperfstat lpar lpar1 memory memory_vminfo mhz n net netfix nettot nfs nfs2 nfs3 nfs4 page_vmgetinfo perf perfsample worms

clean:
	rm -fr adapt cmd cpu cpu_pools cpu_total cpu_total2 cpus curses diskpath disks disks2 disks_service disks_simple diskserve disktot ds evp filesystems fold getprocs64 junk libperfstat lpar lpar1 memory memory_vminfo mhz n net netfix nettot nfs nfs2 nfs3 nfs4 page_vmgetinfo perf perfsample 

tar_source:
	tar cvf ryo_source.tar Makefile adapt.c disks_simple.c disks_service.c diskpath.c disktot.c memory.c memory_vminfo.c net.c nettot.c nfs.c cpu.c cpus.c lpar.c perf.c getprocs64.c worms.c filesystems.c donate.c evp.c

tar_bins: all
	tar cvf ryo_bins.tar adapt disks_simple disks_service diskpath disktot memory memory_vminfo net nettot nfs cpu cpus lpar perf getprocs64 worms filesystems donate evp


adapt:

c:

cmd:

cpu:

cpu_pools:

cpu_total:

cpu_total2:

cpus:

curses:

diskpath:

disks:

disks2:

disks_service:

disks_simple:

diskserve:

disktot:

ds:

evp:

filesystems:

fold:

getprocs64:

junk:

libperfstat:

lpar:

lpar1:

memory:

memory_vminfo:

mhz:

n:

net:

netfix:

nettot:

nfs:

nfs2:

nfs3:

nfs4:

page_vmgetinfo:

perf:

perfsample:

perfstat_diskserv:

x:

y:



