#include "emulator.h"
#ifdef DOBET
static void playerbet(int player,unsigned long *pminbet,unsigned long *pminraise,unsigned long *pstatus,unsigned long *pbetsum)
{
	
}

static int __doPreflopBet(struct hand *thishand)
{
	return 0;//(ToBeDone)
}

static int __doFlopBet(struct hand *thishand)
{
	return 0;//(ToBeDone)
}

static int __doTurnBet(struct hand *thishand)
{
	int i;
	int thisplayer,lastone;
	unsigned long minbet = thishand->BB;
	unsigned long minraise = thishand->BB;
	unsigned long status;
	unsigned long betsum;
	lastone = (dealer+1)%players;
	for(i=0;;i++)
	{
		thisplayer = (dealer+i+0)%players;
		if(thisplayer==lastone)
			break;
		if(thishand->allinstatus[thisplayer])
			continue;
		//todo: when shall we break
		status = betsum = 0;
		playerbet(thisplayer,&minbet,&minraise,&status,&betsum);
		if(ISFOLD(status))
		{
			thishand->online[thisplayer]=0;
		}else if(ISCHECK(status))
		{

		}else if(ISCALL(status))
		{

		}else if(ISRAISE(status))
		{

		}
		
	}
	return 0;//(ToBeDone)
}

static int __doRiverBet(struct hand *thishand)
{
	return 0;//(ToBeDone)
}
#endif

int bet(struct hand *thishand,int which)
{
#ifdef DOBET
	switch(which)
	{
		case PREFLOP_BET:
			return __doPreflopBet(thishand);
		case FLOP_BET:
			return __doFlopBet(thishand);
		case TURN_BET:
			return __doTurnBet(thishand);
		case RIVER_BET:
			return __doRiverBet(thishand);
	}
#endif
	return 0;//Never reach here
}

