const int BOARD_ROWS = 6;
const int BOARD_COLS = 7;
const float COL_1 = 11.58;
const float COL_2 = 15.38;
const float COL_3 = 19.08;
const float COL_4 = 22.85;
const float COL_5 = 26.82;
const float COL_6 = 30.7;
const float COL_7 = 34.9;
const float CENT_CENT = 3.42;

int gameBoard[BOARD_ROWS][BOARD_COLS];
int dummyBoard[BOARD_ROWS][BOARD_COLS];
int scores[BOARD_COLS];

void dispense() //Dispenses a chip
{
	motor[motorC] = 20;
	wait1Msec(350);
	motor[motorC] = -20;
	wait1Msec(1000);
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

int checkBoard()
{
	recallVerSensors();
	recallHorSensors();
	nMotorEncoder[motorA]= 0;
	motor[motorA] = 25;
	while (nMotorEncoder[motorA] < ((180/(PI*2.1575))*COL_7)){
		if (SensorValue[S3] == (int)colorBlue)
		{
			recallHorSensors();
			return 1;
		}
	}
	return -1;

}

bool rowCheck(int row, int col, int token)
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

bool colCheck (int row, int col, int token)
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

bool diagonalCheck(int row, int col, int token)
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

bool checkFullColumn(int col)
{
	for (int i = 0; i < BOARD_ROWS; i++)
	{
		if (gameBoard[i][col] == 0)
			return false;
	}
	return true;
}

int scanCol(int colNum)
{
	if (!checkFullColumn(colNum))
	{
		for(int i = 0; i < BOARD_ROWS; i++)
		{
			wait1Msec(50);
			if (SensorValue[S3] == (int)colorYellow)
			{
				gameBoard[i][colNum] = (int)colorYellow;
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
				while (nMotorEncoder[motorB] < ((180/(PI*1.1))*(CENT_CENT))){}
				motor[motorB] = 0;
			}
		}
	}
	recallVerSensors();
	return -1;
}

int dropPiece (int col, int token)
{
	for(int i = 0; i < BOARD_ROWS; i++)
	{
		if (i != 5)
		{
			if (gameBoard[i+1][col] != 0)
			{
				gameBoard[i][col] = token;
				return i;
			}
		}
		else if(gameBoard[5][col] == 0)
		{
			gameBoard[5][col] = token;
			return 5;
		}
	}
	return 0;
}


void displayWin(int condition)
{
	setSoundVolume(100);
	eraseDisplay();
	if (condition == 1)
	{
		displayBigTextLine(3, "PLAYER WINS");
		playSoundFile("Uh-oh");
		sleep(1000);
	}
	else if (condition == 2)
	{
		displayBigTextLine(3, "gg ez :^)");
		playSoundFile("Thank you");
		sleep(1000);
	}
	else
	{
		displayBigTextLine(3, "Tie!");
		playSoundFile("Good job");
		sleep(1000);
	}

}

int checkWin(int token)
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
	return 3;
}

bool checkFullBoard()
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

int max(int value1, int value2)
{
	if (value1 > value2)
		return value1;
	else if (value2 > value1)
		return value2;
	else
		return value1;
}

int min(int value1, int value2)
{
	if (value1 < value2)
		return value1;
	else if (value2 < value1)
		return value2;
	else
		return value1;
}

void removeToken(int col)
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

int miniMax (int depth, bool ai, int alpha, int beta) //
{
	if (depth == 0 || checkFullBoard())
		return 0;

	if (checkWin((int)colorRed))
		return depth;
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
				dropPiece(i,(int)colorRed); // Depth 4: row = 4,ai depth 2: row = 5,ai
			else
				dropPiece(i,(int)colorYellow); //Depth 3: row = 3,!ai depth 1: row = 5,!ai

			int prevVal = miniMax(depth - 1, !ai, alpha, beta); //

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
			removeToken(i);
			if (alpha >= beta)
				break;
		}
	}
	return bestVal;
}

int evaluateMedian(int count, int high, int *array)
{
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
		for (int i =0; i < 7; i++)
		{
			if (array[i] != -10)
			{
				array[i] = -10;
				i = 7;
			}
		}
		for (int j = 6; j >= 0; j--)
		{
			if (array[j] != -10)
			{
				array[j] = -10;
				j = -1;
			}
		}
		return evaluateMedian(count-2,high, array);
	}
}

