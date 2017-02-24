#include "emulator.h"
//#define FUNCTIONALTEST
void printResult(struct hand *thishand);

unsigned long long hands;
int players;
struct stat statt;

const char *bestname[]={
	"",
	"HIGHCARD",
	"ONEPAIR",
	"TWOPAIR",
	"THREEOFAKIND",
	"STRAIGHT",
	"FLUSH",
	"FULLHOUSE",
	"FOUROFAKIND",
	"STRAIGHTFLUSH",
	"ROYALFLUSH",
};

const char *suitname[]={
	"C",
	"D",
	"H",
	"S",
};

const char *cardname[]={
	"2", //0
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"10",
	"J",
	"Q",
	"K",
	"A", //12
};


/*
* random int in [1,n]
*/
int randint(int n)
{
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {
    // Chop off all of the values that would cause skew...
    long end = RAND_MAX / n; // truncate skew
    //assert (end > 0L);
    end *= n;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % n +1 ;
  }
}

int dealACard()
{
	int cardn,i;
	cardn = randint(decknum);
	//printf("cardn=%d\n",cardn);
	/*for(i=0;i<52;i++) printf("%2d ",i);
	printf("\n");
	for(i=0;i<52;i++) printf("%2d ",deck[i]);
	printf("\n");*/
	for(i=0;cardn;i++)
	{
		//printf("i=%d ",i);
		if(deck[i])
			cardn--;
	}
	//printf("\n");
	i--;
	//printf("dealACard,card=%d\n",i);
	deck[i] = 0;
	decknum--;
	return i;
}


static inline void shuffleCards()
{
	int i;
	decknum = 52;
	for(i=0;i<52;i++)
		deck[i]=1;
}

static inline void initThisHand(struct hand *thishand)
{
	int i;
	shuffleCards();
	thishand->potnum = 0;
	thishand->dealer = dealer;
	thishand->potsum = 0;
	thishand->SB = DEFAULTSB;
	thishand->BB = DEFAULTBB;
	for(i=0;i<players;i++){
		//thishand->allinStatus[i] = 0;
		thishand->online[i]=1;
	}
}


void sortPlayersCards(struct hand *thishand)
{
	int i;
	int mid;
	for(i=0;i<players;i++)
	{
		if(thishand->players[i][0].card % 13 > thishand->players[i][1].card % 13)
		{
			mid = thishand->players[i][0].card;
			thishand->players[i][0].card = thishand->players[i][1].card;
			thishand->players[i][1].card = mid;
		}
	}
	return ;
}


void dealCardsToPlayers(struct hand *thishand)
{
	int i;
	int dealern = thishand->dealer;
	//TODO: should start from small blind.
	for(i=0;i<players;i++)
		thishand->players[(dealern+i+1)%players][0].card = dealACard();
	for(i=0;i<players;i++)
		thishand->players[(dealern+i+1)%players][1].card = dealACard();
	//printf("dealcardsToPlayers\n");
	sortPlayersCards(thishand);
}

/*
void freehands(struct hand *thishand)
{
	if(!thishand) return;
	freehands(thishand->next);
	printResult(thishand);
	free(thishand);
	return;
}*/

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

void flop(struct hand *thishand)
{
	thishand->flopburn.card = dealACard();
	thishand->community[0].card = dealACard();
	thishand->community[1].card = dealACard();
	thishand->community[2].card = dealACard();
}

void turn(struct hand *thishand)
{
	thishand->turnburn.card = dealACard();
	thishand->community[3].card = dealACard();
}

void river(struct hand *thishand)
{
	thishand->riverburn.card = dealACard();
	thishand->community[4].card = dealACard();
}

inline void categorySort(int buffer[7][3],int *dStart,int *hStart,int *sStart)
{
	int i,k,mid;
	for(i=0;i<6;i++)
	{
		for(k=0;k<6-i;k++)
		{
			if(buffer[k][0]>buffer[k+1][0] ||   \
				(buffer[k][0]==buffer[k+1][0] && buffer[k][1]>buffer[k+1][1])  )
			{
				mid = buffer[k][0];
				buffer[k][0] = buffer[k+1][0];
				buffer[k+1][0] = mid;
				mid = buffer[k][1];
				buffer[k][1] = buffer[k+1][1];
				buffer[k+1][1] = mid;
				mid = buffer[k][2];
				buffer[k][2] = buffer[k+1][2];
				buffer[k+1][2] = mid;
			}
		}
	}
	for(i=0;i<7;i++) if(buffer[i][0]!=CLUB) break;
	*dStart = i;
	for(;i<7;i++) if(buffer[i][0]!=DIAMOND) break;
	*hStart = i;
	for(;i<7;i++) if(buffer[i][0]!=HEART) break;
	*sStart = i;
}

