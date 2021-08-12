// Art.IntelliAssig1.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>
#include <math.h>
#include <string>
#include <unordered_map>
using namespace std;

//const goalState values for easier comparisons through out
const string goalState = "123804765";

//Array that stores the row and column location of each tile in goal state
const int goalPositions[9][2] = { {1,1}, {0,0}, {0,1},
							{0,2}, {1,2}, {2,2},
							{2,1}, {2,0}, {1,0} };

//two methods used to convert tile idx location in string to Row and Col
int idxToRow(int r) {
	return r / 3;
}
int idxToCol(int c) {
	return c % 3;
}

//class to store the puzzle and manipulation methods for puzzle board
class eightPuzzle {
private:
	//using string to store a board's status
	string board;
	//int value used to store index location of the empty tile
	int eTile;
public:
	eightPuzzle() {
		board = "";
		eTile = -1;
	}

	//initialization method 
	eightPuzzle(string other) {
		board = other;
		//find and assign the empty tile location
		for (unsigned int i = 0; i < other.size(); i++) {
			if (other.at(i) == '0') {
				eTile = i;
				return;
			}
		}
	}

	//calculates the h value using the # of tiles out of place 
	int misHValue() {
		int answer = 0;
		for (int r = 0; r < 9; r++) {
			if (board[r] != '0' && board[r] != goalState[r])
				answer++;
		}
		return answer;
	}

	//method to calculate the h value using the Manhattan heuristic function
	int manHValue() {
		int answer = 0;
		/*tile locations are converted into Row and Col first then compared
			to goal position.*/
		for (int r = 0; r < 9; r++) {
			int idx = board[r] - '0';
			if (idx != 0) {
				answer += abs((idxToRow(r)) - goalPositions[idx][0])
					+ abs((idxToCol(r)) - goalPositions[idx][1]);
			}
		}
		return answer;
	}

	//method to check if two states match
	bool matchState(eightPuzzle state) {
		if (board.compare(state.board) == 0)
			return true;
		else
			return false;
	}

	//Getter method to return board
	string getBoard() {
		return board;
	}

	//checks if empty tile can move up, if yes, move it up
	bool moveUp() {
		if (idxToRow(eTile) <= 0)
			return false;
		else {
			board[eTile] = board[eTile - 3];
			board[eTile - 3] = '0';
			eTile = eTile - 3;
		}
		return true;
	}

	//checks if empty tile can move down, if yes, move it down
	bool moveDown() {
		if (idxToRow(eTile) >= 2)
			return false;
		else {
			board[eTile]= board[eTile + 3];
			board[eTile + 3] = '0';
			eTile = eTile + 3;
		}
		return true;
	}

	//checks if empty tile can move right, if yes, move it right
	bool moveRight() {
		if (idxToCol(eTile) >= 2)
			return false;
		else {
			board[eTile] = board[eTile + 1];
			board[eTile + 1] = '0';
			eTile = eTile + 1;
		}
		return true;
	}

	//checks if empty tile can move left, if yes, move left
	bool moveLeft() {
		if (idxToCol(eTile) <= 0)
			return false;
		else {
			board[eTile] = board[eTile - 1];
			board[eTile - 1] = '0';
			eTile = eTile - 1;
		}
		return true;
	}
};

//class node mainly to keep track of children and parents of a puzzle node
class node {
public:
	eightPuzzle* state;
	int hValue, gValue;
	vector<node*> children;
	node* parent;

	/*char variable used to keep track of the nodes move from previous state 
	'u'for up, 'd' for down, 'l' for left, 'r' for right, 'o' for original 
	state*/
	char move;

	//initialization method
	node() {
		state = new eightPuzzle();
		hValue = 0;
		gValue = 0;
		parent = NULL;
		move = '\0';
	}

	//initialization methods that also calculate the h value depending
	//on which h value is needed
	node(eightPuzzle s, char hType) {
		state = new eightPuzzle(s);
		if(hType == 'i')
			hValue = (*state).misHValue();
		else {
			hValue = (*state).manHValue();
		}
		gValue = 0;
		parent = NULL;
		move = 'o';
	}

	//copy constructor
	node(const node& b) {
		state = new eightPuzzle(*(b.state));
		hValue = b.hValue;
		gValue = b.gValue;
		move = b.move;
		children = b.children;
		parent = b.parent;
	}

