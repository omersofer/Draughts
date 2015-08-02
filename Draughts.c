#include "Draughts.h"

char gameBoard[BOARD_SIZE][BOARD_SIZE];
char userColor;
int minimaxDepth;
TreeNode* root;
int debugCounter;

int main()
{
	
	print_message(WELCOME_TO_DRAUGHTS);
	init_board();
	int start = getSettings();
	if (start == 1)
		playGame();
	
	//Test();
	//getchar();
}

void print_line(){
	int i;
	printf("  |");
	for (i = 1; i < BOARD_SIZE * 4; i++){
		printf("-");
	}
	printf("|\n");
}

void print_board(char board[BOARD_SIZE][BOARD_SIZE]){
	int i, j;
	print_line();
	for (j = BOARD_SIZE - 1; j >= 0; j--)
	{
		printf((j < 9 ? " %d" : "%d"), j + 1);
		for (i = 0; i < BOARD_SIZE; i++){
			printf("| %c ", board[i][j]);
		}
		printf("|\n");
		print_line();
	}
	printf("   ");
	for (j = 0; j < BOARD_SIZE; j++){
		printf(" %c  ", (char)('a' + j));
	}
	printf("\n");
}

void init_board(void){
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){
			if ((i + j) % 2 == 0){
				if (j <= 3){
					gameBoard[i][j] = WHITE_M;
				}
				else if (j >= 6){
					gameBoard[i][j] = BLACK_M;
				}
				else{
					gameBoard[i][j] = EMPTY;
				}
			}
			else{
				gameBoard[i][j] = EMPTY;
			}
		}
	}
}

void clearBoard(void){
	/*clears the game board to an empty board*/
	for (int i = 0; i < BOARD_SIZE; i++){
		for (int j = 0; j < BOARD_SIZE; j++)
			gameBoard[i][j] = EMPTY;
	}
}

TreeNode* minimax(char board[BOARD_SIZE][BOARD_SIZE], int depth){
	/*returns pointer to root of minmax tree. score field will contain the maximal score,
	move field will contain the move that leads to maximal score*/
	char color;
	char compColor;
	if (userColor == 'w'){
		compColor = 'b';
		if (depth % 2 == 0)
			color = 'b';
		else color = 'w';
	}
	else{//userColor == 'b'
		compColor = 'w';
		if (depth % 2 == 0)
			color = 'w';
		else color = 'b';
	}
	if (depth == minimaxDepth){
		TreeNode *leaf = (TreeNode*)malloc(sizeof(TreeNode));
		if (leaf == NULL){
			perror_message("malloc");
			free(leaf);
			return NULL;
		}
		(*leaf).child = NULL;
		(*leaf).move = NULL;
		(*leaf).parent = NULL;
		(*leaf).score = getScore(board, compColor);
		return leaf;
	}
	else{
		TreeNode *possMove;
		if (depth == 0){
			root = (TreeNode*)malloc(sizeof(TreeNode));
			if (root == NULL){
				perror_message("malloc");
				free(root);
				return NULL;
			}
			(*root).child = NULL;
			(*root).move = NULL;
			(*root).parent = NULL;
			possMove = root;
		}
		else possMove = (TreeNode*)malloc(sizeof(TreeNode));
		if (possMove == NULL){
			perror_message("malloc");
			free(possMove);
			return NULL;
		}
		(*possMove).child = NULL;
		(*possMove).move = NULL;
		(*possMove).parent = NULL;
		ListNode *moves = getMoves(board, color);
		if (moves == NULL){
			freeTree(possMove); //if root was malloced, possMove = root
			return NULL;
		}
		ListNode *currMove = moves;
		if ((*currMove).data == NULL) //empty move list
			currMove = NULL;
		ListNode *firstChild = NULL;
		ListNode *currChild;

		while (currMove != NULL){
			char boardCopy[BOARD_SIZE][BOARD_SIZE];
			copyBoard(board, boardCopy);
			makeMove(boardCopy, (Move*)(*currMove).data);
			if (firstChild == NULL) {
				firstChild = (ListNode*)malloc(sizeof(ListNode));
				if (firstChild == NULL){
					perror_message("malloc");
					free(firstChild);
					freeTree(possMove);
					freeListOfMoves(moves);
					return NULL;
				}
				(*firstChild).data = (TreeNode*)minimax(boardCopy, depth + 1);
				if ((*firstChild).data == NULL){
					free((*firstChild).data);
					free(firstChild);
					freeTree(possMove);
					freeListOfMoves(moves);
					return NULL;
				}
				(*(TreeNode*)(*firstChild).data).parent = possMove;
				(*firstChild).next = NULL;
				currChild = firstChild;
			}
			else{//this is not the first child
				(*currChild).next = (ListNode*)malloc(sizeof(ListNode));
				if ((*currChild).next == NULL){
					perror_message("malloc");
					free((*currChild).next);
					(*currChild).next = NULL;
					(*possMove).child = firstChild;
					freeTree(possMove);
					freeListOfMoves(moves);
					return NULL;
				}
				currChild = (*currChild).next;
				(*currChild).next = NULL;
				(*currChild).data = (TreeNode*)minimax(boardCopy, depth + 1);
				if ((*currChild).data == NULL){
					(*possMove).child = firstChild;
					freeTree(possMove);
					freeListOfMoves(moves);
					return NULL;
				}
				(*(TreeNode*)(*currChild).data).parent = possMove;
			}
			currMove = (*currMove).next;
		}
		(*possMove).child = firstChild;

		if (depth % 2 == 0){
			if ((*possMove).child == NULL)//no move for computer
				(*possMove).score = -100;//coputer lost
			else{
				(*possMove).score = getMaxScore(firstChild);
				//saving the move which led to maximal score
				(*possMove).move = copyMove(getMoveOfScore(moves, firstChild, (*possMove).score));
				if ((*possMove).move == NULL){
					freeTree(possMove);
					freeListOfMoves(moves);
					return NULL;
				}
			}
		}
		else{//depth % 2 != 0
			if ((*possMove).child == NULL)//no moves for user
				(*possMove).score = 100;//user lost, computer won
			else{
				(*possMove).score = getMinScore(firstChild);
				//saving the move which led to minimal score
				(*possMove).move = copyMove(getMoveOfScore(moves, firstChild, (*possMove).score));
				if ((*possMove).move == NULL){
					freeTree(possMove);
					freeListOfMoves(moves);
					return NULL;
				}
			}
		}

		freeListOfMoves(moves);
		return possMove;
	}
}

int getMaxScore(ListNode *TreeNodelist){
	/*returns max score of tree nodes. assumes TreeNodeList is not empty*/
	int max;
	max = (*(TreeNode*)((*TreeNodelist).data)).score;
	TreeNodelist = (*TreeNodelist).next;
	while (TreeNodelist != NULL){
		if ((*(TreeNode*)((*TreeNodelist).data)).score > max)
			max = (*(TreeNode*)((*TreeNodelist).data)).score;
		TreeNodelist = (*TreeNodelist).next;
	}
	return max;
}

int getMinScore(ListNode *TreeNodelist){
	/*returns min score of tree nodes*/
	int min;
	min = (*(TreeNode*)((*TreeNodelist).data)).score;
	TreeNodelist = (*TreeNodelist).next;
	while (TreeNodelist != NULL){
		if ((*(TreeNode*)((*TreeNodelist).data)).score < min)
			min = (*(TreeNode*)((*TreeNodelist).data)).score;
		TreeNodelist = (*TreeNodelist).next;
	}
	return min;
}

Move* getMoveOfScore(ListNode* moves, ListNode* TreeNodeList, int score){
	/*returns a pointer to the move leading to the desired score
	(the scores in TreeNodeList mach to the moves in "moves")*/
	while (moves != NULL){
		if ((*(TreeNode*)(*TreeNodeList).data).score == score){
			return (Move*)((*moves).data);
		}
		moves = (*moves).next;
		TreeNodeList = (*TreeNodeList).next;
	}
	return NULL;
}

