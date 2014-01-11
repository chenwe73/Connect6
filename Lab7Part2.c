//*****************************************************//
// Name: Wei Tao (Tommy) Chen                          //
// Student Number: 1000527290                          //
// Description: AI for connect6 game                   //
// Program features:                                   //
//		two dimensional logarithmic scoring system     //
//		alpha-beta prunning                            //
//		threat-driven critical position selection      //
//		parrallel defence-offence algorithm            //
//*****************************************************//

//#include "lab7_gfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>

// prototypes (from lab6)
char** init(int n);
void freeBoard(char** board, int n);
void printBoard(char **board, int n);
// isStone changed
// findLongest changed
int winner(char** board, int n);
bool placeStone(char** board, int n, int row, int col, char stone);

// prototypes (from Part1)
bool isBoardFilled(char** board, int n);
// computerMove changed
char notStone(char stone);
double minimax(char** board, int n, int depth, int maxDepth, char stone); // changed

// new prototypes
double** initBaseScore(int n);
double score(char **board, int n, int row, int col, char stone);
double seqScore (int connect, int span, int bound);
void printScore(char **board, int n, char stone);
double findMaxScoreBoard(char **board, int n, char stone, int *i, int *j);

bool dirCheck(char **board, int n, int row, int col, int i, int direction, char stone);
int longest(char **board, int n, int row, int col, int direction);
int longest90(char **board, int n, int row, int col);
int longest0(char **board, int n, int row, int col);
int longest45(char **board, int n, int row, int col);
int longest135(char **board, int n, int row, int col);
int findLongest(char **board, int n, int row, int col);
int findLongestBoard(char **board, int n);

bool dirSpanCheck(char **board, int n, int row, int col, int i, int direction, char stone);
int longestSpan(char **board, int n, int row, int col, int direction);
int longestSpan90(char **board, int n, int row, int col);
int longestSpan0(char **board, int n, int row, int col);
int longestSpan45(char **board, int n, int row, int col);
int longestSpan135(char **board, int n, int row, int col);
int findLongestSpan(char **board, int n, int row, int col);

bool isBounded(char** board, int n, int row, int col, int direction);
int bound90(char** board, int n, int row, int col);
int bound0(char** board, int n, int row, int col);
int bound45(char** board, int n, int row, int col);
int bound135(char** board, int n, int row, int col);

void combinedInfo(char **board, int n, int row, int col, int direction, int *l, int *ls, int *b);
void combined (char** board, int n, int row, int col, int direction, int *l, int *ls, int *b);
//int gap90(char** board, int n, int row, int col); // unfinished

bool isInbound(char** board, int n, int row, int col);
bool isStone(char** board, int n, int row, int col, char stone);
bool isNotStone(char** board, int n, int row, int col, char stone);
bool randBoard(char** board, int n);
bool isCritical (char** board, int n, int row, int col);
bool radiusCheck (char** board, int n, int row, int col, int i);

double alphaBeta(char** board, int n, int row, int col, int depth, const int MAXDEPTH, char stone, bool maximizing, double value, double min, double max, bool threatSelect);
double computerMove(char** board, int n, char stone);
double computerMove2(char** board, int n, char stone);

void centerMove(char** board, int n, char stone);
void startingMoveWhite(char** board, int n, int i, int j);

// global variables
const int CONNECT = 6;
const bool DEBUG = false;
const char WHITE = 'W';
const char BLACK = 'B';
const char UNOCCUPIED= 'U';
const int MAXINT = 999999;
const int MININT = -999999;
const double THREATLIMIT = 0.9999/5;
const bool PRUNNING = false;
const int MAXDEPTH = 6;

struct rusage usage; // a structure to hold "resource usage" (including time)
struct timeval start, end; // will hold the start and end times

double **baseScore;
const double MAXBASESCORE = 1.0 / 390625;

double time_start;
double time_end;
bool timeOut = false;
const double TIMELIMIT = 0.95;

