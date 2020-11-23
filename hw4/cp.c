/*
 2020-2 [OPERATING SYSTEMS] Assignment #4
 JuYoung Oh / 20161872 / Electrical & Electronics Engineering
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

/*Define constants*/
#define TRUE 1
#define FALSE 0

#define BUF_SIZE 1024
#define DIR_BUF_SIZE 256

/*Declaration of functions*/
int copy(char src[], char dst[], char root[]);
int explore_dir(char src[], char dst[], char root[]);

int main(int argc, char *argv[]){
	struct stat st_src, st_dst;
	int src_is_reg, dst_is_reg = 0, dst_exist = TRUE, mode;
	int i;

	if(argc < 3){
		printf("Not enough arguments\n");
		return EXIT_FAILURE;
	}

	char *src = argv[1];
	char *dst = argv[2];
	char root[DIR_BUF_SIZE];
	getcwd(root, DIR_BUF_SIZE);

	if(stat(argv[1], &st_src) == -1){
		perror("src stat failed");
		return EXIT_FAILURE;
	}

	if(stat(argv[2], &st_dst) == -1){
		dst_exist = FALSE;
	}

	if(S_ISREG(st_src.st_mode))
		src_is_reg = TRUE;
	else if(S_ISDIR(st_src.st_mode))
		src_is_reg = FALSE;
	else{
		printf("src stat error\n");
		return EXIT_FAILURE;
	}
	
	if(dst_exist){
		if(S_ISREG(st_dst.st_mode))
			dst_is_reg = TRUE;
		else if(S_ISDIR(st_dst.st_mode))
			dst_is_reg = FALSE;
		else{
			printf("dst stat error\n");
			return EXIT_FAILURE;
		}
	}

	if(src_is_reg && (!dst_exist || dst_is_reg))
		mode = 1;  // reg - reg
	else if(src_is_reg && (dst_exist && !dst_is_reg))
		mode = 2;  // reg - dir
	else if(!src_is_reg && (dst_exist && !dst_is_reg))
		mode = 3;  // dir - existing_dir
	else if(!src_is_reg && !dst_exist)
		mode = 4;  // dir - creating_dir 
	else
		mode = 5;  // exception(dir - reg)
	
	if(mode == 1){
		if(copy(src, dst, root) < 0){
			perror("copy");
			return EXIT_FAILURE;
		}
	}
	else if(mode == 2){
		strcat(dst, "/");
		strcat(dst, src);

		if(copy(src, dst, root) < 0){
			perror("copy");
			return EXIT_FAILURE;
		}
	}
	else if(mode == 3){
		chdir(dst);
		if(mkdir(src, S_IRUSR | S_IWUSR | S_IXUSR) < 0){
			perror("mkdir");
			return EXIT_FAILURE;
		}
		chdir(root);
		strcat(dst, "/");
		strcat(dst, src);
		explore_dir(src, dst, root);
	}
	else if(mode == 4){
		if(mkdir(dst, S_IRUSR | S_IWUSR | S_IXUSR) < 0){
			perror("mkdir");
			return EXIT_FAILURE;
		}
		explore_dir(src, dst, root);
	}
	else{
		printf("Error: src is directory and dst is regular file\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*Definition of functions*/
int copy(char src[], char dst[], char root[]){
	int n, in, out;
	char buf[BUF_SIZE];

	chdir(root);

	if((in = open(src, O_RDONLY)) < 0){
		printf("Copy in error\n");
		perror(src);
		return EXIT_FAILURE;
	}

	if((out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) < 0){
		printf("Copy out error\n");
		perror(dst);
		return EXIT_FAILURE;
	}

	while((n = read(in, buf, sizeof(buf))) > 0)
		write(out, buf, n);

	close(out);
	close(in);
	
	return EXIT_SUCCESS;
}

int explore_dir(char src[], char dst[], char root[]){
	DIR *pdir;
	struct dirent *dirt;
	struct stat st;

	char *dir_name[DIR_BUF_SIZE];
	char tmp_src[DIR_BUF_SIZE];
	char tmp_dst[DIR_BUF_SIZE];
	char tmp_dst_dir[DIR_BUF_SIZE];

	int i = 0, count = 0, f_count = 0;

	strcpy(tmp_src, src);
	strcpy(tmp_dst, dst);
	strcat(tmp_src, "/");
	strcat(tmp_dst, "/");

	chdir(root);

	if((pdir = opendir(src)) <= 0){
		perror("opendir");
		return EXIT_FAILURE;
	}

	chdir(src);
	
	while((dirt = readdir(pdir)) != NULL){
		lstat(dirt->d_name, &st);

		if(S_ISDIR(st.st_mode)){
			if(strcmp(dirt->d_name, ".") && strcmp(dirt->d_name, "..")){
				dir_name[count] = dirt->d_name;
				count++;
			}
		}
		else if(S_ISREG(st.st_mode)){
			strcat(tmp_src, dirt->d_name);
			strcat(tmp_dst, dirt->d_name);
			
			chdir(root);
			copy(tmp_src, tmp_dst, root);
			chdir(src);

			strcpy(tmp_src, src);
			strcpy(tmp_dst, dst);
			strcat(tmp_src, "/");
			strcat(tmp_dst, "/");
		}
	}

	for(i = 0; i < count; i++){
		strcpy(tmp_src, src);
		strcat(tmp_src, "/");
		strcat(tmp_src, dir_name[i]);

		strcpy(tmp_dst, dst);
		strcat(tmp_dst, "/");
		strcat(tmp_dst, dir_name[i]);

		strcpy(tmp_dst_dir, root);
		strcat(tmp_dst_dir, "/");
		strcat(tmp_dst_dir, tmp_dst);

		chdir(root);
		if(mkdir(tmp_dst, S_IRUSR | S_IWUSR | S_IXUSR) < 0){
			perror("mkdir");
			return EXIT_FAILURE;
		}
		chdir(src);
		
		if(explore_dir(tmp_src, tmp_dst, root) == -1)
			break;
	}

	closedir(pdir);
	chdir("..");

	return EXIT_SUCCESS;
}