Move* copyMove(Move* move){
	/*returns a deep clone of move*/
	Move *result = (Move*)malloc(sizeof(Move));
	if (result == NULL){
		perror_message("malloc");
		free(result);
		return NULL;
	}
	(*result).curr = (Position*)malloc(sizeof(Position));
	if ((*result).curr == NULL){
		perror_message("malloc");
		free((*result).curr);
		free(result);
		return NULL;
	}
	(*(*result).curr).x = (*(*move).curr).x;
	(*(*result).curr).y = (*(*move).curr).y;
	(*result).capNum = (*move).capNum;

	//copying position list
	ListNode* first = (ListNode*)malloc(sizeof(ListNode));
	if (first == NULL){
		perror_message("malloc");
		free(first);
		free((*result).curr);
		free(result);
		return NULL;
	}
	(*first).data = (Position*)malloc(sizeof(Position));
	if ((*first).data == NULL){
		perror_message("malloc");
		free((*first).data);
		free(first);
		free((*result).curr);
		free(result);
		return NULL;
	}
	ListNode *movePos = (*move).dest;
	(*(Position*)(*first).data).x = (*(Position*)(*movePos).data).x;
	(*(Position*)(*first).data).y = (*(Position*)(*movePos).data).y;
	(*first).next = NULL;
	ListNode* current = first;
	movePos = (*movePos).next;

	while (movePos != NULL){
		(*current).next = (ListNode*)malloc(sizeof(ListNode));
		if ((*current).next == NULL){
			perror_message("malloc");
			freeListOfPositions(first);
			free((*result).curr);
			free(result);
			return NULL;
		}
		current = (*current).next;
		(*current).next = NULL;
		(*current).data = (Position*)malloc(sizeof(Position));
		if ((*current).data == NULL){
			perror_message("malloc");
			freeListOfPositions(first);
			free((*result).curr);
			free(result);
			return NULL;
		}
		(*(Position*)(*current).data).x = (*(Position*)(*movePos).data).x;
		(*(Position*)(*current).data).y = (*(Position*)(*movePos).data).y;
		movePos = (*movePos).next;
	}
	(*result).dest = first;
	return result;
}

void copyBoard(char board[BOARD_SIZE][BOARD_SIZE], char boardCopy[BOARD_SIZE][BOARD_SIZE]){
	/*copys board to boardCopy (inplace - assumes boardCopy was defined)*/

	for (int i = 0; i < BOARD_SIZE; i++){
		for (int j = 0; j < BOARD_SIZE; j++){
			boardCopy[i][j] = board[i][j];
		}
	}
}

void makeMove(char board[BOARD_SIZE][BOARD_SIZE], Move* move){
	/*updated the given board according to the given move*/

	Position* currPos = (*move).curr;
	char sold = board[(*currPos).x][(*currPos).y];
	ListNode* posNode = (*move).dest;
	while (posNode != NULL){
		Position* nextPos = (Position*)(*posNode).data;
		if (abs((*nextPos).x - (*currPos).x) > 1){
			int xCap;
			int yCap;
			if ((*nextPos).x > (*currPos).x)
				xCap = (*nextPos).x - 1;
			else xCap = (*nextPos).x + 1;
			if ((*nextPos).y > (*currPos).y)
				yCap = (*nextPos).y - 1;
			else yCap = (*nextPos).y + 1;
			board[xCap][yCap] = EMPTY;
		}
		board[(*currPos).x][(*currPos).y] = EMPTY;
		board[(*nextPos).x][(*nextPos).y] = sold;
		currPos = nextPos;
		posNode = (*posNode).next;
	}
	if (((*currPos).y == 9) && (sold == WHITE_M))
		board[(*currPos).x][(*currPos).y] = WHITE_K;
	if (((*currPos).y == 0) && (sold == BLACK_M))
		board[(*currPos).x][(*currPos).y] = BLACK_K;
}

int getScore(char board[BOARD_SIZE][BOARD_SIZE], char color){
	/*returns the score of the board, according to color*/

	char opColor;
	if (color == 'w')
		opColor = 'b';
	else opColor = 'w';
	if (isStuck(board, color) == 1)
		return -100;
	else if (isStuck(board, opColor) == 1)
		return 100;
	int w = 0;
	int b = 0;
	for (int i = 0; i < BOARD_SIZE; i++){
		for (int j = 0; j < BOARD_SIZE; j++){
			if (board[i][j] == WHITE_M)
				w++;
			else if (board[i][j] == WHITE_K)
				w = w + 3;
			else if (board[i][j] == BLACK_M)
				b++;
			else if (board[i][j] == BLACK_K)
				b = b + 3;
		}
	}
	if (color == 'w')
		return w - b;
	else return b - w;
}

int getSettings(void){
	/*initializes the global variables minimaxDepth, userColor, board to default and changes it
	according to users commands. returns 1 if the game starts, 0 if quit, and -1 if memmory allocation error*/

	minimaxDepth = 1;
	userColor = 'w';
	init_board();
	int start = 0;

	print_message(ENTER_SETTINGS)
		while (!start){

			char* input = getCommand();
			if (input == NULL){
				free(input);
				return -1;
			}

			const char del[2] = " ";
			char* token;
			token = strtok(input, del); // token == command name

			//decode command and call functions
			if (strcmp(token, "minimax_depth") == 0){
				token = strtok(NULL, del); //get the number
				int num = token[0] - '0';
				if ((num < 1) || (num > 6) || (token[1] != '\0'))
					print_message(WRONG_MINIMAX_DEPTH)
				else minimaxDepth = num;
			}
			else if (strcmp(token, "user_color") == 0){
				token = strtok(NULL, del); //get color
				if (strcmp(token, "black") == 0)
					userColor = 'b';
			}
			else if (strcmp(token, "clear") == 0){
				clearBoard();
			}
			else if (strcmp(token, "rm") == 0){
				token = strtok(NULL, del); //get position
				int x = token[1] - 'a';
				int y = token[3] - '1';
				if (token[4] != '>')
					y = ((y + 1) * 10) + token[4] - '1';
				if ((x < 0) || (x > 9) || (y < 0) || (y > 9) || ((x + y) % 2 != 0)){
					print_message(WRONG_POSITION);
				}
				else gameBoard[x][y] = EMPTY;
			}
			else if (strcmp(token, "set") == 0){
				token = strtok(NULL, del); //get position
				int x = token[1] - 'a';
				int y = token[3] - '1';
				if (token[4] != '>')
					y = ((y + 1) * 10) + token[4] - '1';
				if ((x < 0) || (x > 9) || (y < 0) || (y > 9) || ((x + y) % 2 != 0)){
					print_message(WRONG_POSITION);
				}
				else{
					token = strtok(NULL, del); //get white\black
					char color;
					if (strcmp(token, "white") == 0)
						color = 'w';
					else color = 'b';

					token = strtok(NULL, del); //get m\k
					if (strcmp(token, "m") == 0){
						if (color == 'w')
							gameBoard[x][y] = WHITE_M;
						else gameBoard[x][y] = BLACK_M;
					}
					else{ //token == 'k'
						if (color == 'w')
							gameBoard[x][y] = WHITE_K;
						else gameBoard[x][y] = BLACK_K;
					}
				}
			}
			else if (strcmp(token, "print") == 0){
				print_board(gameBoard);
			}
			else if (strcmp(token, "quit") == 0){
				free(input);
				return 0;
			}
			else if (strcmp(token, "start") == 0){
				if (islegalInit())
					start = 1;
				else print_message(WROND_BOARD_INITIALIZATION);
			}
			else print_message(ILLEGAL_COMMAND);
			free(input);
		}
	return 1;
}

