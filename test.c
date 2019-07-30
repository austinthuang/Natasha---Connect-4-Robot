/*All required constants, each column constant was tuned so that when the robot
starts to drive to each column, it would stop in the perfect position*/
const int BOARD_ROWS = 6;
const int BOARD_COLS = 7;
const int EMPTY_SLOT = 0;
const float COL_1 = 11.58;
const float COL_2 = 15.38;
const float COL_3 = 19.08;
const float COL_4 = 22.85;
const float COL_5 = 26.92;
const float COL_6 = 30.7;
const float COL_7 = 34.9;
const float CENT_CENT = 3.42;
const float WHEEL_RAD = 2.1575;

/*main functional arrays declared here since a 2D array cannot be passed into
a function in RobotC*/
int gameBoard[BOARD_ROWS][BOARD_COLS]; //main gameboard
int dummyBoard[BOARD_ROWS][BOARD_COLS];	//dummy gameboard
int scores[BOARD_COLS]; //scores array to keep track of minimax scores

//Dispenses a chip by running motorC in one direction for 0.7 seconds and back
void dispense()
{
	motor[motorC] = 20; //motorC is for dispenser
	wait1Msec(700);
	motor[motorC] = -20;
	wait1Msec(700);
	motor[motorC] = 0;
}

/*calibrates position of color sensor by defining a "zero" position to retain 
accuracy of scanning*/
void recallVerSensors()
{
	motor[motorB]= -40; //motorB is for colour sensor
	while(SensorValue[S1] == 0){} //recalls until touch sensor is hit
	motor[motorB] = 0;
	nMotorEncoder[motorB] = 0;
}

/*Calibrates position of main drive motor by defining a "zero" position to
retain accuracy of scanning and dispensing (critical to function properly)*/
void recallHorSensors()
{
	motor[motorA]= -35; //motorA is for main drive motor
	while(SensorValue[S2] == 0){} //recalls until touch sensor is hit
	motor[motorA] = 0;
	nMotorEncoder[motorA] = 0;
}

//Function to check if board is present, if not the game will end
int checkBoard()
{
	recallVerSensors(); //First recalls colour sensor and dispensing hub
	recallHorSensors();
	nMotorEncoder[motorA]= 0;
	motor[motorA] = 25;
	//drives until where last column of board would be
	while (nMotorEncoder[motorA] < ((180/(PI*WHEEL_RAD))*COL_7))
	{
		//if board is there, recalls back to zero position, ready to play
		if (SensorValue[S3] == (int)colorBlue) 
		{
			recallHorSensors();
			return 1;
		}
	}
	motor[motorA]=0;
	setSoundVolume(100);
	playSoundFile("Error alarm"); //If not seen, make error sound
	sleep(1000);
	recallHorSensors();
	return -1;	//End program since board was not found

}

//checks for a horizontal win, relative to last token placed
bool rowCheck(int row, int col, int token)
{
	//if last token placed does not match parameter, automatically not a win
	if (gameBoard[row][col] != token)
		return false;

	//starts a counter to keep track of common tokens next to each other
	int counter = 1;
	bool cont = true; //used to check if next token is the same colour

	//go right until end of board is reached
	for (int i = col + 1; i < BOARD_COLS && cont == true; i++)
	{
		if (gameBoard[row][i] == token)
			counter++; //+1 if token next to last token placed is the same
		else
			cont = false; //if another colour is encountered, breaks
	}
	cont = true; //resets back to true so we can go the other way
	
	//go left until other end of board is reached
	for (int i = col - 1; i > 0 && cont == true; i--)
	{
		if (gameBoard[row][i] == token)
			counter++; //same as above
		else
			cont = false; //if opposite token encountered, breaks
	}
	//returns true if there is 4 or more in a row
	if (counter >= 4)
		return true;
	else
		return false;
}

