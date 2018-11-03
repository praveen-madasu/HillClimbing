/*
	HillClimbing.cpp
	ITCS 6150 Intelligent Systems
	Author: Shanmukha Praveen Madasu
*/
#include <iostream>
#include <random>
#include <chrono>
#include <math.h>
#include "HillClimbing.h"

using namespace std;

int BOARD_SIZE = 0;

//change here for number of tests to be conducted like 100, 200, 300, 400, 500
#define NUM_OF_TESTS 	100

//we can choose another approach by dividing with total number of failure steps, but this is the best approach
#define SA_FAILURE_RATE	 86			//Average failure rate for steepest variant
#define SM_FAILURE_RATE	  6			//Average failure rate for sideways variant

bool bSuccess, bIncorrectChoice = false;
bool bSteepestAscent, bSidewaysMove = false;	//boolean variable to store steepest ascent & sideways move variants
bool bRandRestSA, bRandRestSM = false;		//boolean variable to store steepest ascent & sideways move random restart variants
bool printPath = false;				//for printing only 1st 3 initial configurations
float searchCost = 0;
float successCost = 0;				//stores success count
float failureCost = 0;				//stores failure count
float randomRestarts = 0;			//stores number of random restarts
float successCount = 0;				//stores success steps
float failureCount = 0;				//stores failure count
float moves = 0;				//to calculate searchCost

//struct to store best child successor information
struct bestMove
{
	int row;
	int col;
	int collisions;
};

//Print the board having board size selected by the user
void printBoard(int *board, int BOARD_SIZE)
{
	for (int i = BOARD_SIZE - 1; i >= 0; i--)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (board[j] == i)
				cout << "Q ";
			else
				cout << "- ";
				
			if (j == BOARD_SIZE - 1)
				cout << endl;
		}
	}
	cout << endl;
}

//calculates number of collisions from the current board
int getHeuristicCost(int *board, int BOARD_SIZE)
{
	int heur = 0;
	int offset;
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = i + 1; j < BOARD_SIZE; j++)
		{
			if (board[i] == board[j])
				++heur;

			offset = j - i;

			if ((board[i] == board[j] - offset) || (board[i] == board[j] + offset))
			{
				++heur;
			}
		}
	}
	return heur;
}

//generates random board based on the board size
void generateRandomBoard(int *board, int BOARD_SIZE)
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		board[i] = rand() % BOARD_SIZE;
	}
}

//copies current board to a child board
void copytoChild(int *child, int *board, int BOARD_SIZE)
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		child[i] = board[i];
	}
}

//gets all the successors and their heuristic cost based on the board size
void getSuccessors(int *child, int *board, int *successors, int BOARD_SIZE, bool printPath)
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (board[i] != j)
			{
				child[i] = j;
				successors[i * BOARD_SIZE + j] = getHeuristicCost(child, BOARD_SIZE);
				//prints only for the 1st 3 initial configurations
				if(printPath)
				{
					if(successors[i * BOARD_SIZE + j] < 10)
						cout << "0" << successors[i * BOARD_SIZE + j] << " " ;
					else
						cout << successors[i * BOARD_SIZE + j] << " ";
				}
				child[i] = board[i];
			}
			else
			{
				successors[i * BOARD_SIZE + j] = 1000;		//Queen's position, assigning higher value to make sure it will not be picked
				if(printPath)
					cout << " Q" << " ";
			}
		}
		if(printPath)
			cout << endl;
	}
}

//gets the local maximum, in the sense least collisions among the successors
void getLeastHeurCost(int *successors, int &leastHeur, int BOARD_SIZE, bool printPath)
{
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (successors[i] < leastHeur)
			leastHeur = successors[i];
	}
	
	if(printPath)
		cout << "Edge value: " << leastHeur << endl;
}

//gets the best child successors and pushes to the vector
vector<bestMove> storeBestChildren(int *successors, int &leastHeur, int BOARD_SIZE)
{
	vector<bestMove> bestChildren;
	bestMove temp;
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (successors[i] == leastHeur)
		{
			temp.row = i / BOARD_SIZE;
			temp.col = i % BOARD_SIZE;
			temp.collisions = successors[i];

			bestChildren.push_back(temp);
		}
	}

	return bestChildren;
}

