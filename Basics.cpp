#include<cstdio>
#include<cstdlib>
#include<cstdint>
#include<ctime>

#define White 0
#define Black 1
#define Pawns 2
#define Knights 3
#define Bishops 4
#define Rooks 5
#define Queens 6
#define Kings 7

#define INVALID1BYTE 0XFF
#define INVALID4BYTE 0XFF

uint8_t _King=0XF;
uint8_t _Pawn=0;
uint64_t _Boards[8]={0XFFFFULL,0XFFFFULL<<48,0XFFULL<<48|0XFFULL<<8,0X42ULL<<56|0X42ULL,0X24ULL<<56|0X24ULL,0X81ULL<<56|0X81ULL,0X8ULL<<56|0X8ULL,0X10ULL<<56|0X10ULL};

int16_t PositionValues[16][64]={{},{},
	{+0,+0,+0,+0,+0,+0,+0,+0,+5,+10,+10,-20,-20,+10,+10,+5,+5,-5,-10,+0,+0,-10,-5,+5,+0,+0,+0,+20,+20,+0,+0,+0,+5,+5,+10,+25,+25,+10,+5,+5,+10,+10,+20,+30,+30,+20,+10,+10,+50,+50,+50,+50,+50,+50,+50,+50,+0,+0,+0,+0,+0,+0,+0,+0},
	{-50,-40,-30,-30,-30,-30,-40,-50,-40,-20,+0,+5,+5,+0,-20,-40,-30,+5,+10,+15,+15,+10,+5,-30,-30,+0,+15,+20,+20,+15,+0,-30,-30,+5,+15,+20,+20,+15,+5,-30,-30,+0,+10,+15,+15,+10,+0,-30,-40,-20,+0,+0,+0,+0,-20,-40,-50,-40,-30,-30,-30,-30,-40,-50},
	{-20,-10,-10,-10,-10,-10,-10,-20,-10,+5,+0,+0,+0,+0,+5,-10,-10,+10,+10,+10,+10,+10,+10,-10,-10,+0,+10,+10,+10,+10,+0,-10,-10,+5,+5,+10,+10,+5,+5,-10,-10,+0,+5,+10,+10,+5,+0,-10,-10,+0,+0,+0,+0,+0,+0,-10,-20,-10,-10,-10,-10,-10,-10,-20},
	{+0,+0,+0,+5,+5,+0,+0,+0,-5,+0,+0,+0,+0,+0,+0,-5,-5,+0,+0,+0,+0,+0,+0,-5,-5,+0,+0,+0,+0,+0,+0,-5,-5,+0,+0,+0,+0,+0,+0,-5,-5,+0,+0,+0,+0,+0,+0,-5,+5,+10,+10,+10,+10,+10,+10,+5,+0,+0,+0,+0,+0,+0,+0,+0},
	{-20,-10,-10,-5,-5,-10,-10,-20,-10,+0,+5,+0,+0,+0,+0,-10,-10,+5,+5,+5,+5,+5,+0,-10,+0,+0,+5,+5,+5,+5,+0,-5,-5,+0,+5,+5,+5,+5,+0,-5,-10,+0,+5,+5,+5,+5,+0,-10,-10,+0,+0,+0,+0,+0,+0,-10,-20,-10,-10,-5,-5,-10,-10,-20},
	{+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0},
	{},{},
	{+0,+0,+0,+0,+0,+0,+0,+0,+50,+50,+50,+50,+50,+50,+50,+50,+10,+10,+20,+30,+30,+20,+10,+10,+5,+5,+10,+25,+25,+10,+5,+5,+0,+0,+0,+20,+20,+0,+0,+0,+5,-5,-10,+0,+0,-10,-5,+5,+5,+10,+10,-20,-20,+10,+10,+5,+0,+0,+0,+0,+0,+0,+0,+0},
	{-50,-40,-30,-30,-30,-30,-40,-50,-40,-20,+0,+5,+5,+0,-20,-40,-30,+0,+10,+15,+15,+10,+0,-30,-30,+5,+15,+20,+20,+15,+5,-30,-30,+0,+15,+20,+20,+15,+0,-30,-30,+5,+10,+15,+15,+10,+5,-30,-40,-20,+0,+0,+0,+0,-20,-40,-50,-40,-30,-30,-30,-30,-40,-50},
	{-20,-10,-10,-10,-10,-10,-10,-20,-10,+0,+0,+0,+0,+0,+0,-10,-10,+0,+5,+10,+10,+5,+0,-10,-10,+5,+5,+10,+10,+5,+5,-10,-10,+0,+10,+10,+10,+10,+0,-10,-10,+10,+10,+10,+10,+10,+10,-10,-10,+5,+0,+0,+0,+0,+5,-10,-20,-10,-10,-10,-10,-10,-10,-20},
	{+0,+0,+0,+0,+0,+0,+0,+0,+5,+10,+10,+10,+10,+10,+10,+5,-5,+0,+0,+0,+0,+0,+0,-5,-5,+0,+0,+0,+0,+0,+0,-5,-5,+0,+0,+0,+0,+0,+0,-5,-5,+0,+0,+0,+0,+0,+0,-5,-5,+0,+0,+0,+0,+0,+0,-5,+0,+0,+0,+5,+5,+0,+0,+0},
	{-20,-10,-10,-5,-5,-10,-10,-20,-10,+0,+0,+0,+0,+0,+0,-10,-10,+0,+5,+5,+5,+5,+0,-10,-5,+0,+5,+5,+5,+5,+0,-5,+0,+0,+5,+5,+5,+5,+0,-5,-10,+5,+5,+5,+5,+5,+0,-10,-10,+0,+5,+0,+0,+0,+0,-10,-20,-10,-10,-5,-5,-10,-10,-20},
	{+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0,+0}};