int isStuck(char board[BOARD_SIZE][BOARD_SIZE], char color){
	/*returns 1 if player with color "color" is stuck, 0 otherwise*/

	//go through the board and find all moves for relevant player (by color black/white)
	for (int i = 0; i < BOARD_SIZE; i++){
		for (int j = 0; j < BOARD_SIZE; j++){
			if ((i + j) % 2 == 0){
				if (((color == 'w') && ((board[i][j] == WHITE_M) || (board[i][j] == WHITE_K))) || //the color is white and the soldier is white
					((color == 'b') && ((board[i][j] == BLACK_M) || (board[i][j] == BLACK_K)))){ //the color is black and the soldier is black

					//check regular step down&left
					if ((i - 1 >= 0) && (j - 1 >= 0) && (board[i - 1][j - 1] == EMPTY)){
						if (!(board[i][j] == WHITE_M))
							return (0);
					}
					//check regular step down&right
					if ((i + 1 < BOARD_SIZE) && (j - 1 >= 0) && (board[i + 1][j - 1] == EMPTY)){
						if (!(board[i][j] == WHITE_M))
							return (0);
					}
					//check regular step up&left
					if ((i - 1 >= 0) && (j + 1 < BOARD_SIZE) && (board[i - 1][j + 1] == EMPTY)){
						if (!(board[i][j] == BLACK_M))
							return (0);
					}
					//check regular step up&right
					if ((i + 1 < BOARD_SIZE) && (j + 1 < BOARD_SIZE) && (board[i + 1][j + 1] == EMPTY)){
						if (!(board[i][j] == BLACK_M))
							return (0);
					}

					//check check down&left capture position
					if ((i - 1 >= 0) && (j - 1 >= 0)){
						if ((((board[i][j] == WHITE_K) || (board[i][j] == WHITE_M)) && ((board[i - 1][j - 1] == BLACK_M) || (board[i - 1][j - 1] == BLACK_K))) ||
							(((board[i][j] == BLACK_K) || (board[i][j] == BLACK_M)) && ((board[i - 1][j - 1] == WHITE_M) || (board[i - 1][j - 1] == WHITE_K)))){
							if ((i - 2 >= 0) && (j - 2 >= 0)){
								if (board[i - 2][j - 2] == EMPTY)
									return(0);
							}
						}
					}
					//check down&right capture position
					if ((i + 1 < BOARD_SIZE) && (j - 1 >= 0)){
						if ((((board[i][j] == WHITE_K) || (board[i][j] == WHITE_M)) && ((board[i + 1][j - 1] == BLACK_M) || (board[i + 1][j - 1] == BLACK_K))) ||
							(((board[i][j] == BLACK_K) || (board[i][j] == BLACK_M)) && ((board[i + 1][j - 1] == WHITE_M) || (board[i + 1][j - 1] == WHITE_K)))){
							if ((i + 2 < BOARD_SIZE) && (j - 2 >= 0)){
								if (board[i + 2][j - 2] == EMPTY)
									return(0);
							}
						}
					}

					//check up&left capture position
					if ((i - 1 >= 0) && (j + 1 < BOARD_SIZE)){
						if ((((board[i][j] == WHITE_K) || (board[i][j] == WHITE_M)) && ((board[i - 1][j + 1] == BLACK_M) || (board[i - 1][j + 1] == BLACK_K))) ||
							(((board[i][j] == BLACK_K) || (board[i][j] == BLACK_M)) && ((board[i - 1][j + 1] == WHITE_M) || (board[i - 1][j + 1] == WHITE_K)))){
							if ((i - 2 >= 0) && (j + 2 < BOARD_SIZE)){
								if (board[i - 2][j + 2] == EMPTY)
									return(0);
							}
						}
					}

					//check up&right capture position
					if ((i + 1 < BOARD_SIZE) && (j + 1 < BOARD_SIZE)){
						if ((((board[i][j] == WHITE_K) || (board[i][j] == WHITE_M)) && ((board[i + 1][j + 1] == BLACK_M) || (board[i + 1][j + 1] == BLACK_K))) ||
							(((board[i][j] == BLACK_K) || (board[i][j] == BLACK_M)) && ((board[i + 1][j + 1] == WHITE_M) || (board[i + 1][j + 1] == WHITE_K)))){
							if ((i + 2 < BOARD_SIZE) && (j + 2 < BOARD_SIZE)){
								if (board[i + 2][j + 2] == EMPTY)
									return(0);
							}
						}
					}

				}
			}
		}
	}
	return(1); //if did not find any step all over the board- return that stuck
}

void playGame(void){
	/*updates board according to user and computer moves*/
	int GameOver = 0;
	if (userColor == 'w'){
		while (!GameOver){
			GameOver = userTurn();
			if (!GameOver){
				GameOver = computerTurn();
			}
		}
	}
	else{
		while (!GameOver){
			GameOver = computerTurn();
			if (!GameOver){
				GameOver = userTurn();
			}
		}
	}
}

int computerTurn(void){

	char compColor;
	if (userColor == 'w')
		compColor = 'b';
	else
		compColor = 'w';

	TreeNode *root = minimax(gameBoard, 0);
	if (root == NULL){
		return (-1);
	}
	Move *move = (*root).move;
	makeMove(gameBoard, move);
	ListNode* moves = (ListNode*)malloc(sizeof(ListNode));
	if (moves == NULL){
		perror_message("malloc");
		freeTree(root);
		return(-1);
	}
	(*moves).data = move;
	(*moves).next = NULL;

	printf("Computer: move ");
	printMoveList(moves);

	print_board(gameBoard);

	int score = getScore(gameBoard, compColor);
	if (score == 100){
		if (compColor == 'w')
			printf("White player wins!\n");
		else
			printf("Black player wins!\n");
		freeTree(root);
		free(moves);
		return(1);
	}
	freeTree(root);
	free(moves);
	return(0);
}

int UpdateMovesList(ListNode* nextMoves, int newCx, int newCy){
	/*updates move list as follows - insert <newCx,newCy> as the current position and updates the destination list
	(inserts the old curr to be first in destination list)*/
	ListNode *temp = nextMoves;
	while (temp != NULL){
		(*(Move*)(*temp).data).capNum++;
		ListNode* firstDest = (ListNode*)malloc(sizeof(ListNode));
		if (firstDest == NULL){
			perror_message("malloc");
			return(-1);
		}
		(*firstDest).data = (*(Move*)(*temp).data).curr;
		(*firstDest).next = (*(Move*)(*temp).data).dest;
		(*(Move*)(*temp).data).dest = firstDest;
		(*(Move*)(*temp).data).curr = NULL;
		Position* newCurr = (Position*)malloc(sizeof(Position));
		if (newCurr == NULL){
			perror_message("malloc");
			return(-1);
		}
		(*newCurr).x = newCx;
		(*newCurr).y = newCy;
		(*(Move*)(*temp).data).curr = newCurr;
		temp = (*temp).next;
	}
	return (1);
}

ListNode* getLastNode(ListNode* node, ListNode* lastNext){
	/* returns a pointer to the last node of the given linked list*/
	ListNode* temp = node;
	while ((*temp).next != lastNext)
		temp = (*temp).next;
	return temp;
}

ListNode* getMoves(char board[BOARD_SIZE][BOARD_SIZE], char color){
	/*returns all possible moves for "player"*/

	ListNode* moves = (ListNode*)malloc(sizeof(ListNode));
	if (moves == NULL){
		perror_message("malloc");
		return(NULL);
	}
	(*moves).data = NULL;
	(*moves).next = NULL;
	//go through the board and find all moves for relevant player (by color black/white)
	for (int i = 0; i < BOARD_SIZE; i++){
		for (int j = 0; j < BOARD_SIZE; j++){
			if ((i + j) % 2 == 0){
				if (((color == 119) && ((board[i][j] == 107) || (board[i][j] == 109))) || //the color is white and the soldier is white
					((color == 98) && ((board[i][j] == 75) || (board[i][j] == 77)))){ //the color is black and the soldier is blace
					Position* p = (Position*)malloc(sizeof(Position));					
					if (p == NULL){
						perror_message("malloc");
						freeListOfMoves(moves);
						return(NULL);
					}
					(*p).x = i;
					(*p).y = j;
					if ((*moves).data == NULL){
						freeListOfMoves(moves);
						moves = getSoldierMoves(board, p, 1, board[i][j]);
						if (moves == NULL){
							return(NULL);
						}
					}

					else{
						ListNode* last = getLastNode(moves, NULL);
						ListNode* temp = getSoldierMoves(board, p, 1, board[i][j]);
						if (temp == NULL){
							freeListOfMoves(moves);
							return(NULL);
						}
						if ((*temp).data == NULL){
							free(temp);
						}
						else{
							(*last).next = temp;
						}
					}
				}
			}

		}
	}

	// find the maxCapture possible in moves in order to create the possibleMoves list to return
	ListNode* temp = moves;
	int maxCap = 0;
	if ((*temp).data != NULL){
		maxCap = (*((Move*)((*temp).data))).capNum;
		while ((*temp).next != NULL){
			temp = (*temp).next;
			int curCap = (*((Move*)((*temp).data))).capNum;
			if (curCap > maxCap)
				maxCap = curCap;
		}
	}

	// go through the moves found and remove moves with capNum < maxCap

	if (maxCap > 0){
		int foundFirst = 0;
		ListNode* tempP = moves;
		ListNode* removeP;
		while (!foundFirst){
			if ((*((Move*)((*tempP).data))).capNum == maxCap)
				foundFirst = 1;
			else
				tempP = (*tempP).next;
		}
		if (tempP != moves){
			removeP = moves;
			moves = tempP;
			(*getLastNode(removeP, moves)).next = NULL;
			freeListOfMoves(removeP);
		}


		while (((*tempP).next) != NULL){
			if ((*((Move*)((*((*tempP).next)).data))).capNum == maxCap)
				tempP = (*tempP).next;
			else{
				removeP = (*tempP).next;
				(*tempP).next = ((*(*tempP).next)).next;
				(*removeP).next = NULL;
				freeListOfMoves(removeP);
			}
		}
	}

	return moves;
}

