#include"Basics.cpp"

class Engine {

	public:

	uint8_t King;
	uint8_t Pawn;
	uint64_t Boards[8];
	uint8_t Positions[16][8];
	int16_t DynamicPieceValues[16][8];
	uint64_t *GeneratedMoves[16][8];

	//constructor for class Engine
	Engine(uint8_t __King=_King,uint8_t __Pawn=_Pawn,uint64_t *__Boards=_Boards) {
		King=__King;
		Pawn=__Pawn;
		Boards[White]=__Boards[White];
		Boards[Black]=__Boards[Black];
		Boards[Pawns]=__Boards[Pawns];
		Boards[Knights]=__Boards[Knights];
		Boards[Bishops]=__Boards[Bishops];
		Boards[Rooks]=__Boards[Rooks];
		Boards[Queens]=__Boards[Queens];
		Boards[Kings]=__Boards[Kings];
		uint8_t i,j;
		for(i=0;i<16;++i)
			for(j=0;j<8;++j) {
				Positions[i][j]=INVALID1BYTE;
				DynamicPieceValues[i][j]=0;
				GeneratedMoves[i][j]=0;
			}
	}

	//returns bitboard with position of smallest piece in board or colour c
	uint64_t smallestpiece(uint64_t board,uint8_t colour,uint8_t &_piece) {
		uint64_t _board;
		for(_piece=Pawns;_piece<=Kings;++_piece) {
			_board=board&Boards[_piece]&Boards[colour];
			if(_board)
				return _board&-_board;
		}
		return 0;
	}

	//returns static exchange score for 'move'
	int16_t staticexchangeevaluator(uint32_t move) {
		uint8_t fromsquare=move&077,frompiece=move>>6&0XF,tosquare=move>>10&077,topiece=move>>16&0XF,enpassant=move>>20&077,promote=move>>26&7,castle=move>>29&7;
		if(!topiece)
			return 0;
		int16_t gain[32];
		uint64_t *_GeneratedMoves[16][8],capturers=0,capturersquare=Squares[fromsquare];
		uint8_t d=0,i,j,k,capturercolour=frompiece>>3,capturerpiece=frompiece;
		for(i=0;i<16;++i)
			for(j=0;j<8&&Positions[i][j]!=INVALID1BYTE;++j) {
				_GeneratedMoves[i][j]=(uint64_t *)malloc(3*sizeof(uint64_t));
				for(k=0;k<3;++k)
					_GeneratedMoves[i][j][k]=GeneratedMoves[i][j][k];
				if(_GeneratedMoves[i][j][1]&Squares[tosquare])
					capturers|=Squares[Positions[i][j]];
			}
		gain[d]=PieceValues[topiece&7];
		do {
			++d;
			gain[d]=PieceValues[capturerpiece&7]-gain[d-1];
			if(maximum(-gain[d-1],gain[d])<0)
				break;
			capturers^=capturersquare;
			for(i=0;i<16;++i)
				for(j=0;j<8&&Positions[i][j]!=INVALID1BYTE;++j) {
					if(Squares[Positions[i][j]]&capturersquare)
						_GeneratedMoves[i][j][0]=_GeneratedMoves[i][j][1]=_GeneratedMoves[i][j][2]=0;
					else if((_GeneratedMoves[i][j][1]&capturersquare)&&(_GeneratedMoves[i][j][2]&Squares[tosquare])) {
						_GeneratedMoves[i][j][1]^=capturersquare|tosquare;
						capturers|=Squares[Positions[i][j]];
					}
				}
			capturercolour^=1;
			capturersquare=smallestpiece(capturers,capturercolour,capturerpiece);
		} while(capturersquare);
		while(--d)
			gain[d-1]=-maximum(-gain[d-1],gain[d]);
		for(i=0;i<16;++i)
			for(j=0;j<8&&Positions[i][j]!=INVALID1BYTE;++j)
				free(_GeneratedMoves[i][j]);
		return gain[0];
	}


