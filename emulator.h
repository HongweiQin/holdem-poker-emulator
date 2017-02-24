#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//if we don't save every hand's information into our file based database,
//the emulation would go much faster
//uncomment to disable file based database
//#define NODATABASE


#define CFGFILE "configurations/Config"
#define CFG_ITEMS 2
//This is NOT the number of players! The number of players should be defined in the Config file.
#define CFG_PLAYERS 0
#define CFG_HANDS 1

#define DEFAULTSB 5
#define DEFAULTBB 10

#define S_ALLIN (1)
#define S_RAISE (1 << 1)
#define S_CALL (1 << 2)
#define S_CHECK (1 << 3)
#define S_FOLD (1 << 4)

#define ISALLIN(x) ((x)&S_ALLIN)
#define ISRAISE(x) ((x)&S_RAISE)
#define ISCALL(x) ((x)&S_CALL)
#define ISCHECK(x) ((x)&S_CHECK)
#define ISFOLD(x) ((x)&S_FOLD)

extern const char *bestname[];

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
	//cards
	struct card community[5];
	struct card flopburn,turnburn,riverburn;
	struct card players[10][2];
	
	struct pokerHand pPokerHand[10];//each players' poker hand

	int numberOfWinners;
	int winners[10];
	int winboard[10];

	unsigned long SB;//SB num
	unsigned long BB;//BB num
	int online[10];//whether the player is online;
	int potnum;
	unsigned long potsum;
	int allinstatus[10];//whether the player is already allin
	//struct pot thePot[10];
	
};

struct stat{
	unsigned long long total;
	int players;
	unsigned long long sheet[13][13];
	unsigned long long involve[13][13];
};


int randint(int n);
void input(void);
void freememory(void);
int bet(struct hand *thishand,int which);
void statinit(struct stat *stat,int players,FILE **pfp);
void do_stat(struct stat *stat,struct hand *thishand,FILE *fp);



