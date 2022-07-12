> Apricot ：杏色，显示的是红色比红色要淡
>
> Peach : 桃色，但显示出来是翠绿色
>
> each ：刚刚删掉Peach想测试小写是否可以（是可以的），而看到这个竟然也有颜色，还挺好看
>
> Bittersweet ：苦乐参半色，嘻嘻
>
> RedOrange ：红色
>
> BrickRed ：比苦乐参半色要浅
>
> Salmon ：橙色的感觉
>
> Magenta ：粉紫的感觉
>
> VioletRed ：深粉的感觉
>
> Rhodamine ：龙井绿茶的感觉

---



#### 线程创建<br>

> int err=pthread_create(pthread_t * tid , pthread_attr_t * attr , void* (*tjobs)(void *) , void * arg)
>
> ​																								void*    jobs(void *)<br>

```c
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include<signal.h>
void * jobs(void* arg)
{
    while(1)
    {
        sleep(1);
    }
}
int main()
{
    pthread_t tid;
    int err;
    int flags=0;
    while(1)
    {
        if((err=pthread_create(&tid,NULL,jobs,(void *)&var))>0)
        {
            printf("pthread_create error:%s\n",strerror(err));/*输出错误日志*/
            exit(0);
        }
        printf("thread number %d\n",++flags);
    }
}
```

#### 线程ID获取<br>

> 主线程传出的ID有效性较差无法确定线程状态，pthread_self ()可保证当前线程有效<br>pthread_t tid = pthread_self() #返回调用进程ID<br>

```c
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include<signal.h>
void * jobs(void* arg)
{
    printf("thread runing... arg=%d\n",*(int*)arg);
    printf("普通线程ID：0x%x\n",(unsigned int)pthread_self());
    while(1)
    {
        sleep(1);
    }
}
int main()
{
    pthread_t tid;
    int var=1024;
    pthread_create(&tid,NULL,jobs,(void *)&var);
    printf("主线程tid:0x%x\n",(unsigned int)pthread_self());
    while(1)
    {
        sleep(1);
    }
    return 0;
}
```

#### 线程取消<br>

> 线程A可通过cancel【取消事件(系统调用)】/signal【信号(中断、异常或系统调用)】结束线程B<br>

> pthread_cancel(pthread_t tid) # 可以通过系统事件取消结束某个目标线程<br>如果程序中包含取消机制，那么需要被调用的线程内部有系统调用，检测处理取消事件<br>pthread_testcancel(); # 产生一次系统调用的函数<br>

```c
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include<signal.h>
void * jobs(void* arg)
{
    pthread_testcancel();
}
int main()
{
    pthread_t tid;
    int var=1024;
    pthread_create(&tid,NULL,jobs,(void *)&var);
   	sleep(5);
    pthread_cancel(tid)
    while(1)
    {
        sleep(1);
    }
    return 0;
}
```

#### 线程回收<br>

> pthread_join(pthread tid , void **reval) #除了回收线程的TCB资源外，还可以将线程返回值或者退出码获取进行二次检测<br>join是阻塞回收函数<br>

```c
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include<signal.h>
void * jobs(void* arg)
{
    printf("P thread tid：0x%x\n",(unsigned int)pthread_self());
    sleep(5);
    return (void *)8;
}
int main()
{
    pthread_t tid;
    int var=1024;
    pthread_create(&tid,NULL,jobs,(void *)&var);
    void * reval=NULL;
    pthread_join(tid,&reval);
    printf("P thread exit code ：%d\n",(int)reval);
    while(1)
    {
        sleep(1);
    }
    return 0;
}
```

#### 线程分离态<br>

> 线程被创建后，默认为回收态线程，这类线程结束后需要手动回收，通过 *pthread_join* 回收<br>如果用户不想回收可以将回收态线程改为分离态线程，分离态线程结束后系统自动回收线程资源，无需用户干预<br>pthread_detach(pthread_t tid) #通过这个函数将线程设置为分离态（线程退出状态转换）<br><br><font color='VioletRed'>线程函数使用没有明确限制，所有线程只要知道对方tid，即可进行取消回收分离等操作</font><br>分离态与回收态是互斥的，线程只能保持一种状态，如果对一个分离线程进行回收操作会失败，对一个已处于回收阶段的线程设置分离也会失败<br>

