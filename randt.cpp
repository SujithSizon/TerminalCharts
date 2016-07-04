#include <stdio.h>
#include <random>
#include <unistd.h>

static int RandRange(int Min, int Max)
	{
	int diff = Max-Min;
	return (int) (((double)(diff+1)/RAND_MAX) * rand() + Min);
	}

int main(void)
	{
	int i,y=0;
	FILE *fp;
	fp = fopen("rand", "w");
	while(1){
		for(i=0;i<14;i++)
		{
		y=((y + rand()%9 - 4));
		if (y > 100)
			{y=100;
			}
		else if (y < -50)
			{y=-50;
			}
		/*if(i != 1)*/
		/*{return -n;*/
		/*}*/
		fprintf(fp, "%d, %d, %d, %d\n", y, RandRange(y-5,y+5), RandRange(y-4,y+4), RandRange(y-6,y+6));
		}
		int q,l;
		for(q=0;q<12000;q++)
		for(l=0;l<12000;l++)
		{}
		}   
	fclose(fp);
	return 0;
	}