//choose randomly among the best child successors
bool chooseBestChild(vector<bestMove> bestChildren, int *board, int BOARD_SIZE, bool printPath)
{
	bestMove temp;
	if (!bestChildren.empty())
	{
		int randInt = rand() % bestChildren.size();
		temp = bestChildren[randInt];
		
		//prints only for the 1st 3 initial configurations
		if(printPath)
		{
			cout << "Best Child picked [row: " << (temp.row + 1) << ", column: " << (temp.col + 1) << "]" << endl << endl;
		}
		
		board[temp.row] = temp.col;
		if (!temp.collisions)
		{
			return true;
		}
	}
	return false;
}

//algorithm for the steepest Ascent Hill Climbing
bool steepestHillClimbing(int *board, float &successCost, float &failureCost, int BOARD_SIZE, bool printPath)
{
	float moves = 0;
	int collisions, leastHeur;
	int child[BOARD_SIZE];
	int successors[BOARD_SIZE * BOARD_SIZE];
	vector<bestMove> bestChildren;

	while (true)
	{
		collisions = getHeuristicCost(board, BOARD_SIZE);

		copytoChild(child, board, BOARD_SIZE);
		getSuccessors(child, board, successors, BOARD_SIZE, printPath);

		leastHeur = collisions;
		getLeastHeurCost(successors, leastHeur, BOARD_SIZE, printPath);
		
		if (leastHeur >= collisions)
		{
			failureCost += moves;
			if(printPath)
			{
				cout << "Failed!!! Got collisions either same or greater!!!" << endl;
			}
			return false;
		}

		bestChildren = storeBestChildren(successors, leastHeur, BOARD_SIZE);
		if (chooseBestChild(bestChildren, board, BOARD_SIZE, printPath))
		{
			successCost += moves;
			if(printPath)
			{
				cout << "Success!!!" << endl;
			}
			return true;
		}
		else
		{
			++moves;
			bestChildren.clear();
		}
	}
}

//algorithm for the sideways hill climbing
bool sidewaysHillClimbing(int *board, float &successCost, float &failureCost, int BOARD_SIZE, bool printPath)
{
	float moves = 0;
	int counter = 0;
	int collisions, leastHeur;
	int successors[BOARD_SIZE * BOARD_SIZE];
	int child[BOARD_SIZE];
	vector<bestMove> bestChildren;

	while (true)
	{
		collisions = getHeuristicCost(board, BOARD_SIZE);

		copytoChild(child, board, BOARD_SIZE);
		getSuccessors(child, board, successors, BOARD_SIZE, printPath);

		leastHeur = collisions;
		getLeastHeurCost(successors, leastHeur, BOARD_SIZE, printPath);

		if (leastHeur == collisions)
		{
			counter++;
			if (counter == 100)
			{
				failureCost += moves;
				if(printPath)
					cout << "Failed!!! limit exceeded" << endl;
				return false;
			}
		}
		else if (leastHeur > collisions)
		{
			if(printPath)
				cout << "Heuristic is Greater than current node!!! returning\n";
			return false;
		}
		else if((leastHeur < collisions) && (counter > 0))
		{
			counter = 0;
		}

		bestChildren = storeBestChildren(successors, leastHeur, BOARD_SIZE);
		if (chooseBestChild(bestChildren, board, BOARD_SIZE, printPath))
		{
			successCost += moves;
			if(printPath)
				cout << "success!!!\n";

			return true;
		}
		else
		{
			++moves;
			bestChildren.clear();
		}
	}
}