	//assignemnt operator constructor
	node& operator=(const node& b) {
		if (this != &b) {
			if (state != nullptr)
				delete state;
			state = new eightPuzzle(*(b.state));
			hValue = b.hValue;
			gValue = b.gValue;
			move = b.move;
			children = b.children;
			parent = b.parent;
		}
		return *this;
	}

	//move constructor
	node(node&& other) noexcept
		: state(NULL) {
		state = other.state;
		hValue = other.hValue;
		gValue = other.gValue;
		move = other.move;
		children = other.children;
		parent = other.parent;

		other.state = NULL;
	}
	~node() {
		delete state;
	}

	//method that returns the f value
	int fValue() {
		return (hValue + gValue);
	}

	/* method that determines which moves of the empty tile are possible
	and add the possible new states as children to the old state*/
	void addChildren(int gValue, char hType) {
		if (move != 'd' && (*state).moveUp()) {
			node* n = new node(*state, hType);
			n->gValue = gValue;
			n->move = 'u';
			n->parent = this;
			children.push_back(n);
			(*state).moveDown();
		}

		if (move != 'u' && (*state).moveDown()) {
			node* n = new node(*state, hType);
			n->gValue = gValue;
			n->move = 'd';
			n->parent = this;
			children.push_back(n);
			(*state).moveUp();
		}

		if (move != 'r' && (*state).moveLeft()) {
			node* n = new node(*state, hType);
			n->gValue = gValue;
			n->move = 'l';
			n->parent = this;
			children.push_back(n);
			(*state).moveRight();
		}

		if (move != 'l' && (*state).moveRight()) {
			node* n = new node(*state, hType);
			n->gValue = gValue;
			n->move = 'r';
			n->parent = this;
			children.push_back(n);
			(*state).moveLeft();
		}
	}

	/*same method as above except only add the children with f value
	less than the current L value. This method also returns the children
	in a sorted order.*/
	void addChildLessL(int gValue, char hType, int L) {
		if (move != 'd' && (*state).moveUp()) {
			node* n = new node(*state, hType);
			n->gValue = gValue;
			if (n->fValue() < L) {
				n->move = 'u';
				n->parent = this;
				children.push_back(n);
			}		
			(*state).moveDown();
		}

		if (move != 'u' && (*state).moveDown()) {
			node* n = new node(*state, hType);
			n->gValue = gValue;
			if (n->fValue() < L) {
				n->move = 'd';
				n->parent = this;
				if (children.empty()) {
					children.push_back(n);
				}
				else{
					insertCOrdered(n);
				}
			}
			(*state).moveUp();
		}

		if (move != 'r' && (*state).moveLeft()) {
			node* n = new node(*state, hType);
			n->gValue = gValue;
			if (n->fValue() < L) {
				n->move = 'l';
				n->parent = this;
				if (children.empty())
					children.push_back(n);
				else {
					insertCOrdered(n);
				}
			}
			(*state).moveRight();
		}

		if (move != 'l' && (*state).moveRight()) {
			node* n = new node(*state, hType);
			n->gValue = gValue;
			if (n->fValue() < L) {
				n->move = 'r';
				n->parent = this;
				if (children.empty())
					children.push_back(n);
				else{
					insertCOrdered(n);
				}
			}
			(*state).moveLeft();
		}
	}
	
	/*helper method for the addChildLessL function. This function inserts 
	the children in a sorted order into the children vector*/
	void insertCOrdered(node* n){
		int idx = 0;
		vector<node*>::iterator i = children.begin();
		for (vector<node*>::iterator i = children.begin();
			i != children.end(); i++) {
			if (n->fValue() >= (*i)->fValue()) {
				children.insert(i, n);
				break;
			}
			idx++;
		}
		if (idx == children.size())
			children.push_back(n);
	}

	/*calculates the new L number for iterative deepening A* search. It
	returns the minimum f value which exceeded the old L number among the 
	children of an expanded node. When a tie occurs, I added 1 to old L*/
	int newL(int oldL) {
		int lowest = 1000000;
		int f;
		if (move != 'd' && (*state).moveUp()) {
			f = (*state).manHValue() + gValue + 1;
			if (oldL < f && f < lowest) {
				lowest = (*state).manHValue() + gValue + 1;
			}
			(*state).moveDown();
		}
		if (move != 'u' && (*state).moveDown()) {
			f = (*state).manHValue() + gValue + 1;
			if (oldL < f && f < lowest) {
				lowest = (*state).manHValue() + gValue + 1;
			}
			(*state).moveUp();
		}
		if (move != 'r' && (*state).moveLeft()) {
			f = (*state).manHValue() + gValue + 1;
			if (oldL < f && f < lowest) {
				lowest = (*state).manHValue() + gValue + 1;
			}
			(*state).moveRight();
		}
		if (move != 'l' && (*state).moveRight()) {
			f = (*state).manHValue() + gValue + 1;
			if (oldL < f && f < lowest) {
				lowest = (*state).manHValue() + gValue + 1;
			}
			(*state).moveLeft();
		}
		if (lowest == 1000000)
			return oldL + 1;
		return lowest;
	}

};

