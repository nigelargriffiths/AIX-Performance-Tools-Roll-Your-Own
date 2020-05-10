#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <fstab.h>
#include <sys/statfs.h>

int main()
{
	int	i;
	int	fd;
	struct fstab *fstab_buffer;
	struct stat stat_buffer;
	struct statfs statfs_buffer;
	float	fs_size, fs_free, fs_size_used, fs_inodes_used;

	printf("Filesystem         Size MB     Free MB  %%Used %%Inodes MountPoint\n");
	setfsent();
	for (i = 0; (fstab_buffer = getfsent() ) != NULL; i++) {
		if (stat(fstab_buffer->fs_file, &stat_buffer) != -1 ) {
			if (stat_buffer.st_flag & FS_MOUNT) {
				if ( (fd = open(fstab_buffer->fs_file, O_RDONLY)) != -1) {
					if (fstatfs( fd, &statfs_buffer) != -1) {
						if (!strncmp(fstab_buffer->fs_spec, "/proc", 5)) { /* /proc gives invalid/insane values */
							fs_size = 0;
							fs_free = 0;
							fs_size_used = 100.0;
							fs_inodes_used = 100.0;
						} else {
							fs_size = (float)statfs_buffer.f_blocks * 4.0 / 1024.0;
							fs_free = (float)statfs_buffer.f_bfree * 4.0 / 1024.0;
							fs_size_used = ((float)statfs_buffer.f_blocks - (float)statfs_buffer.f_bfree)
							 / (float)statfs_buffer.f_blocks * 100.0;
							fs_inodes_used = ((float)statfs_buffer.f_files - (float)statfs_buffer.f_ffree)
							 / (float)statfs_buffer.f_files * 100.0;
						}
						printf("%-14s %11.2f %11.2f %5.1f%% %5.1f%% %s\n",
						    fstab_buffer->fs_spec,
						    fs_size,
						    fs_free,
						    fs_size_used,
						    fs_inodes_used,
						    fstab_buffer->fs_file);
					} else {
						perror("error");
						printf("fstatfs() of %s failed errno=%d\n", fstab_buffer->fs_file, errno);
					}
					close(fd);
				} else {
					perror("error");
					printf("open(%s,O_RDONLY) failed errno=%d\n", fstab_buffer->fs_file, errno);
				}
			} else  {
				printf("%s not mounted\n", fstab_buffer->fs_file);
			}
		} else  {
			perror("error");
			printf("stat of %s failed errno=%d\n", fstab_buffer->fs_file, errno);
		}
	}
	endfsent();
	return 0;
}