	//returns current position score in favour of 'colour'
	//assumes piecepopulator has been called at board state
	int16_t evaluator(uint8_t colour) {
		uint8_t pawns=count(Boards[colour]&Boards[Pawns]),i,j;
		int16_t score=0;
		for(i=(uint8_t)((colour<<3)|Pawns);i&7;++i)
			for(j=0;j<8&&Positions[i][j]!=INVALID1BYTE;++j)
				score+=PieceValues[i&7]+PositionValues[i][Positions[i][j]]+AdjustmentValues[i&7][pawns]+DynamicPieceValues[i][j];
		pawns=count(Boards[colour^1]&Boards[Pawns]);
		for(i=(uint8_t)(((colour^1)<<3)|Pawns);i&7;++i)
			for(j=0;j<8&&Positions[i][j]!=INVALID1BYTE;++j)
				score-=PieceValues[i&7]+PositionValues[i][Positions[i][j]]+AdjustmentValues[i&7][pawns]+DynamicPieceValues[i][j];
		return score;
	}

	//returns bitboards of roamingsquares, firstoccupiedsquares, batteried/xrayedsquares of 'piece' in board 'position'
	//allocates 3 unsigned long long: must be freed
	uint64_t *piecemovegenerator(uint8_t _position,uint8_t _piece) {
		uint64_t *moves=(uint64_t *)malloc(3*sizeof(uint64_t)),board=Boards[0]|Boards[1],_board,__board,___board,____board,_____board,pawns=Boards[2];
		uint8_t i;
		moves[2]=moves[1]=moves[0]=0;
		switch(_piece&7) {
			case Pawns:
				switch(_piece>>3) {
					case White:
						pawns|=(pawns<<8)&(~board);
						pawns|=(pawns<<8)&(~board);
						moves[0]=MovesBySquare[_position][6]&pawns&(~board);
						moves[1]=MovesBySquare[_position][8]&board;
						if(Squares[Pawn]<<8&MovesBySquare[_position][8])
							moves[1]|=Squares[Pawn]<<8;
					break;
					case Black:
						pawns|=(pawns>>8)&(~board);
						pawns|=(pawns>>8)&(~board);
						moves[0]=MovesBySquare[_position][7]&pawns&(~board);
						moves[1]=MovesBySquare[_position][9]&board;	
						if(Squares[Pawn]>>8&MovesBySquare[_position][9])
							moves[1]|=Squares[Pawn]>>8;
					break;
				}
	 		break;
			case Knights:
				moves[0]=MovesBySquare[_position][4]&(~board);
				moves[1]=MovesBySquare[_position][4]&board;
			break;
			case Kings:
				moves[0]=MovesBySquare[_position][5]&(~board);
				moves[1]=MovesBySquare[_position][5]&board;
			break;
			case Bishops: //Bishops, Rooks and Queens not made into separate function despite same code to reduce function call overheads
				_board=Squares[_position];
				__board=0;
				___board=Boards[Bishops]|Boards[Queens];
				____board=board&~Squares[_position];
				for(i=0;i<7;++i) {
					_____board=_board&MovesBySquare[_position][2];
					_____board|=(_____board>>9)|(_____board<<9);
					_____board&=MovesBySquare[_position][2];
					_board|=_____board;
					_____board=_board&MovesBySquare[_position][3];
					_____board|=(_____board>>7)|(_____board<<7);
					_____board&=MovesBySquare[_position][3];
					_board|=_____board;
					_____board=__board&MovesBySquare[_position][2];
					_____board|=(_____board>>9)|(_____board<<9);
					_____board&=MovesBySquare[_position][2];
					__board|=_____board;
					_____board=__board&MovesBySquare[_position][3];
					_____board|=(_____board>>7)|(_____board<<7);
					_____board&=MovesBySquare[_position][3];
					__board|=_____board;
					moves[1]|=_board&____board;
					__board|=_board&(___board)&~Squares[_position];
					__board&=~(_board&~board);
					moves[2]|=__board&____board;
					__board&=~(__board&____board&~___board);
					_board&=~board;
				}
				moves[0]|=_board&~Squares[_position];
				moves[2]&=~moves[1];
			break;
			case Rooks:
				_board=Squares[_position];
				__board=0;
				___board=Boards[Rooks]|Boards[Queens];
				____board=board&~Squares[_position];
				for(i=0;i<7;++i) {
					_____board=_board&MovesBySquare[_position][0];
					_____board|=(_____board>>1)|(_____board<<1);
					_____board&=MovesBySquare[_position][0];
					_board|=_____board;
					_____board=_board&MovesBySquare[_position][1];
					_____board|=(_____board>>8)|(_____board<<8);
					_____board&=MovesBySquare[_position][1];
					_board|=_____board;
					_____board=__board&MovesBySquare[_position][0];
					_____board|=(_____board>>1)|(_____board<<1);
					_____board&=MovesBySquare[_position][0];
					__board|=_____board;
					_____board=__board&MovesBySquare[_position][1];
					_____board|=(_____board>>8)|(_____board<<8);
					_____board&=MovesBySquare[_position][1];
					__board|=_____board;
					moves[1]|=_board&____board;
					__board|=_board&(___board)&~Squares[_position];
					__board&=~(_board&~board);
					moves[2]|=__board&____board;
					__board&=~(__board&____board&~___board);
					_board&=~board;
				}
				moves[0]|=_board&~Squares[_position];
				moves[2]&=~moves[1];
			break;
			case Queens:
				_board=Squares[_position];
				__board=0;
				___board=Boards[Bishops]|Boards[Queens];
				____board=board&~Squares[_position];
				for(i=0;i<7;++i) {
					_____board=_board&MovesBySquare[_position][2];
					_____board|=(_____board>>9)|(_____board<<9);
					_____board&=MovesBySquare[_position][2];
					_board|=_____board;
					_____board=_board&MovesBySquare[_position][3];
					_____board|=(_____board>>7)|(_____board<<7);
					_____board&=MovesBySquare[_position][3];
					_board|=_____board;
					_____board=__board&MovesBySquare[_position][2];
					_____board|=(_____board>>9)|(_____board<<9);
					_____board&=MovesBySquare[_position][2];
					__board|=_____board;
					_____board=__board&MovesBySquare[_position][3];
					_____board|=(_____board>>7)|(_____board<<7);
					_____board&=MovesBySquare[_position][3];
					__board|=_____board;
					moves[1]|=_board&____board;
					__board|=_board&(___board)&~Squares[_position];
					__board&=~(_board&~board);
					moves[2]|=__board&____board;
					__board&=~(__board&____board&~___board);
					_board&=~board;
				}
				moves[0]|=_board&~Squares[_position];
				moves[2]&=~moves[1];
				_board=Squares[_position];
				__board=0;
				___board=Boards[Rooks]|Boards[Queens];
				for(i=0;i<7;++i) {
					_____board=_board&MovesBySquare[_position][0];
					_____board|=(_____board>>1)|(_____board<<1);
					_____board&=MovesBySquare[_position][0];
					_board|=_____board;
					_____board=_board&MovesBySquare[_position][1];
					_____board|=(_____board>>8)|(_____board<<8);
					_____board&=MovesBySquare[_position][1];
					_board|=_____board;
					_____board=__board&MovesBySquare[_position][0];
					_____board|=(_____board>>1)|(_____board<<1);
					_____board&=MovesBySquare[_position][0];
					__board|=_____board;
					_____board=__board&MovesBySquare[_position][1];
					_____board|=(_____board>>8)|(_____board<<8);
					_____board&=MovesBySquare[_position][1];
					__board|=_____board;
					moves[1]|=_board&____board;
					__board|=_board&(___board)&~Squares[_position];
					__board&=~(_board&~board);
					moves[2]|=__board&____board;
					__board&=~(__board&____board&~___board);
					_board&=~board;
				}
				moves[0]|=_board&~Squares[_position];
				moves[2]&=~moves[1];
			break;
		}
		return moves;
	}

