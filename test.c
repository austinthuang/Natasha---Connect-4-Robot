const int BOARD_ROWS = 6;
const int BOARD_COLS = 7;
const float COL_1 = 11.58;
const float COL_2 = 15.38;
const float COL_3 = 19.08;
const float COL_4 = 22.85;
const float COL_5 = 26.92;
const float COL_6 = 30.7;
const float COL_7 = 34.9;
const float CENT_CENT = 3.42;
const float WHEEL_RAD = 2.1575;

int gameBoard[BOARD_ROWS][BOARD_COLS];
int dummyBoard[BOARD_ROWS][BOARD_COLS];			//dummy placeholder to keep current game state before robot turn
int scores[BOARD_COLS];

void dispense() //Dispenses a chip
{
	motor[motorC] = 20; // motor C is dispenser
	wait1Msec(700);
	motor[motorC] = -20;
	wait1Msec(700);
	motor[motorC] = 0;
}

void recallVerSensors() //resets the motor encoder for the motor running in the vertical xis
{
	motor[motorB]= -40;
	while(SensorValue[S1] == 0){}
	motor[motorB] = 0;
	nMotorEncoder[motorB] = 0;
}

void recallHorSensors() //resets the motor encoder for the motor running in the horizontal axis
{
	motor[motorA]= -35;
	while(SensorValue[S2] == 0){}
	motor[motorA] = 0;
	nMotorEncoder[motorA] = 0;
}

int checkBoard()	//Check if the board is there
{
	recallVerSensors();
	recallHorSensors();
	nMotorEncoder[motorA]= 0;
	motor[motorA] = 25;
	while (nMotorEncoder[motorA] < ((180/(PI*WHEEL_RAD))*COL_7))
	{
		if (SensorValue[S3] == (int)colorBlue) //if it sees the board,reset position
		{
			recallHorSensors();
			return 1;
		}
	}
	motor[motorA]=0;
	setSoundVolume(100);
	playSoundFile("Error alarm");		//If not seen, make error sound
	sleep(1000);
	recallHorSensors();
	return -1;											// End program

}

bool rowCheck(int row, int col, int token) 		//Check if there is a horizontal win
{
	if (gameBoard[row][col] != token)
		return false;

	int counter = 1;
	bool cont = true;

	//go right
	for (int i = col + 1; i < BOARD_COLS && cont == true; i++)
	{
		if (gameBoard[row][i] == token)
			counter++;
		else
			cont = false;
	}
	cont = true;
	//go left
	for (int i = col - 1; i > 0 && cont == true; i--)
	{
		if (gameBoard[row][i] == token)
			counter++;
		else
			cont = false;
	}
	if (counter >= 4)
		return true;
	else
		return false;
}

bool colCheck (int row, int col, int token)		//Check for vertical win
{
	if (gameBoard[row][col] != token)
		return false;

	int counter = 1;
	bool cont  = true;
	//go down
	for (int i = row + 1; i < BOARD_ROWS && cont == true; i++)
	{
		if (gameBoard[i][col] == token)
			counter++;
		else
			cont = false;
	}
	cont = true;
	//go up
	for (int i = row - 1; i > 0 && cont == true; i--)
	{
		if (gameBoard[i][col] == token)
			counter++;
		else
			cont = false;
	}
	if (counter >= 4)
		return true;
	else
		return false;
}

bool diagonalCheck(int row, int col, int token)		//Check for diagonal win
{
	if (gameBoard[row][col] != token)
		return false;

	int counter = 1;

	//bottom left to top right
	int trow = row - 1, tcol = col + 1;
	bool cont = true;
	while (trow >= 0 && tcol < BOARD_COLS && cont == true)
	{
		if (gameBoard[trow][tcol] == token)
			counter++;
		else
			cont = false;
		trow--;
		tcol++;
	}

	trow = row + 1;
	tcol = col - 1;
	cont = true;
	//top right to bottom left
	while (trow < BOARD_ROWS && tcol >= 0 && cont == true)
	{
		if (gameBoard[trow][tcol] == token)
			counter++;
		else
			cont = false;
		trow++;
		tcol--;
	}
	if (counter >= 4)
		return true;

	//top left to bottom right
	counter = 1;
	trow = row + 1;
	tcol = col + 1;
	cont = true;
	while (trow < BOARD_ROWS && tcol < BOARD_COLS && cont == true)
	{
		if (gameBoard[trow][tcol] == token)
			counter++;
		else
			cont = false;
		trow++;
		tcol++;
	}

	//bottom right to top left
	trow = row - 1;
	tcol = col - 1;
	cont = true;
	while (trow >= 0 && tcol >= 0 && cont == true)
	{
		if (gameBoard[trow][tcol] == token)
			counter++;
		else
			cont = false;
		trow--;
		tcol--;
	}
	if (counter >= 4)
		return true;
	else
		return false;
}

