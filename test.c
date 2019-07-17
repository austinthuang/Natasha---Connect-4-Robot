const float HOR_MID_DIST = 3.5;
const float CALIBRATE_BOARD = 9.0;
const float COL_1 = 11.58;
const float COL_5 = 25.42;
const float COL_7 = 32.64;

void Dispense()
{
	motor[motorC] = 5;
	wait1Msec(1000);
	motor[motorC] = -5;
	wait1Msec(1000);
	motor[motorC] = 0;
	//while(getButtonPress(buttonAny)){}
}

void recallVerSensors() //resets the motor encoder for the motor running in the vertical xis
{
	motor[motorA]= -100;
	while(SensorValue[S1] == 0){}
	motor[motorA] = 0;
	nMotorEncoder[motorA] = 0;
}

void recallHorSensors() //resets the motor encoder for the motor running in the horizontal axis
{
	motor[motorB]= -100;
	while(SensorValue[S2] == 0){}
	motor[motorB] = 0;
	nMotorEncoder[motorB] = 0;
}

task main()
{
	//while(!getButtonPress(buttonBack)){
	//Dispense();
	//Controlled();
	//simultaneous();

	SensorType[S2] = sensorEV3_Touch;

	while(!getButtonPress(buttonAny)){}

		recallHorSensors();
		wait1Msec(500);
		motor[motorB]= 25;
		while (nMotorEncoder[motorB] < ((180/(PI*2.1575))*(COL_7))){}
		motor[motorB] = 0;
		Dispense();


}
