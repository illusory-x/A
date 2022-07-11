#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include<signal.h>
int blocksize;
int sfd;
int dfs;
char * s_file;
char * d_file;
int pram_check(int argc,const char * sfile, int pronum);
int file_block(const char * sfile, int pronum);
void * copy(void *pos)
{
    
    int block_size = blocksize;
    int copy_pos = *(int *)pos;
    printf("copy_pos=%d\n",copy_pos);
    char buffer[block_size];
    bzero(buffer,sizeof(buffer));
    ssize_t readlen;
    //修改偏移量
    lseek(sfd,copy_pos,SEEK_SET);
    lseek(dfs,copy_pos,SEEK_SET);
    //读写
    readlen=read(sfd,buffer,sizeof(buffer));
    write(dfs,buffer,readlen);
    close(sfd);
    close(dfs);
}

int pram_check(int argc,const char * sfile, int pronum)
{
    if(argc<3)
    {
        printf("ERROR:缺少必要参数,请重试!\n");
        exit(0);
    }
    if(access(sfile,F_OK)!=0)
    {
        printf("ERROR:源文件不存在!\n");
        exit(0);
    }
    if(pronum<=0||pronum>100)
    {
        printf("ERROR:进程数量不允许小于0或大于100!\n");
        exit(0);
    }
    return 0;
}
int file_block(const char *sfile,int pronum)
{
    int fd=open(sfile,O_RDONLY);
    int fsize = lseek(fd,0,SEEK_END);
    if(fsize % pronum == 0)
    {
        return fsize/pronum;
    }
    else
    {
        return fsize/pronum +1;
    }
}
int main(int argc, char * argv[])
{
    int pronum;
    if(argv[3]==0)
    {
        pronum=3;
    }
    else
    {
        pronum = atoi(argv[3]);
    }
    pram_check(argc,argv[1],pronum);
    blocksize=file_block(argv[1],pronum);
    
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);

   //设置分离态
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    //创建
    pthread_t tid;
    int err;
    int flag;
    for(flag=0;flag<pronum;flag++)
    {
        sfd = open(argv[1],O_RDONLY);;
    	dfs = open(argv[2],O_RDWR|O_CREAT,0664);
        int pos;
        pos=flag * blocksize;
        /*char str_blockzise[50];
        char str_pos[50];
        bzero(str_blockzise,sizeof(str_blockzise));
        bzero(str_pos,sizeof(str_pos));
        sprintf(str_blocksize,"%d",blocksize);
        sprintf(str_pos,"%d",pos);*/
        if((err=pthread_create(&tid,&attr,copy,(void *)&pos))>0)
        {
            printf("pthread_create error:%s\n",strerror(err));
            exit(0);
        }
        printf("thread number %d\n",flag);
        usleep(100);
    }
    pthread_attr_destroy(&attr);
    return 0;    
}
