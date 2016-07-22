/*
 * File: life.cpp
 * --------------
 * Jeremiah M. Flaga
 * Started May 31, 2011 at about 9:00 PM
 *		Got PLATEAU and DONUT mode at about 11:00 PM
 *		Done everything except GenerateRandomStartingConfiguration() on June 2, 2011 at 2:40 AM
 *		Done everything except on June 2, 2011 at 3:20 AM
 */
 
#include "genlib.h"
#include "extgraph.h"
#include "simpio.h"
#include <iostream>
#include <fstream>

#include "grid.h"			//jmf needed for a grid
#include "lifeGraphics.h" 
#include "random.h" 

//type declaration
enum LifeMode { PLATEAU, DONUT, MIRROR};

//function prototypes
void Welcome();
void NextGeneration(Grid<int> & boardPrev, Grid<int> & boardNext, LifeMode mode);
void PlayGame();
int CalculateNeighbors(Grid<int> & board, int row, int col, LifeMode mode);
bool IsOccupied(Grid<int> & board, int row, int col, LifeMode mode);
void AskUserForInputFile(string prompt, ifstream & infile);
void FillGrid(Grid<int> & boardCurr, ifstream & infile);
bool HasChangesBetweenGenerations(Grid<int> & boardCurr);
void GenerateRandomStartingConfiguration(Grid<int> & board);



int main()
{
	SetWindowTitle("Life!");
	InitGraphics();
	Welcome();
	
	PlayGame();
	return 0;
}

void Welcome()
{
	SetPointSize(48);
	MovePen(GetWindowWidth()/2-TextStringWidth("Welcome To Life!")/2,GetWindowHeight()/2);
	DrawTextString("Welcome To Life!");
	//WaitForMouseDown();
	//Pause(2);
	UpdateDisplay();
	cout << "Welcome to the game of Life, a simulation of the lifecycle of a bacteria colony." << endl;
	cout << "Cells live and die by the following rules:" << endl;
	cout << "\tA cell with 1 or fewer neighbors dies of loneliness" << endl;
	cout << "\tLocations with 2 neighbors remain stable" << endl;
	cout << "\tLocations with 3 neighbors will spontaneously create life" << endl;
	cout << "\tLocations with 4 or more neighbors die of overcrowding" << endl;
	cout << "In the animation, new cells are dark and lighten as they age." << endl;
	cout << "Hit RETURN when ready: ";

	GetLine();
}

/*
 * Function: PlayGame
 * Usage: PlayGame();
 * --------------------------------
 * 
 * This function plays the game of life
 */

