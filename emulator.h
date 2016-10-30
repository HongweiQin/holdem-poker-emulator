#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define CFGFILE "configurations/Config"
#define CFG_ITEMS 2
//This is NOT the number of players! The number of players should be defined in the Config file.
#define CFG_PLAYERS 0
#define CFG_HANDS 1

//default configurations
#define DEF_PLAYERS 7
#define DEF_HANDS 10

struct cfgItem{
	char name[15];
	void (*I)(char *);//input funtion
	void (*P)(void);//print function
	void *item;
};

struct cfgItem config[CFG_ITEMS];


#define CLUB 0
#define DIAMOND 1
#define HEART 2
#define SPADE 3

#define ISCLUB(x) ((x)<DIAMOND*13)?1:0
#define ISDIAMOND(x) ((x)<HEART && (x)>=DIAMOND)?1:0
#define ISHEART(x) ((x)<SPADE && (x)>=HEART)?1:0
#define ISSPADE(x) ((x)>=SPADE)?1:0

int deck[52];
int dealer;
int decknum;

#define PREFLOP_BET 0
#define FLOP_BET 1
#define TURN_BET 2
#define RIVER_BET 3


#define typebuf(x) buffer[x][0]
#define numbuf(x) buffer[x][1]
#define origbuf(x) buffer[x][2]



enum bestHand{
	HIGHCARD = 1,
	ONEPAIR,
	TWOPAIR,
	THREEOFAKIND,
	STRAIGHT,
	FLUSH,
	FULLHOUSE,
	FOUROFAKIND,
	STRAIGHTFLUSH,
	ROYALFLUSH,
};

struct card{
	int card;
};

struct pokerHand{
	enum bestHand name;
	int sortnum[5];
};

struct hand{
	int dealer;
	int numberOfWinners;
	int winners[10];
	struct card community[5];
	struct card flopburn,turnburn,riverburn;
	struct card players[10][2];
	struct pokerHand pPokerHand[10];
	struct hand *next;
};

int randint(int n);
void input(void);
void freememory(void);
int bet(struct hand *thishand,int which);

