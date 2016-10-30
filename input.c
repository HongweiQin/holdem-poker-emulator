#include "emulator.h"
#define BUFLEN 500
#define MAXSPACE 10

static void __cfgPlayersInput(char *cfg)
{
	int players;
	players = atoi(cfg);
	if(players > 10) players = 10;
	if(players < 2) players = 2;
	*((int *)config[CFG_PLAYERS].item) = players;
	return;
}

static void __cfgPlayersPrint()
{
	if(config[CFG_PLAYERS].item)
		printf("%d",*((int *)config[CFG_PLAYERS].item));
	return;
}

static void __cfgHandsInput(char *cfg)
{
	*((unsigned long long *)config[CFG_HANDS].item) = (unsigned long long)atoll(cfg);
	return;
}

static void __cfgHandsPrint()
{
	if(config[CFG_HANDS].item)
		printf("%llu",*((unsigned long long *)config[CFG_HANDS].item));
	return;
}


static void initDefaultCfg()
{
	sprintf(config[CFG_PLAYERS].name,"players");
	config[CFG_PLAYERS].item = malloc(sizeof(int));
	*((int *)config[CFG_PLAYERS].item) = DEF_PLAYERS;
	config[CFG_PLAYERS].I = __cfgPlayersInput;
	config[CFG_PLAYERS].P = __cfgPlayersPrint;

	sprintf(config[CFG_HANDS].name,"hands");
	config[CFG_HANDS].item = malloc(sizeof(unsigned long long));
	*((unsigned long long *)config[CFG_HANDS].item) = DEF_HANDS;
	config[CFG_HANDS].I = __cfgHandsInput;
	config[CFG_HANDS].P = __cfgHandsPrint;
}

static void doConfig(char *buffer)
{
	int i,j;
	int space;
	char *p;
	
	if(buffer[0]=='#') return;
	space = MAXSPACE;
	while(space--)
	{
		if(buffer[0]!=' ' && buffer[0]!='\t') break;
		buffer++;
	}
	for(i=0;i<CFG_ITEMS;i++)
	{
		for(j=0;j<strlen(config[i].name);j++)
		{
			if(buffer[j]!=config[i].name[j]) break;
		}
		if(j<strlen(config[i].name))//dismatch
			continue;
		//match
		//ignore spaces
		space = MAXSPACE;
		p=buffer+j;
		while(space--)
		{
			if(*p!=' ' && *p!='\t') break;
			p++;
		}
		//must be =
		if(*p != '=' ) continue;
		p++;
		//ignore spaces
		space = MAXSPACE;
		while(space--)
		{
			if(*p!=' ' && *p!='\t') break;
			p++;
		}
		//assign
		config[i].I(p);
		break;
	}
	return;
}

static int readconfig(void)
{
	FILE *fp;
	char buf[BUFLEN];
	printf("Load configurations.\n");
	fp=fopen(CFGFILE,"r");
	if(!fp)
		goto out;
	while(fgets(buf,BUFLEN,fp))
		doConfig(buf);
	
	fclose(fp);
	printf("Done.\n");
	return 0;
out:
	return -1;
}

void printCfg()
{
	int i;
	for(i=0;i<CFG_ITEMS;i++)
	{
		printf("%s=",config[i].name);
		config[i].P();
		printf("\n");
	}
}

void input()
{
	initDefaultCfg();
	if(readconfig())
		printf("Read configuration file failed. Use default config\n");
	printCfg();
	return;
}