/*checks for a vertical win, relative to last token placed, very similar to 
rowCheck*/
bool colCheck (int row, int col, int token)
{
	//if last token placed does not match paramenter, automatically not a win
	if (gameBoard[row][col] != token)
		return false;

	//starts a counter to keep track of common tokens next to each other
	int counter = 1;
	bool cont  = true; //Used to check if next token is the same colour
	
	//go down until end of board is reached
	for (int i = row + 1; i < BOARD_ROWS && cont == true; i++)
	{
		if (gameBoard[i][col] == token)
			counter++; //+1 if token next to last token placed is the same
		else
			cont = false; //if opposite token is encountered, breaks
	}
	cont = true; //resets to true so we can go the other way
	
	//go up until other end of board is reached
	for (int i = row - 1; i > 0 && cont == true; i--)
	{
		if (gameBoard[i][col] == token)
			counter++; //+1 if token next to last token placed is the same
		else
			cont = false; //if opposite token is encountered, breaks
	}
	//returns true if there are 4 or more in a row
	if (counter >= 4)
		return true;
	else
		return false;
}

//Checks for a diagonal win, relative to last token placed
bool diagonalCheck(int row, int col, int token)	
{
	//if last token placed does not match paramenter, automatically not a win
	if (gameBoard[row][col] != token)
		return false;

	//starts a counter to keep trach of common tokens next to each other
	int counter = 1;

	//init temporary vars to start check a row above and a column to the right
	int trow = row - 1, tcol = col + 1;
	bool cont = true; //used to check if next token is same colour
	
	//goes from bottom left to top right until end of board is reached
	while (trow >= 0 && tcol < BOARD_COLS && cont == true)
	{
		if (gameBoard[trow][tcol] == token)
			counter++; //+1 if token next to last token placed is the same
		else
			cont = false; //if opposite token is encountered, breaks
			
		//increments temporary row and column to check next token in diagonal
		trow--;
		tcol++;
	}
	
	//overwrites temporary vars to start check row below and column to the left
	trow = row + 1;
	tcol = col - 1;
	cont = true; //resets so we can go the other way
	//goes from top right to bottom left until end of board is reached
	while (trow < BOARD_ROWS && tcol >= 0 && cont == true)
	{
		if (gameBoard[trow][tcol] == token)
			counter++; //+1 if token next to last token placed is the same
		else
			cont = false;
		//increments temporary row and column to check next token in diagonal
		trow++;
		tcol--;
	}
	
	/*now there is a possibility that we could have had a win from this 
	diagonal, so before checking the next diagonal we must check if we already
	have 4 or more in a row so we don't have to go through the rest of the
	code*/
	if (counter >= 4)
		return true;

	//resets counter to one to prevent stacking from previous run
	counter = 1;
	
	//temporary vars to check a row below and a column to the right
	trow = row + 1;
	tcol = col + 1;
	cont = true;
		
	//goes from top left to bottom right until end of board is reached
	while (trow < BOARD_ROWS && tcol < BOARD_COLS && cont == true)
	{
		if (gameBoard[trow][tcol] == token)
			counter++; //+1 if token next to last token placed is the same
		else
			cont = false; //breaks if opposite token encountered
		//increments temporary vars to check next token in diagonal 
		trow++;
		tcol++;
	}

	//overwrites temporary vars to check other direction in diagonal
	trow = row - 1;
	tcol = col - 1;
	cont = true; //resets to go the other way
	//goes from bottom right to top left until end of board is reached
	while (trow >= 0 && tcol >= 0 && cont == true)
	{
		if (gameBoard[trow][tcol] == token)
			counter++; //+1 if token next to last token placed is the same
		else
			cont = false; //breaks if opposite token encountered
		//increments temporary vars to check next token in diagonal
		trow--;
		tcol--;
	}
	//returns true if there are 4 or more in a row
	if (counter >= 4)
		return true;
	else
		return false;
}

//Checks if a column is full
bool checkFullColumn(int col)
{
	for (int i = 0; i < BOARD_ROWS; i++)
	{
		//if even one slot in column is "0", returns false (not full)
		if (gameBoard[i][col] == EMPTY_SLOT)
			return false;
	}
	return true; //else returns true
}