```c
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include<signal.h>
void * jobs(void* arg)
{
    pthread_detach(pthread_self());
    printf("P thread tid：0x%x\n",(unsigned int)pthread_self());
    sleep(5);
    return NULL;
}
int main()
{
    pthread_t tid;
    
    pthread_create(&tid,NULL,jobs,NULL);
    sleep(10);
    void * reval=NULL;
    int err;
    if((err=pthread_join(tid,&reval))>0)
    {
        printf("join call failed %s\n",strerror(err));
    }
    printf("P thread exit code ：%ld\n",(long int)reval);
    while(1)
    {
        sleep(1);
    }
    return 0;
}
```

#### 线程退出的几种方式<br>

> exit(0)：无论是主线程调用还是普通线程调用，直接关闭结束进程，杀死所有线程一般该函数在退出处理才使用

> return 0：主线程retrun，整个进程结束退出，普通线程工作使用return，只要当前线程结束并返回一个值

> pthread_cancel：可以通过该函数取消一个目标线程

> pthread_exit()：无论主线程还是普通线程，调用该函数导致线程退出，不影响整个进程

> 默认情况一个线程被cancel取消，使用join回收时会得到-1，我们可以根据此返回值察觉取消事件，线程工作代码实现时退出码不允许使用-1，要保留给cancel

```c
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include<signal.h>
void * jobs(void* arg)
{
    printf("P thread tid：0x%x\n",(unsigned int)pthread_self());
    while(1)
    {
        sleep(1);
    }
}
int main()
{
    pthread_t tid;
    printf("Master thread tid:0x%x Runing...\n",(unsigned int)pthread_self());
    pthread_create(&tid,NULL,jobs,NULL);
    void *reval=NULL;
    sleep(1);
    pthread_cancel(tid);//取消先成功
    pthread_join(tid,&reval);//回收线程
    printf("printf P thread exit code:%ld\n",(long int)reval);
    while(1)
    {
        sleep(1);
    }
}
```

#### 线程属性<br>

> 在线程创建前就决定线程的一些内部状态
>
> > 如果没有特殊要求使用默认属性的线程就可以,在有特殊要求的开发场合可以通过改属性改变线程的退出状态,修改线程的大小,提高线程数量

> pthread_attr_t attr #线程属性结构体<br>
>
> <font color='Rhodamine '>线程退出状态:分离或回收</font><br>
>
> 线程优先级指针<br>
>
> 线程警界缓冲区大小(4096)<br>
>
> <font color='Rhodamine'>线程栈大小与线程栈地址</font><br>
>
> > x86 32Ubuntu
> >
> > 排除进程默认占用的虚拟地址剩余 2G+ /8M=381
> >
> > 64位有系统限制,修改线程大小也不会提高线程数量

##### 修改属性流程

>定义线程属性<br>初始化线程属性 pthread_attr_init()#初始化并申请内存<br>根据需要修改属性内容 
>
>> 修改退出状态<br>修改栈地址与大小<br>
>
>创建线程时使用自定义属性 pthread_create(&tid,&attr,jobs,NULL)<br>释放线程属性 pthread_destroy()<br>

```c
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include<signal.h>

void* jobs(void *arg)
{
    while(1)
    {
        sleep(1);
    }
}
int main()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    //查看默认属性中退出状态
    int detachstate;
    pthread_attr_getdetachstate(&attr,&detachstate);
    if(detachstate == PTHREAD_CREATE_JOINABLE)
    {
        printf("PTHREAD_CREATE_JOINABLE!\n");
    }
    else
    {
        printf("PTHREAD_CREATE_DETACHED\n");
    }
    //设置分离态
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    //创建线程使用自定义属性
    pthread_t tid;
    pthread_create(&tid,&attr,jobs,NULL);
    int err;
    if((err=pthread_join(tid,NULL))>0)
    {
        printf("join error:%S\n",strerror(err));
        exit(0);
    }
    pthread_attr_destroy(&attr);
    return 0;
}
```

###### 修改栈信息,提高线程数量

> 根据需要修改属性内容 :设置好栈大小后自行申请内存空间而后设置到线程属性,线程创建后使用自定义栈<br>#获取某个线程属性中得到栈地址和栈大小,传出到变量中<br>pthread_attr_getstack(pthread_attr_t*attr, void **stackaddr , size_t *stacksize)