ListNode* getSoldierMoves(char board[BOARD_SIZE][BOARD_SIZE], Position* p, int isFirst, char soldier){
	/*returns all possible moves for soldier in position p*/

	int x = (*p).x;
	int y = (*p).y;
	free(p);
	ListNode* result = (ListNode*)malloc(sizeof(ListNode));
	if (result == NULL){
		perror_message("malloc");
		return(NULL);
	}
	(*result).data = NULL;
	(*result).next = NULL;

	if (isFirst){

		if (soldier == WHITE_M){
			if ((x + 1 < BOARD_SIZE) && (y + 1 < BOARD_SIZE) && (board[x + 1][y + 1] == EMPTY)){
				ListNode* temp = mallocOneMoveList(x, y, x + 1, y + 1, 0);
				if (temp == NULL){
					freeListOfMoves(result);
					return (NULL);
				}
				if ((*result).data == NULL){
					freeListOfMoves(result);
					result = temp;
				}
				else{
					(*temp).next = result;
					result = temp;
				}
			}
			if ((x - 1 >= 0) && (y + 1 < BOARD_SIZE) && (board[x - 1][y + 1] == EMPTY)){
				ListNode* temp = mallocOneMoveList(x, y, x - 1, y + 1, 0);
				if (temp == NULL){
					freeListOfMoves(result);
					return (NULL);
				}
				if ((*result).data == NULL){
					freeListOfMoves(result);
					result = temp;
				}
				else{
					(*temp).next = result;
					result = temp;
				}
			}
		}

		if (soldier == BLACK_M){
			if ((x + 1 < BOARD_SIZE) && (y - 1 >= 0) && (board[x + 1][y - 1] == EMPTY)){
				ListNode* temp = mallocOneMoveList(x, y, x + 1, y - 1, 0);
				if (temp == NULL){
					freeListOfMoves(result);
					return (NULL);
				}
				if ((*result).data == NULL){
					freeListOfMoves(result);
					result = temp;
				}
				else{
					(*temp).next = result;
					result = temp;
				}
			}
			if ((x - 1 >= 0) && (y - 1 >= 0) && (board[x - 1][y - 1] == EMPTY)){
				ListNode* temp = mallocOneMoveList(x, y, x - 1, y - 1, 0);
				if (temp == NULL){
					freeListOfMoves(result);
					return (NULL);
				}
				if ((*result).data == NULL){
					freeListOfMoves(result);
					result = temp;
				}
				else{
					(*temp).next = result;
					result = temp;
				}
			}
		}

		if ((soldier == WHITE_K) || (soldier == BLACK_K)){

			//chech down&left diagonal
			int i = x - 1;
			int j = y - 1;
			int stop = 0;
			while ((i >= 0) && (j >= 0) && (!stop)){
				if (board[i][j] == EMPTY){
					ListNode* temp = mallocOneMoveList(x, y, i, j, 0);
					if (temp == NULL){
						freeListOfMoves(result);
						return (NULL);
					}
					if ((*result).data == NULL){
						freeListOfMoves(result);
						result = temp;
					}
					else{
						(*temp).next = result;
						result = temp;
					}
				}
				else{
					stop = 1;
					if ((((soldier == WHITE_K) && ((board[i][j] == BLACK_M) || (board[i][j] == BLACK_K))) ||
						((soldier == BLACK_K) && ((board[i][j] == WHITE_M) || (board[i][j] == WHITE_K))))
						&& (i - 1 >= 0) && (j - 1 >= 0) && (board[i - 1][j - 1] == EMPTY)
						&& (i != x - 1) && (j != y - 1)){
						Position* nextStep = (Position*)malloc(sizeof(Position));						
						if (nextStep == NULL){
							perror_message("malloc");
							freeListOfMoves(result);
							return(NULL);
						}
						(*nextStep).x = i - 1;
						(*nextStep).y = j - 1;
						char boardCopy[BOARD_SIZE][BOARD_SIZE];
						copyBoard(board, boardCopy);
						boardCopy[i][j] = EMPTY;
						boardCopy[x][y] = EMPTY;
						boardCopy[i - 1][j - 1] = soldier;
						ListNode* nextMoves = getSoldierMoves(boardCopy, nextStep, 0, soldier);
						if (nextMoves == NULL){
							freeListOfMoves(result);
							return (NULL);
						}
						if ((*nextMoves).data == NULL){
							freeListOfMoves(nextMoves);
							ListNode* temp = mallocOneMoveList(x, y, i - 1, j - 1, 1);
							if (temp == NULL){
								freeListOfMoves(result);
								return (NULL);
							}
							if ((*result).data == NULL){
								freeListOfMoves(result);
								result = temp;
							}
							else{
								(*temp).next = result;
								result = temp;
							}
						}
						else{
							int res = UpdateMovesList(nextMoves, x, y);
							if (res == -1){
								freeListOfMoves(nextMoves);
								freeListOfMoves(result);
								return (NULL);
							}
							if ((*result).data == NULL){
								freeListOfMoves(result);
								result = nextMoves;
							}
							else{
								ListNode* temp = getLastNode(result, NULL);
								(*temp).next = nextMoves;
							}
						}
					}
				}
				i--;
				j--;
			}


			//chech down&right diagonal
			i = x + 1;
			j = y - 1;
			stop = 0;
			while ((i < BOARD_SIZE) && (j >= 0) && (!stop)){
				if (board[i][j] == EMPTY){
					ListNode* temp = mallocOneMoveList(x, y, i, j, 0);
					if (temp == NULL){
						freeListOfMoves(result);
						return (NULL);
					}
					if ((*result).data == NULL){
						freeListOfMoves(result);
						result = temp;
					}
					else{
						(*temp).next = result;
						result = temp;
					}
				}
				else{
					stop = 1;
					if ((((soldier == WHITE_K) && ((board[i][j] == BLACK_M) || (board[i][j] == BLACK_K))) ||
						((soldier == BLACK_K) && ((board[i][j] == WHITE_M) || (board[i][j] == WHITE_K))))
						&& (i + 1 < BOARD_SIZE) && (j - 1 >= 0) && (board[i + 1][j - 1] == EMPTY)
						&& (i != x + 1) && (j != y - 1)){
						Position* nextStep = (Position*)malloc(sizeof(Position));
						if (nextStep == NULL){
							perror_message("malloc");
							freeListOfMoves(result);
							return(NULL);
						}
						(*nextStep).x = i + 1;
						(*nextStep).y = j - 1;
						char boardCopy[BOARD_SIZE][BOARD_SIZE];
						copyBoard(board, boardCopy);
						boardCopy[i][j] = EMPTY;
						boardCopy[x][y] = EMPTY;
						boardCopy[i + 1][j - 1] = soldier;
						ListNode* nextMoves = getSoldierMoves(boardCopy, nextStep, 0, soldier);
						if (nextMoves == NULL){
							freeListOfMoves(result);
							return (NULL);
						}
						if ((*nextMoves).data == NULL){
							freeListOfMoves(nextMoves);
							ListNode* temp = mallocOneMoveList(x, y, i + 1, j - 1, 1);
							if (temp == NULL){
								freeListOfMoves(result);
								return (NULL);
							}
							if ((*result).data == NULL){
								freeListOfMoves(result);
								result = temp;
							}
							else{
								(*temp).next = result;
								result = temp;
							}
						}
						else{
							int res = UpdateMovesList(nextMoves, x, y);
							if (res == -1){
								freeListOfMoves(nextMoves);
								freeListOfMoves(result);
								return (NULL);
							}
							if ((*result).data == NULL){
								freeListOfMoves(result);
								result = nextMoves;
							}
							else{
								ListNode* temp = getLastNode(result, NULL);
								(*temp).next = nextMoves;
							}
						}
					}
				}
				i++;
				j--;

			}

			//chech up&left diagonal
			i = x - 1;
			j = y + 1;
			stop = 0;
			while ((i >= 0) && (j < BOARD_SIZE) && (!stop)){
				if (board[i][j] == EMPTY){
					ListNode* temp = mallocOneMoveList(x, y, i, j, 0);
					if (temp == NULL){
						freeListOfMoves(result);
						return (NULL);
					}
					if ((*result).data == NULL){
						freeListOfMoves(result);
						result = temp;
					}
					else{
						(*temp).next = result;
						result = temp;
					}
				}
				else{
					stop = 1;
					if ((((soldier == WHITE_K) && ((board[i][j] == BLACK_M) || (board[i][j] == BLACK_K))) ||
						((soldier == BLACK_K) && ((board[i][j] == WHITE_M) || (board[i][j] == WHITE_K))))
						&& (i - 1 >= 0) && (j + 1 < BOARD_SIZE) && (board[i - 1][j + 1] == EMPTY)
						&& (i != x - 1) && (j != y + 1)){
						Position* nextStep = (Position*)malloc(sizeof(Position));
						if (nextStep == NULL){
							perror_message("malloc");
							freeListOfMoves(result);
							return(NULL);
						}
						(*nextStep).x = i - 1;
						(*nextStep).y = j + 1;
						char boardCopy[BOARD_SIZE][BOARD_SIZE];
						copyBoard(board, boardCopy);
						boardCopy[i][j] = EMPTY;
						boardCopy[x][y] = EMPTY;
						boardCopy[i - 1][j + 1] = soldier;
						ListNode* nextMoves = getSoldierMoves(boardCopy, nextStep, 0, soldier);
						if (nextMoves == NULL){
							freeListOfMoves(result);
							return (NULL);
						}
						if ((*nextMoves).data == NULL){
							freeListOfMoves(nextMoves);
							ListNode* temp = mallocOneMoveList(x, y, i - 1, j + 1, 1);
							if (temp == NULL){
								freeListOfMoves(result);
								return (NULL);
							}
							if ((*result).data == NULL){
								freeListOfMoves(result);
								result = temp;
							}
							else{
								(*temp).next = result;
								result = temp;
							}
						}
						else{
							int res = UpdateMovesList(nextMoves, x, y);
							if (res == -1){
								freeListOfMoves(nextMoves);
								freeListOfMoves(result);
								return (NULL);
							}
							if ((*result).data == NULL){
								freeListOfMoves(result);
								result = nextMoves;
							}
							else{
								ListNode* temp = getLastNode(result, NULL);
								(*temp).next = nextMoves;
							}
						}
					}
				}
				i--;
				j++;

			}

			//chech up&right diagonal
			i = x + 1;
			j = y + 1;
			stop = 0;
			while ((i < BOARD_SIZE) && (j < BOARD_SIZE) && (!stop)){
				if (board[i][j] == EMPTY){
					ListNode* temp = mallocOneMoveList(x, y, i, j, 0);
					if (temp == NULL){
						freeListOfMoves(result);
						return (NULL);
					}
					if ((*result).data == NULL){
						freeListOfMoves(result);
						result = temp;
					}
					else{
						(*temp).next = result;
						result = temp;
					}
				}
				else{
					stop = 1;
					if ((((soldier == WHITE_K) && ((board[i][j] == BLACK_M) || (board[i][j] == BLACK_K))) ||
						((soldier == BLACK_K) && ((board[i][j] == WHITE_M) || (board[i][j] == WHITE_K))))
						&& (i + 1 < BOARD_SIZE) && (j + 1 < BOARD_SIZE) && (board[i + 1][j + 1] == EMPTY)
						&& (i != x + 1) && (j != y + 1)){
						Position* nextStep = (Position*)malloc(sizeof(Position));
						if (nextStep == NULL){
							perror_message("malloc");
							freeListOfMoves(result);
							return(NULL);
						}
						(*nextStep).x = i + 1;
						(*nextStep).y = j + 1;
						char boardCopy[BOARD_SIZE][BOARD_SIZE];
						copyBoard(board, boardCopy);
						boardCopy[i][j] = EMPTY;
						boardCopy[x][y] = EMPTY;
						boardCopy[i + 1][j + 1] = soldier;
						ListNode* nextMoves = getSoldierMoves(boardCopy, nextStep, 0, soldier);
						if (nextMoves == NULL){
							freeListOfMoves(result);
							return (NULL);
						}
						if ((*nextMoves).data == NULL){
							freeListOfMoves(nextMoves);
							ListNode* temp = mallocOneMoveList(x, y, i + 1, j + 1, 1);
							if (temp == NULL){
								freeListOfMoves(result);
								return (NULL);
							}
							if ((*result).data == NULL){
								freeListOfMoves(result);
								result = temp;
							}
							else{
								(*temp).next = result;
								result = temp;
							}
						}
						else{
							int res = UpdateMovesList(nextMoves, x, y);
							if (res == -1){
								freeListOfMoves(nextMoves);
								freeListOfMoves(result);
								return (NULL);
							}
							if ((*result).data == NULL){
								freeListOfMoves(result);
								result = nextMoves;
							}
							else{
								ListNode* temp = getLastNode(result, NULL);
								(*temp).next = nextMoves;
							}
						}
					}
				}
				i++;
				j++;
			}
		}
	}
	//Capture moves for (Man) or (king after first capture)
	if ((!((soldier == WHITE_M) && (y == BOARD_SIZE - 1))) && ((!((soldier == BLACK_M) && (y == 0))))){
		//chech down&left capture position
		if ((x - 1 >= 0) && (y - 1 >= 0)){
			if ((((soldier == WHITE_K) || (soldier == WHITE_M)) && ((board[x - 1][y - 1] == BLACK_M) || (board[x - 1][y - 1] == BLACK_K))) ||
				(((soldier == BLACK_K) || (soldier == BLACK_M)) && ((board[x - 1][y - 1] == WHITE_M) || (board[x - 1][y - 1] == WHITE_K)))){
				if ((x - 2 >= 0) && (y - 2 >= 0)){
					if (board[x - 2][y - 2] == EMPTY){
						Position* nextStep = (Position*)malloc(sizeof(Position));
						if (nextStep == NULL){
							perror_message("malloc");
							freeListOfMoves(result);
							return(NULL);
						}
						(*nextStep).x = x - 2;
						(*nextStep).y = y - 2;
						char boardCopy[BOARD_SIZE][BOARD_SIZE];
						copyBoard(board, boardCopy);
						boardCopy[x - 1][y - 1] = EMPTY;
						boardCopy[x][y] = EMPTY;
						boardCopy[x - 2][y - 2] = soldier;
						ListNode* nextMoves = getSoldierMoves(boardCopy, nextStep, 0, soldier);
						if (nextMoves == NULL){
							freeListOfMoves(result);
							return (NULL);
						}
						if ((*nextMoves).data == NULL){
							freeListOfMoves(nextMoves);
							ListNode* temp = mallocOneMoveList(x, y, x - 2, y - 2, 1);
							if (temp == NULL){
								freeListOfMoves(result);
								return (NULL);
							}
							if ((*result).data == NULL){
								freeListOfMoves(result);
								result = temp;
							}
							else{
								(*temp).next = result;
								result = temp;
							}
						}
						else{
							int res = UpdateMovesList(nextMoves, x, y);
							if (res == -1){
								freeListOfMoves(nextMoves);
								freeListOfMoves(result);
								return (NULL);
							}
							if ((*result).data == NULL){
								freeListOfMoves(result);
								result = nextMoves;
							}
							else{
								ListNode* temp = getLastNode(result, NULL);
								(*temp).next = nextMoves;
							}
						}
					}
				}

			}
		}
		//chech down&right capture position
		if ((x + 1 < BOARD_SIZE) && (y - 1 >= 0)){
			if ((((soldier == WHITE_K) || (soldier == WHITE_M)) && ((board[x + 1][y - 1] == BLACK_M) || (board[x + 1][y - 1] == BLACK_K))) ||
				(((soldier == BLACK_K) || (soldier == BLACK_M)) && ((board[x + 1][y - 1] == WHITE_M) || (board[x + 1][y - 1] == WHITE_K)))){
				if ((x + 2 < BOARD_SIZE) && (y - 2 >= 0)){
					if (board[x + 2][y - 2] == EMPTY){
						Position* nextStep = (Position*)malloc(sizeof(Position));
						if (nextStep == NULL){
							perror_message("malloc");
							freeListOfMoves(result);
							return(NULL);
						}
						(*nextStep).x = x + 2;
						(*nextStep).y = y - 2;
						char boardCopy[BOARD_SIZE][BOARD_SIZE];
						copyBoard(board, boardCopy);
						boardCopy[x + 1][y - 1] = EMPTY;
						boardCopy[x][y] = EMPTY;
						boardCopy[x + 2][y - 2] = soldier;
						ListNode* nextMoves = getSoldierMoves(boardCopy, nextStep, 0, soldier);
						if (nextMoves == NULL){
							freeListOfMoves(result);
							return (NULL);
						}
						if ((*nextMoves).data == NULL){
							freeListOfMoves(nextMoves);
							ListNode* temp = mallocOneMoveList(x, y, x + 2, y - 2, 1);
							if (temp == NULL){
								freeListOfMoves(result);
								return (NULL);
							}
							if ((*result).data == NULL){
								freeListOfMoves(result);
								result = temp;
							}
							else{
								(*temp).next = result;
								result = temp;
							}
						}
						else{
							int res = UpdateMovesList(nextMoves, x, y);
							if (res == -1){
								freeListOfMoves(nextMoves);
								freeListOfMoves(result);
								return (NULL);
							}
							if ((*result).data == NULL){
								freeListOfMoves(result);
								result = nextMoves;
							}
							else{
								ListNode* temp = getLastNode(result, NULL);
								(*temp).next = nextMoves;
							}
						}
					}
				}
			}
		}
		//chech up&left capture position
		if ((x - 1 >= 0) && (y + 1 < BOARD_SIZE)){
			if ((((soldier == WHITE_K) || (soldier == WHITE_M)) && ((board[x - 1][y + 1] == BLACK_M) || (board[x - 1][y + 1] == BLACK_K))) ||
				(((soldier == BLACK_K) || (soldier == BLACK_M)) && ((board[x - 1][y + 1] == WHITE_M) || (board[x - 1][y + 1] == WHITE_K)))){
				if ((x - 2 >= 0) && (y + 2 < BOARD_SIZE)){
					if (board[x - 2][y + 2] == EMPTY){
						Position* nextStep = (Position*)malloc(sizeof(Position));
						if (nextStep == NULL){
							perror_message("malloc");
							freeListOfMoves(result);
							return(NULL);
						}
						(*nextStep).x = x - 2;
						(*nextStep).y = y + 2;
						char boardCopy[BOARD_SIZE][BOARD_SIZE];
						copyBoard(board, boardCopy);
						boardCopy[x - 1][y + 1] = EMPTY;
						boardCopy[x][y] = EMPTY;
						boardCopy[x - 2][y + 2] = soldier;
						ListNode* nextMoves = getSoldierMoves(boardCopy, nextStep, 0, soldier);
						if (nextMoves == NULL){
							freeListOfMoves(result);
							return (NULL);
						}
						if ((*nextMoves).data == NULL){
							freeListOfMoves(nextMoves);
							ListNode* temp = mallocOneMoveList(x, y, x - 2, y + 2, 1);
							if (temp == NULL){
								freeListOfMoves(result);
								return (NULL);
							}
							if ((*result).data == NULL){
								freeListOfMoves(result);
								result = temp;
							}
							else{
								(*temp).next = result;
								result = temp;
							}
						}
						else{
							int res = UpdateMovesList(nextMoves, x, y);
							if (res == -1){
								freeListOfMoves(nextMoves);
								freeListOfMoves(result);
								return (NULL);
							}
							if ((*result).data == NULL){
								freeListOfMoves(result);
								result = nextMoves;
							}
							else{
								ListNode* temp = getLastNode(result, NULL);
								(*temp).next = nextMoves;
							}
						}
					}
				}
			}
		}
		//chech up&right capture position
		if ((x + 1 < BOARD_SIZE) && (y + 1 < BOARD_SIZE)){
			if ((((soldier == WHITE_K) || (soldier == WHITE_M)) && ((board[x + 1][y + 1] == BLACK_M) || (board[x + 1][y + 1] == BLACK_K))) ||
				(((soldier == BLACK_K) || (soldier == BLACK_M)) && ((board[x + 1][y + 1] == WHITE_M) || (board[x + 1][y + 1] == WHITE_K)))){
				if ((x + 2 < BOARD_SIZE) && (y + 2 < BOARD_SIZE)){
					if (board[x + 2][y + 2] == EMPTY){
						Position* nextStep = (Position*)malloc(sizeof(Position));
						if (nextStep == NULL){
							perror_message("malloc");
							freeListOfMoves(result);
							return(NULL);
						}
						(*nextStep).x = x + 2;
						(*nextStep).y = y + 2;
						char boardCopy[BOARD_SIZE][BOARD_SIZE];
						copyBoard(board, boardCopy);
						boardCopy[x + 1][y + 1] = EMPTY;
						boardCopy[x][y] = EMPTY;
						boardCopy[x + 2][y + 2] = soldier;
						ListNode* nextMoves = getSoldierMoves(boardCopy, nextStep, 0, soldier);
						if (nextMoves == NULL){
							freeListOfMoves(result);
							return (NULL);
						}
						if ((*nextMoves).data == NULL){
							freeListOfMoves(nextMoves);
							ListNode* temp = mallocOneMoveList(x, y, x + 2, y + 2, 1);
							if (temp == NULL){
								freeListOfMoves(result);
								return (NULL);
							}
							if ((*result).data == NULL){
								freeListOfMoves(result);
								result = temp;
							}
							else{
								(*temp).next = result;
								result = temp;
							}
						}
						else{
							int res = UpdateMovesList(nextMoves, x, y);
							if (res == -1){
								freeListOfMoves(nextMoves);
								freeListOfMoves(result);
								return (NULL);
							}
							if ((*result).data == NULL){
								freeListOfMoves(result);
								result = nextMoves;
							}
							else{
								ListNode* temp = getLastNode(result, NULL);
								(*temp).next = nextMoves;
							}
						}
					}
				}
			}
		}
	}


	return result;
}