void PlayGame()
{
	LifeMode mode = PLATEAU;
	int rows, cols, choice;
	
	Grid<int> boardPrev,
			  boardCurr;
	
	ifstream infile;
	
	bool anotherRun;


	string configFiles[] = { "Diehard", "Dinner Table", "Fish", "Flower", 
							 "Glider", "Glider Explosion", "Glider Gun", "Quilt Square", 
							 "Seeds", "Simple Bar", "Snowflake", "Spiral", 
							 "StableDonut", "StableMirror", "StablePlateau", "TicTacToe"};

	do{ //start of simulation

		
		//----------------------------------------------------------
		do{
			cout << "\n\n\nYou can start your colony with random cells or read from a prepared file\n"
				 << "\nSelect a starting configuration file:\n"
				 << "	 0 - I would like a random start\n"
				 << "	 1 - I want to manually enter the filename\n"
				 << "	 2 - " << configFiles[0] << "\n"
				 << "	 3 - " << configFiles[1] << "\n"
				 << "	 4 - " << configFiles[2] << "\n"
				 << "	 5 - " << configFiles[3] << "\n"
				 << "	 6 - " << configFiles[4] << "\n"
				 << "	 7 - " << configFiles[5] << "\n"
				 << "	 8 - " << configFiles[6] << "\n"
				 << "	 9 - " << configFiles[7] << "\n"
				 << "	10 - " << configFiles[8] << "\n"
				 << "	11 - " << configFiles[9] << "\n"
				 << "	12 - " << configFiles[10] << "\n"
				 << "	13 - " << configFiles[11] << "\n"
				 << "	14 - " << configFiles[12] << "\n"
				 << "	15 - " << configFiles[13] << "\n"
				 << "	16 - " << configFiles[14] << "\n"
				 << "	17 - " << configFiles[15] << "\n"
				 << "\n		-> ";

			choice = GetInteger();
			switch(choice)
			{
				case 0:
					GenerateRandomStartingConfiguration(boardCurr);
					break;
				case 1:
					AskUserForInputFile("Please enter filename: ", infile);
					FillGrid(boardCurr, infile);
					break;
				case 2:			case 3:			case 4:			case 5:
				case 6:			case 7:			case 8:			case 9:
				case 10:		case 11:		case 12:		case 13:
				case 14:		case 15:		case 16:		case 17:
					
					infile.open(configFiles[choice-2].c_str());
					if(infile.fail())
					{
						cout << "File not found.\nBe sure to include the file \""
							 << configFiles[choice-2] << "\" in the same directory as the .exe file\n\n";
						infile.clear();
						continue;
					}

					FillGrid(boardCurr, infile);
					infile.close();

					break;

				default:
					cout << "Invalid choice...\n\n\n";
			}
		}while(choice > 17);// while invalid choice




		//--------------------------------------------------------------------
		InitLifeGraphics(boardCurr.numRows(), boardCurr.numCols());
		//display starting configuration file
		for (int i = 0; i < boardCurr.numRows(); i++)
			for (int j = 0; j < boardCurr.numCols(); j++)
				DrawCellAt(i, j, boardCurr[i][j]);
		UpdateDisplay();
		boardPrev.resize(boardCurr.numRows(), boardCurr.numCols());





		//------------------------------------------------------
		do{
			cout << "Select how cells on the edges will count their neighbors:\n"
				 << "	 1 - PLATEAU mode (cells on edges just have fewer neighbors)\n"
				 << "	 2 - DONUT mode   (cells on edge wrap around to find neighbors)\n"
				 << "	 3 - MIRROR mode  (cells on edge reflect back to find neighbors)\n"
				 << "\n		-> ";

			choice = GetInteger();
			switch(choice)
			{
				case 1:
					mode = PLATEAU;
					break;
				case 2:
					mode = PLATEAU;
					break;
				case 3:
					mode = PLATEAU;
					break;
				default:
					cout << "Invalid choice...\n\n\n";
			}
		}while(choice > 3);// while invalid choice




		//----------------------------------------------------------
		double pauseDuration = 0;
		do{
			cout << "Select how fast to run the simulation:\n"
				 << "	 0 - As fast as this chip can go!\n"
				 << "	 1 - 0.1 seconds\n"
				 << "	 2 - 0.2 seconds\n"
				 << "	 3 - 0.5 seconds\n"
				 << "	 4 - 1.0 seconds\n"
				 << "	 5 - 2.0 seconds\n"
				 << "	 6 - 5.0 seconds\n"
				 << "	 7 - Manual (Hit \'Enter\' to advance to the next generation)\n"
				 << "\n		-> ";

			choice = GetInteger();
			switch(choice)
			{
				case 0:
					pauseDuration = 0;
					break;
				case 1:
					pauseDuration = 0.1;
					break;
				case 2:
					pauseDuration = 0.2;
					break;
				case 3:
					pauseDuration = 0.5;
					break;
				case 4:
					pauseDuration = 1.0;
					break;
				case 5:
					pauseDuration = 2.0;
					break;
				case 6:
					pauseDuration = 5.0;
					break;
				case 7:
					pauseDuration = 10; //jmf anything greater than 5.0
					break;
				
				default:
					cout << "Invalid choice...\n\n\n";
			}
		}while(choice > 7);// while invalid choice

		

		
		//-------------------------------------------------------------
		//
		if( !(pauseDuration > 5.0) )
			cout << "Click and hold the mouse button on the graphics window to end the simulation\n\n";
		
		do
		{
			//display grid as graphics
			for (int i = 0; i < boardCurr.numRows(); i++)
			{
				for (int j = 0; j < boardCurr.numCols(); j++)
				{
					DrawCellAt(i, j, boardCurr[i][j]);
					//cout << boardCurr[i][j] << " ";
				}
				//cout << endl;
			}

			if(pauseDuration > 5.0) //jmf ( manual )
			{
				cout << "Press \'Enter\'";
				GetLine();
			}
			else
				Pause(pauseDuration);

			if(MouseButtonIsDown())
				break;
			boardPrev = boardCurr;
			NextGeneration(boardPrev, boardCurr, mode);

			UpdateDisplay();

		}while( HasChangesBetweenGenerations(boardCurr) );


		//-------------------------------------------------------------------
		//ask for another run
		string str;
		while(true)
		{
			cout << "Would you like to run another simulation? ";
			
			str = GetLine();
			if(ConvertToLowerCase(str) == "yes")
			{
				anotherRun = true;
				break;
			}
			else if(ConvertToLowerCase(str) == "no")
			{
				anotherRun = false;
				exit(0);
				break;
			}
			else
				cout << "Please answer yes or no.\n";
		}

	}while(anotherRun);//end of simulation

}



/*
 * Function: NextGeneration
 * Usage: boardCurr = NextGeneration(baordPrev, mode);
 * --------------------------------
 * 
 * This function returns a grid which contains the next generation 
 * given the previous generation and mode of generating generations as arguments
 */