//algorithm for the random restart hill climbing both with & without sideways move
int randomRestartHillClimbing(int *board, float &searchCost, int BOARD_SIZE, int bRandRestSA, int bRandRestSM)
{
	float moves = 0;
	int counter = 0;
	float randomRestarts = 0;
	int collisions, leastHeur;
	int child[BOARD_SIZE];
	int successors[BOARD_SIZE * BOARD_SIZE];
	vector<bestMove> bestChildren;

	while (true)
	{
		collisions = getHeuristicCost(board, BOARD_SIZE);

		copytoChild(child, board, BOARD_SIZE);
		getSuccessors(child, board, successors, BOARD_SIZE, false);

		leastHeur = collisions;
		getLeastHeurCost(successors, leastHeur, BOARD_SIZE, false);

		if(bRandRestSA)								//random restart without sideways move
		{
			if (leastHeur >= collisions)
			{
				generateRandomBoard(board, BOARD_SIZE);			//generate random board if it fails
				randomRestarts++;
				continue;
			}
		}

		if(bRandRestSM)								//random restart with sideways move
		{
			if (leastHeur == collisions)
			{
				counter++;
				if (counter == 100)
				{
					generateRandomBoard(board, BOARD_SIZE);		//generate random board if it fails
					randomRestarts++;
					continue;
				}
			}
			else if (leastHeur > collisions)
			{
				generateRandomBoard(board, BOARD_SIZE);			//generate random board if it fails
				randomRestarts++;
				continue;
			}
			else
				counter = 0;
		}

		bestChildren = storeBestChildren(successors, leastHeur, BOARD_SIZE);
		if (chooseBestChild(bestChildren, board, BOARD_SIZE, false))
		{
			searchCost += moves;
			return randomRestarts;
		}
		else
		{
			++moves;
			bestChildren.clear();
		}
	}

	searchCost += moves;
	return randomRestarts;
}

int main()
{
	int board[BOARD_SIZE];
	srand(time(NULL));
	char ch;

	cout << "Select the board size:\n";
	cin >> BOARD_SIZE;
	
	cout << "Select one of these Hill-Climbing options\n";
	cout << "1) Steepest Ascent \n2) Sideways Move \n3) Random Restart without Sideways \n4) Random Restart with Sideways\n";
	cin >> ch;

	switch(ch)
	{
		case '1':
			bSteepestAscent = true;
			break;
			
		case '2':
			bSidewaysMove = true;
			break;
			
		case '3':
			bRandRestSA = true;
			break;
			
		case '4':
			bRandRestSM = true;
			break;
			
		default:
			cout << "Incorrect selection!!! Please choose again\n";
			bIncorrectChoice = true;
			break;
	}
	
	if(bIncorrectChoice)								//exit if the user chooses incorrect option
		return 0;
	
	//if user chooses any of the steepest ascent or sideways move it will come here
	if(bSteepestAscent || bSidewaysMove)
	{
		for (int i = 0; i < NUM_OF_TESTS; i++)
		{
			if(i < 3)							//printing path only for 1st 3 configurations
				printPath = true;
			else
				printPath = false;
			
			generateRandomBoard(board, BOARD_SIZE);
			if(bSteepestAscent)
				bSuccess = steepestHillClimbing(board, successCost, failureCost, BOARD_SIZE, printPath);
					
			if(bSidewaysMove)
				bSuccess = sidewaysHillClimbing(board, successCost, failureCost, BOARD_SIZE, printPath);
				
			if (bSuccess)
				successCount++;
				
			if(printPath)
				printBoard(board, BOARD_SIZE);
		}
		
		failureCount = NUM_OF_TESTS - successCount;
		cout << " Average successCost: " << round(successCost / successCount) << endl;
		cout << " Average failureCost: " << round(failureCost / failureCount) << endl;
		cout << ((successCount / NUM_OF_TESTS) * 100) << "% Success Rate\n";
	}

	//if user chooses any of the random restart option, it comes here
	if(bRandRestSA || bRandRestSM)
	{	
		for (int i = 0; i < 100; i++)
		{
			generateRandomBoard(board, BOARD_SIZE);
			randomRestarts += randomRestartHillClimbing(board, searchCost, BOARD_SIZE, bRandRestSA, bRandRestSM);
		}
		
		if(bRandRestSA)
			cout << "Average randomRestarts: " << round(randomRestarts/SA_FAILURE_RATE) << endl;
		else
			cout << "Average randomRestarts: " << round(randomRestarts/SM_FAILURE_RATE) << endl;

		cout << "Average searchCost: " << round(searchCost/100) << endl;
	}
}
