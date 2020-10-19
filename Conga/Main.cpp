

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

const int POS_INF = 100;
const int NEG_INF = -100;
const int depth = 4;

enum Player { black, white, openSquare };

struct Space {
	int value;
	Player player;
};

struct PossibleMoves {
	int x;
	int y;
	int x_dir;
	int y_dir;

	PossibleMoves() {}
	PossibleMoves(int x, int y, int x_dir, int y_dir) : x(x), y(y), x_dir(x_dir), y_dir(y_dir) {
		// initialize
	}


};

struct playersMove {
	bool moveAvailable;
	struct PossibleMoves turn;
};

void PlayersMoveMade(int x, int y, int x_dir, int y_dir, struct Space board[4][4]) {
	Player player = board[x][y].player;

	// find blocked position
	int futureXs = x;
	int futureYs = y;
	while (true) {
		futureXs += x_dir;
		futureYs += y_dir;
		if (futureXs < 0 || futureYs < 0 || futureXs >= 4 || futureYs >= 4) {
			break;
		}
		if (board[futureXs][futureYs].player != player && board[futureXs][futureYs].player != openSquare) {
			break;
		}
	};

	// check if its blocked
	if (abs(futureXs - x) <= 1 && abs(futureYs - y) <= 1) {
		// there is no move
		return;
	}

	// move the pieces
	if (abs(futureXs - x) == 2 || abs(futureYs - y) == 2) {
		// move everything into the first block
		board[x + x_dir][y + y_dir].player = board[x][y].player;
		board[x + x_dir][y + y_dir].value += board[x][y].value;
		board[x][y].player = openSquare;
		board[x][y].value = 0;
	}
	else if (abs(futureXs - x) == 3 || abs(futureYs - y) == 3) {
		// move one into the first block
		board[x + x_dir][y + y_dir].player = board[x][y].player;
		board[x + x_dir][y + y_dir].value += 1;
		board[x][y].value -= 1;

		// then move everything into the second block
		if (board[x][y].value != 0) {
			board[x + 2 * x_dir][y + 2 * y_dir].player = board[x][y].player;
			board[x + 2 * x_dir][y + 2 * y_dir].value += board[x][y].value;
		}

		board[x][y].player = openSquare;
		board[x][y].value = 0;
	}
	else {
		// move one into the first block
		board[x + x_dir][y + y_dir].player = board[x][y].player;
		board[x + x_dir][y + y_dir].value += 1;
		board[x][y].value -= 1;

		// then move two into the second block, and if not enough, move one only
		if (board[x][y].value >= 2) {
			board[x + 2 * x_dir][y + 2 * y_dir].player = board[x][y].player;
			board[x + 2 * x_dir][y + 2 * y_dir].value += 2;
			board[x][y].value -= 2;
		}
		else if (board[x][y].value == 1) {
			board[x + 2 * x_dir][y + 2 * y_dir].player = board[x][y].player;
			board[x + 2 * x_dir][y + 2 * y_dir].value += 1;
			board[x][y].value = 0;;
		}

		// then the rest to the last block
		if (board[x][y].value != 0) {
			board[futureXs - x_dir][futureYs - y_dir].player = board[x][y].player;
			board[futureXs - x_dir][futureYs - y_dir].value += board[x][y].value;
		}

		board[x][y].player = openSquare;
		board[x][y].value = 0;
	}
}

