#include <iostream>
#include <random>
#include <chrono>
#include "HillClimbing.h"

using namespace std;

#define BOARD_SIZE 8
#define NUM_OF_TESTS 200

#define STEEPEST	0
//#define SIDEWAYS	1

int board[BOARD_SIZE];
int moves = 0;

void printBoard(int(&board)[BOARD_SIZE])
{
	for (int i = BOARD_SIZE - 1; i >= 0; i--)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (board[j] == i)
				cout << "1 ";
			else
				cout << "0 ";
			if (j == BOARD_SIZE - 1)
				cout << endl;
		}
	}
}

int getHeuristicCost(int (&board)[BOARD_SIZE])
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

void generateRandomBoard(int (&board)[BOARD_SIZE])
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		board[i] = rand() % BOARD_SIZE;
	}
}

struct verticalMove
{
	int row;
	int col;
	int collisions;
};

void copytoChild(int(&child)[BOARD_SIZE], int(&board)[BOARD_SIZE])
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		child[i] = board[i];
	}
}

void getSuccessors(int(&child)[BOARD_SIZE], int board[BOARD_SIZE], int (&successors)[BOARD_SIZE * BOARD_SIZE])
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (board[i] != j)
			{
				child[i] = j;
				successors[i * BOARD_SIZE + j] = getHeuristicCost(child);
				child[i] = board[i];
			}
			else
			{
				successors[i * BOARD_SIZE + j] = 1000;
			}
		}
	}
}

//Get the local maximum
void getLeastHeurCost(int successors[BOARD_SIZE * BOARD_SIZE], int &leastHeur)
{
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (successors[i] < leastHeur)
			leastHeur = successors[i];
	}
}

vector<verticalMove> storeBestChildren(int successors[BOARD_SIZE * BOARD_SIZE], int &leastHeur)
{
	vector<verticalMove> bestChildren;
	verticalMove temp;
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

bool chooseBestChild(vector<verticalMove> bestChildren, int(&board)[BOARD_SIZE])
{
	verticalMove temp;
	if (!bestChildren.empty())
	{
		int randInt = rand() % bestChildren.size();
		temp = bestChildren[randInt];

		board[temp.row] = temp.col;
		if (!temp.collisions)
		{
			//	printBoard(board);
			//	cout << endl;
			return true;
		}
	}
	return false;
}

bool steepestHillClimbing(int (&board)[BOARD_SIZE], int &successCost, int &failureCost)
{
//	int moves = 0;
	int collisions, leastHeur;
	int child[BOARD_SIZE];
	int successors[BOARD_SIZE * BOARD_SIZE];
	vector<verticalMove> bestChildren;

	while (true)
	{
		collisions = getHeuristicCost(board);

		copytoChild(child, board);
		getSuccessors(child, board, successors);

		leastHeur = collisions;
		getLeastHeurCost(successors, leastHeur);

		if (leastHeur >= collisions)
		{
			failureCost = moves;
			return false;
		}

		bestChildren = storeBestChildren(successors, leastHeur);
		if (chooseBestChild(bestChildren, board))
		{
			successCost = moves;
			return true;
		}
		else
		{
			++moves;
			bestChildren.clear();
		}
	}
}

bool sidewaysHillClimbing(int(&board)[BOARD_SIZE], int &successCost, int &failureCost)
{
	int counter = 0;
	int collisions, leastHeur;
	int successors[BOARD_SIZE * BOARD_SIZE];
	int child[BOARD_SIZE];
	vector<verticalMove> bestChildren;

	while (true)
	{
		collisions = getHeuristicCost(board);

		copytoChild(child, board);
		getSuccessors(child, board, successors);

		leastHeur = collisions;
		getLeastHeurCost(successors, leastHeur);

		if (leastHeur == collisions)
		{
			counter++;
			if (counter == 100)
			{
				failureCost = moves;
				return false;
			}
		}
		else if (leastHeur > collisions)
		{
			failureCost = moves;
			return false;
		}
		else
			counter = 0;

		bestChildren = storeBestChildren(successors, leastHeur);
		if (chooseBestChild(bestChildren, board))
		{
			successCost = moves;
			return true;
		}
		else
		{
			++moves;
			bestChildren.clear();
		}
	}
}

int randomRestartHillClimbing(int(&board)[BOARD_SIZE], int &searchCost)
{
	int moves = 0;
	int counter = 0;
	int randomRestarts = 0;
	int collisions, leastHeur;
	int child[BOARD_SIZE];
	int successors[BOARD_SIZE * BOARD_SIZE];
	vector<verticalMove> bestChildren;

	while (true)
	{
		collisions = getHeuristicCost(board);

		copytoChild(child, board);
		getSuccessors(child, board, successors);

		leastHeur = collisions;
		getLeastHeurCost(successors, leastHeur);

#ifdef STEEPEST
		if (leastHeur >= collisions)
		{
			generateRandomBoard(board);
			randomRestarts++;
			continue;
		}
#endif

#ifdef SIDEWAYS
		if (leastHeur == collisions)
		{
			counter++;
			if (counter == 100)
				return false;
		}
		else if (leastHeur > collisions)
			return false;
		else
			counter = 0;
#endif

		bestChildren = storeBestChildren(successors, leastHeur);
		if (chooseBestChild(bestChildren, board))
		{
			searchCost = moves;
			return randomRestarts;
		}
		else
		{
			++moves;
			bestChildren.clear();
		}
	}

	searchCost = moves;
	return randomRestarts;
}

int main()
{
	bool bSuccess = false;
	int searchCost = 0;
	int successCost = 0;
	int failureCost = 0;
	int randomRestarts = 0;
	float successCount = 0;
	srand(time(NULL));



	for (int i = 0; i < NUM_OF_TESTS; i++)
	{
		generateRandomBoard(board);
		bSuccess = steepestHillClimbing(board, successCost, failureCost);
	//	bSuccess = sidewaysHillClimbing(board, successCost, failureCost);

		if (bSuccess)
			successCount++;
	}

	cout << "Steepet HillClimbing algorithm:\n" ;
	cout << "Average successCost: " << successCost / NUM_OF_TESTS << endl;
	cout << "Average failureCost: " << failureCost / NUM_OF_TESTS << endl;
	cout << "successCount: " << successCount << endl;
	cout << ((successCount / NUM_OF_TESTS) * 100) << "% Success Rate\n";
	
/*
	generateRandomBoard(board);
	randomRestarts = randomRestartHillClimbing(board, searchCost);
	cout << "Steepest hill Climbing with random restart: \n";
	cout << "randomRestarts: " << randomRestarts << endl;
	cout << "searchCost: " << searchCost << endl;
	*/
}
