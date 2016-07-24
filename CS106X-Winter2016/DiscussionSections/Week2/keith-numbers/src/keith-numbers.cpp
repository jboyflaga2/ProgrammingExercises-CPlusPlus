/**
 * Provides the implementation of a program that counts up
 * from 1, but lists as many Keith numbers as it can find up to 10000.
 */

/*
 * Jboy Flaga
 * Started:     July 25, 2016 12:20 AM
 * Finished:
 *
 */
#include <iostream>
#include <fstream>
using namespace std;

#include "console.h"
#include "simpio.h"
#include "vector.h"
#include "queue.h"
#include "stack.h"

void introduce() {
	ifstream infile("keith-numbers.txt");
	if (!infile) error("Failed to open keith-numbers.txt.  Terminating.");
	
	while (true) {
		string line;
		getline(infile, line);
		if (infile.fail()) break;
		cout << line << endl;
	}
	
	cout << endl;
	getLine("Hit [Enter] to start: ");
	cout << endl;
}

bool isKeithNumber(int n) {
    Queue<int> currentNumbersQueue;
    Stack<int> tempStack;

    int currentN = n, rightmostDigit, numOfDigits = 0;
    while(currentN > 0) {
        rightmostDigit = currentN % 10;
        tempStack.push(rightmostDigit);
        currentN = currentN / 10;
        ++numOfDigits;
    }

    //enqueue each digit (leftmost digit first)
    while(!tempStack.isEmpty()) {
        currentNumbersQueue.enqueue(tempStack.pop());
    }

    // add each digit in the queue and enqueue them again except the first one
    int currentSum, currentNumFromQueue;
    while(true) {
        currentSum = 0;
        for(int i = 0; i < numOfDigits; i++) {
            currentNumFromQueue = currentNumbersQueue.dequeue();
            currentSum = currentSum + currentNumFromQueue;

            if(i != 0) { // do not enqueue the leftmost digit
                currentNumbersQueue.enqueue(currentNumFromQueue);
            }
        }

        if(currentSum == n) { return true; }

        if(currentSum > n) { return false; }

        //enqueue next number
        currentNumbersQueue.enqueue(currentSum);

    }

    return false;
}

int main() {
	introduce();
    for (int n = 1; n < 10000; n++) {
		// replace the following with the logic needed to print
		// only Keith numbers, along with the sequence that
		// proves it's a Keith number (as described in the section handout)
        if(isKeithNumber(n))
            cout << n << endl;
	}
	
	cout << endl;
	cout << "That's all of them! " << endl;
	return 0;
}