inline void numSort(int buffer[8][3])
{
	int i,k,mid;
	for(i=0;i<6;i++)
	{
		for(k=0;k<6-i;k++)
		{
			if(numbuf(k)<numbuf(k+1))
			{
				mid = buffer[k][0];
				buffer[k][0] = buffer[k+1][0];
				buffer[k+1][0] = mid;
				mid = buffer[k][1];
				buffer[k][1] = buffer[k+1][1];
				buffer[k+1][1] = mid;
				mid = buffer[k][2];
				buffer[k][2] = buffer[k+1][2];
				buffer[k+1][2] = mid;
			}
		}
	}
}

inline void calculatePokerhand(struct hand *thishand,int p)
{
	int buffer[8][3];
	int dStart,hStart,sStart;
	int isflush;
	int start,end;
	enum bestHand best;
	int type;
	int i;
	int high;
	int isstraight;
	int same;
	int foknum,thoknum,twoknum[3],onum[7];
	int twopointer,onepointer;
	int straightnum;
	int sortnum[15][5];
	
	typebuf(0) = thishand->players[p][0].card / 13;
	numbuf(0) = thishand->players[p][0].card % 13 + 2 ;//14==A, 13==K
	origbuf(0) = thishand->players[p][0].card;
	typebuf(1) = thishand->players[p][1].card / 13;
	numbuf(1) = thishand->players[p][1].card % 13 + 2 ;
	origbuf(1) = thishand->players[p][1].card;
	typebuf(2) = thishand->community[0].card / 13;
	numbuf(2) = thishand->community[0].card % 13 + 2 ;
	origbuf(2) = thishand->community[0].card;
	typebuf(3) = thishand->community[1].card / 13;
	numbuf(3) = thishand->community[1].card % 13 + 2 ;
	origbuf(3) = thishand->community[1].card;
	typebuf(4) = thishand->community[2].card / 13;
	numbuf(4) = thishand->community[2].card % 13 + 2 ;
	origbuf(4) = thishand->community[2].card;
	typebuf(5) = thishand->community[3].card / 13;
	numbuf(5) = thishand->community[3].card % 13 + 2 ;
	origbuf(5) = thishand->community[3].card;
	typebuf(6) = thishand->community[4].card / 13;
	numbuf(6) = thishand->community[4].card % 13 + 2 ;
	origbuf(6) = thishand->community[4].card;
	
	categorySort(buffer,&dStart,&hStart,&sStart);//small to big
	//printf("dStart=%d,hStart=%d,sStart=%d\n",dStart,hStart,sStart);
	best=HIGHCARD;
	//flush? straightflush? royalflush?
	isflush=0;
	if(dStart>4)
	{
		isflush = 1;
		type = CLUB;
	}
	else if(hStart-dStart>4)
	{
		isflush = 1;
		type = DIAMOND;
	}
	else if(sStart-hStart>4)
	{
		isflush = 1;
		type = HEART;
	}
	else if(7-sStart>4)
	{
		isflush = 1;
		type = SPADE;
	}
	if(isflush)
	{
		best = FLUSH;
		switch(type)
		{
			case CLUB:
				start=0;
				end=dStart;
				break;
			case DIAMOND:
				start=dStart;
				end=hStart;
				break;
			case HEART:
				start=hStart;
				end=sStart;
				break;
			case SPADE:
				start=sStart;
				end=7;
				break;
		}
		sortnum[FLUSH][0] = numbuf(end-1);
		sortnum[FLUSH][1] = numbuf(end-2);
		sortnum[FLUSH][2] = numbuf(end-3);
		sortnum[FLUSH][3] = numbuf(end-4);
		sortnum[FLUSH][4] = numbuf(end-5);
		isstraight = 0;
		high = end-1;
		for(i=end-2;i>=start;i--)
		{
			if(numbuf(i)+1 == numbuf(i+1))
			{
				if(high-i > 3)//straight
				{
					isstraight = 1;
					break;
				}
				continue;
			}
			high = i;
		}
		if(!isstraight && high == start+3 &&   \
			numbuf(start)==2 && numbuf(end-1)==14)
			isstraight = 1;//A2345
		if(isstraight)
		{
			best = STRAIGHTFLUSH;
			sortnum[STRAIGHTFLUSH][0] = numbuf(high);//we just need one
			if(numbuf(high)==14)
				best = ROYALFLUSH;
		}
	}
	numSort(buffer);//big to small
	//others
	same=1;
	foknum = 0;
	thoknum=0;
	twoknum[0]=twoknum[1]=twoknum[2]=0;
	onum[0]=onum[1]=onum[2]=onum[3]=onum[4]=0;
	twopointer=-1;
	onepointer=0;
	onum[0]=numbuf(0);
	for(i=1;i<7;i++)
	{
		if(numbuf(i)==numbuf(i-1)) same++;
		else same=1;
		if(same==4)
		{
			//four of a kind
			foknum = numbuf(i);
			// 4 is not a 3
			if(thoknum == foknum) thoknum = 0;
		}
		switch(same)
		{
			case 3:
				if(!thoknum)
				{
					thoknum=numbuf(i);
					// 3 is not a 2
					//thoknum[twopointer] = 0;//perhaps we don't need to do this
					twopointer--;
					break;
				}
				else
				{
					//we've already have a 3, which is bigger than this one. 
					//Let's take this one as a 2.
					//Don't modify "same". It may become 4 on the next round.
					//Don't do anything. Just break.
					break;
				}
			case 2:
				twopointer++;
				twoknum[twopointer]=numbuf(i);
				// 2 is not a 1
				//onum[onepointer] = 0;//perhaps we don't need to do this
				onepointer--;
				break;
			case 1:
				onepointer++;
				onum[onepointer]=numbuf(i);
				break;
		}
	}
/*
	printf("********\n");
	printf("foknum=%d\n",foknum);
	printf("thoknum=%d\n",thoknum);
	printf("twoknum=%d,%d,%d\n",twoknum[0],twoknum[1],twoknum[2]);
	printf("onum=%d,%d,%d,%d,%d\n",onum[0],onum[1],onum[2],onum[3],onum[4]);
	printf("2p=%d\n",twopointer);
	printf("1p=%d\n",onepointer);
	printf("========\n");
*/
	if(foknum)//four of a kind
	{
		if(best < FOUROFAKIND)
		{
			best=FOUROFAKIND;
			sortnum[FOUROFAKIND][0] = foknum;//we deed two
			sortnum[FOUROFAKIND][1] = (twoknum[0]>onum[0])?(twoknum[0]):(onum[0]);
			sortnum[FOUROFAKIND][1] = (thoknum>sortnum[FOUROFAKIND][1])?thoknum:(sortnum[FOUROFAKIND][1]);
		}
	}
	else if(thoknum)
	{
		//FullHouse or three-of-a-kind
		if(twopointer>=0) //FullHouse
		{
			if(best < FULLHOUSE)
			{
				best=FULLHOUSE;
				//we need two sortnums
				sortnum[FULLHOUSE][0] = thoknum;
				sortnum[FULLHOUSE][1] = twoknum[0];
			}
		}
		else
		{
			//three of a kind
			if(best < THREEOFAKIND)
			{
				best=THREEOFAKIND;
				//we need three sortnums
				sortnum[THREEOFAKIND][0] = thoknum;
				sortnum[THREEOFAKIND][1] = onum[0];
				sortnum[THREEOFAKIND][2] = onum[1];
			}
		}
	}
	else if(twoknum[0])
	{
		//two-pair or one-pair
		if(twopointer>=1) //two-pair
		{
			if(best < TWOPAIR)
			{
				best=TWOPAIR;
				//we need three sortnums
				sortnum[TWOPAIR][0] = twoknum[0];
				sortnum[TWOPAIR][1] = twoknum[1];
				sortnum[TWOPAIR][2] = 
					(twopointer==2)?((twoknum[2]>onum[0])?twoknum[2]:onum[0]):onum[0];
				//In case of three pairs, we need to compare the third pair with the biggest one-card.
			}
		}
		else //one-pair
		{
			if(best < ONEPAIR)
			{
				best=ONEPAIR;
				//we need 4 sortnums
				sortnum[ONEPAIR][0] = twoknum[0];
				sortnum[ONEPAIR][1] = onum[0];
				sortnum[ONEPAIR][2] = onum[1];
				sortnum[ONEPAIR][3] = onum[2];
			}
		}
	}
	//straight?
	isstraight=0;
	high = 0;
	straightnum = 1;
	for(i=1;i<7;i++)
	{
		if(numbuf(i)+ 1 == numbuf(i-1))
		{
			straightnum++;
			if(straightnum > 4)//straight
			{
				isstraight = 1;
				break;
			}
			continue;
		}
		else if(numbuf(i) == numbuf(i-1))
		{
			//same card, ignore
			continue;
		}
		straightnum = 1;
		high = i;
	}
	if(!isstraight && straightnum==4 &&   \
		numbuf(6)==2 && numbuf(0)==14)
		isstraight = 1;//5432A
	if(isstraight && best < STRAIGHT)
	{
		best = STRAIGHT;
		//we need 1 sortnum
		sortnum[STRAIGHT][0] = numbuf(high);
	}
	//High card, which is default.
	//we need 5 sortnums
	sortnum[HIGHCARD][0] = numbuf(0);
	sortnum[HIGHCARD][1] = numbuf(1);
	sortnum[HIGHCARD][2] = numbuf(2);
	sortnum[HIGHCARD][3] = numbuf(3);
	sortnum[HIGHCARD][4] = numbuf(4);
	
	thishand->pPokerHand[p].name = best;
	thishand->pPokerHand[p].sortnum[0] = sortnum[best][0];
	thishand->pPokerHand[p].sortnum[1] = sortnum[best][1];
	thishand->pPokerHand[p].sortnum[2] = sortnum[best][2];
	thishand->pPokerHand[p].sortnum[3] = sortnum[best][3];
	thishand->pPokerHand[p].sortnum[4] = sortnum[best][4];

	
}