//scans a column to check for player's most recently placed token
int scanCol(int colNum)
{
	/*first checks if the column is full based on parameter, otherwise we can
	skip this column*/
	if (!checkFullColumn(colNum))
	{
		//goes down each row and checks for a newly placed yellow token
		for(int i = 0; i < BOARD_ROWS; i++)
		{
			wait1Msec(50); //needed to insert a wait to scan accurately
			//as soon as a new yellow token is placed, exits the function
			if (SensorValue[S3] == (int)colorYellow)
			{
				//updates robot's "gameboard", to aid with decision making
				gameBoard[i][colNum] = (int)colorYellow;
				return i; //returns row of newly placed token
			}
			/*prevents scanning of previously placed tokens to make
			scanning faster*/
			else if(i < (BOARD_ROWS-1))
			{
				//if next token is already "placed", exits the function
				if (gameBoard[i+1][colNum] != 0)
					return -1; //returns "bad value" to exit loop
			}
			//drives colour sensor down to check next row if no new token
			if (i<BOARD_ROWS-1)
			{
				//drives to the center of each slot to ensure accurate scan
				nMotorEncoder[motorB] = 0;
				motor[motorB] = 15;
				while (nMotorEncoder[motorB] < ((180/(PI*1.1))*(CENT_CENT))){}
				motor[motorB] = 0;
			}
		}
	}
	//after scanning a column, resets colour sensor to rest encoder
	recallVerSensors();
	return -1; //returns "bad" value if no new token
}

//Updates gameboard by inserting tokens into the array
int dropPiece (int col, int token)
{
	//goes down the specified column
	for(int i = 0; i < BOARD_ROWS; i++)
	{
		if (i != 5)
		{
			/*if there is a token below the current row, token is placed
			into the current row to prevent overwriting*/
			if (gameBoard[i+1][col] != 0)
			{
				gameBoard[i][col] = token;
				return i; //returns the row of dropped token
			}
		}
		//if entire column was empty, places token in the last row
		else if(gameBoard[5][col] == 0)
		{
			gameBoard[5][col] = token;
			return 5; //returns row of dropped token
		}
	}
	return 0;
}

//based on who won, displays a message and plays some lit music
void displayWin(int condition)
{
	//MAX VOLUME
	setSoundVolume(100);
	eraseDisplay();
	//player win = 1, robot win = 2, tie = 3
	if (condition == 1)
	{
		displayBigTextLine(3, "PLAYER WINS");
		playSoundFile("Japan");
		sleep(10000);
	}
	else if (condition == 2)
	{
		displayBigTextLine(3, "gg ez :^)");
		playSoundFile("Kermit");
		sleep(10000);
	}
	else
	{
		displayBigTextLine(3, "Tie!");
		playSoundFile("Good job");
		sleep(1000);
	}

}

//function to check if the board is full
bool checkFullBoard()
{
	for (int i = 0; i < BOARD_ROWS; i++)
	{
		for(int j = 0; j < BOARD_COLS; j++)
		{
			if (gameBoard[i][j] == EMPTY_SLOT)
				return false; //returns false if there is any empty slots
		}
	}
	return true; //if no empty slots, returns true
}

//combines all check win functions into one massive function
int checkWin(int token)
{
	/*checks the entire board, this is relatively time consuming and should
	be improved if we had more time*/
	for(int i = 0; i < BOARD_ROWS; i++)
	{
		for(int j = 0; j < BOARD_COLS; j++)
		{
			//first 3 check for robot win, last 3 check player win
			if (rowCheck(i,j,token) && token == (int)colorRed)
				return 2;
			else if (colCheck(i,j,token) && token == (int)colorRed)
				return 2;
			else if (diagonalCheck(i,j,token) && token == (int)colorRed)
				return 2;
			else if (rowCheck(i,j,token) && token == (int)colorYellow)
				return 1;
			else if (colCheck(i,j,token) && token == (int)colorYellow)
				return 1;
			else if (diagonalCheck(i,j,token) && token == (int)colorYellow)
				return 1;
		}
	}

	if (!checkFullBoard())
		return 0;
	
	return 3;
}

//Finds maximum of 2 values, aids with the minimax function
int max(int value1, int value2)
{
	if (value1 > value2)
		return value1;
	else if (value2 > value1)
		return value2;
	else
		return value1;
}

//Finds minimum of 2 values, aids with the minimax function
int min(int value1, int value2)
{
	if (value1 < value2)
		return value1;
	else if (value2 < value1)
		return value2;
	else
		return value1;
}