vector<PossibleMoves> findMoves(struct Space board[4][4], Player player) {
	vector<PossibleMoves> moveList;

	// collect the list of moves
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (board[i][j].player != player) continue;
			if (i - 1 >= 0 && j - 1 >= 0 && (board[i - 1][j - 1].player == openSquare || board[i - 1][j - 1].player == player)) moveList.push_back(PossibleMoves(i, j, -1, -1));
			if (i - 1 >= 0 && (board[i - 1][j].player == openSquare || board[i - 1][j].player == player)) moveList.push_back(PossibleMoves(i, j, -1, 0));
			if (i - 1 >= 0 && j + 1 < 4 && (board[i - 1][j + 1].player == openSquare || board[i - 1][j + 1].player == player)) moveList.push_back(PossibleMoves(i, j, -1, 1));
			if (j - 1 >= 0 && (board[i][j - 1].player == openSquare || board[i][j - 1].player == player)) moveList.push_back(PossibleMoves(i, j, 0, -1));
			if (j + 1 < 4 && (board[i][j + 1].player == openSquare || board[i][j + 1].player == player)) moveList.push_back(PossibleMoves(i, j, 0, 1));
			if (i + 1 < 4 && j - 1 >= 0 && (board[i + 1][j - 1].player == openSquare || board[i + 1][j - 1].player == player)) moveList.push_back(PossibleMoves(i, j, 1, -1));
			if (i + 1 < 4 && (board[i + 1][j].player == openSquare || board[i + 1][j].player == player)) moveList.push_back(PossibleMoves(i, j, 1, 0));
			if (i + 1 < 4 && j + 1 < 4 && (board[i + 1][j + 1].player == openSquare || board[i + 1][j + 1].player == player)) moveList.push_back(PossibleMoves(i, j, 1, 1));
		}
	}

	return moveList;
}

struct playersMove whiteturn(struct Space board[4][4]) {
	// get list of moves
	vector<PossibleMoves> moveList = findMoves(board, white);
	playersMove move;

	// initialize random seed
	srand(time(NULL));

	// randomly pick a move if there is one
	if (moveList.size() == 0) {
		move.moveAvailable = false;
		return move;
	}
	else {
		PossibleMoves randomMove = moveList[rand() % moveList.size()];
		PlayersMoveMade(randomMove.x, randomMove.y, randomMove.x_dir, randomMove.y_dir, board);
		move.moveAvailable = true;
		move.turn = randomMove;
		return move;
	}
}

void copyboard(struct Space newBoard[4][4], struct Space board[4][4]) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			newBoard[i][j].player = board[i][j].player;
			newBoard[i][j].value = board[i][j].value;
		}
	}
}

void OutputBoard(struct Space board[4][4]) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			cout << board[i][j].value;
			if (board[i][j].player == black) {
				cout << "b | ";
			}
			else if (board[i][j].player == white) {
				cout << "w | ";
			}
			else {
				cout << "  | ";
			}
		}
		cout << endl;
	}
}


int evaluationFunction(struct Space board[4][4]) {

	int blackFlagBoard[4][4] = { 0 };
	int whiteFlagBoard[4][4] = { 0 };



	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (board[i][j].player == black) {
				if (i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1].player == openSquare) blackFlagBoard[i - 1][j - 1] = 1;
				if (i - 1 >= 0 && board[i - 1][j].player == openSquare) blackFlagBoard[i - 1][j] = 1;
				if (i - 1 >= 0 && j + 1 < 4 && board[i - 1][j + 1].player == openSquare) blackFlagBoard[i - 1][j + 1] = 1;
				if (j - 1 >= 0 && board[i][j - 1].player == openSquare) blackFlagBoard[i][j - 1] = 1;
				if (j + 1 < 4 && board[i][j + 1].player == openSquare) blackFlagBoard[i][j + 1] = 1;
				if (i + 1 < 4 && j - 1 >= 0 && board[i + 1][j - 1].player == openSquare) blackFlagBoard[i + 1][j - 1] = 1;
				if (i + 1 < 4 && board[i + 1][j].player == openSquare) blackFlagBoard[i + 1][j] = 1;
				if (i + 1 < 4 && j + 1 < 4 && board[i + 1][j + 1].player == openSquare) blackFlagBoard[i + 1][j + 1] = 1;
			}
			else if (board[i][j].player == white) {
				if (i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1].player == openSquare) whiteFlagBoard[i - 1][j - 1] = 1;
				if (i - 1 >= 0 && board[i - 1][j].player == openSquare) whiteFlagBoard[i - 1][j] = 1;
				if (i - 1 >= 0 && j + 1 < 4 && board[i - 1][j + 1].player == openSquare) whiteFlagBoard[i - 1][j + 1] = 1;
				if (j - 1 >= 0 && board[i][j - 1].player == openSquare) whiteFlagBoard[i][j - 1] = 1;
				if (j + 1 < 4 && board[i][j + 1].player == openSquare) whiteFlagBoard[i][j + 1] = 1;
				if (i + 1 < 4 && j - 1 >= 0 && board[i + 1][j - 1].player == openSquare) whiteFlagBoard[i + 1][j - 1] = 1;
				if (i + 1 < 4 && board[i + 1][j].player == openSquare) whiteFlagBoard[i + 1][j] = 1;
				if (i + 1 < 4 && j + 1 < 4 && board[i + 1][j + 1].player == openSquare) whiteFlagBoard[i + 1][j + 1] = 1;
			}
		}
	}



	int whiteResult = 0;
	int blackResult = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			blackResult += blackFlagBoard[i][j];
			whiteResult += whiteFlagBoard[i][j];
		}
	}


	if (whiteResult == 0) {
		return POS_INF;
	}
	else if (blackResult == 0) {
		return NEG_INF;
	}
	else {
		return blackResult - whiteResult;
	}
}