int islegalInit(void){
	/*checks if the initialitzation is legal*/

	int black = 0;
	int white = 0;
	for (int i = 0; i < BOARD_SIZE; i++){
		for (int j = 0; j < BOARD_SIZE; j++){
			if ((i + j) % 2 == 0){
				if ((gameBoard[i][j] == WHITE_M) || (gameBoard[i][j] == WHITE_K))
					white++;
				if ((gameBoard[i][j] == BLACK_M) || (gameBoard[i][j] == BLACK_K))
					black++;
			}
		}
	}
	if ((white == 0) || (white > 20) || (black == 0) || (black > 20))
		return 0;
	return 1;
}

char* getCommand(){
	/*receives the comand from the user as string*/

	int maxlen = 10;
	int i = 0;
	char c;
	char* input = (char*)malloc(maxlen * sizeof(char));
	if (input == NULL){
		print_message("malloc");
		free(input);
		return NULL;
	}
	while ((c = getchar()) != '\n'){
		if ((c != '\r')){
			input[i] = c;
			i++;
		}
		if (i == maxlen){ //if input is full, resize input array
			maxlen = maxlen * 2;
			char *temp = (char*)realloc(input, maxlen * sizeof(char));
			if (temp == NULL){
				perror_message("realloc");
				free(input);
				return NULL;
			}
			input = temp;
		}
	}
	input[i] = '\0';
	return input;
}