	void piecepopulator() {
		uint8_t piececounter[16],_piece,_position,__piece,i,j,k;
		uint64_t board=Boards[White]|Boards[Black],_board;
		for(i=0;i<16;++i) {
			piececounter[i]=0;
			for(j=0;j<8&&Positions[i][j]!=INVALID1BYTE;++j)	{
				Positions[i][j]=INVALID1BYTE;
				free(GeneratedMoves[i][j]); //check if needed later
				GeneratedMoves[i][j]=0;
				DynamicPieceValues[i][j]=0;
			}
		}
		while(board) {
			_position=position(board&-board);
			_piece=piece(_position);
			Positions[_piece][piececounter[_piece]]=_position;
			GeneratedMoves[_piece][piececounter[_piece]]=piecemovegenerator(_position,_piece);
			_board=GeneratedMoves[_piece][piececounter[_piece]][0];
			DynamicPieceValues[_piece][piececounter[_piece]]+=MobilityValues[_piece&7]*count(_board);
			_board=GeneratedMoves[_piece][piececounter[_piece]][1];
			while(_board) {
				__piece=piece(position(_board&-_board));
				if(((_piece>>3)^(__piece>>3))==1)
					DynamicPieceValues[_piece][piececounter[_piece]]+=PieceValues[__piece&7]/40;
				else if((__piece&7)!=7)
					DynamicPieceValues[_piece][piececounter[_piece]]+=PieceValues[__piece&7]/80;
				_board&=_board-1;
			}
			_board=GeneratedMoves[_piece][piececounter[_piece]][2];
			while(_board) {
				__piece=piece(position(_board&-_board));
				if(((_piece>>3)^(__piece>>3))==1)
					DynamicPieceValues[_piece][piececounter[_piece]]+=PieceValues[__piece&7]/60;
				else if((__piece&7)!=7)
					DynamicPieceValues[_piece][piececounter[_piece]]+=PieceValues[__piece&7]/80;
				_board&=_board-1;
			}
			piececounter[_piece]++;
			board&=board-1;
		}
	}

