#include"Engine.cpp"

class Node {

	public:

	Node *up,*down,*left,*right;
	uint32_t move;
	int16_t score;

	//constructor for class Node
	Node(Node *u=0,Node *d=0,Node *l=0,Node *r=0,uint32_t m=0,int16_t s=0) {
		up=u;
		down=d;
		left=l;
		right=r;
		move=m;
		score=s;
	}
};

class Tree {

	public:

	Node *root;
	Engine *engine;

	//constructor for class Tree
	//assumes root and engine are valid
	//assumes engine is initialized at root
	Tree(Node *r,Engine *e) {
		root=r;
		engine=e;
	}

	//grows each nonnull terminal
	void growNode(Node *base,uint8_t colour) {
		engine->piecepopulator();
		uint8_t nodes,i;
		int16_t score=-engine->evaluator(colour);
		uint32_t *moves=engine->sidemovegenerator(colour,nodes);
		if(!nodes) {
			free(moves);
			return;
		}
		base->down=new Node(base,0,0,0,moves[0],score);
		Node *node=base->down;
		for(i=1;i<nodes;++i) {
			node->right=new Node(base,0,node,0,moves[i],score);
			node=node->right;
		}
		free(moves);
	}

	//grows the tree by one depth at each nonnull terminal
	void growTree(Node *base,uint8_t colour) {
		if(!base->down) 
			growNode(base,colour);
		else {
			Node *node=base->down,*temp;
			colour^=1;
			while(node)
				if(!node->move) {
					temp=node->right;
					deleteNode(node);
					node=temp;
				}
				else {
					engine->movemaker(node->move);
					growTree(node,colour);
					engine->movemaker(node->move);
					node=node->right;
				}
		}
	}

	//deletes 'node' and its subtree
	void chopTree(Node *base) {
		Node *node;
		while(base->down) {
			node=base->down;
			chopTree(base->down);
			base->down=node->right;
			if(node->right) {
				node->right->left=0;
			}
			free(node);
		}
	}

	//returns the number of nodes in the Tree at node
	int countNodes(Node *base) {
		if(!base)
			return 0;
		int count=1;
		Node *node=base->down;
		while(node) {
			count+=countNodes(node);
			node=node->right;
		}
		return count;
	}

	//prints the principal variation at Node node
	void debug_printPrincipal() {
		Node *node=root,*newnode=0;
		while(node) {
			newnode=node;
			printf("%+05d:%08x:",newnode->score,newnode->move);
			movedecoder(newnode->move);
			printf("\n");
			node=node->down;
		}
	}

	//allows traversal of entire tree at 'base'
	void debug_traverseTree(Node *base) {
		if(!base)
			return;
		printf("w:0x%08x s:0x%08x a:0x%08x d:0x%08x score:%+05d ",(uint32_t)base->up,(uint32_t)base->down,(uint32_t)base->left,(uint32_t)base->right,base->score);
		movedecoder(base->move);
		uint8_t c;
		scanf("%*c%c",&c);
		switch(c) {
			case 'w':	debug_traverseTree(base->up);		break;
			case 'a':	debug_traverseTree(base->left);		break;
			case 's':	debug_traverseTree(base->down);		break;
			case 'd':	debug_traverseTree(base->right);	break;
		}
	}

	//swaps 'node' with its right sibling
	void swapNode(Node *node) {
		Node *swap=node->right;
		node->right=swap->right;
		if(swap->right)
			swap->right->left=node;
		swap->right=node;
		swap->left=node->left;
		if(node->left)
			node->left->right=swap;
		else //either it has a left or an up guaranteed by tree structure
			node->up->down=swap;
		node->left=swap;
	}

	//deletes 'node'
	void deleteNode(Node *node) {
		if(node->left)
			node->left->right=node->right;
		if(node->right)
			node->right->left=node->left;
		if(node->up&&node->up->down==node)
			node->up->down=node->right;
		free(node);
	}

	//sorts immediately lower level of tree at 'base'
	void sortNode(Node *base) {
		if(!base->down)
			return;
		Node *node=base->down;
		while(node->left)
			swapNode(node->left);
		if(!base->down->right)
			return;
		uint8_t swaps;
		do {
			node=base->down->right;
			swaps=0;
			while(node->right) {
				if(node->right->score>=node->score)
					node=node->right;
				else {
					swapNode(node);
					swaps++;
				}
			}
		} while(swaps);
	}

	//returns best possible score after performing an 'alpha' 'beta' prune on tree at 'base'
	int16_t alphabeta(Node *base,int16_t alpha,int16_t beta) {
		if(!base->down)
			return base->score;
		Node *node=base->down;
		while(node) {
			base->score=-alphabeta(node,-beta,-alpha);
			if(base->score>=beta) {
				base->score=beta;
				sortNode(base);
				return beta;
			}
			if(base->score>alpha) {
				alpha=base->score;
				base->down=node;
			}
			node=node->right;
		}
		sortNode(base);
		base->score=alpha;
		return alpha;
	}

	int16_t deepen(Node *base,uint8_t colour) {
		growTree(base,colour);
		return alphabeta(base,-32676,32676);
	}

	//zero window alpha beta search without prune
	int16_t zwalphabeta(Node *base,int16_t beta) {
		if(!base->down)
			return base->score;
		Node *node=base->down;
		while(node) {
			base->score=-zwalphabeta(node,-beta+1);
			if(base->score>=beta) {
				base->score=beta;
				sortNode(base);
				return beta;
			}
			node=node->right;
		}
		sortNode(base);
		base->score=beta;
		return beta-1;
	}

	//principal variation alpha beta prune with zero window search
	int16_t pvalphabeta(Node *base,int16_t alpha,int16_t beta) {
		if(!base->down)
			return base->score;
		Node *node=base->down;
		bool pvs=true;
		while(node) {
			if(pvs)
				base->score=-pvalphabeta(node,-beta,-alpha);
			else {
				base->score=-zwalphabeta(node,-alpha);
				if(base->score>alpha)
					base->score=-pvalphabeta(node,-beta,-alpha);
			}
			if(base->score>=beta) {
				chopTree(base);
				base->down=0;
				base->move=0;
				base->score=beta;
				return beta;
			}
			if(base->score>alpha) {
				alpha=base->score;
				base->down=node;
				pvs=false;
			}
			node=node->right;
		}
		sortNode(base);
		base->score=alpha;
		return alpha;
	}
};