//Return 1 if p1 wins.
//Return 0 if it's a tie.
//Return -1 if p2 wins.
int comparePokerhand(struct hand *thishand,int p1,int p2)
{
	int look,i;
	//printf("compareBetween %d and %d\n",p1,p2);
	if(thishand->pPokerHand[p1].name > thishand->pPokerHand[p2].name)
		return 1;
	if(thishand->pPokerHand[p1].name < thishand->pPokerHand[p2].name)
		return -1;
	switch(thishand->pPokerHand[p1].name)
	{
		case ROYALFLUSH:
			return 0;
		case STRAIGHTFLUSH:
			look = 1;
			break;
		case FOUROFAKIND:
			look = 2;
			break;
		case FULLHOUSE:
			look = 2;
			break;
		case FLUSH:
			look = 5;
			break;
		case STRAIGHT:
			look = 1;
			break;
		case THREEOFAKIND:
			look = 3;
			break;
		case TWOPAIR:
			look = 3;
			break;
		case ONEPAIR:
			look = 4;
			break;
		case HIGHCARD:
			look = 5;
			break;
	}
	for(i=0;i<look;i++)
	{
		if(thishand->pPokerHand[p1].sortnum[i] > thishand->pPokerHand[p2].sortnum[i])
			return 1;
		if(thishand->pPokerHand[p1].sortnum[i] < thishand->pPokerHand[p2].sortnum[i])
			return -1;
	}
	return 0;
}