int evaluateMove(struct Space board[4][4], struct PossibleMoves newMove, int numMove, Player player, int prune) {

	// make a copy of the board and make the move
	struct Space newBoard[4][4];
	copyboard(newBoard, board);
	PlayersMoveMade(newMove.x, newMove.y, newMove.x_dir, newMove.y_dir, newBoard);
	//printBoard(newBoard);

	if (numMove == depth) {
		//cout << "depth reached" << endl;
		return evaluationFunction(newBoard);
	}

	vector<PossibleMoves> moveList = findMoves(newBoard, player);
	if (moveList.size() == 0) {
		return evaluationFunction(newBoard);
	}

	// traverse the tree (depth-first)
	int result = (player == black ? NEG_INF : POS_INF);
	Player nextPlayer = (player == black ? white : black);
	int temp = 0;
	for (int i = 0; i < moveList.size(); i++) {
		temp = evaluateMove(newBoard, moveList[i], numMove + 1, nextPlayer, result);
		if (player == white) {
			// min node
			if (temp < prune) {
				// prune the node
				return temp;
			}
			else if (temp < result) {
				result = temp;
			}
		}
		else {
			// max node
			if (temp > prune) {
				// prune the node
				return temp;
			}
			else if (temp > result) {
				result = temp;
			}
		}
	}

	return result;
}


struct playersMove blacksturn(struct Space board[4][4]) {
	vector<PossibleMoves> moveList = findMoves(board, black);
	PossibleMoves bestMove;
	playersMove move;
	int result = NEG_INF;
	int temp;

	// check if there is any move left to do
	if (moveList.size() == 0) {
		//cout << "black has no move" << endl;
		move.moveAvailable = false;
		return move;
	}

	// get the best move
	for (int i = 0; i < moveList.size(); i++) {
		temp = evaluateMove(board, moveList[i], 0, black, NEG_INF);
		if (temp > result) {
			bestMove = moveList[i];
			result = temp;
		}
	}

	// make the move
	PlayersMoveMade(bestMove.x, bestMove.y, bestMove.x_dir, bestMove.y_dir, board);

	move.moveAvailable = true;
	move.turn = bestMove;
	return move;
}

int main() {

	struct Space board[4][4];

	// initialize
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			board[i][j].player = openSquare;
			board[i][j].value = 0;
		}
	}
	board[0][0].player = black;
	board[0][0].value = 10;
	board[3][3].player = white;
	board[3][3].value = 10;

	cout << "GAME STARTED" << endl;

	// black and white rotate make move
	struct playersMove move;
	int moveCount = 0;
	while (true)
	{
		move = blacksturn(board);
		moveCount++;
		if (move.moveAvailable) {
			cout << "black moved: x=" << move.turn.x << ", y=" << move.turn.y << ", x_dir=" << move.turn.x_dir << ", y_dir=" << move.turn.y_dir << endl;
		}
		else {
			cout << "black has no move left" << endl;
			break;
		}
		OutputBoard(board);

		move = whiteturn(board);
		moveCount++;
		if (move.moveAvailable) {
			cout << "white moved: x=" << move.turn.x << ", y=" << move.turn.y << ", x_dir=" << move.turn.x_dir << ", y_dir=" << move.turn.y_dir << endl;
		}
		else {
			cout << "white has no move left" << endl;
			break;
		}
		OutputBoard(board);
	}

	cout << "GAME ENDED: moveCount=" << moveCount << endl;
	return 0;
}