// main function
int main(void)
{
	//initLab7Gfx();
	// declearing variables
	int n = 19;
	char** board;
	char stone;
	int row, col;
	char computer = BLACK;
	char human = WHITE;


	// inputting dimension
	if (!DEBUG)
	{
		printf("Enter board dimensions (n): ");
		scanf("%d", &n);
	}
	board = init(n);
	printBoard(board, n);
	baseScore = initBaseScore(n);

	if (DEBUG)
	{
		printf("board setup\n");	
		//....................................
		//board[5][13] = BLACK;
		//board[5][12] = BLACK;
		//board[5][11] = BLACK;
		//board[5][10] = BLACK;
		
		printBoard(board, n);
		printf("\n");
		//....................................
		printScore(board, n, WHITE);
		printf("\n");
		//computerMove(board, n, BLACK);
		

		
		// random board
		/*while (true)
		{
			n = rand() % 14 + 6;
			board = init(n);
			printf("%d\n", n);
			if (randBoard (board, n) == true)
			{
				printBoard(board, n);
				printf("\n");
				printScore(board, n, computer);
			}
			freeBoard(board, n);
			printf("\n");
		}*/

		int i = 0;
		char temp;
		double maxTime = 0;
		double time;
		int turnMaxTime;
		// computer vs. computer
		board[n/2][n/2] = BLACK;
		printBoard(board, n);
		while (!(isBoardFilled(board, n) || winner(board, n) != 0))
		//for (int i = 0; i < 48;)
		{
			printf("*****************************************************************************\n");
			if (i % 2 == 0)
			{
				printf("White: %d\n", i);
				time = computerMove2(board, n, WHITE);
			}
			else
			{
				printf("Black: %d\n", i);
				time = computerMove(board, n, BLACK);
			}
			
			printf("Time: %lf(s)\n", time);
			if (time > maxTime)
			{
				maxTime = time;
				turnMaxTime = i;
			}

			//scanf("%c", &temp);
			i++;
		}
		printf("Longest Time: %lf(s) at %d\n", maxTime, turnMaxTime);
	}
	
	// inputting computer stone color
	printf("Computer playing B or W?: ");
	scanf(" %c", &stone);
	if (stone == 'B')
		computer = BLACK;
	else if (stone == 'W')
		computer = WHITE;
	else
	{
		printf("custom color!");
		computer = stone;
	}
	human = notStone(computer);
	
	
	// starting moves
	if (computer == BLACK)
	{
		centerMove(board, n, BLACK);
	}
	else
	{
		do
		{
			printf("Lay down a stone (ROW COL): ");
			scanf("%d%d", &row, &col);
		} while (!placeStone(board, n, row, col, human));
		printBoard(board, n);

		startingMoveWhite(board, n, row, col);
	}
	
	// AI moves
	while (!(isBoardFilled(board, n) || winner(board, n) != 0))
	{
		do
		{
			printf("Lay down a stone (ROW COL): ");
			scanf("%d%d", &row, &col);
		} while (!placeStone(board, n, row, col, human));
		printBoard(board, n);

		if (!(isBoardFilled(board, n) || winner(board, n) != 0))
			computerMove(board, n, computer);
	}

	// computing winner
	if (winner(board, n) > 0)
		printf("Black player wins.\n");
	else if (winner(board, n) < 0)
		printf("White player wins.\n");
	else
		printf("Draw!\n");
	
	// finializing
	freeBoard(board, n);
	//closeLab7Gfx();
	return 0;
}





// new functions