bool checkFullColumn(int col)		//Check if there is a column fully filled
{
	for (int i = 0; i < BOARD_ROWS; i++)
	{
		if (gameBoard[i][col] == 0)
			return false;
	}
	return true;
}

int scanCol(int colNum)			//Scanning for a player's token
{
	if (!checkFullColumn(colNum))
	{
		for(int i = 0; i < BOARD_ROWS; i++)
		{
			wait1Msec(50);
			if (SensorValue[S3] == (int)colorYellow)
			{
				gameBoard[i][colNum] = (int)colorYellow;		//update game board state
				return i;
			}
			else if(i < (BOARD_ROWS-1))//checks to see if board already has next index saved
			{
				if (gameBoard[i+1][colNum] != 0)
					return -1; // dummy place hold value to exit loop
			}
			if (i<BOARD_ROWS-1)
			{
				nMotorEncoder[motorB] = 0;
				motor[motorB] = 15;
				while (nMotorEncoder[motorB] < ((180/(PI*1.1))*(CENT_CENT))){}	//Drives colour sensor down
				motor[motorB] = 0;
			}
		}
	}
	recallVerSensors();
	return -1;
}

int dropPiece (int col, int token)		//Updates gameboard
{
	for(int i = 0; i < BOARD_ROWS; i++)
	{
		if (i != 5)
		{
			if (gameBoard[i+1][col] != 0)		//Place token if there is already a token below it
			{
				gameBoard[i][col] = token;
				return i;
			}
		}
		else if(gameBoard[5][col] == 0)		//place token on bottom if bottom is empty
		{
			gameBoard[5][col] = token;
			return 5;
		}
	}
	return 0;
}