int16_t AdjustmentValues[8][9]={{},{},{0,0,0,0,0,0,0,0,0},{-20,-16,-12,-8,-4,0,4,8,12},{0,0,0,0,0,0,0,0,0},{15,12,9,6,3,0,-3,-6,-9},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0}};
int16_t MobilityValues[8]={0,0,1,7,5,5,7,10};
int16_t PieceValues[8]={0,0,100,320,330,500,900,20000};
uint64_t Squares[64];
uint64_t MovesBySquare[64][13];

//initializes Squares and MovesBySquare
void initializer(uint64_t s=1) {
	uint8_t i,j;
   	for(i=0;i<64;++i) {
   		Squares[i]=1ULL<<i;
   		MovesBySquare[i][0]=0XFFULL<<(i&070); //rook
   		MovesBySquare[i][1]=0X0101010101010101ULL<<(i&07); //rook
   		MovesBySquare[i][2]=i%8>i/8?0X8040201008040201ULL>>8*(i%8-i/8):(i%8<i/8?0X8040201008040201ULL<<8*(i/8-i%8):0X8040201008040201ULL); //bishop
   		MovesBySquare[i][3]=i%8+i/8<7?0X0102040810204080ULL>>8*(7-i%8-i/8):(i%8+i/8>7?0X0102040810204080ULL<<8*(i%8+i/8-7):0X0102040810204080ULL); //bishop
   		MovesBySquare[i][4]=i>18?0XA1100110AULL<<(i-18):(i<18?0XA1100110AULL>>(18-i):0XA1100110AULL); //knight
   		MovesBySquare[i][5]=i>9?0X70507ULL<<(i-9):(i<9?0X70507ULL>>(9-i):0X70507ULL); //king
   		if(i>7&&i<56) {
   			MovesBySquare[i][6]=1ULL<<(i+8)|(i/8==1?1ULL<<(i+16):0ULL); //pawn
   			MovesBySquare[i][7]=1ULL<<(i-8)|(i/8==6?1ULL<<(i-16):0ULL); //pawn
   			MovesBySquare[i][8]=5ULL<<(i+7); //pawn
   			MovesBySquare[i][9]=5ULL<<(i-9); //pawn
   		}
   		switch(i&7) {
   			case 0:	MovesBySquare[i][4]&=~0X4040404040404040ULL;
			   		MovesBySquare[i][8]&=~0X8080808080808080ULL;
			   		MovesBySquare[i][9]&=~0X8080808080808080ULL;
   			case 1:	MovesBySquare[i][4]&=~0X8080808080808080ULL;
   					MovesBySquare[i][5]&=~0X8080808080808080ULL;	break;
   			case 7:	MovesBySquare[i][4]&=~0X0202020202020202ULL;
   					MovesBySquare[i][8]&=~0X0101010101010101ULL;
   					MovesBySquare[i][9]&=~0X0101010101010101ULL;
   			case 6:	MovesBySquare[i][4]&=~0X0101010101010101ULL;
					MovesBySquare[i][5]&=~0X0101010101010101ULL;	break;
   		}
		MovesBySquare[i][10]=MovesBySquare[i][0]|MovesBySquare[i][1];
		MovesBySquare[i][11]=MovesBySquare[i][2]|MovesBySquare[i][3];
		MovesBySquare[i][12]=MovesBySquare[i][10]|MovesBySquare[i][11];
   	}
	MovesBySquare[4][5]|=0X44ULL; //white king castles
	MovesBySquare[60][5]|=0X44ULL<<56; //black king castles
}

//returns number of 1 bits in binary
uint8_t count(uint64_t binary) {
	uint8_t count;
	for(count=0;binary;++count)
		binary&=binary-1;
	return count;
}

//returns boardsquare of bitboard
uint8_t position(uint64_t bitboard) {
	uint8_t position;
	for(position=0;bitboard!=1;++position)
		bitboard>>=1;
	return position;
}

//returns new array  a[0] to a[b-1], array c, array a[b] to a[a.length-1], array d
uint32_t *splice(uint32_t *a,uint8_t b,uint32_t *c,uint32_t *d,uint8_t la,uint8_t lc,uint8_t ld) {
	uint32_t *n=(uint32_t *)malloc((la+lc+ld)*sizeof(uint32_t));
	uint8_t i=0;
	for(;i<b;++i)
		n[i]=a[i];
	for(;i<b+lc;++i)
		n[i]=c[i-b];
	for(;i<la+lc;++i)
		n[i]=a[i-lc];
	for(;i<la+lc+ld;++i)
		n[i]=d[i-la-lc];
	return n;
}

//outputs the bit'board'
void boardout(uint64_t board) {
	uint8_t i,j;
	for(i=8;i;--i,putchar('\n'))
		for(j=8;j;--j)
			if(Squares[8*i-j]&board)
				putchar('1');
			else
				putchar('0');
}

//prints decoded segments of 'move'
void movedecoder(uint32_t move) {
	uint32_t fromsquare=move&077,frompiece=move>>6&0XF,tosquare=move>>10&077,topiece=move>>16&0XF,enpaint16_tant=move>>20&077,promote=move>>26&7,castle=move>>29&7;
	printf("%c%1d-%c%1d:",(uint8_t)('a'+(fromsquare&7)),1+(fromsquare>>3),(uint8_t)('a'+(tosquare&7)),1+(tosquare>>3));
	printf("%02d-%02d:%02x:%01x:%01x",(uint8_t)frompiece,(uint8_t)topiece,(uint8_t)enpaint16_tant,(uint8_t)promote,(uint8_t)castle);
}

//returns the maximum of two numebrs 'one' and 'two'
int16_t maximum(int16_t one,int16_t two) {
	if(one>two)
		return one;
	else
		return two;
}