// initialize a board of size n of baseScore
double** initBaseScore(int n)
{
	// allocating memery
	double** board = (double**)malloc(sizeof(double*) * n);
	for (int i = 0; i < n; i++)
		board[i] = (double*)malloc(sizeof(double) * n);
	//printf("memory allocated!\n");
	
	// setting baseScore
	for (int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			if (i <= n/2 && j <= n/2)
				board[i][j] = 1 - 1.0 / (n/2) * (n/2 - fmin(i, j));
			else if (i >= n/2 && j >= n/2)
				board[i][j] = 1 - 1.0 / (n/2) * (n/2 - fmin(fabs(i - n + 1), fabs(j - n + 1)));
			else if (i > n/2 && j < n/2)
				board[i][j] = 1 - 1.0 / (n/2) * (n/2 - fmin(fabs(i - n + 1), j) );
			else if (i < n/2 && j > n/2)
				board[i][j] = 1 - 1.0 / (n/2) * (n/2 - fmin(i, fabs(j - n + 1)));
	if (n % 2 == 0)
		board[n/2][n/2] = 1 - 1.0 / (n/2) * (n/2 - fmin(fabs(n/2 - n + 1), fabs(n/2 - n + 1)));

	for (int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			board[i][j] *= MAXBASESCORE;
	//printf("board setup!\n");

	//printBoard(board, n);
	return board;
}

// calculates the score
double score(char **board, int n, int row, int col, char stone)
{
	// maximum score is 1.
	double result = 0.0;
	int connect;
	int span;
	int bound;
	
	// score 90
	combined (board, n, row, col, 90, &connect, &span, &bound);
	const double seqScore90 = seqScore(connect, span, bound);
	// score 0
	combined (board, n, row, col, 0, &connect, &span, &bound);
	const double seqScore0 = seqScore(connect, span, bound);
	// score 45
	combined (board, n, row, col, 45, &connect, &span, &bound);
	const double seqScore45 = seqScore(connect, span, bound);
	// score 135
	combined (board, n, row, col, 135, &connect, &span, &bound);
	const double seqScore135 = seqScore(connect, span, bound);

	result = seqScore90 + seqScore0 + seqScore45 + seqScore135 + baseScore[row][col];
	return result;
}

// computes the score in one dimension
double seqScore (int connect, int span, int bound)
{
	double result = 0;
	if (connect >= 6) // connect 6!
		result = 100.0; // win! // max score
	else if (span < 6) // span < 6
		result = 0.0; //waste move // min score
	else // span >= 6 && longest < 6
	{
		if (connect == 5) // connect 5
			if (span > 6 && bound == 0)
				result = 1.0 / 2;
			else if (span == 6 || bound == 1) // else if (span == 6 || bound == 1)
				result = 1.0 / 4;
			else
				scanf("%d", &result);
		else if (connect == 4) // connect 4
			if (span > 6 && bound == 0)
				result = 1.0 / 5;
			else if (span == 6 || bound == 1)
				result = 1.0 / 25;
			else
				scanf("%d", &result);
		else if (connect == 3) // connect 3
			if (span > 6 && bound == 0)
				result = 1.0 / 25;
			else if (span == 6 || bound == 1)
				result = 1.0 / 625;
			else
				scanf("%d", &result);
		else if (connect == 2) // connect 2
			if (span > 6 && bound == 0)
				result = 1.0 / 625;
			else if (span == 6 || bound == 1)
				result = 1.0 / 390625;
			else
				scanf("%d", &result);
		else // connect 1
			result = 0;
	}

	return result;
}

// prints the score board
void printScore(char **board, int n, char stone)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
			if (board[i][j] == UNOCCUPIED)
			{
				placeStone(board, n, i, j, stone); // temporarily place stone
				printf("%3.0lf", score(board, n, i, j, stone) * 100);
				board[i][j] = UNOCCUPIED; // remove the stone
			}
				
			else
				printf("%3c", board[i][j]);
		printf("\n");
	}
}

// highest score
double findMaxScoreBoard(char **board, int n, char stone, int *i, int *j)
{
	double result = 0;
	double currentScore;
	int row;
	int col;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			if (board[i][j] == UNOCCUPIED && (radiusCheck(board, n, i, j, 1) || radiusCheck(board, n, i, j, 2)))
			{
				placeStone(board, n, i, j, stone); // temporarily place stone
				currentScore = score(board, n, i, j, stone);
				if (currentScore > result)
				{
					result = currentScore;
					row = i;
					col = j;
				}
				board[i][j] = UNOCCUPIED; // remove the stone
			}

	*i = row;
	*j = col;
	return result;
}

// finding the longest continuous sequence.........................................................

// direction condition
bool dirCheck(char **board, int n, int row, int col, int i, int direction, char stone)
{
	bool result;
	if (direction == 90)
		result = isStone(board, n, row - i, col, stone);
	else if (direction == 270)
		result = isStone(board, n, row + i, col, stone);
	else if (direction == 180)
		result = isStone(board, n, row, col - i, stone);
	else if (direction == 0)
		result = isStone(board, n, row, col + i, stone);
	else if (direction == 45)
		result = isStone(board, n, row - i, col + i, stone);
	else if (direction == 225)
		result = isStone(board, n, row + i, col - i, stone);
	else if (direction == 135)
		result = isStone(board, n, row - i, col - i, stone);
	else if (direction == 315)
		result = isStone(board, n, row + i, col + i, stone);
	else
		printf("E");
	return result;
}

// longest in a single direction
int longest(char **board, int n, int row, int col, int direction)
{
	int result = 0;
	bool done = false;
	char stone = board[row][col];
	for (int i = 1; !done; i++)
	{
		if (dirCheck(board, n, row, col, i, direction, stone))
			result++;
		else
			done = true;
	}
	return result;
}

// longest in the direction and negative direction + 1
int longest90(char **board, int n, int row, int col)
{
	return 1 + longest(board, n, row, col, 90) + longest(board, n, row, col, 270);
}

int longest0(char **board, int n, int row, int col)
{
	return 1 + longest(board, n, row, col, 0) + longest(board, n, row, col, 180);
}

int longest45(char **board, int n, int row, int col)
{
	return 1 + longest(board, n, row, col, 45) + longest(board, n, row, col, 225);
}

