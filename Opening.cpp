#define MAXMOVES 30
#define MAXINFO 20

#include"Tree.cpp"

typedef struct {
	uint32_t moves[MAXMOVES];
	int16_t info[MAXINFO];
} opening;

//assumes string length < 4*MAXMOVES
//calls movegenrator: must be freed
opening *algebraicopeningcreator(uint8_t *string) {
	opening *_opening=(opening *)malloc(sizeof(opening));
	uint8_t _position,__position,i=0,_count;
	uint32_t *move;
	initializer();
	Engine engine;
	while(string[i<<2]!='\0') {
		_position=__position=0;
		_position|=(string[i<<2|0]-'a');
		_position|=(string[i<<2|1]-'1')<<3;
		__position|=(string[i<<2|2]-'a');
		__position|=(string[i<<2|3]-'1')<<3;
		engine.piecepopulator();
		move=engine.movegenerator(_position,Squares[__position],_count);
		if(_count!=1) {
			free(move);
			free(_opening);
			engine.~Engine();
			return 0;
		}
		_opening->moves[i++]=move[0];
		engine.movemaker(move[0]);
		free(move);
	}
	for(;i<MAXMOVES;++i)
		_opening->moves[i]=0;
	for(i=0;i<MAXINFO;++i)
		_opening->info[i]=0;
	engine.~Engine();
	return _opening;
}

//matches '_opening' with '__opening'
uint32_t openingmatcher(opening *_opening,opening *__opening) {
	uint32_t match=0,i;
	for(i=0;i<MAXMOVES;++i)
		match|=_opening->moves[i]^__opening->moves[i];
	return match;
}

//adds opening 'string' to book 'filename'
uint32_t filewriter(const char *filename,uint8_t *string) {
	FILE *_file=fopen(filename,"rb"),*__file=fopen("temp.dat","wb");
	if(!_file||!__file)
		return 0;
	opening *_opening=algebraicopeningcreator(string),*__opening=(opening *)malloc(sizeof(opening));
	uint32_t match=0,_count=0;
	while(fread(__opening,sizeof(opening),1,_file)==1) {
		match|=!openingmatcher(_opening,__opening);
		fwrite(__opening,sizeof(opening),1,__file);
		_count++;
	}
	if(!match) {
		fwrite(_opening,sizeof(opening),1,__file);
		_count++;
	}
	fclose(_file);
	fclose(__file);
	free(_opening);
	remove(filename);
	rename("temp.dat",filename);
	return _count;
}

//returns count of openings read from 'filename'
uint32_t filereader(const char *filename) {
	FILE *_file=fopen(filename,"rb");
	if(!_file)
		return 0;
	opening *_opening=(opening *)malloc(sizeof(opening));
	uint32_t _count=0;
	while(fread(_opening,sizeof(opening),1,_file)==1)
		_count++;
	fclose(_file);
	free(_opening);
	return _count;
}

//returns count of openings written to book 'filename'
uint32_t openingswriter(const char *filename) {
	FILE *_file=fopen(filename,"r");
	if(!_file)
		return 0;
	opening *_opening=(opening *)malloc(sizeof(opening));
	uint8_t string[4*MAXMOVES+1],_count=0;
	while(fscanf(_file,"%[^\n]\n",string)!=EOF) {
		filewriter("openings.dat",string);
		_count++;
	}
	fclose(_file);
	free(_opening);
	return _count;
}

opening *scoreopening(opening *_opening,uint32_t time) {
	initializer();
	Engine engine;
	uint32_t i,j;
	for(i=0;_opening->moves[i]&&i<MAXMOVES;++i)
		engine.movemaker(_opening->moves[i]);
	engine.piecepopulator();
	Node root(0,0,0,0,_opening->moves[i-1],engine.evaluator(i&1));
	Tree tree(&root,&engine);
	clock_t _time=clock();
	for(j=0;clock()<time+_time;++j)
		_opening->info[j]=tree.deepen(tree.root,i&1);
	tree.debug_printPrincipal();
	engine.~Engine();
	tree.~Tree();
	return _opening;
}