	//checks if castling is legal for '_piece' in '_position'
	//assumes piecepopulator has been called at present board state
	bool castlecheck(uint8_t _piece,uint8_t _position) {
		if(_position&0X4) {
			if(!(King&(0X1<<2*(_piece>>3))))
				return false;
			_position--;
		}
		else {
			if(!(King&(0X2<<2*(_piece>>3))))
				return false;
			_position++;
		}
		uint8_t i,j;
		uint64_t match=0;
		for(i=(uint8_t)((((_piece>>3)^1)<<3)|Pawns);i&7;++i)
			for(j=0;j<8&&Positions[i][j]!=INVALID1BYTE;++j)
				match|=(Squares[_position-1]|Squares[_position]|Squares[_position+1])&(GeneratedMoves[i][j][0]|GeneratedMoves[i][j][1]);
		return(!match);
	}

	//returns move of tosquares 'bitboard' of piece at '_position' and updates reference 'count' to actual number of moves
	//allocates count(bitboard) +/- specialmoves unsigned int: must be freed
	//assumes piecepopulator has been called at present board state
	uint32_t *movegenerator(uint8_t _position,uint64_t bitboard,uint8_t &_count) {
		_count=count(bitboard);
		uint32_t _piece,__position,*moves=(uint32_t *)malloc(_count*sizeof(uint32_t));
		uint8_t i;
		for(i=0;bitboard;++i) {
			_piece=piece(_position);
			moves[i]=_piece<<6|_position; //frompiece and fromsquare
			__position=position(bitboard&-bitboard); //pops last bit
			moves[i]|=((uint32_t)piece(__position)<<6|__position)<<10; //topiece and tosquare
			moves[i]|=(uint32_t)Pawn<<20; //ensuring enpassant reset
			switch(_piece&7) { //special moves
				case Pawns: //special pawn moves
					if(_position==16+__position||__position==16+_position) { //in case of double pawn push implying possible enpassant
						moves[i]^=((uint32_t)__position)<<20;
					}
					if(__position>55||__position<8) { //in case of pawn promotion
						uint32_t *newarray,*promotions=(uint32_t *)malloc(3*sizeof(uint32_t));
						promotions[2]=promotions[1]=promotions[0]=moves[i];
						moves[i]|=Queens<<26;
						promotions[0]|=Rooks<<26;
						promotions[1]|=Bishops<<26;
						promotions[2]|=Knights<<26;
						newarray=splice(moves,0,promotions,0,_count,3,0);
						free(promotions);
						free(moves);
						moves=newarray;
						i+=3;
						_count+=3;
					}
				break;
				case Rooks:
					switch(_position) {
						case 0: //white queenside rook moves
							if(King&0X2) //for first time
								moves[i]|=0X2<<29;
						break;
						case 7:
							if(King&0X1)
								moves[i]|=0X1<<29;
						break;
						case 56:
							if(King&0X8)
								moves[i]|=0X6<<29;
						break;
						case 63:
							if(King&0X4)
								moves[i]|=0X5<<29;
						break;
					}
				break;
				case Kings:
					if((_position+2==__position)||(__position+2==_position))
						if(castlecheck(_piece,__position))
							moves[i]|=((((uint32_t)King>>2*(_piece>>3))&3)|((_piece>>1)&4))<<29;
						else {
							i--;
							_count--;
						}
				break;
			}
			bitboard&=bitboard-1; //resets last bit
		}
		return moves;
	}