int longest135(char **board, int n, int row, int col)
{
	return 1 + longest(board, n, row, col, 135) + longest(board, n, row, col, 315);
}

int findLongest(char **board, int n, int row, int col)
{
	return fmax(fmax(longest90(board, n, row, col), longest0(board, n, row, col)), 
			fmax(longest45(board, n, row, col), longest135(board, n, row, col)));
}

// finding the longest avalible span ..............................................................

// direction condition
bool dirSpanCheck(char **board, int n, int row, int col, int i, int direction, char stone)
{
	bool result;
	if (direction == 90)
		result = isNotStone(board, n, row - i, col, notStone(stone));
	else if (direction == 270)
		result = isNotStone(board, n, row + i, col, notStone(stone));
	else if (direction == 180)
		result = isNotStone(board, n, row, col - i, notStone(stone));
	else if (direction == 0)
		result = isNotStone(board, n, row, col + i, notStone(stone));
	else if (direction == 45)
		result = isNotStone(board, n, row - i, col + i, notStone(stone));
	else if (direction == 225)
		result = isNotStone(board, n, row + i, col - i, notStone(stone));
	else if (direction == 135)
		result = isNotStone(board, n, row - i, col - i, notStone(stone));
	else if (direction == 315)
		result = isNotStone(board, n, row + i, col + i, notStone(stone));
	else
		printf("E");
	return result;
}

// longest span in a single direction
int longestSpan(char **board, int n, int row, int col, int direction)
{
	int result = 0;
	bool done = false;
	char stone = board[row][col];
	for (int i = 1; !done; i++)
	{
		if (dirSpanCheck(board, n, row, col, i, direction, stone))
			result++;
		else
			done = true;
	}
	return result;
}

// longest in the direction and negative direction + 1
int longestSpan90(char **board, int n, int row, int col)
{
	return 1 + longestSpan(board, n, row, col, 90) + longestSpan(board, n, row, col, 270);
}

int longestSpan0(char **board, int n, int row, int col)
{
	return 1 + longestSpan(board, n, row, col, 0) + longestSpan(board, n, row, col, 180);
}

int longestSpan45(char **board, int n, int row, int col)
{
	return 1 + longestSpan(board, n, row, col, 45) + longestSpan(board, n, row, col, 225);
}

int longestSpan135(char **board, int n, int row, int col)
{
	return 1 + longestSpan(board, n, row, col, 135) + longestSpan(board, n, row, col, 315);
}

int findLongestSpan(char **board, int n, int row, int col)
{
	return fmax(fmax(longestSpan90(board, n, row, col), longestSpan0(board, n, row, col)), 
			fmax(longestSpan45(board, n, row, col), longestSpan135(board, n, row, col)));
}

// bound number ....................................................................................

// is the direction bounded
bool isBounded(char **board, int n, int row, int col, int direction)
{
	bool result = true;
	bool done = false;
	char stone = board[row][col];
	for (int i = 1; !done; i++)
	{
		if (dirCheck(board, n, row, col, i, direction, stone))
			;
		else if (dirCheck(board, n, row, col, i, direction, UNOCCUPIED))
		{
			result = false;
			done = true;
		}
		else
			done = true;
	}
	return result;
}

// bound number
int bound90(char** board, int n, int row, int col)
{
	return isBounded(board, n, row, col, 90) + isBounded(board, n, row, col, 270);
}

int bound0(char** board, int n, int row, int col)
{
	return isBounded(board, n, row, col, 0) + isBounded(board, n, row, col, 180);
}

int bound45(char** board, int n, int row, int col)
{
	return isBounded(board, n, row, col, 45) + isBounded(board, n, row, col, 225);
}

int bound135(char** board, int n, int row, int col)
{
	return isBounded(board, n, row, col, 135) + isBounded(board, n, row, col, 315);
}

// combined ....................................................................................

// combined function (return isBounded)
void combinedInfo(char **board, int n, int row, int col, int direction, int *l, int *ls, int *b)
{
	int longest = 0;
	bool doneLongest = false;
	int longestSpan = 0;
	bool doneLongestSpan = false;
	bool isBounded = true;
	bool doneBounded = false;
	char stone = board[row][col];
	
	for (int i = 1; !doneLongest || !doneLongestSpan || !doneBounded; i++)
	{
		if (!doneLongest && dirCheck(board, n, row, col, i, direction, stone))
			longest++;
		else if (!doneLongest && dirCheck(board, n, row, col, i, direction, UNOCCUPIED))
		{
			isBounded = false;
			doneBounded = true;
			doneLongest = true;
		}
		else
		{
			doneLongest = true;
			doneBounded = true;
		}

		if (!doneLongestSpan && dirSpanCheck(board, n, row, col, i, direction, stone))
			longestSpan++;
		else
			doneLongestSpan = true;
	}

	
	*l = longest;
	*ls = longestSpan;
	*b = isBounded;
}

