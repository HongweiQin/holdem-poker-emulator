#include "emulator.h"

void freehands(struct hand* thishand)
{
	if(!thishand) return;
	freehands(thishand->next);
	free(thishand);
	return;
}

void freememory()
{
	int i;
	for(i=0;i<CFG_ITEMS;i++)
		free(config[i].item);
	return;
}

void initdeck()
{
	int i;
	for(i=0;i<52;i++)
		deck[i] = 1;
}

inline void nextDealer(int players)
{
	dealer++;
	if(dealer == players) dealer=0;
}

void shuffleCards(struct hand *thishand)
{

}

void flop(struct hand *thishand)
{

}

void turn(struct hand *thishand)
{

}

void river(struct hand *thishand)
{

}

void showdown(struct hand *thishand)
{

}

void count(struct hand *thishand)
{

}

int main()
{
	unsigned long long hands;
	int players;
	struct hand *thishand = NULL,*lasthand;
	
	input();
	hands = *((unsigned long long *)config[CFG_HANDS].item);
	players = *((int *)config[CFG_PLAYERS].item);
	//Here we go
	dealer=0;
	while(hands--)
	{
		thishand = (struct hand *) malloc(sizeof(struct hand));
		shuffleCards(thishand);
		if(bet(thishand,PREFLOP_BET))
			goto statistic;
		flop(thishand);
		if(bet(thishand,FLOP_BET))
			goto statistic;
		turn(thishand);
		if(bet(thishand,TURN_BET))
			goto statistic;
		river(thishand);
		if(bet(thishand,RIVER_BET))
			goto statistic;
		showdown(thishand);
statistic:
		count(thishand);
		nextDealer(players);
		lasthand = thishand;
	}
	freehands(thishand);
	freememory();
	return 0;
}

