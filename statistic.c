#include "emulator.h"

void printcard2buf(char *buf,int card)
{
	int num=card%13;//0-12   2-A
	switch(card/13)
	{
		case 0:
			buf[0]='C';
			break;
		case 1:
			buf[0]='D';
			break;
		case 2:
			buf[0]='H';
			break;
		case 3:
			buf[0]='S';
			break;
	}
	if(num<8)
		buf[1]='2'+num;
	else if(num==8)
		buf[1]='T';
	else if(num==9)
		buf[1]='J';
	else if(num==10)
		buf[1]='Q';
	else if(num==11)
		buf[1]='K';
	else if(num==12)
		buf[1]='A';
}

void statinit(struct stat *stat,int players,FILE **pfp)
{
	int i,k;
	char buf[20];

	stat->total = 0;
	stat->players = players;
	for(i=0;i<13;i++)
		for(k=0;k<13;k++)
			stat->involve[i][k] = stat->sheet[i][k] = 0;
#ifdef NODATABASE
//if we don't save every hand's information into our file based database, the emulation would go much faster
	*pfp = NULL;
	return;
#endif

	sprintf(buf,"database%d\0",players);
	*pfp = fopen(buf,"a");
	if(!*pfp){
		printf("can't open database file!");
	}
}

void do_stat(struct stat *stat,struct hand *thishand,FILE *fp)
{
	int i,k;
	int small,big;
	char pbuf[3];
	for(i=0;i<stat->players;i++)
	{
		small = thishand->players[i][0].card%13;
		big = thishand->players[i][1].card%13;
		if(thishand->players[i][0].card/13 == thishand->players[i][1].card/13)//suited
		{
			stat->involve[big][small]++;
			if(thishand->winboard[i])
				stat->sheet[big][small]++;
		}
		else//off suited
		{
			stat->involve[small][big]++;
			if(thishand->winboard[i])
				stat->sheet[small][big]++;
		}
	}
	stat->total++;
	pbuf[2]='\0';
	if(fp)
	{
		fprintf(fp,"********\n********\n");
		fprintf(fp,"players=%d\ncommunity cards:\n  ",stat->players);
		for(i=0;i<5;i++)
		{
			printcard2buf(pbuf,thishand->community[i].card);
			fprintf(fp," %s",pbuf);
		}
		fprintf(fp,"\nwinnner:");
		for(i=0;i<thishand->numberOfWinners;i++)
			fprintf(fp," %d",thishand->winners[i]);
		fprintf(fp,"\n");
		for(i=0;i<stat->players;i++)
		{
			fprintf(fp,"--------\nplayer %d\nhand:",i);
			printcard2buf(pbuf,thishand->players[i][0].card);
			fprintf(fp," %s",pbuf);
			printcard2buf(pbuf,thishand->players[i][1].card);
			fprintf(fp," %s\n",pbuf);
			fprintf(fp,"pokerhand:%s\n",bestname[thishand->pPokerHand[i].name]);
			
		}
		
	}
}

void printStat(struct stat *stat)
{
	int i,k;
	unsigned long long div;
	double a,b,c;
	//if(!div) return;
	printf("@@@@@@@stat@@@@@@@\n");
	printf("players=%d\ninvolve=\n",stat->players);
	for(i=0;i<13;i++)
	{
		for(k=0;k<13;k++)
		{
			printf("%4llu\t",stat->involve[i][k]);
		}
		printf("\n");
	}
	printf("sheet=\n",stat->players);
	for(i=0;i<13;i++)
	{
		for(k=0;k<13;k++)
		{
			b=stat->involve[i][k];
			b/=100;
			//div = stat->involve[i][k]/100;
			if(b==0)
				printf("?\t");
			else{
				a=stat->sheet[i][k];
				printf("%4.2lf\t",a/b);
			}
		}
		printf("\n");
	}
	return;
}

