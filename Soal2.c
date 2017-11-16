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

static int xmp_truncate(const char *path, off_t size)
{
	int res;
  	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}



static int xmp_chmod(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	res = chmod(fpath, mode);
	if (res == -1)
		return -errno;

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
  	//spesifikasi 1 start
  	char buffss [1000];
  	strcpy(buffss, fpath);
  	int len_a =strlen(fpath);
  	int len_b=len_a-4;
  	int len_c=len_b-1; //spesifikasi4
  	//test extension
  	char *filename = (strstr(fpath, "/Documents/"));
  	if (filename!=NULL)
  	{
  		 	if (buffss[len_b]=='.'&& 
  		(
  			(buffss[len_b+1]=='d'&&buffss[len_b+2]=='o'&&buffss[len_b+3]=='c')||
  			(buffss[len_b+1]=='p'&&buffss[len_b+2]=='d'&&buffss[len_b+3]=='f')
  		))
	  	{
	  		filename=strchr(filename,'D');
			filename=strchr(filename,'/');
	  		//show error dialog wwww
	  		char command [500];
	  		strcpy(command,"zenity --error --text=\"Terjadi kesalahan! File berisi konten berbahaya.\"");
	  		system(command);
	  		//spesifikasi 1 end  		
	  		//----------------------------------------
	  		//spesifikasi 2 start
	  		sprintf(command,"mkdir %s/Documents/rahasia/", dirpath);
	  		system(command);
	  		sprintf(command,"mv %s %s/Documents/rahasia%s.ditandai",fpath,dirpath, filename);
	  		system(command);
	  		sprintf(command,"chmod 000 %s/Documents/rahasia%s.ditandai",dirpath, filename);
	  		//xmp_chmod(command, "000")
	  		system(command);
	  		//return -errno;
	  		//spesifikasi 2 end
	  	}
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

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	res = lchown(fpath, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
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
	.chmod		= xmp_chmod,
	.truncate	= xmp_truncate,
	.chown		= xmp_chown,
	.read		= xmp_read,
	.write		= xmp_write,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
