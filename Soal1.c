#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/pif/Modul4";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;
  int fd = 0 ;
  	char buffss [1000];
  	strcpy(buffss, fpath);
  	int len_a =strlen(buffss);
  	int len_b=len_a-4;
  	if (buffss[len_b]=='.'&& 
  		(
  			(buffss[len_b+1]=='d'&&buffss[len_b+2]=='o'&&buffss[len_b+3]=='c')||
  			(buffss[len_b+1]=='p'&&buffss[len_b+2]=='d'&&buffss[len_b+3]=='f')
  		)
  		)
  	{
  		//show error dialog wwww
  		char command [500];
  		strcpy(command,"echo \"Terjadi kesalahan! File berisi konten berbahaya.\"");
  		system(command);
  		//how!?
  		strcat(buffss, ".ditandai");
  		rename(fpath, buffss);
  		sprintf(command,"mkdir %s/rahasia", dirpath);
  		system(command);
  		sprintf(command,"mv %s %s/rahasia/%s.ditandai",buffss,dirpath, path);
  		system(command);
  		sprintf(command,"chmod 000 %s/rahasia/%s.ditandai",dirpath, path);
  		system(command);
  		return -errno;
  	}
  	else 
  	{
  		(void) fi;
		fd = open(fpath, O_RDONLY);
		if (fd == -1)
			return -errno;

		res = pread(fd, buf, size, offset);
		if (res == -1)
			res = -errno;

		close(fd);
		return res;	
  	}
	
}


static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	char *filename = (strstr(fpath, "/Downloads/"));
	if (filename!=NULL)
	{
		char command[500];
		sprintf(command, "mkdir %s/Downloads/Simpanan", dirpath);
		system(command);
		sprintf(fpath,"%s/Downloads/Simpanan/%s", dirpath, filename);
	}

	(void) fi;
	fd = open(fpath, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
