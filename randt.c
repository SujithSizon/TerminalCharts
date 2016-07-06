#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
static int RandRange(int Min, int Max)
	{int diff = Max-Min;
	return (int) (((double)(diff+1)/RAND_MAX) * rand() + Min);
	}
static struct values
	{int o, h, l, c;
	}args;

void *myfunc (void *arg)
	{int y=0;
	struct values *arg_struct = (struct values*) arg;
	int i;
	while(1){
		for(i=0;i<14;i++)
			{
			y=((y + rand()%9 - 4));
			if (y > 280)
				{y=280;
				}
			else if (y < 120)
				{y=120;
				}
			/*if(i != 1)
			*{return -n;
			*}*/
			arg_struct->o= y;
			arg_struct->h= RandRange(y,y+20);
			arg_struct->l= RandRange(y-20,y);
			arg_struct->c= RandRange(y-20,y+20);
			}
		sleep(0.9);		
		}
	return 0;
	}	


int main(int argc, char **argv)
	{
	pthread_t t1;
	pthread_create(&t1, NULL, myfunc, &args);
	FILE *fp;
	fp = fopen("rand", "w");
	while(1){
		fprintf(fp, "%d, %d, %d, %d\n", args.o,args.h,args.l,args.c);
		printf("open:%d, high:%d, low:%d, close:%d\n", args.o,args.h,args.l,args.c);
		sleep(0.9);
		}
	fclose(fp);
	return 0;
	}