// combined 1D info
void combined (char** board, int n, int row, int col, int direction, int *l, int *ls, int *b)
{
	int connect;
	int span;
	int bound;
	combinedInfo(board, n, row, col, direction, &connect, &span, &bound);
	int connect_;
	int span_;
	int bound_;
	combinedInfo(board, n, row, col, direction + 180, &connect_, &span_, &bound_);

	*l = 1 + connect + connect_;
	*ls = 1 + span + span_;
	*b = bound + bound_;
}



// gap number ....................................................................................

// connect with gaps // unfinished
int gap90 (char** board, int n, int row, int col)
{
	int result;
	int seq0 = 1;
	int seq45 = 1;
	//int seq90 = 1;
	int seq135 = 1;
	bool done0 = false, done45 = false, done90 = false, done135 = false;
	bool done_0 = false, done_45 = false, done_90 = false, done_135 = false;
	char stone = board[row][col];
	int i = 1;

	int seq90 = 0;
	int seq_90 = 0;
	
	while (!(done90 && done_90))
	{
		if (!done90 && isStone(board, n, row - i, col, stone))
			seq90++;
		else
			done90 = true;
		if (!done_90 && isStone(board, n, row + i, col, stone))
			seq_90++;
		else
			done_90 = true;
		i++;
	}

	int netSeq90 = seq90 + seq_90 + 1;
	int wing = 6 - netSeq90;
	bool gapDone90 = false, gapDone_90 = false;
	int gap90 = 0;
	int gap_90 = 0;
	int tempGapSpace90 = 0;
	int tempGapSpace_90 = 0;
	int gapSpace90 = 0;
	int gapSpace_90 = 0;
	int tempRow;
	
	printf("90:%d _90:%d net:%d\n", seq90, seq_90, netSeq90);
	
	for (int i = 1; i <= wing && (!(gapDone90 && gapDone_90)); i++)
	{
		tempRow = row - seq90 - i;
		if (!gapDone90 && isNotStone(board, n, tempRow, col, notStone(stone)))
		{
			if (isStone(board, n, tempRow, col, UNOCCUPIED))
				tempGapSpace90++;
			if (isStone(board, n, tempRow, col, stone))
			{
				gapSpace90 = tempGapSpace90;
				gap90++;
			}
		}
		else
			gapDone90 = true;

		tempRow = row + seq_90 + i;
		if (!gapDone_90 && isNotStone(board, n, tempRow, col, notStone(stone)))
		{
			if (isStone(board, n, tempRow, col, UNOCCUPIED))
				tempGapSpace_90++;
			if (isStone(board, n, tempRow, col, stone))
			{
				gapSpace_90 = tempGapSpace_90;
				gap_90++;
			}
		}
		else
			gapDone_90 = true;
	}
	printf("gap90=%d, gap_90=%d, gapSpace90=%d, gapSpace_90=%d\n", gap90, gap_90, gapSpace90, gapSpace_90);
	
	//printf("seq0:%d\n", seq0);
	//printf("seq45:%d\n", seq45);
	//printf("seq90:%d\n", seq90);
	//printf("seq135:%d\n", seq135);
	
	//result = fmax(fmax(seq0, seq45), fmax(seq90, seq135));
	return gap90 + gap_90;
}

// is the coordinate legal
bool isInbound (char** board, int n, int row, int col)
{
	return (row >= 0 && row < n && col >= 0 && col < n);
}

// is the coordinate same as stone
bool isStone (char** board, int n, int row, int col, char stone)
{
	return (isInbound (board, n, row, col) && board[row][col] == stone);
}

// is the coordinate not stone
bool isNotStone (char** board, int n, int row, int col, char stone)
{
	return (isInbound (board, n, row, col) && board[row][col] != stone);
}

// random board generator
bool randBoard (char** board, int n)
{
	int temp;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
		{
			temp = rand() % 4;
			if (temp == 0)
				board[i][j] = WHITE;
			else if (temp == 1)
				board[i][j] = BLACK;
			else
				board[i][j] = UNOCCUPIED;
		}
	return (winner(board, n) == 0);
}