	//returns moves of 'colour', captures first
	//assumes piecepopulator has been called at board state
	//calls splice: must be freed
	uint32_t *sidemovegenerator(uint8_t colour,uint8_t &_count) {
		uint32_t *codedmoves=0,*capturesgenerated=0,*noncapturesgenerated=0,*newarraypointer;
		uint64_t *moves=(uint64_t *)malloc(3*sizeof(uint64_t));
		uint8_t i,j,captures,noncaptures,__count;
		for(i=(uint8_t)((colour<<3)|Pawns),_count=__count=0;i&7;++i)
			for(j=0;j<8&&Positions[i][j]!=INVALID1BYTE;++j) {
				moves[0]=GeneratedMoves[i][j][0];
				moves[1]=GeneratedMoves[i][j][1];
				moves[2]=GeneratedMoves[i][j][2];
				switch(i) { //selecting captures from attacks and defenses
					case (White<<3)|Pawns:	moves[1]&=Boards[colour^1]|(Squares[Pawn]<<8);	break;
					case (Black<<3)|Pawns:	moves[1]&=Boards[colour^1]|(Squares[Pawn]>>8);	break;
					default:	moves[1]&=Boards[colour^1];
				}
				capturesgenerated=movegenerator(Positions[i][j],moves[1],captures);
				noncapturesgenerated=movegenerator(Positions[i][j],moves[0],noncaptures);
				newarraypointer=splice(codedmoves,__count,capturesgenerated,noncapturesgenerated,_count,captures,noncaptures);
				if(capturesgenerated) {
					free(capturesgenerated);
					capturesgenerated=0;
				}
				if(noncapturesgenerated) {
					free(noncapturesgenerated);
					noncapturesgenerated=0;
				}
				if(codedmoves) {
					free(codedmoves);
					codedmoves=0;
				}
				codedmoves=newarraypointer;
				_count+=captures+noncaptures;
				__count+=captures;
			}
		free(moves);
		return codedmoves;
	}