void showdown(struct hand *thishand)
{
	int i;
	int ret;
	//printf("ShowDown!\n");
	for(i=0;i<players;i++)
		calculatePokerhand(thishand,i);
	thishand->numberOfWinners = 1;
	thishand->winners[0] = 0;
	//printf("calculatePokerhandFinished\n");
	for(i=1;i<players;i++)
	{
		ret = comparePokerhand(thishand,i,thishand->winners[0]);
		if(ret==1)
		{
			thishand->winners[0] = i;
			thishand->numberOfWinners = 1;
		}
		else if(!ret)
		{
			thishand->winners[thishand->numberOfWinners] = i;
			thishand->numberOfWinners++;
		}
	}
	for(i=0;i<players;i++)
		thishand->winboard[i]=0;
	for(i=0;i<thishand->numberOfWinners;i++)
		thishand->winboard[thishand->winners[i]] = 1;
	//TODO: We should sort players here. Since pots may be more than 1!
	return;
}

/*
inline void printcard(struct card *pcard)
{
	printf("%s%s",
			suitname[pcard->card/13],cardname[pcard->card % 13]);
}*/

void printResult(struct hand *thishand)
{
	int i,k;
	printf("=========Results=========\n");
	printf("Dealer:\n\tplayer%d\n",thishand->dealer);
	printf("Community cards:\n\t");
	for(i=0;i<5;i++)
	{
		printf("%s%s ",
			suitname[thishand->community[i].card/13],cardname[thishand->community[i].card % 13]);
	}
	printf("\n");
	for(i=0;i<players;i++)
	{
		printf("player%d:\n",i);
		printf("\tPrivate cards:\n\t\t");
		printf("%s%s %s%s\n",
			suitname[thishand->players[i][0].card/13],cardname[thishand->players[i][0].card % 13],
			suitname[thishand->players[i][1].card/13],cardname[thishand->players[i][1].card % 13]);
		printf("\tPokerhand:\n");
		printf("\t\t%s\n",bestname[thishand->pPokerHand[i].name]);
		printf("\tsortnum:\n\t\t");
		for(k=0;k<5;k++)
			printf("%d ",thishand->pPokerHand[i].sortnum[k]);
		printf("\n");
	}
	printf("Winner%s:\n",(thishand->numberOfWinners>1)?"s are":" is");
	for(i=0;i<thishand->numberOfWinners;i++)
		printf("\tplayer%d ",thishand->winners[i]);
	printf("\n");
	
}

