#ifndef DRAUGHTS_
#define DRAUGHTS_

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>


#define WHITE_M 'm'
#define WHITE_K 'k'
#define BLACK_M 'M'
#define BLACK_K 'K'
#define EMPTY ' '

#define BOARD_SIZE 10
#define DEBUG 0
#define MALLOCFAIL 1

typedef char** board_t;
#define WELCOME_TO_DRAUGHTS "Welcome to Draughts!\n"
#define ENTER_SETTINGS "Enter game settings:\n" 
#define WRONG_MINIMAX_DEPTH "Wrong value for minimax depth. The value should be between 1 to 6\n"
#define WRONG_POSITION "Invalid position on the board\n"
#define NO_DICS "The specified position does not contain your piece\n"
#define ILLEGAL_COMMAND "Illegal command, please try again\n"
#define ILLEGAL_MOVE "Illegal move\n"
#define WROND_BOARD_INITIALIZATION "Wrong board initialization\n"

#define ENTER_YOUR_MOVE "Enter your move:\n" 
#define perror_message(func_name) (fprintf(stderr, "Error: standard function %s has failed\n", func_name))
#define print_message(message) (printf("%s", message));


typedef struct ListNode{
	void *data;
	struct ListNode *next;
}ListNode;

typedef struct Position{
	int x;
	int y;
}Position;

typedef struct Move{
	Position *curr;
	ListNode *dest;
	int capNum;
}Move;

typedef struct TreeNode{
	int score;
	Move *move;
	struct TreeNode *parent;
	ListNode *child;
}TreeNode;

extern char gameBoard[BOARD_SIZE][BOARD_SIZE];
extern char userColor;
extern int minimaxDepth;
extern TreeNode* root;
extern int debugCounter;


/****************************************Memory treatment added****************************************/

void print_board(char board[BOARD_SIZE][BOARD_SIZE]);
/*prints board*/

void init_board(void);
/*init board to full board*/

void clearBoard(void);
/*clears the game board to an empty board*/

void copyBoard(char board[BOARD_SIZE][BOARD_SIZE], char boardCopy[BOARD_SIZE][BOARD_SIZE]);
/*copys board to boardCopy (inplace - assumes boardCopy was defined)*/

int islegalInit(void);
/*checks if the initialitzation is legal*/

void playGame(void);
/*updates board according to user and computer moves*/

int computerTurn(void);
/*updates board according to computer's move, returns 1 if the game is over, 0 otherwise*/

int isStuck(char board[BOARD_SIZE][BOARD_SIZE], char color);
/*returns 1 if player with color "color" is stuck, 0 otherwise*/

ListNode* getMoves(char board[BOARD_SIZE][BOARD_SIZE], char color);
/*returns all possible moves for "player"*/

ListNode* getSoldierMoves(char board[BOARD_SIZE][BOARD_SIZE], Position* p, int isFirst, char soldier);
/*returns all possible moves for soldier in position p*/

ListNode* getLastNode(ListNode* node, ListNode* lastNext);
/* returns a pointer to the last node of the given linked list*/

int UpdateMovesList(ListNode* nextMoves, int newCx, int newCy);
/*updates move list as follows - insert <newCx,newCy> as the current position and updates the destination list
(inserts the old curr to be first in destination list)*/

void makeMove(char board[BOARD_SIZE][BOARD_SIZE], Move* move);
/*updated the given board according to the given move*/

int getScore(char board[BOARD_SIZE][BOARD_SIZE], char color);
/*returns the score of the board, according to color*/

void printMoveList(ListNode *moves);
/*prints a list of moves*/

void printPositionList(ListNode *positions);
/*prints a list of positions*/

void printPosition(Position *position);
/* prints one position*/

TreeNode* minimax(char board[BOARD_SIZE][BOARD_SIZE], int depth);
/*returns pointer to root of minmax tree. score field will contain the maximal score,
move field will contain the move that leads to maximal score*/

int getMaxScore(ListNode *TreeNodelist);
/*returns max score of tree nodes. assumes TreeNodeList is not empty*/

int getMinScore(ListNode *TreeNodelist);
/*returns min score of tree nodes.  assumes TreeNodeList is not empty*/

Move* getMoveOfScore(ListNode* moves, ListNode* TreeNodeList, int score);
/*returns a pointer to the move leading to the desired scor
(the scores in TreeNodeList mach to the moves in "moves")*/

Move* copyMove(Move* move);
/*returns a deep clone of move*/

char* getCommand();
/*receives the comand from the user as string*/

int getSettings(void);
/*initializes the global variables minimaxDepth, userColor, board to default and changes it
according to users commands. returns 1 if the game starts, 0 if quit, and -1 if memmory allocation error*/

int userTurn(void);
/*prints possible moves and updates board according to user's commands, returns 1 if the game is over, 0 otherwise*/

int legalPositions(int currX, int currY, char* positionsString);
/*checks if all positions are leagal(<currX,currY> and all positions in positionsString)*/

int legalMove(Position* curr, char* positionsString);
/*checks if the move starting from curr to destinations in positionsString is leagal.
assumes boardGame[curr.x][curr.y] has a disc in it*/

int sameDestinations(ListNode *positionsList, char* positionsString);
/*checks if positionsList has the same positions as in positionsString (also in the same order)*/

Move* constructMove(Position *curr, char* positionsString);
/*returns a Move struct with curr as current position and all destination positions from positionsString*/

ListNode* mallocOneMoveList(int currx, int curry, int destx, int desty, int isCap);
/*creates a list of moves containing one move with one destination*/



/****************************************Memory tested!****************************************/

void freeListOfPositions(ListNode *positionList);
/*frees a list of positions (including everything)*/

void freeMove(Move *move);
/*frees a single move*/

void freeListOfMoves(ListNode *moveList);
/*frees a list of moves (including everything)*/

void freeTree(TreeNode* tree);
/*frees a tree (including everything)*/

void freeListOfTreeNode(ListNode *treeNodeList);
/*frees a list of tree nodes*/


/****************************Tests************************************/

void legalPositionsTest(void);

void constructMoveTest(void);

void sameDestinationsTest(void);

void legalMoveTest(void);

void getMovesTest(void);

void minimaxTest(void);

void getCommandTest(void);

void getScoreTest(void);

void makeMoveTest(void);

ListNode* makePositionList(void);

ListNode* makeMoveList(void);

void printPositionTest(void);

void printPositionListTest(void);

void printMoveListTest(void);

void freeListOfPositionsTest(void);

void freeMoveTest(void);

void freeListOfMovesTest(void);

void freeTreeTest(void);

void computerTurnTest(void);

void userTurnTest(void);

void mallocFailedTest(void);


#endif  

