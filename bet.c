#include "emulator.h"

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
	return 0;//(ToBeDone)
}

static int __doRiverBet(struct hand *thishand)
{
	return 0;//(ToBeDone)
}

int bet(struct hand *thishand,int which)
{
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
	return 0;//Never reach here
}