int median(int *array)
{
	int dummy[7] = {-10,-10,-10,-10,-10,-10,-10};
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
		if (array[j] == high)
		{
			dummy[j] = array[j];
			count++;
		}
	}
	return evaluateMedian(count, high, dummy);
}


void driveAndDispense(float *columns, int &row, int &col) //Test hardware
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
			scores[i] = miniMax(4, false, -10000, 10000); //
			removeToken(i);
		}
		else
			scores[i] = -6;
	}
	setSoundVolume(100);
	playSoundFile("Ready");
	col = median(scores);

	displayTextLine(3, "%d %d %d %d %d %d %d", scores[0],scores[1],scores[2],scores[3],scores[4],scores[5],scores[6]);
	displayTextLine(4, "%d", col);
	wait1Msec(2000);
	motor[motorA]= 25;
	//col = random(6);
	/*while(checkFullColumn(col))
	{
	col = random(6);
	}*/
	while (nMotorEncoder[motorA] < ((180/(PI*2.1575))*(columns[col]))){}
	motor[motorA] = 0;
	wait1Msec(500);
	eraseDisplay();
	wait1Msec(50);
	dispense();

	for (int i = 0; i < BOARD_ROWS; i++) // Fill Dummy array
	{
		for (int j = 0; j < BOARD_COLS; j++)
		{
			gameBoard[i][j] = dummyBoard[i][j];
		}
	}
	row = dropPiece(col, (int)colorRed);
}

void endScreen()
{
	eraseDisplay();
	displayBigTextLine(5, "PLAY AGAIN?");
	displayBigTextLine(8, "DOWN FOR NO");
	displayBigTextLine(10, "ANY FOR YES");
	wait1Msec(2000);
	eraseDisplay();
	for(int i = 0;i < BOARD_ROWS; i++)
	{
		displayBigTextLine(2 * i, "%d %d %d %d %d %d %d", gameBoard[i][0],gameBoard[i][1],gameBoard[i][2],gameBoard[i][3],gameBoard[i][4],gameBoard[i][5],gameBoard[i][6]);
	}
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

	if (checkBoard() == 1)
	{
		int win = 0;
		bool playAgain = true;//0= no win, 1 = player win, 2 = robot win, 3 = tie
		while(playAgain == true)
		{
			int lastPieceRow = 0, lastPieceCol = 0;
			for(int i = 0; i < BOARD_ROWS; i++)
			{
				for(int j = 0; j < BOARD_COLS; j++)
				{
					gameBoard[i][j] = 0;
				}
			}
			while(win == 0)
			{
				//wait for player turn
				for(int i = 0;i < BOARD_ROWS; i++) // display array to screen
				{
					displayBigTextLine(2 * i, "%d %d %d %d %d %d %d", gameBoard[i][0],gameBoard[i][1],gameBoard[i][2],gameBoard[i][3],gameBoard[i][4],gameBoard[i][5],gameBoard[i][6]);
				}
				time1[T1] = 0;

				while(!SensorValue[S4])
				{
					if (time1[T1] % 20000 == 0)
					{
						setSoundVolume(100);
						playSoundFile("Ready");
						sleep(1000);
					}
				}
				eraseDisplay();
				for(int i = 0; i < BOARD_COLS; i++)
				{
					motor[motorA]= 20;
					while (nMotorEncoder[motorA] < ((180/(PI*2.1575))*(columns[i]))){}
					motor[motorA] = 0;
					lastPieceRow = scanCol(i);
					lastPieceCol = i;
					if (SensorValue[S4] == 1)
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

				if (checkWin((int)colorYellow) == 1)
				{
					win = 1;
					displayWin(win);
					wait1Msec(5000);
				}
				else
				{
					driveAndDispense(columns, lastPieceRow, lastPieceCol);
					recallVerSensors();
					recallHorSensors();
					if (checkWin((int)colorRed) == 2)
					{
						win = 2;
						displayWin(win);
						wait1Msec(5000);
					}
				}
				if (checkFullBoard())
					win = 3;
			}
			endScreen();
			win = 0;
			bool restart = false;
			while(playAgain == true && !restart)
			{
				if (getButtonPress(buttonEnter))
					playAgain = false;

				else if (SensorValue[S4])
					restart = true;
			}
		}
	}
}