#ifdef FUNCTIONALTEST
void functionaltest()
{
	char goon;
	int i,k;
	char ch;
	char buf[30];
	int num;
	struct hand *thishand;
	do
	{
		thishand = (struct hand *) malloc(sizeof(struct hand));
		players = 2;
		thishand->dealer = dealer = 0;
		printf("2 players, dealer == player 0\n");
		printf("input community cards\n");
		for(i=0;i<5;i++)
		{
			ch=getchar();
			scanf("%s",buf);
			getchar();
			num = atoi(buf);
			if(ch=='S') num+=13*3;
			else if(ch=='H') num+=13*2;
			else if(ch=='D') num+=13;
			thishand->community[i].card = num-2;
		}
		for(k=0;k<2;k++)
		{
			printf("input 2 cards of player %d\n",k);
			for(i=0;i<2;i++)
			{
				ch=getchar();
				scanf("%s",buf);
				getchar();
				num = atoi(buf);
				if(ch=='S') num+=13*3;
				else if(ch=='H') num+=13*2;
				else if(ch=='D') num+=13;
				thishand->players[k][i].card = num-2;
			}
		}
		showdown(thishand);
		printResult(thishand);
		free(thishand);
		printf("goon?\n");
		goon = getchar();
		getchar();
	}while(goon=='y');
	return;
}
#endif

int main()
{
	struct hand *thishand = NULL;
	unsigned long long progress,mark;
	FILE *fp;
	srand((unsigned)time(NULL));

#ifdef FUNCTIONALTEST
		functionaltest();
		return 0;
#endif

	input();
	hands = progress= *((unsigned long long *)config[CFG_HANDS].item);
	players = *((int *)config[CFG_PLAYERS].item);
	statinit(&statt,players,&fp);
	//Here we go
	dealer=0;
	mark=hands/20;
	thishand = (struct hand *) malloc(sizeof(struct hand));
	if(!thishand)
	{
		printf("Not enough memory!\n");
		return 0;
	}
	while(progress--)
	{
		if(hands-progress > mark)//progress bar
		{
			mark += hands/20;
			printf(".");
			fflush(stdout);
		}
		//printf("-------hands=%llu--------\n",hands);
		initThisHand(thishand);
		//printf("shuffleDone\n");
		
		dealCardsToPlayers(thishand);
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
		do_stat(&statt,thishand,fp);
		nextDealer(players);
		
	}
	free(thishand);
	printf("\n");
	//freehands(thishand);
	printStat(&statt);
	freememory();
	if(fp)
		fclose(fp);
	return 0;
}