//class to store the nodes of open list for the two A* searches.
//This is a priority queue which automatically keeps the least f value first
class priorityQueue {
private:
	vector<node*> queue;

public:

	//initialization methods 
	priorityQueue() {}

	priorityQueue(node* n) {
		queue.push_back(n);
	}

	//method for adding new nodes to the queue while keeping the queue sorted.
	void add(node* n) {
		if (empty() || n->fValue() >= queue.back()->fValue())
			queue.push_back(n);
		else if (n->fValue() <= queue.at(0)->fValue())
			queue.insert(queue.begin(), n);
		else {
			int upperBound = queue.size() - 1;
			int lowerBound = 0;
			int index = queue.size() / 2;
			while (index < queue.size() - 1 && index > 0) {
				if (queue.at(index)->fValue() < n->fValue()) {
					lowerBound = index + 1;
				}
				else if (queue.at(index)->fValue() > n->fValue()) {
					upperBound = index - 1;
				}
				else {
					queue.insert(queue.begin() + index, n);
					return;
				}
				index = (lowerBound + upperBound) / 2;
			}
		}
	}

	//method to delete a node using index
	void erase(int i) {
		queue.erase(queue.begin() + i);
	}

	//method to delete the first element of queue
	void pop() {
		queue.erase(queue.begin());
	}

	//getter methods to get any nodes from this queue
	node** begin() {
		return &(queue.front());
	}

	node** at(int i) {
		return &(queue.at(i));
	}
	node** end() {
		return &(queue.back());
	}

	//getter method to return the size of queue
	int size() {
		return int(queue.size());
	}

	//method to check if queue is empty
	bool empty() {
		if (queue.size() == 0)
			return true;
		return false;
	}
};

/*method that performs A* search using the hType parameter to determine the 
required heuristic function. */
void aStarSearch(eightPuzzle cState, int& numNodeExp, string& moveSeq, char hType) {
	
	//initialize the open states queue and add the start node to it
	node* start = new node(cState, hType);
	priorityQueue* openS = new priorityQueue(start);

	//use a hashmap to store pointers to the open nodes list
	//so as to enhance searching time
	unordered_map<string, node*> openM;
	string key = start->state->getBoard();
	openM.emplace(key, start);

	//use hashmap to store the pointers to the closed nodes
	unordered_map<string, node*> closedS;
	
	int idx = 0;
	//boolean value to check if goal state has been found
	bool goal = false;
	while (!goal) {

		//get the lowest fValue node and increment the number of nodes expanded
		node** current = (*openS).begin();
		node* curr = &(**current);
		numNodeExp++;
		
		// if that node equals the goal state
		if ((*curr).hValue == 0) {

			//we've found the goal!
			//traverse the tree to get the move sequences, and return; 
			cout << "we reached goal!!!!" << endl;
			moveSeq.insert(moveSeq.begin(), (*curr).move);
			while ((*curr).parent != NULL) {
				moveSeq.insert(moveSeq.begin(), (*curr).parent->move);
				*curr = *((*curr).parent);
			}
			goal = true;
			return;
		}

		string key = (*(*openS).begin())->state->getBoard();
		closedS.emplace(key, (*(*openS).begin()));
		(*openS).pop();
		openM.erase(key);

		//find and add all the children states of the current state	
		(*curr).addChildren(curr->gValue+1, hType);

		//for each children states, do the following
		bool done = false;
		for (vector<node*>::iterator child = (*curr).children.begin();
			child != (*curr).children.end(); child++) {
			done = false;
			
			//if the child state is also in the closed list 
			if (!closedS.empty()) {
				unordered_map<string, node*>::const_iterator closedN
					= closedS.find((*child)->state->getBoard());
				if (closedN != closedS.end()) {

					//and the child's gValue is lower than the gValue of the node in the closed list
					if ((*child)->gValue < closedN->second->gValue) {

						//remove that node from closed state, and add the child node to the open list
						(*openS).add(*child);
						string key = (*child)->state->getBoard();
						openM.emplace(key, *child);
						closedS.erase(closedN);
					}
					//done with current child
					done = true;
				}
			}
	
			//elseif the current child is in the open list
			if (!done && (!openM.empty())) {
				string key = (*child)->state->getBoard();
				unordered_map<string, node*>::const_iterator openN
					= openM.find(key);
				if (openN != openM.end()) {

					//and the child's gValue is lower than the gValue of the node in the open list
					if ((*child)->gValue < openN->second->gValue) {
							
						//erase the found node from open list and add the current child to open list
						openM.erase(key);
						openM.emplace(key, *child);

						//also update the open list priority queue with the same action
						for (int i = 0; i < (*openS).size(); i++) {
							node* openSN = (*(*openS).at(i));
							if ((*child)->state->matchState((*(*openSN).state))) {
								if ((*child)->gValue < (*openSN).gValue) {
									(*openS).erase(i);
									(*openS).add(*child);
									break;
								}
							}
						}
					}
					//done with current child
					done = true;
					break;
				}
			}

			//else the current child is not in open or the closed list
			if (!done) {

				//add the child to open list
				(*openS).add(*child);
				string key = (*child)->state->getBoard();
				openM.emplace(key, *child);
			}
		}
	}
}

