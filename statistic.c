#include "emulator.h"
void statinit(struct stat *stat,int players)
{
	int i,k;
	stat->total = 0;
	stat->players = players;
	for(i=0;i<13;i++)
		for(k=0;k<13;k++)
			stat->involve[i][k] = stat->sheet[i][k] = 0;
}

void do_stat(struct stat *stat,struct hand *thishand)
{
	int i,k;
	int small,big;
	for(i=0;i<stat->players;i++)
	{
		small = thishand->players[i][0].card%13;
		big = thishand->players[i][1].card%13;
		if(thishand->players[i][0].card/13 == thishand->players[i][1].card/13)//same suit
		{
			stat->involve[big][small]++;
			if(thishand->winboard[i])
				stat->sheet[big][small]++;
		}
		else//different suit
		{
			stat->involve[small][big]++;
			if(thishand->winboard[i])
				stat->sheet[small][big]++;
		}
	}
	stat->total++;
}

void printStat(struct stat *stat)
{
	int i,k;
	unsigned long long div;
	if(!div) return;
	printf("@@@@@@@stat@@@@@@@\n");
	printf("players=%d\nsheet=\n",stat->players);
	for(i=0;i<13;i++)
	{
		for(k=0;k<13;k++)
		{
			div = stat->involve[i][k]/100;
			if(!div)
				printf("?\t");
			else
				printf("%4llu\t",stat->sheet[i][k]/div);
		}
		printf("\n");
	}
	return;
}