int userTurn(void){
	/*prints possible moves and updates board according to user's comands*/

	int finished = 0;
	char* input;
	while (!finished){
		print_message(ENTER_YOUR_MOVE);
		input = getCommand();
		if (input == NULL){
			free(input);
			return -1;
		}
		const char del[2] = " ";
		char* token;
		token = strtok(input, del); // token == command name
		//decode command and call functions
		if (strcmp(token, "move") == 0){
			token = strtok(NULL, del); //get the current position
			int currX = token[1] - 'a';
			int currY = token[3] - '1';
			if (token[4] != '>')
				currY = ((currY + 1) * 10) + token[4] - '1';
			token = strtok(NULL, del); //token = "to"
			token = strtok(NULL, del); //get positions list
			if (!legalPositions(currX, currY, token))
				print_message(WRONG_POSITION)
			else{
				if ((userColor == 'w' && (gameBoard[currX][currY] == BLACK_M || gameBoard[currX][currY] == BLACK_K)) ||
					(userColor == 'b' && (gameBoard[currX][currY] == WHITE_M || gameBoard[currX][currY] == WHITE_K)))
					print_message(NO_DICS)
				else{
					Position *curr = (Position*)malloc(sizeof(Position));
					if (curr == NULL){
						perror_message("malloc");
						free(curr);
						free(input);
						return -1;
					}
					(*curr).x = currX;
					(*curr).y = currY;
					if (!legalMove(curr, token)){
						print_message(ILLEGAL_MOVE)
						free(curr);
					}
					else{
						Move *userMove;
						userMove = constructMove(curr, token);
						if (userMove == NULL){
							free(curr);
							free(input);
							return -1;
						}
						makeMove(gameBoard, userMove);
						
						//doesn't free curr, it is inserted into userMove
						freeMove(userMove);

						print_board(gameBoard);

						int score;
						score = getScore(gameBoard, userColor);
						if (score == 100){
							if (userColor == 'w')
								printf("White player wins!\n");
							else
								printf("Black player wins!\n");
							free(input);
							return(1);
						}
						finished = 1;
					}
				}
			}

		}

		else if (strcmp(token, "get_moves") == 0){
			ListNode* moves = getMoves(gameBoard, userColor);
			if (moves == NULL){
				free(input);
				return -1;
			}
			printMoveList(moves);
			freeListOfMoves(moves);
		}

		else if (strcmp(token, "quit") == 0){
			free(input);
			return(1);
		}

		else print_message(ILLEGAL_COMMAND);
		free(input);
	}
	return(0);
}

int legalPositions(int currX, int currY, char* positionsString){
	/*checks if all positions are leagal(<currX,currY> and all positions in positionsString).
	assumes positionsString is in the right format*/

	if (currX < 0 || currX > 9 || currY < 0 || currY > 9 || ((currX + currY) % 2 != 0))
		return 0;

	int i = 0;
	int x;
	int y;
	while (positionsString[i] != '\0'){
		x = positionsString[i + 1] - 'a';
		y = positionsString[i + 3] - '1';
		if (positionsString[i + 4] != '>'){
			y = ((y + 1) * 10) + positionsString[i + 4] - '1';
			i++;
		}
		if (x < 0 || x > 9 || y < 0 || y > 9 || ((x + y) % 2 != 0))
			return 0;
		i = i + 5;
	}
	return 1;
}