	//makes or unmakes 'move'
	void movemaker(uint32_t move) {
		uint32_t fromsquare=move&077,frompiece=move>>6&0XF,tosquare=move>>10&077,topiece=move>>16&0XF,enpassant=move>>20&077,promote=move>>26&7,castle=move>>29&7;
		King^=(castle&3)<<2*(castle>>2); //resets movedside global king flags
		Boards[frompiece&7]^=Squares[fromsquare]|Squares[tosquare]; //resets from piece board
		Boards[frompiece>>3]^=Squares[fromsquare]|Squares[tosquare]; //resets from colour board
		if(topiece) { //if captures
			Boards[topiece&7]^=Squares[tosquare]; //resets to piece board
			Boards[topiece>>3]^=Squares[tosquare]; //resets to colour board
		}
		if(Pawn&&(frompiece==Pawns)&&(tosquare+8==Pawn||Pawn+8==tosquare)&&(fromsquare+1==Pawn||Pawn+1==fromsquare)) { //if enpassant captures
			Boards[Pawns]^=Squares[Pawn]; //resets pawn board
			Boards[(frompiece>>3)^1]^=Squares[Pawn]; //resets colour board
		}
		Pawn^=enpassant; //resets global pawn flags
		if(promote) { //if pawn promotes
			Boards[Pawns]^=Squares[tosquare];
			Boards[promote]^=Squares[tosquare];
		}
		if(((frompiece&7)==Kings)&&((fromsquare==2+tosquare)||(tosquare==2+fromsquare))) //if castles
			switch(castle) {
				case 1:
					Boards[White]^=0XALL<<4; //resets colour board
					Boards[Rooks]^=0XALL<<4; //resets rook board
				break;
				case 2:
					Boards[White]^=0X9LL;
					Boards[Rooks]^=0X9LL;
				break;
				case 3:
					switch(tosquare) {
						case 2:
							Boards[White]^=0X9LL;
							Boards[Rooks]^=0X9LL;
						break;
						case 6:
							Boards[White]^=0XALL<<4;
							Boards[Rooks]^=0XALL<<4;
						break;
					}
				break;
				case 5:
					Boards[Black]^=0XALL<<60;
					Boards[Rooks]^=0XALL<<60;
				break;
				case 6:
					Boards[Black]^=0X9LL<<56;
					Boards[Rooks]^=0X9LL<<56;
				break;
				case 7:
					switch(tosquare) {
						case 58:
							Boards[Black]^=0X9LL<<56;
							Boards[Rooks]^=0X9LL<<56;
						break;
						case 62:
							Boards[Black]^=0XALL<<60;
							Boards[Rooks]^=0XALL<<60;
						break;
					}
				break;
			}
	}

	//returns piece at boardsquare position
	uint8_t piece(uint8_t position) { 
		uint8_t piece=0,i;
		for(i=0;i<8;++i)
			if(Squares[position]&Boards[i]) {
				piece|=i;
				if(i<2)
					piece<<=3;
				else
					return piece;
			}
		return piece;
	}

	//inputs the board
	void debug_boardin() {
		uint32_t p;
		uint8_t i;
		for(i=0;i<8;++i)
			Boards[i]=0;
		for(i=0;i<64;++i)	{
			scanf("%2d",&p);
			if(p&7)	{
				Boards[p&7]|=Squares[i];
				Boards[p>>3]|=Squares[i];
			}
		}
	}

	//desructor for class Engine
	~Engine() {
		uint8_t i,j;
		for(i=0;i<16;++i)
			for(j=0;j<8&&Positions[i][j]!=INVALID1BYTE;++j)	{
				Positions[i][j]=INVALID1BYTE;
				free(GeneratedMoves[i][j]); //check if needed later
				GeneratedMoves[i][j]=0;
				DynamicPieceValues[i][j]=0;
			}
	}
};