/*method that performs Depth-first Branch and Bound search and the Iterative deepening A*
search. This method uses the searchType parameter to distinguish the search method employed */
void dFBBSearch(eightPuzzle cState, int &numNodeExp, string& moveSeq, char searchType) {
	//keeps star time to help find the optimal solution found time for DFBnB
	auto startTime = chrono::steady_clock::now();

	//node to keep track of the goal node when found
	node* goalNode = new node();

	//node used to keep track of the last L value for iterative deepening search 
	// and helps to find the next L value
	node* lastNode = new node(cState, 'm');

	//boolean value used for Iterative Deepening search to see if goal state has been found
	bool itGoal = false;

	//vector used to store all the new candidate L values
	vector<int> newL;

	//variable to store the current L value
	int L;

	//if Depth-first branch and bound seach L = 100000, if Iterative deepening search  
	// L = the smallest fvalue from the last expanded layer
	if (searchType == 'd')
		L = 100000;
	else {
		L = lastNode->fValue();
	}
	
	//while Iterative deepening search has not found the goal
	while (!itGoal) {

		//initialize open list & closed list, add start node to open list
		node* start = new node(cState, 'm');
		vector<node*> openL;
		openL.push_back(start);

		//the original gValue is equal to 0
		int gValue = 0;

		//while there are still nodes left in the open list
		while (!(openL).empty()) {
			
			//take out the last node from open list make it the current node 
			//and increment the number of nodes expended
			node** current = &(openL.back());
			node* curr = &(**current);
			numNodeExp++;


			//if the current node matches the goal state
			if ((curr)->hValue == 0) {
				
				//and if the current node's f value is lower than the current L
				if ((curr)->fValue() < L) {

					//update the time to find the new goal node
					auto found = chrono::steady_clock::now();
					auto diff = found - startTime;
					auto diffSec = chrono::duration_cast <chrono::milliseconds>(diff);
					//cout << diffSec.count() << "ms" << endl; used to calculate the time optimal
					//solution was found for BFBnB. * commented out to save space on output window

					//update the L value, update the goal node, delete the current node from open list
					L = (curr)->fValue();
					goalNode = (curr);
					openL.pop_back();

					//itGoal is set to true to signal that for Iterative Deepending search, we've found the goal
					itGoal = true;
				}
			}

			//else deletet the current node from open list
			else {
				openL.pop_back();

				//find all current node's children and add to open list
				(curr)->addChildLessL(curr->gValue + 1, 'm', L);
				for (unsigned int i = 0; i < (*curr).children.size(); i++) {
						openL.push_back((*curr).children.at(i));
				}
				//for Iterative deepening search, add all new candidate L values to the newL vector
				if (searchType == 'i') {
					newL.push_back((curr)->newL(L));
				}
			}
		}
		//end of search for Depth-first branch and bround 
		if (searchType == 'd')
			break;

		//else if Iterative deepening search, find the new L value
		else {
			L = newL[0];
			for (unsigned int i = 0; i < newL.size(); i++) {
				if (newL[i] < L)
					L = newL[i];
			}
			//clear the newL vector list for use next time
			newL.clear();
		}
	}
	//when done with search, traverse path from goal node back to start node
	//and store the moves in moveSeq vector
	cout << "we reached goal!!!!" << endl;
	moveSeq.insert(moveSeq.begin(), (*goalNode).move);
	while ((*goalNode).parent != NULL) {
		moveSeq.insert(moveSeq.begin(), (*goalNode).parent->move);
		*goalNode = *((*goalNode).parent);
	}
}
int main() {

	//struct to store each start state and the various statistics needed for report
	struct puzzleProblem {
		string stateName;
		string puzzleBoard;
		int numNodeExp[4];
		string moveSeq[4];
		int exeTime[4];
	};

	//initailize all the states and add them to the puzzleProblem vector
	puzzleProblem easy{ "easy", "134862705", 0 };
	puzzleProblem medium{ "medium", "281043765", 0 }; 
	puzzleProblem hard{ "hard", "281463075", 0};
	puzzleProblem worst{ "worst", "567408321", 0};
	vector<puzzleProblem> answer{ easy, medium, hard, worst };

	//for each search algorithm, input all four start states and record the number of nodes expended,
	//the optimal move sequence found, and the time it takes to complete search.

	//A* search with manhattan heuristic
	for (unsigned int i = 0; i < answer.size(); i++) {
		auto start = chrono::steady_clock::now();
		aStarSearch(answer[i].puzzleBoard, answer[i].numNodeExp[0], answer[i].moveSeq[0], 'm');
		auto end = chrono::steady_clock::now();
		auto diff = end - start;
		auto diffSec = chrono::duration_cast<chrono::milliseconds>(diff);
		answer[i].exeTime[0] = diffSec.count();

		cout << "Manhattan heuristic A* - " << answer[i].stateName << ": # nodes expended: " << answer[i].numNodeExp[0];
		cout << "move sequence is: " << answer[i].moveSeq[0] << endl << "Execution time ms: " << answer[i].exeTime[0] << endl << endl;
	}

	//Depth-first Branch and Bound search
	for (unsigned int i = 0; i < answer.size(); i++) {
		auto start = chrono::steady_clock::now();
		dFBBSearch(answer[i].puzzleBoard, answer[i].numNodeExp[1], answer[i].moveSeq[1], 'd');
		auto end = chrono::steady_clock::now();
		auto diff = end - start;
		auto diffSec = chrono::duration_cast<chrono::milliseconds>(diff);
		answer[i].exeTime[1] = diffSec.count();

		cout << "Depth-first branch and bound - " << answer[i].stateName << ": # nodes expended: " << answer[i].numNodeExp[1];
		cout << "move sequence is: " << answer[i].moveSeq[1] << endl << "Execution time ms: " << answer[i].exeTime[1] << endl << endl;
	}

	//Iterative deepening search
	for (unsigned int i = 0; i < answer.size(); i++) {
		auto start = chrono::steady_clock::now();
		dFBBSearch(answer[i].puzzleBoard, answer[i].numNodeExp[2], answer[i].moveSeq[2], 'i');
		auto end = chrono::steady_clock::now();
		auto diff = end - start;
		auto diffSec = chrono::duration_cast<chrono::milliseconds>(diff);
		answer[i].exeTime[2] = diffSec.count();

		cout << "Iterative deepening A* - " << answer[i].stateName << ": # nodes expended: " << answer[i].numNodeExp[2];
		cout << "move sequence is: " << answer[i].moveSeq[2] << endl << "Execution time ms: " << answer[i].exeTime[2] << endl << endl;
	}

	//A* search with number of tiles out of place heuristic
	for (unsigned int i = 0; i < answer.size(); i++) {
		auto start = chrono::steady_clock::now();
		aStarSearch(answer[i].puzzleBoard, answer[i].numNodeExp[3], answer[i].moveSeq[3], 'i');
		auto end = chrono::steady_clock::now();
		auto diff = end - start;
		auto diffSec = chrono::duration_cast<chrono::milliseconds>(diff);
		answer[i].exeTime[3] = diffSec.count();

		cout << "Misplaced Tiles A* search - " << answer[i].stateName << ": # nodes expended: " << answer[i].numNodeExp[3];
		cout << "move sequence is: " << answer[i].moveSeq[3] << endl << "Execution time ms: " << answer[i].exeTime[3] << endl << endl;
	}

	return 0;
}