int legalMove(Position *curr, char* positionsString){
	/*checks if the move starting from curr to destinations in positionsString is leagal.
	assumes boardGame[curr.x][curr.y] has a disc in it*/

	ListNode *moveList = getMoves(gameBoard, userColor);
	if (moveList == NULL)
		return -1;
	ListNode *tempMove = moveList;
	if ((*tempMove).data == NULL)//empty move list
		tempMove = NULL;
	while (tempMove != NULL){
		Move *move = (Move*)(*tempMove).data;
		if (((*curr).x == (*(*move).curr).x) && ((*curr).y == (*(*move).curr).y) &&
			sameDestinations((*move).dest, positionsString)){
			freeListOfMoves(moveList);
			return 1;
		}
		tempMove = (*tempMove).next;
	}
	freeListOfMoves(moveList);
	return 0;
}

int sameDestinations(ListNode *positionsList, char* positionsString){
	/*checks if positionsList has the same positions as in positionsString (also in the same order)*/

	ListNode *temp = positionsList;
	Position *p;
	int i = 0;
	int x;
	int y;
	while (temp != NULL && (positionsString[i] != '\0')){
		p = (Position*)(*temp).data;
		x = positionsString[i + 1] - 'a';
		y = positionsString[i + 3] - '1';
		if (positionsString[i + 4] != '>'){
			y = ((y + 1) * 10) + positionsString[i + 4] - '1';
			i++;
		}
		if ((x != (*p).x) || (y != (*p).y))
			return 0;
		temp = (*temp).next;
		i = i + 5;
	}
	if (temp == NULL && positionsString[i] == '\0')
		return 1;
	else return 0;
}

Move* constructMove(Position *curr, char* positionsString){
	/*returns a Move struct with curr as current position and all destination positions from positionsString*/

	int i = 0;

	Move *result = (Move*)malloc(sizeof(Move));
	if (result == NULL){
		perror_message("malloc");
		free(result);
		return NULL;
	}
	(*result).curr = curr;
	ListNode* dest = (ListNode*)malloc(sizeof(ListNode));
	if (dest == NULL){
		perror_message("malloc");
		free(dest);
		free(result);
		return NULL;
	}
	(*dest).data = (Position*)malloc(sizeof(Position));
	if ((*dest).data == NULL){
		perror_message("malloc");
		free((*dest).data);
		free(dest);
		free(result);
		return NULL;
	}
	(*(Position*)(*dest).data).x = positionsString[1] - 'a';
	(*(Position*)(*dest).data).y = positionsString[3] - '1';
	if (positionsString[4] != '>'){
		(*(Position*)(*dest).data).y = (((*(Position*)(*dest).data).y + 1) * 10) + positionsString[4] - '1';
		i++;
	}
	(*dest).next = NULL;
	(*result).dest = dest;
	ListNode *temp = dest;

	i = i + 5;
	while (positionsString[i] != '\0'){
		(*temp).next = (ListNode*)malloc(sizeof(ListNode));
		if ((*temp).next == NULL){
			perror_message("malloc");
			freeListOfPositions(dest);
			free(result);
			return NULL;
		}
		temp = (*temp).next;
		(*temp).next = NULL;
		(*temp).data = (Position*)malloc(sizeof(Position));
		if ((*temp).data == NULL){
			perror_message("malloc");
			freeListOfPositions(dest);
			free(result);
			return NULL;
		}
		(*(Position*)(*temp).data).x = positionsString[i + 1] - 'a';
		(*(Position*)(*temp).data).y = positionsString[i + 3] - '1';
		if (positionsString[i + 4] != '>'){
			(*(Position*)(*temp).data).y = (((*(Position*)(*temp).data).y + 1) * 10) + positionsString[i + 4] - '1';
			i++;
		}
		i = i + 5;
	}
	return result;
}

ListNode* mallocOneMoveList(int currx, int curry, int destx, int desty, int isCap){
	/*creates a list of moves containing one move with one destination*/
	ListNode* temp = (ListNode*)malloc(sizeof(ListNode));
	if (temp == NULL){
		perror_message("malloc");
		free(temp);
		return NULL;
	}
	(*temp).next = NULL;
	(*temp).data = (Move*)malloc(sizeof(Move));
	if ((*temp).data == NULL){
		perror_message("malloc");
		free((*temp).data);
		free(temp);
		return NULL;
	}
	if (isCap)
		(*(Move*)((*temp).data)).capNum = 1;
	else (*(Move*)((*temp).data)).capNum = 0;
	(*(Move*)((*temp).data)).curr = (Position*)malloc(sizeof(Position));
	if ((*(Move*)((*temp).data)).curr == NULL){
		perror_message("malloc");
		free((*(Move*)((*temp).data)).curr);
		free((*temp).data);
		free(temp);
		return NULL;
	}
	(*(*(Move*)((*temp).data)).curr).x = currx;
	(*(*(Move*)((*temp).data)).curr).y = curry;
	(*(Move*)((*temp).data)).dest = (ListNode*)malloc(sizeof(ListNode));
	if ((*(Move*)((*temp).data)).dest == NULL){
		perror_message("malloc");
		free((*(Move*)((*temp).data)).dest);
		free((*(Move*)((*temp).data)).curr);
		free((*temp).data);
		free(temp);
		return NULL;
	}
	(*(*(Move*)((*temp).data)).dest).next = NULL;
	(*(*(Move*)((*temp).data)).dest).data = (Position*)malloc(sizeof(Position));
	if ((*(*(Move*)((*temp).data)).dest).data == NULL){
		perror_message("malloc");
		free((*(*(Move*)((*temp).data)).dest).data);
		free((*(Move*)((*temp).data)).dest);
		free((*(Move*)((*temp).data)).curr);
		free((*temp).data);
		free(temp);
		return NULL;
	}
	(*(Position*)(*(*(Move*)((*temp).data)).dest).data).x = destx;
	(*(Position*)(*(*(Move*)((*temp).data)).dest).data).y = desty;
	return temp;
}

void freeListOfPositions(ListNode *positionList){
	/*frees a list of positions (including everything)*/

	if (positionList != NULL){
		freeListOfPositions((*positionList).next);
		free((*positionList).data);
		free(positionList);
	}
}

void freeMove(Move *move){
	/*frees a single move*/
	if (move != NULL){
		if ((*move).curr != NULL)
			free((*move).curr);
		freeListOfPositions((*move).dest);
		free(move);
	}
}

void freeListOfMoves(ListNode *moveList){
	/*frees a list of moves (including everything)*/

	if (moveList != NULL){
		freeListOfMoves((*moveList).next);
		freeMove((Move*)(*moveList).data);
		free(moveList);
	}
}

void freeTree(TreeNode* tree){
	/*frees a tree (including everything)*/
	if (tree != NULL){
		if ((*tree).child != NULL)
			freeListOfTreeNode((*tree).child);
		if ((*tree).move != NULL)
			freeMove((*tree).move);
		free(tree);
	}
}

void freeListOfTreeNode(ListNode *treeNodeList){
	/*frees a list of tree nodes*/

	if (treeNodeList != NULL){
		freeListOfTreeNode((*treeNodeList).next);
		freeTree((TreeNode*)(*treeNodeList).data);
		free(treeNodeList);
	}
}

void printPosition(Position *position){
	/* prints one position*/
	printf("<%c,%d>", (*position).x + 'a', (*position).y + 1);
}

void printPositionList(ListNode *positions){
	/*prints a list of positions*/

	while (positions != NULL){
		Position *position = (Position*)(*positions).data;
		printPosition(position);
		positions = (*positions).next;
	}
	printf("\n");
}

void printMoveList(ListNode *moves){
	/*prints a list of moves*/

	if ((*moves).data == NULL){//empty move list
		return;
	}

	while (moves != NULL){
		Move *move = (Move*)(*moves).data;
		printPosition((Position*)((*move).curr));
		printf(" to ");
		printPositionList((*move).dest);
		moves = (*moves).next;
	}
}


/************************************Tests******************************************/

void legalPositionsTest(void){
	printf("%d\n", legalPositions(1, 1, "<d,10><h,2>"));
	printf("%d\n", legalPositions(9, 1, "<d,10><h,2>"));
	printf("%d\n", legalPositions(0, 4, "<d,11><h,2>"));
	printf("%d\n", legalPositions(1, 1, "<b,1><k,2>"));
}

void sameDestinationsTest(void){
	clearBoard();
	gameBoard[9][9] = BLACK_M;
	gameBoard[8][8] = WHITE_M;
	gameBoard[6][8] = WHITE_M;
	print_board(gameBoard);

	Position *p = (Position*)malloc(sizeof(Position));
	(*p).x = 9;
	(*p).y = 9;

	ListNode* moveList = getSoldierMoves(gameBoard, p, 1, gameBoard[(*p).x][(*p).y]);
	printMoveList(moveList);
	Move* move = (Move*)((*moveList).data);
	char *s = "<h,8><f,10>\0";
	printf("%d", sameDestinations((*move).dest, s));
}