void displayWin(int condition)		//Displays on EV3 screen that someone won, and play music
{
	setSoundVolume(100);
	eraseDisplay();
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

int checkWin(int token)			//Puts all win checks into one
{
	for(int i = 0; i < BOARD_ROWS; i++)
	{
		for(int j = 0; j < BOARD_COLS; j++)
		{
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
	for(int i = 0; i < BOARD_ROWS; i++)
	{
		for(int j = 0; j < BOARD_COLS; j++)
		{
			if (gameBoard[i][j] == 0)
				return 0;
		}
	}
	return 3;			//Tie
}

bool checkFullBoard()		//Check if the entire board is filled
{
	for (int i = 0; i < BOARD_ROWS; i++)
	{
		for(int j = 0; j < BOARD_COLS; j++)
		{
			if (gameBoard[i][j] == 0)
				return false;
		}
	}
	return true;
}

int max(int value1, int value2) //Find maximum between two values
{
	if (value1 > value2)
		return value1;
	else if (value2 > value1)
		return value2;
	else
		return value1;
}

int min(int value1, int value2)	//Find minimum between two values
{
	if (value1 < value2)
		return value1;
	else if (value2 < value1)
		return value2;
	else
		return value1;
}

void removeToken(int col)		//Remove a token from the game board
{
	bool top = false;
	for(int i = 0; i < BOARD_ROWS && top == false; i++)
	{
		if (gameBoard[i][col] != 0)
		{
			gameBoard[i][col] = 0;
			top = true;
		}
	}
}

//Robot's decision making algorithm
//Robot sees a number of turns in the future based on depth value
//Robot places tokens in all combinations and decides what results in its win, or the player's win
//Returns int score value for that column in scores array
int miniMax (int depth, bool ai, int alpha, int beta)
{
	if (depth == 0 || checkFullBoard())
		return 0;

	if (checkWin((int)colorRed))
	{
		return depth;
	}
	if (checkWin((int)colorYellow))
		return -depth;

	int bestVal = 10000;
	if (ai)
		bestVal = -10000;

	for (int i = 0; i < BOARD_COLS; i++)
	{
		if (!checkFullColumn(i))
		{
			if (ai)
				dropPiece(i,(int)colorRed); //Drop a robot token into anywhere it can be placed
			else
				dropPiece(i,(int)colorYellow); //Drip a player token into anywhere it can be placed

			int prevVal = miniMax(depth - 1, !ai, alpha, beta); //Recursively called until depth = 0

			if (ai)
			{
				bestVal = max(prevVal, bestVal);
				alpha = max(alpha, bestVal);
			}
			else
			{
				bestVal = min(prevVal, bestVal); //0,0,0,0,0,0,0
				beta = min(beta, bestVal);
			}
			removeToken(i);		//Remove the token added for decision making

			//Alpha-Beta Pruning - as soon as this statement is true, it breaks out of the recursive funcition
			//Used for optimization - decrease number of combinations the robot has to analyze
			if (alpha >= beta)
				break;
		}
	}
	return bestVal;
}

int evaluateMedian(int count, int high, int *array) //Find the median of all columns with the 'best' move
{
	if (count == 1 || count == 2) //If only 1 or 2 best columns, pick the first one in array
	{
		for (int i = 0; i < 7; i++)
		{
			if (array[i] == high)
				return i;
		}
	}
	else
	{
		for (int i =0; i < 7; i++)		//Look for median from the left
		{
			if (array[i] != -10)
			{
				array[i] = -10;
				i = 7;
			}
		}
		for (int j = 6; j >= 0; j--)		//Look for median from the right
		{
			if (array[j] != -10)
			{
				array[j] = -10;
				j = -1;
			}
		}
		return evaluateMedian(count-2,high, array);			//Called recursively until one value is left
	}
}

int median(int *array)		//Find the highest scores from the array
{
	int dummy[7] = {-10,-10,-10,-10,-10,-10,-10};			//dummy to save what is in the array
	int count = 0;

	int high = -10000000;
	for (int i = 0; i < 7; i ++)
	{
		if (array[i] > high)
		{
			high = array[i];
		}
	}
	for (int j = 0; j < 7; j++)
	{
		if (array[j] == high)		//Count how many columns have the highest score
		{
			dummy[j] = array[j];
			count++;
		}
	}
	return evaluateMedian(count, high, dummy);		//Call evaluateMedian to find median
}


void driveAndDispense(float *columns, int &row, int &col) //Function for Robot's Turn
{
	for (int i = 0; i < BOARD_ROWS; i++) // Fill Dummy array
	{
		for (int j = 0; j < BOARD_COLS; j++)
		{
			dummyBoard[i][j] = gameBoard[i][j];
		}
	}

	for (int i = 0; i < BOARD_COLS; i++)
	{
		if (!checkFullColumn(i))
		{
			row = dropPiece(i,(int)colorRed);
			scores[i] = miniMax(5, false, -10000, 10000); //Assign score for each column
			removeToken(i);
		}
		else
			scores[i] = -6;		//a full column has the lowest score
	}
	setSoundVolume(100);		//Make sound to signify that it decided
	playSoundFile("Ready");
	bool win = false;
	//Choose the column to put it in based on the scores
	col = median(scores);

	displayTextLine(3, "%d %d %d %d %d %d %d", scores[0],scores[1],scores[2],
	scores[3],scores[4],scores[5],scores[6]); //Display scores of each column
	displayTextLine(5, "%d", col);
	wait1Msec(5000);
	for(int i = 0; i < BOARD_COLS && win == false; i++) //Make a sound when it is making its winning move
	{
		if (scores[i] == 4)
		{
			setSoundVolume(100);
			playSoundFile("supaHOT");
			sleep(7000);
			win = true;
		}
	}
	motor[motorA]= 25;		//Move to column
	while (nMotorEncoder[motorA] < ((180/(PI*2.1575))*(columns[col]+0.4))){}
	motor[motorA] = 0;
	wait1Msec(500);
	eraseDisplay();
	wait1Msec(50);
	dispense();		//Dispense token

	for (int i = 0; i < BOARD_ROWS; i++) // Bring gameboard back to current state
	{
		for (int j = 0; j < BOARD_COLS; j++)
		{
			gameBoard[i][j] = dummyBoard[i][j];
		}
	}
	row = dropPiece(col, (int)colorRed);	// Add new robot token
}

void endScreen()		//Screen after game ended
{
	eraseDisplay();
	for(int i = 0;i < BOARD_ROWS; i++) //Display final game state
	{
		displayBigTextLine(2 * i, "%d %d %d %d %d %d %d", gameBoard[i][0],gameBoard[i][1],
		gameBoard[i][2],gameBoard[i][3],gameBoard[i][4],gameBoard[i][5],
		gameBoard[i][6]);
	}
	wait1Msec(4000);
	eraseDisplay();
	displayBigTextLine(5, "PLAY AGAIN?");
	displayBigTextLine(8, "DOWN FOR NO");
	displayBigTextLine(10, "ANY FOR YES");
}

task main()
{
	SensorType[S1] = sensorEV3_Touch; //vertical reset
	SensorType[S2] = sensorEV3_Touch; //horizontal reset
	SensorType[S3] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S3] = modeEV3Color_Color;
	wait1Msec(50);
	SensorType[S4] = sensorEV3_Touch; //Next turn sensor
	float columns[7] = {COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7};

	if (checkBoard() == 1) //if the board is present
	{
		int win = 0;
		bool playAgain = true;
		while(playAgain == true) 	// Game begins here
		{
			int lastPieceRow = 0, lastPieceCol = 0;
			for(int i = 0; i < BOARD_ROWS; i++) 	// Reset Game state
			{
				for(int j = 0; j < BOARD_COLS; j++)
				{
					gameBoard[i][j] = 0;
				}
			}
			while(win == 0)		//0= no win, 1 = player win, 2 = robot win, 3 = tie
			{
				//wait for player turn
				for(int i = 0;i < BOARD_ROWS; i++) // display game state to screen
				{
					displayBigTextLine(2 * i, "%d %d %d %d %d %d %d", gameBoard[i][0],gameBoard[i][1],gameBoard[i][2],gameBoard[i][3],gameBoard[i][4],gameBoard[i][5],gameBoard[i][6]);
				}
				time1[T1] = 0; 	//reset timer

				while(!SensorValue[S4])
				{
					if (time1[T1] % 20000 == 0) // Alert player every 20 seconds
					{
						setSoundVolume(100);
						playSoundFile("Ready");
						sleep(1000);
					}
				}
				eraseDisplay();
				for(int i = 0; i < BOARD_COLS; i++)  	//Scan the board for new player token
				{
					motor[motorA]= 20;
					while (nMotorEncoder[motorA] < ((180/(PI*2.1575))*(columns[i]))){}
					motor[motorA] = 0;
					lastPieceRow = scanCol(i);
					lastPieceCol = i;
					if (SensorValue[S4] == 1)				//If touch sensor is pressed, reset and restart scan
					{
						i = -1;
						recallVerSensors();
						recallHorSensors();
					}

					if (lastPieceRow != -1)
						i = 7;
					recallVerSensors();
				}
				recallHorSensors();

				if (checkWin((int)colorYellow) == 1)		//Check for player win
				{
					win = 1;
					displayWin(win);
					wait1Msec(5000);
				}
				else
				{
					driveAndDispense(columns, lastPieceRow, lastPieceCol); 		// Make decision and dispense chip
					recallVerSensors();																				// Reset robot position
					recallHorSensors();
					if (checkWin((int)colorRed) == 2)			//Check for Robot win
					{
						win = 2;
						displayWin(win);
						wait1Msec(5000);
					}
				}
				if (checkFullBoard())				//Check for a full board i.e. a tie
					win = 3;
			}
			endScreen(); // Display Play again
			win = 0;
			bool restart = false;
			while(playAgain == true && !restart)
			{
				if (getButtonPress(buttonEnter))		// If pressed enter, end program
				{
					playAgain = false;
				}
				else if (SensorValue[S4])		//If pressed touch sensor, play again
				{
					restart = true;
					wait1Msec(3000);
				}
			}
		}
	}
}