/*removes the topmost token from the gameboard at a specified column, aids with
the minimax function*/
void removeToken(int col)
{
	bool top = false;
	for(int i = 0; i < BOARD_ROWS && top == false; i++)
	{
		//the first encountered not empty slot is converted into an empty slot
		if (gameBoard[i][col] != EMPTY_SLOT)
		{
			gameBoard[i][col] = EMPTY_SLOT;
			top = true; //breaks the loop once we remove a token
		}
	}
}

/*Minimax is our robot's decision making algorithm. The 4 parameters specified
include the depth, a bool for who we want to maximize, and 2 optimizing 
parameters called alpha and beta

The algorithm works by looking a certain amount of turns ahead into the
future. For each column and each turn, it "pretends" to place a token for x
amount of turns (specified by depth), and checks for a win. 

This function is recursively called until either there is a win for either 
player, or the depth reaches 0 (depth decrements with every recursive call). At
this point, a "score" is assigned for each column, and the robot places a token
in the column with the highest score.

However, this is a very time consuming and computationally heavy operation, and
so we decided to optimize it with alpha-beta pruning. This is a search 
algorithm that aims to decrease the number of nodes that are evaluated by the
minimax algorithm in its search tree (how many moves it looks ahead and all 
the possible chip placement combinations.

Alpha beta pruning stops evaluating a move when at least one possibility has
been found that proves this move to be worse than a previously evaluated move.
This way, parts of the search tree that cannot influence the score are removed,
thus decreasing compute time greatly*/

//Minimax function
int miniMax (int depth, bool ai, int alpha, int beta)
{
	//Exit condition for function, since this is recursively called :)
	if (depth == 0 || checkFullBoard())
		return 0;

	/*at each call, the function checks if there is a win for either player,
	first checks for robot win*/
	if (checkWin((int)colorRed))
	{
		/*returns an arbitrary score for the column, robot win is good so we
		make that a positive score*/
		return depth; 
	}
	if (checkWin((int)colorYellow))
	{
		//player win is bad, so we return a negative score
		return -depth;
	}

	/*inits values to compare with alpha and beta, negate who we want to max
	since we want bestVal to be the max of the recursive call*/
	int bestVal = 10000;
	if (ai)
		bestVal = -10000;

	/*goes through the board, inserts a token at each column and recursively
	calls minimax from there*/
	for (int i = 0; i < BOARD_COLS; i++)
	{
		/*first checks if the column is full, skips it and assigns a negative
		value if it is*/
		if (!checkFullColumn(i))
		{
			//drops a red piece if we are maximizing ai, otherwise yellow
			if (ai)
				dropPiece(i,(int)colorRed);
			else
				dropPiece(i,(int)colorYellow);

		//minimax is recursively called until either depth = 0 or board full
			int prevVal = miniMax(depth - 1, !ai, alpha, beta);
			
			/*if we want to maximize ai, we want the score to be the 
			maximum of the recursive calls, and alpha to be the max of itself
			or bestVal*/
			if (ai)
			{
				bestVal = max(prevVal, bestVal);
				alpha = max(alpha, bestVal);
			}
			//Otherwise we taken the minimum
			else
			{
				bestVal = min(prevVal, bestVal);
				beta = min(beta, bestVal);
			}
			/*once we finish evaluating, we remove all the tokens we "pretended"
			to place*/
			removeToken(i);		
			
			/*checks if alpha is greater than the worst possible cases, which
			is the condition to "prune" a node and stop the function*/
			if (alpha >= beta)
				break;
		}
	}
	return bestVal; //returns the score for the a column
}

/*Find the median of all columns with the 'best' move, since it was required
that the game be recreatable for the purpose of this project*/
int evaluateMedian(int count, int high, int *array)
{
	//If only 1 or 2 best columns, pick the first one in array
	if (count == 1 || count == 2) 
	{
		for (int i = 0; i < 7; i++)
		{
			if (array[i] == high)
				return i;
		}
	}
	else
	{
		//Eliminates first left most column that is not equal to high val
		for (int i =0; i < 7; i++)		
		{
			if (array[i] != -10)
			{
				array[i] = -10;
				i = 7;
			}
		}
		//Eliminates first right most column that is not equal to high val
		for (int j = 6; j >= 0; j--)		
		{
			if (array[j] != -10)
			{
				array[j] = -10;
				j = -1;
			}
		}
		//Called recursively until num high columns equals 1 or 2
		return evaluateMedian(count-2,high, array);
	}
}