void legalMoveTest(void){
	clearBoard();
	gameBoard[1][5] = WHITE_M;
	gameBoard[2][6] = BLACK_M;
	gameBoard[4][6] = BLACK_M;
	print_board(gameBoard);

	Position *p = (Position*)malloc(sizeof(Position));
	(*p).x = 1;
	(*p).y = 5;

	char *s = "<d,8><f,6>\0";
	printf("%d", legalMove(p, s));

}

void constructMoveTest(void){

	Position* curr = (Position*)malloc(sizeof(Position));
	(*curr).x = 9;
	(*curr).y = 9;

	char *s = "<h,8><f,10>\0";

	Move* move = constructMove(curr, s);
	ListNode* moveList = (ListNode*)malloc(sizeof(ListNode));
	(*moveList).data = move;
	(*moveList).next = NULL;
	printMoveList(moveList);
}

void getCommandTest(void){
	char* s;
	s = getCommand();
	print_message(s)
}

void getMovesTest(void){
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){
			gameBoard[i][j] = EMPTY;
		}
	}
	gameBoard[0][0] = WHITE_M;
	gameBoard[1][1] = BLACK_M;
	gameBoard[3][1] = BLACK_M;

	print_board(gameBoard);

	//Position *p = (Position*)malloc(sizeof(Position));
	//(*p).x = 1;
	//(*p).y = 1;
	ListNode *moves = getMoves(gameBoard, 'w');
	printMoveList(moves);
	freeListOfMoves(moves);

}

void isStuckTest(void){

	clearBoard();
	//gameBoard[0][6] = BLACK_M;
	//gameBoard[1][5] = BLACK_M;
	gameBoard[0][0] = BLACK_K;
	gameBoard[2][4] = WHITE_M;
	gameBoard[2][2] = WHITE_M;
	gameBoard[1][1] = WHITE_M;

	//gameBoard[7][1] = BLACK_M;
	gameBoard[6][2] = WHITE_M;


	print_board(gameBoard);

	printf("isStuck on white - %d", isStuck(gameBoard, 'w'));
}

void minimaxTest(void){
	minimaxDepth = 3;
	userColor = 'w';

	clearBoard();
	gameBoard[0][6] = BLACK_M;
	gameBoard[1][5] = BLACK_M;
	gameBoard[2][4] = WHITE_M;
	gameBoard[2][2] = WHITE_M;
	gameBoard[1][1] = WHITE_M;

	gameBoard[7][1] = BLACK_M;
	gameBoard[6][2] = WHITE_M;


	print_board(gameBoard);

	//ListNode *moves = getMoves(gameBoard, 'b');
	//printMoveList(moves);

	TreeNode *root = minimax(gameBoard, 0);
	Move *move = (*root).move;
	makeMove(gameBoard, move);

	print_board(gameBoard);
	freeTree(root);
}

void getScoreTest(void){
	clearBoard();

	gameBoard[0][0] = WHITE_M;
	gameBoard[1][1] = BLACK_M;
	gameBoard[2][2] = BLACK_K;

	print_board(gameBoard);

	int score;
	score = getScore(gameBoard, 'b');
	printf("score for black = %d\n", score);
}

void makeMoveTest(void){
	clearBoard();

	gameBoard[1][5] = WHITE_M;
	gameBoard[2][6] = BLACK_M;
	gameBoard[4][8] = BLACK_M;

	print_board(gameBoard);

	//Position *p = (Position*)malloc(sizeof(Position));
	//(*p).x = 3;
	//(*p).y = 3;
	ListNode *moves = getMoves(gameBoard, 'w');
	Move *move = (Move*)(*moves).data;

	makeMove(gameBoard, move);
	print_board(gameBoard);

}

ListNode* makePositionList(void){
	ListNode *node = (ListNode*)malloc(sizeof(ListNode));
	ListNode *head = node;
	for (int i = 0; i < 5; i++){
		(*node).data = (Position*)malloc(sizeof(Position));
		Position *pos = (Position*)(*node).data;
		(*pos).x = i;
		(*pos).y = i;
		if (i != 4){
			(*node).next = (ListNode*)malloc(sizeof(ListNode));
			node = (*node).next;
		}
		else (*node).next = NULL;
	}
	return head;
}

ListNode* makeMoveList(void){
	ListNode *node = (ListNode*)malloc(sizeof(ListNode));
	ListNode *head = node;
	for (int i = 0; i < 5; i++){
		(*node).data = (Move*)malloc(sizeof(Move));
		Move *move = (Move*)(*node).data;
		(*move).curr = (Position*)malloc(sizeof(Position));
		(*(*move).curr).x = i;
		(*(*move).curr).y = i;
		(*move).dest = makePositionList();
		if (i != 4){
			(*node).next = (ListNode*)malloc(sizeof(ListNode));
			node = (*node).next;
		}
		else (*node).next = NULL;
	}
	return head;
}

void printPositionTest(void){
	Position* pos = (Position*)malloc(sizeof(Position));
	(*pos).x = 3;
	(*pos).y = 5;
	printPosition(pos);
}

void printPositionListTest(void){
	ListNode *head = makePositionList();
	printPositionList(head);
}

void printMoveListTest(void){
	ListNode *head = makeMoveList();
	printMoveList(head);
}

void freeListOfPositionsTest(void){
	ListNode *head = makePositionList();
	printPositionList(head);
	freeListOfPositions(head);
}

void freeMoveTest(void){

	Move* move = (Move*)malloc(sizeof(Move));
	(*move).curr = (Position*)malloc(sizeof(Position));
	(*(*move).curr).x = 0;
	(*(*move).curr).y = 0;

	(*move).dest = makePositionList();

	freeMove(move);

}

void freeListOfMovesTest(void){
	ListNode *head = makeMoveList();
	printMoveList(head);
	freeListOfMoves(head);
}

void freeTreeTest(void){

	minimaxDepth = 2;
	userColor = 'w';

	clearBoard();
	gameBoard[0][6] = BLACK_M;
	gameBoard[1][5] = BLACK_M;
	gameBoard[2][4] = WHITE_M;
	gameBoard[2][2] = WHITE_M;
	gameBoard[1][1] = WHITE_M;

	gameBoard[7][1] = BLACK_M;
	gameBoard[6][2] = WHITE_M;


	print_board(gameBoard);

	//ListNode *moves = getMoves(gameBoard, 'b');
	//printMoveList(moves);

	TreeNode *root = minimax(gameBoard, 0);
	Move *move = (*root).move;
	makeMove(gameBoard, move);

	print_board(gameBoard);
	freeTree(root);
}

void computerTurnTest(void){
	minimaxDepth = 3;
	userColor = 'w';

	clearBoard();
	gameBoard[0][6] = BLACK_M;
	gameBoard[1][5] = BLACK_M;
	gameBoard[2][4] = WHITE_M;
	gameBoard[2][2] = WHITE_M;
	gameBoard[1][1] = WHITE_M;

	gameBoard[7][1] = BLACK_M;
	gameBoard[6][2] = WHITE_M;


	print_board(gameBoard);

	//ListNode *moves = getMoves(gameBoard, 'b');
	//printMoveList(moves);

	computerTurn();
}

void userTurnTest(void){
	minimaxDepth = 3;
	userColor = 'b';

	clearBoard();
	gameBoard[0][6] = BLACK_M;
	gameBoard[1][5] = BLACK_M;
	gameBoard[2][4] = WHITE_M;
	gameBoard[2][2] = WHITE_M;
	gameBoard[1][1] = WHITE_M;

	gameBoard[7][1] = BLACK_M;
	gameBoard[6][2] = WHITE_M;


	print_board(gameBoard);

	//ListNode *moves = getMoves(gameBoard, 'b');
	//printMoveList(moves);

	userTurn();
}

void mallocFailedTest(void){
	minimaxDepth = 4;
	userColor = 'w';

	clearBoard();

	gameBoard[8][2] = WHITE_K;
	gameBoard[3][7] = WHITE_K;
	gameBoard[5][1] = WHITE_K;
	gameBoard[1][5] = WHITE_K;
	gameBoard[5][5] = BLACK_M;
	gameBoard[3][5] = BLACK_M;
	gameBoard[3][3] = BLACK_M;
	gameBoard[5][3] = BLACK_M;
	gameBoard[6][6] = WHITE_M;
	gameBoard[2][6] = WHITE_M;
	gameBoard[0][0] = WHITE_M;
	gameBoard[2][0] = WHITE_M;
	gameBoard[1][1] = BLACK_M;

	//gameBoard[3][7] = WHITE_K;
	//gameBoard[1][5] = BLACK_M;
	//gameBoard[7][7] = BLACK_M;
	//gameBoard[9][9] = BLACK_K;
	//gameBoard[7][3] = BLACK_K;


	debugCounter = 0;
	//if (DEBUG){
	//	debugCounter++;
	//	if (debugCounter == MALLOCFAIL){
	//		free(nextStep);
	//		nextStep = NULL;
	//	}
	//}


	print_board(gameBoard);

	playGame();
}