// is there a piece in radius
bool isCritical (char** board, int n, int row, int col)
{
	bool result;
	bool blackThreat;
	bool whiteThreat;
	// place both black and white stone there to see its score
	placeStone(board, n, row, col, BLACK);
	blackThreat = score(board, n, row, col, BLACK) >= THREATLIMIT;
	board[row][col] = UNOCCUPIED;
	placeStone(board, n, row, col, WHITE);
	whiteThreat = score(board, n, row, col, WHITE) >= THREATLIMIT;
	board[row][col] = UNOCCUPIED;
	result = (blackThreat || whiteThreat);
	return result;
}

bool radiusCheck (char** board, int n, int row, int col, int i)
{
	return (isStone(board, n, row - i, col, BLACK) || isStone(board, n, row - i, col, WHITE)
			|| isStone(board, n, row + i, col, BLACK) || isStone(board, n, row + i, col, WHITE)
			|| isStone(board, n, row, col - i, BLACK) || isStone(board, n, row, col - i, WHITE)
			|| isStone(board, n, row, col + i, BLACK) || isStone(board, n, row, col + i, WHITE)
			|| isStone(board, n, row - i, col + i, BLACK) || isStone(board, n, row - i, col + i, WHITE)
			|| isStone(board, n, row + i, col - i, BLACK) || isStone(board, n, row + i, col - i, WHITE)
			|| isStone(board, n, row - i, col - i, BLACK) || isStone(board, n, row - i, col - i, WHITE)
			|| isStone(board, n, row + i, col + i, BLACK) || isStone(board, n, row + i, col + i, WHITE));
}

// alpha-beta pruning (stone is the opponent stone)
double alphaBeta(char** board, int n, int row, int col, int depth, const int MAXDEPTH, char stone, bool maximizing, double value, double min, double max, bool threatSelect)
{
	// time management
	if (timeOut)
		return 0;
	getrusage(RUSAGE_SELF, &usage);
	end = usage.ru_utime;
	time_end = end.tv_sec + end.tv_usec/1000000.0; // in seconds
	double timeUsed = time_end - time_start;
	if  (timeUsed > TIMELIMIT)
	{
		timeOut = true;
		if (DEBUG)
		{
			printf("TIME OUT!!! %lf\n", timeUsed);
			//char temp;
			//scanf("%c", &temp);
		}
		return 0;
	}


	double result;
	double currentScore;
	int maxRow = n/2, maxCol = n/2;
	const bool root = (depth == MAXDEPTH);
	double tempScore;
	bool locationCheck;
	bool critical;
	bool done = true;
	
	// computes score
	if (root)
		value = 0;
	else
	{
		placeStone(board, n, row, col, stone); // temporarily place stone
		tempScore = score(board, n, row, col, stone);
		if (maximizing)
			value = value - tempScore;
		else
			value = value + tempScore;
	}	
	
	if (depth <= 0 || winner(board, n) != 0 || isBoardFilled(board, n)) // base case // leaf node
	{
		result = value;
	}
	else if (maximizing) // max node
	{
		result = min;
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				locationCheck = (isStone(board, n, i, j, UNOCCUPIED) && (radiusCheck(board, n, i, j, 1) || radiusCheck(board, n, i, j, 2)));
				if (locationCheck)
				{
					if (threatSelect)
						critical = isCritical(board, n, i, j);
					else
						critical = true;
				}
				else
					critical = false;
				
				if (critical)
				{
					done = false;
					currentScore = alphaBeta(board, n, i, j, depth-1, MAXDEPTH, notStone(stone), !maximizing, value, result, max, threatSelect); 
					//printf("|%d-%c(%d%d):%lf| ", depth-1, notStone(stone), i, j, currentScore);
					
					if (DEBUG && root)
						printf("%3.0lf", currentScore * 100);
					
					// keep the maximum score
					if (currentScore > result)
					{
						result = currentScore;
						if (root)
						{
							maxRow = i;
							maxCol = j;
						}
					}
					// pruning
					if (PRUNNING && result > max)
					{
						//printf("%d, %d \n", row, col);
						board[row][col] = UNOCCUPIED; // remove the stone
						return max;
					}
				}
				else
				{
					if (DEBUG && root)
						printf("%3c", board[i][j]);
				}
			}
			if (DEBUG && root)
				printf("\n");
		}
	}
	else // min node
	{
		result = MAXINT;
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				locationCheck = (isStone(board, n, i, j, UNOCCUPIED) && (radiusCheck(board, n, i, j, 1) || radiusCheck(board, n, i, j, 2)));
				if (locationCheck)
				{
					if (threatSelect)
						critical = isCritical(board, n, i, j);
					else
						critical = true;
				}
				else
					critical = false;
				
				if (critical)
				{
					done = false;
					currentScore = alphaBeta(board, n, i, j, depth-1, MAXDEPTH, notStone(stone), !maximizing, value, min, result, threatSelect);
					//printf("|%d-%c(%d%d):%lf| ", depth-1, notStone(stone), i, j, currentScore);
					// keep the minimum score
					if (currentScore < result)
						result = currentScore;
					// pruning
					if (PRUNNING && result < min)
					{
						board[row][col] = UNOCCUPIED; // remove the stone
						return min;
					}
				}
			}
		}
	}
	
	// final decision
	if (!timeOut && root)
	{
		placeStone(board, n, maxRow, maxCol, notStone(stone));
		printf("Computer lays a stone at ROW %d COL %d.\n", maxRow, maxCol);
	}
	else if (!root) // remove stone no matter timeOut or not
		board[row][col] = UNOCCUPIED; // remove the stone
	if (done)
		result = value;
	return result;
}