//Find the highest scores from the array
int median(int *array)		
{
	//dummy array filled with values lower than possible min max values
	int dummy[7] = {-10,-10,-10,-10,-10,-10,-10};
	int count = 0;

	int high = -10000000;
	//finds highest value in array
	for (int i = 0; i < 7; i ++)
	{
		if (array[i] > high)
		{
			high = array[i];
		}
	}
	//counts number of highest values
	for (int j = 0; j < 7; j++)
	{
		//Count how many columns have the highest score
		if (array[j] == high)
		{
			dummy[j] = array[j];
			count++;
		}
	}
	//Call evaluateMedian to find median
	return evaluateMedian(count, high, dummy);
}

//Function to drive to a specified column and dispense a chip in it
void driveAndDispense(float *columns, int &row, int &col)
{
	//Fills a dummy array that mirrors the gameboard in its present state
	for (int i = 0; i < BOARD_ROWS; i++) 
	{
		for (int j = 0; j < BOARD_COLS; j++)
		{
			dummyBoard[i][j] = gameBoard[i][j];
		}
	}

	/*Decision making: Runs the minimax algorithm for reach column and returns
	the score for each column*/
	for (int i = 0; i < BOARD_COLS; i++)
	{
		if (!checkFullColumn(i))
		{
			//"pretends" to place a piece, and calls minimax for that column
			row = dropPiece(i,(int)colorRed);
			
			//a depth of 5 looks 5 turns into the future
			scores[i] = miniMax(5, false, -10000, 10000);
			removeToken(i); //removes the piece we "pretended" to place
		}
		else
			scores[i] = -6;		//a full column has the lowest score
	}
	setSoundVolume(100);		//Make sound to signify that it decided
	playSoundFile("Ready");
	bool win = false;
	
	//Choose the median column to put it in based on the scores
	col = median(scores);

 	//Display scores of each column (mostly for debugging)
	displayTextLine(3, "%d %d %d %d %d %d %d", scores[0],scores[1],scores[2],
	scores[3],scores[4],scores[5],scores[6]);
	displayTextLine(5, "%d", col); //Displays col index of highest score col
	wait1Msec(5000);
	
	 //Make a sound when it is making its winning move (just for fun :))
	for(int i = 0; i < BOARD_COLS && win == false; i++)
	{
		if (scores[i] == 4)
		{
			setSoundVolume(100);
			playSoundFile("supaHOT");
			sleep(7000);
			win = true; //exit condition for play cycle loop
		}
	}
	
	//Moves to chosen column and dispenses a chip
	motor[motorA]= 25;	
	while (nMotorEncoder[motorA] < ((180/(PI*2.1575))*(columns[col]+0.4))){}
	motor[motorA] = 0;
	wait1Msec(500);
	eraseDisplay();
	wait1Msec(50);
	dispense();

	//Brings gameboard back to current state
	for (int i = 0; i < BOARD_ROWS; i++) 
	{
		for (int j = 0; j < BOARD_COLS; j++)
		{
			gameBoard[i][j] = dummyBoard[i][j];
		}
	}
	//updates gameboard with the newly dropped robot token
	row = dropPiece(col, (int)colorRed);
}

//Screen to be displayed after game ends
void endScreen()
{
	eraseDisplay();
	//Displays final game state for 4 seconds
	for(int i = 0;i < BOARD_ROWS; i++) 
	{
		displayBigTextLine(2 * i, "%d %d %d %d %d %d %d", gameBoard[i][0],
		gameBoard[i][1], gameBoard[i][2],gameBoard[i][3],gameBoard[i][4],
		gameBoard[i][5], gameBoard[i][6]);
	}
	wait1Msec(4000);
	eraseDisplay();
	
	//gives the user the option to play again
	displayBigTextLine(5, "PLAY AGAIN?");
	displayBigTextLine(8, "ENTER FOR NO");
	displayBigTextLine(10, "TOUCH FOR YES");
}