```c
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include<signal.h>

void * jobs(void *arg)
{
    while(1)
    {
        sleep(1);
    }
}
int main()
{
    void * stackaddr=NULL;
    size_t stacksize;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_getstack(&attr,&stackaddr,&stacksize);
    printf("addr[%p] size[%d]\n",stackaddr,(int)stacksize);
    int flag=0;
    int err;
    while(1)
    {
        stacksize = 0x100000;//1M
        
        if((stackaddr = (void*)malloc(stacksize))==NULL)
        {
            perror("malloc call failed");
            exit(0);
        }
        pthread_attr_setstack(&attr,stackaddr,stacksize);
        if((err=pthread_create(&tid,&attr,jobs,NULL))>0)
        {
            printf("thread create error:%s\n",strerror(err));
            exit(0);
        }
        printf("thread No.%d\n",++flag);
    }
    pthread_attr_deatroy(&attr);
}
```

##### 线程拷贝<br>

```C
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

```

#### 互斥锁<br>

> pthread_mutex_t lock；#互斥锁类型<br>pthread_mutex_init(pthread_mutex_t* lockaddr, pthread_mutexattr_t* attr);<br>pthread_mutex_destroy(pthread_mutex_t* lockaddr);<br>pthread_mutex_lock(pthread_mutex_t* lockaddr);<br>pthread_mutex_unlock(pthread_mutex_t* lockaddr);

```c++
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include<signal.h>
#define X 5000
int code;
pthread_mutex_t lock;
void* add_code(void* arg)
{
    pthread_detach(pthread_self());
    int tmp;
    for(int i=0;i<X;i++)
    {
        pthread_mutex_lock(&lock);
        tmp = code;
        printf("thread tid 0x%xx ++code=%d\n",(unsigned int)pthread_self(),++tmp);
        code = tmp;
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}
int main()
{
    pthread_t tid;
    pthread_mutex_init(&lock,NULL);
    pthread_create(&tid,NULL,add_code,NULL);
    pthread_create(&tid,NULL,add_code,NULL);
    while(1)
    {
        sleep(1);
    }
    pthread_mutex_destroy(&lock);
    return 0;
}
```

#### 读写锁(pthread_rwlock_t)<br>

基于互斥锁实现的一种所应用:读共享写独占,读写互斥<br>在原有互斥锁的基础上提高了全局资源的利用率<br>

> pthread_rwlock_t lock；#互斥锁类型<br>pthread_rwlock_init(pthread_rwlock_t* lockaddr, pthread_rwlock_t* attr);<br>pthread_rwlock_destroy(pthread_rwlock_t* lockaddr);<br>pthread_rwlock_wrlock(pthread_rwlock_t* lockaddr);<br>pthread_rwlock_rdlock(pthread_rwlock_t* lockaddr);<br>pthread_rwlock_unlock(pthread_rwlock_t* lockaddr);

```C++
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include<signal.h>
pthread_rwlock_t lock;
int code;
void* thread_read(void *arg)
{
    while(1)
    {
        pthread_rwlock_rdlock(&lock);
        printf("Read thread tid 0x%xx Outcode=%d\n",(unsigned int)pthread_self(),code);
        pthread_rwlock_unlock(&lock);
        usleep(200000);
    }
   
}
void* thread_write(void *arg)
{
    while(1)
    {
        pthread_rwlock_wrlock(&lock);
        printf("Write thread tid 0x%xx Outcode=%d\n",(unsigned int)pthread_self(),++code);
        pthread_rwlock_unlock(&lock);
        usleep(200000);
    }
   
}
int main()
{
    pthread_t tids[8];
    pthread_rwlock_init(&lock,NULL);
    int i=0;
    for(i;i<3;i++)
    {
        pthread_create(&tids[i],NULL,thread_write,NULL);
    }
    for(i;i<8;i++)
    {
        pthread_create(&tids[i],NULL,thread_read,NULL);
    }
    while(i--)
    {
        pthread_join(tids[8],NULL);
    }
}
```

#### 自旋锁(旋转锁)<br>

> 旋转锁互斥特征方面与互斥锁基本相同,不同之处在于自旋锁不会挂起,以(R)的状态请求资源(就近原则不适用旋转锁),旋转锁系统开销大,所有请求线程都是全速运行的<br>如果不在意系统开销,希望提高锁的使用频率选择旋转锁否则使用互斥锁
