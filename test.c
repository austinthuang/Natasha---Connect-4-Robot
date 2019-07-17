const float HOR_MID_DIST = 3.5;
const float CALIBRATE_BOARD = 9.0;
const int BOARD_ROWS = 6;
const int BOARD_COLS = 7;
const int SLOT_E = 0;
const int SLOT_R = 5;
const int SLOT_Y = 4;
const int BLUE = 2;
const float COL_1 = 11.58;
const float COL_2 = 15.38;
const float COL_3 = 19.08;
const float COL_4 = 22.85;
const float COL_5 = 26.82;
const float COL_6 = 30.22;
const float COL_7 = 33.9;

void Dispense() //Dispenses a chip
{
	motor[motorC] = 20;
	wait1Msec(350);
	motor[motorC] = -20;
	wait1Msec(1000);
	motor[motorC] = 0;
	//while(getButtonPress(buttonAny)){}
}

void recallVerSensors() //resets the motor encoder for the motor running in the vertical xis
{
	motor[motorB]= -100;
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



void test()
{
	for (int i = 0; i < 3; i++)
	{
		recallHorSensors();
		wait1Msec(500);
		motor[motorA]= 35;
		while (nMotorEncoder[motorA] < ((180/(PI*2.1575))*(COL_7))){}
		motor[motorA] = 0;
		Dispense();
		recallHorSensors();
	}
}


task main()
{
	SensorType[S1] = sensorEV3_Touch;
	SensorType[S2] = sensorEV3_Touch;
	SensorType[S3] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S3] = modeEV3Color_Color;
	wait1Msec(50);
	float columns[7] = {COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7};

	//creates virtual gameboard and fills it with all empty values
	int gameBoard[BOARD_ROWS * BOARD_COLS];
	for(int i = 0; i < BOARD_ROWS; i++)
	{
		gameBoard[i] = 0;
	}
	for(int i = 0; i < 7; i++)
	{
		recallHorSensors();
		wait1Msec(500);
		motor[motorA]= 25;
		while (nMotorEncoder[motorA] < ((180/(PI*2.1575))*(columns[i]))){}
		motor[motorA] = 0;
		Dispense();
	}

	while(!getButtonPress(buttonAny)){}
	test();

}