// helper function
double computerMove(char** board, int n, char stone)
{
	getrusage(RUSAGE_SELF, &usage);
	start = usage.ru_utime;
	time_start = start.tv_sec + start.tv_usec/1000000.0; // in seconds

	double tempTime = 0;
	int opRow;
	int opCol;
	int row;
	int col;
	double maxScoreBoardOpponent = findMaxScoreBoard(board, n, notStone(stone), &opRow, &opCol);
	double maxScoreBoardMe = findMaxScoreBoard(board, n, stone, &row, &col);
	if (DEBUG)
		printf("max score for opponent = %lf\n", maxScoreBoardOpponent);

	if (maxScoreBoardMe > 1) // computer wins
	{
		if (DEBUG)
			printf("WIN\n");
		placeStone(board, n, row, col, stone);
		printf("Computer lays a stone at ROW %d COL %d.\n", row, col);
	}
	else if (maxScoreBoardOpponent > 1) // opponent wins on next move
	{
		if (DEBUG)
			printf("BLOCK\n");
		placeStone(board, n, opRow, opCol, stone);
		printf("Computer lays a stone at ROW %d COL %d.\n", opRow, opCol);
	}
	else if (maxScoreBoardOpponent < THREATLIMIT && maxScoreBoardMe < THREATLIMIT) // low threat
	{
		if (DEBUG)
			printf("No threat.\n");
		// recursion call at a MAXDEPTH of 2
		alphaBeta(board, n, -1, -1, 3, 3, notStone(stone), true, 0, MININT, MAXINT, false);
	}
	else // high threat
	{
		if (DEBUG)
			printf("Threat exists!\n");
		// recursion call
		alphaBeta(board, n, -1, -1, MAXDEPTH, MAXDEPTH, notStone(stone), true, 0, MININT, MAXINT, true);
		//scanf("%c", &temp);
	}
	// timed out
	if (timeOut)
	{
		// restart time counter
		getrusage(RUSAGE_SELF, &usage);
		end = usage.ru_utime;
		time_end = end.tv_sec + end.tv_usec/1000000.0; // in seconds
		tempTime = time_end - time_start;
		
		getrusage(RUSAGE_SELF, &usage);
		start = usage.ru_utime;
		time_start = start.tv_sec + start.tv_usec/1000000.0; // in seconds

		if (DEBUG)
			printf("Timed Out. depth 2 move\n");
		timeOut = false;
		alphaBeta(board, n, -1, -1, 2, 2, notStone(stone), true, 0, MININT, MAXINT, false);
	}
	
	printBoard(board, n);

	getrusage(RUSAGE_SELF, &usage);
	end = usage.ru_utime;
	time_end = end.tv_sec + end.tv_usec/1000000.0; // in seconds
	return time_end - time_start + tempTime;
}

// second AI
double computerMove2(char** board, int n, char stone)
{
	getrusage(RUSAGE_SELF, &usage);
	start = usage.ru_utime;
	time_start = start.tv_sec + start.tv_usec/1000000.0; // in seconds

	int temp;
	int row;
	int col;
	double maxScoreBoardOpponent = findMaxScoreBoard(board, n, notStone(stone), &temp, &temp);
	double maxScoreBoardMe = findMaxScoreBoard(board, n, stone, &row, &col);
	printf("max score for opponent = %lf\n", maxScoreBoardOpponent);

	alphaBeta(board, n, -1, -1, 2, 2, notStone(stone), true, 0, MININT, MAXINT, false);
	printBoard(board, n);

	getrusage(RUSAGE_SELF, &usage);
	end = usage.ru_utime;
	time_end = end.tv_sec + end.tv_usec/1000000.0; // in seconds
	return time_end - time_start;
}