void NextGeneration(Grid<int> & boardPrev, Grid<int> & boardNext, LifeMode mode)
{
	int nNeighbors = 0;

	for(int i = 0; i < boardPrev.numRows(); i++)
	{
		for(int j = 0; j < boardPrev.numCols(); j++)
		{
			nNeighbors = CalculateNeighbors(boardPrev, i, j, mode);

			if(nNeighbors == 1)
				boardNext[i][j] = 0;
			else if(nNeighbors == 2)
				boardNext[i][j] = (boardPrev[i][j] == 0) ?  (0) : (boardPrev[i][j] + 1);
			else if(nNeighbors == 3)
				boardNext[i][j] = boardPrev[i][j] + 1;
			else
				boardNext[i][j] = 0;
		}
	}
}




void GenerateRandomStartingConfiguration(Grid<int> & board)
{
	cout << " GenerateRandomStartingConfiguration \n";
	board.resize(10, 10);
	Randomize();

	for(int i =0; i < board.numRows(); i++)
		for(int j =0; j < board.numCols(); j++)
			board[i][j] = RandomChance(0.5);

}



/*
 * Function: HasNoChangesBetweenGenerations
 * Usage: while( HasNoChangesBetweenGenerations(boardCurr) ){...}
 * --------------------------------
 * 
 * This function returns true if all cells in a grid contains zero or MaxAge,
 *  (OR a number between 0 AND MaxAge
 *  which means that no new alive cells are being created for a while already
 */
bool HasChangesBetweenGenerations(Grid<int> & board)
{
	foreach(int cell in board)
		if(cell > 0 && cell <= MaxAge)
			return true;

	cout << "no Changes\n";
	return false;
}



void FillGrid(Grid<int> & boardCurr, ifstream & infile)
{
	string str;

	infile.seekg(ios::beg); // go to the beginning of the file: Added on Aug 21, 2011

	while(infile.peek() == '#') // Ignore comments
	{
		getline(infile, str);
		//cout << str << endl;
	}

	int rows, cols;
	getline(infile, str);
	//cout << "str = " << str <<endl;
	rows = StringToInteger(str);
	getline(infile, str);
	cols = StringToInteger(str);

	//cout << "rows = " << rows << " cols = " << cols << endl;

	boardCurr.resize(rows, cols);

	for(int i = 0; i < rows; i++)
	{
		getline(infile, str);

		for(int j = 0; j < cols; j++)
		{
			if(str[j] == '-')
				boardCurr[i][j] = 0;
			else
				boardCurr[i][j] = 1;
			//cout << boardCurr[i][j] << ' ';
		}
		//cout << endl;
	}
}



/*
 * Function: CalculateNeighbors
 * Usage: nNeighbors = CalculateNeighbors(boardPrev, i, j, mode);
 * --------------------------------
 * 
 * This function returns the number of neighbors of a spwcified cell in a grid
 */
int CalculateNeighbors(Grid<int> & board, int row, int col, LifeMode mode)
{
	int nNeighbors = IsOccupied(board, row-1, col-1, mode) +
					 IsOccupied(board, row-1, col  , mode) +
					 IsOccupied(board, row-1, col+1, mode) +
					 IsOccupied(board, row  , col-1, mode) +

					 IsOccupied(board, row+1, col+1, mode) +
					 IsOccupied(board, row+1, col  , mode) +
					 IsOccupied(board, row+1, col-1, mode) +
					 IsOccupied(board, row  , col+1, mode);

	
	
	return nNeighbors;

}

/*
 * Function: IsOccupied
 * Usage: occupied = IsOccupied(board, row, col)
 * --------------------------------
 * 
 * This function returns the number of neighbors of a spwcified cell in a grid
 */
bool IsOccupied(Grid<int> & board, int row, int col, LifeMode mode)
{
	int nRows = board.numRows();
	int nCols = board.numCols();
	if(mode == PLATEAU)
	{
		if(row >= 0 && row < nRows)
			if(col >= 0 && col < nCols)
			{
				if(board[row][col] > 0)
					return true; // or 1 (ONE)
			}
	}
	else if(mode == DONUT)
	{
		if(row == -1)
			row = nRows-1;
		if(col == -1)
			col = nCols-1;

		row = row % nRows;
		col = col % nCols;

		if(board[row][col] > 0)
			return true; // or 1 (ONE)
				
	}
	else if(mode == MIRROR)
	{
		if(row == -1)
			row = 0;
		if(col == -1)
			col = 0;

		if(row == nRows)
			row = nRows-1;
		if(col == nCols)
			col = nCols-1;

		if(board[row][col] > 0)
			return true; // or 1 (ONE)
	}

	return false; // or 0 (ZERO)
}


/* From CS 106B Course reader chapter 4, p 131
* Opens a text file whose name is entered by the user. If the file
* does not exist, the user is given additional chances to enter a
* valid file. The prompt string is used to tell the user what kind
* of file is required.
*/
void AskUserForInputFile(string prompt, ifstream & infile) {
	while (true) {
		cout << prompt;
		string filename = GetLine();
		infile.open(filename.c_str());
		if (!infile.fail()) 
			break;
		cout << "Unable to open " << filename << endl;
		infile.clear();
	}
}