task main()
{
	//initializes all necessary sensors for robot to function
	SensorType[S1] = sensorEV3_Touch; //vertical reset
	SensorType[S2] = sensorEV3_Touch; //horizontal reset
	SensorType[S3] = sensorEV3_Color; //colour sensor
	wait1Msec(50);
	SensorMode[S3] = modeEV3Color_Color;
	wait1Msec(50);
	SensorType[S4] = sensorEV3_Touch; //Next turn sensor
	
	//array that contains all of the tuned column constants
	float columns[7] = {COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7};

	//Only proceeds if the gameboard is present
	if (checkBoard() == 1)
	{
		//vars to check who won and to check if the user wants to play again
		int win = 0;
		bool playAgain = true;
		
		//Full program cycle begins here
		while(playAgain == true)
		{
			/*vars to keep track of the last placed token's row and column
			to prevent rescanning slots*/
			int lastPieceRow = 0, lastPieceCol = 0;
			//Resets Game state to be an empty board
			for(int i = 0; i < BOARD_ROWS; i++) 	
			{
				for(int j = 0; j < BOARD_COLS; j++)
				{
					gameBoard[i][j] = EMPTY_SLOT;
				}
			}
			//0= no win, 1 = player win, 2 = robot win, 3 = tie
			//Initializes full play cycle
			while(win == 0)		
			{
				//wait for player turn and displays game state after every turn
				for(int i = 0;i < BOARD_ROWS; i++)
				{
					displayBigTextLine(2 * i, "%d %d %d %d %d %d %d", 
					gameBoard[i][0],gameBoard[i][1],gameBoard[i][2],
					gameBoard[i][3],gameBoard[i][4],gameBoard[i][5],
					gameBoard[i][6]);
				}
				//starts a time to remind the player to make a move
				time1[T1] = 0;
				
				//end of the player's turn is signified by touch sensor input
				while(!SensorValue[S4])
				{
					 //Alert player every 20 seconds until they decide
					if (time1[T1] % 20000 == 0)
					{
						setSoundVolume(100);
						playSoundFile("Ready");
						sleep(1000);
					}
				}
				eraseDisplay();
				//Scan the board for the newly placed player token
				for(int i = 0; i < BOARD_COLS; i++)
				{
					motor[motorA]= 20;
					while (nMotorEncoder[motorA] < ((180/(PI*2.1575))*
					(columns[i]))){}
					motor[motorA] = 0;
					
					//keeps track of the last token's row and column
					lastPieceRow = scanCol(i);
					lastPieceCol = i;
					
					/*Panic function: If the colour sensor did not sense the 
					newly placed chip, we reset i and restart the scanning of
					the board*/
					if (SensorValue[S4] == 1)			
					{
						i = -1;
						recallVerSensors();
						recallHorSensors();
					}

					//breaks the loop once we find the newly placed token
					if (lastPieceRow != -1)
						i = 7;
					recallVerSensors(); //recalls colour sensor after each col
				}
				//recalls dispensing hub after finished scanning
				recallHorSensors();
				
				//First we check for player win
				if (checkWin((int)colorYellow) == 1)
				{
					win = 1;
					displayWin(win);
					wait1Msec(5000);
				}
				/*if no player win, we let the robot decide its move and 
				dispense a chip into its desired column*/
				else
				{
					driveAndDispense(columns, lastPieceRow, lastPieceCol); 	
					//resets after making a move
					recallVerSensors();															
					recallHorSensors();
					
					//now we check for robot win
					if (checkWin((int)colorRed) == 2)
					{
						win = 2;
						displayWin(win);
						wait1Msec(5000);
					}
				}
				//If no win from either, we check for a full board i.e. a tie
				if (checkFullBoard())				
					win = 3;
					
			/*if nothing we keep going through the play cycle until 
			someone wins or there is a tie*/
			}
			//Displays end screen once game is over
			endScreen();
			win = 0; //resets win for if they want to play again
			bool restart = false; //make a bool if player wants to play again
			
			//waits for user input
			while(playAgain == true && !restart)
			{
				//if enter is pressed, program ends
				if (getButtonPress(buttonEnter))
				{
					playAgain = false;
				}
				//If touch sensor is pressed, game starts over
				else if (SensorValue[S4])		
				{
					restart = true;
					wait1Msec(3000);
				}
			}
		}
	}
}