// place stone at center
void centerMove(char** board, int n, char stone)
{
	int row, col;
	row = n/2;
	col = n/2;
	placeStone(board, n, row, col, stone);
	printf("Computer lays a stone at ROW %d COL %d.\n", row, col);
	printBoard(board, n);
}

// starting move
void startingMoveWhite(char** board, int n, int i, int j)
{
	if (i != n/2 || j != n/2)
		centerMove(board, n, WHITE);
	else
		computerMove(board, n, WHITE);
}









// from Part1 ..........................................................................................

// is the board filled
bool isBoardFilled (char** board, int n)
{
	bool result = true;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			if (board[i][j] == UNOCCUPIED)
				result = false;
	return result;
}

// oppponent's stone
char notStone(char stone)
{
	char result;
	if (stone == BLACK)
		result = WHITE;
	else if (stone == WHITE)
		result = BLACK;
	else
		result = UNOCCUPIED;
	return result;
}

// MiniMax algorithm
double minimax(char** board, int n, int depth, int maxDepth, char stone)
{
	double currentScore;
	double maxScore = MININT; // maximum score to be returned
	int maxRow, maxCol;
	const bool final = (depth == maxDepth);

	
	if (depth <= 0 || winner(board, n) != 0 || isBoardFilled(board, n)) // base case
	{
		return 0;
	}
	else
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				if (isStone(board, n, i, j, UNOCCUPIED)) // for every UNOCCUPIED
				{
					placeStone(board, n, i, j, stone); // temporarily place stone at Pcurrent
					// Score = LongestSequence(Pcurrent) - LongestSequence(Pnext) // recursive call
					currentScore = score(board, n, i, j, stone) - minimax(board, n, depth-1, maxDepth, notStone(stone)); 
					//printf("%c(%d%d):%d| ", stone, i, j, score);
					board[i][j] = UNOCCUPIED; // remove the stone

					
					if (DEBUG && final)
						printf("%3.0lf", currentScore * 100);
					
					// keep the maximum score
					if (currentScore > maxScore)
					{
						maxScore = currentScore;
						if (final)
						{
							maxRow = i;
							maxCol = j;
						}
					}
				}
				else
				{
					if (DEBUG && final)
						printf("%3c", board[i][j]);
				}
			}
			if (DEBUG && final)
				printf("\n");
		}
	}

	// final decision
	if (final)
	{
		placeStone(board, n, maxRow, maxCol, stone);
		printf("Computer lays a stone at ROW %d COL %d.\n", maxRow, maxCol);
	}
	return maxScore;
}




// from lab6 ...........................................................................................

// initializes the board to UNOCCUPIED
char** init(int n)
{
	// allocating memery
	char** board = (char**)malloc(sizeof(char*) * n);
	for (int i = 0; i < n; i++)
		board[i] = (char*)malloc(sizeof(char) * n);
	//printf("memory allocated!\n");
	
	// setting to all UNOCCUPIED
	for (int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			board[i][j] = UNOCCUPIED;
	//printf("board setup!\n");

	//printBoard(board, n);
	return board;
}

// freeing memery
void freeBoard(char** board, int n)
{
	for (int i = 0; i < n; i++)
		free(board[i]);
	free(board);
	//printf("memory freed!\n");
}

// prints out the board
void printBoard(char **board, int n)
{
	if (DEBUG)
	{
		printf("   ");
		for (int i = 0; i < n; i++)
			printf("%3d", i);
		printf("\n");
		printf("   ");
		for (int i = 0; i < n; i++)
			printf("___");
		printf("\n");
	}
	for (int i = 0; i < n; i++)
	{
		if (DEBUG)
			printf("%2d|", i);
		for (int j = 0; j < n; j++)
			if (DEBUG)
				printf("%3c", board[i][j]);
			else
				printf("%c", board[i][j]);
		printf("\n");
	}
}



// check the winner (positive = BLACK, negative = WHITE, 0 = draw)
int winner(char** board, int n)
{
	int result = 0;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			if (board[i][j] == BLACK && findLongest(board, n, i, j) >= CONNECT)
				result++;
			else if (board[i][j] == WHITE && findLongest(board, n, i, j) >= CONNECT)
				result--;
			else
				;
	return result;
}

// place stone
bool placeStone(char** board, int n, int row, int col, char stone)
{
	bool result = true;
	if (!isStone(board, n, row, col, UNOCCUPIED))
	{
		printf("That square is occupied.\n");
		if (DEBUG)
		{
			char temp;
			scanf("%c", &temp);
		}
		result = false;
	}
	else
		board[row][col] = stone;	
	return result;
}
