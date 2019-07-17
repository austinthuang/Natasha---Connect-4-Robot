void simultaneous()
{
	while(!getButtonPress(buttonAny)){}
	nMotorEncoder[motorA] = 0;
	motor[motorA] = 75;
	motor[motorB] = 50;
	while(nMotorEncoder[motorA] < 20 * (180.0 / (PI * 2.1))) {}
	motor[motorA] = 0;
	motor[motorB] = 0;
	wait1Msec(500);
	nMotorEncoder[motorA] = 0;
	motor[motorA] = -75;
	motor[motorB] = -50;
	while(nMotorEncoder[motorA] > -20 * (180.0 / (PI * 2.1))) {}
	motor[motorA] = 0;
	motor[motorB] = 0;
}

void Controlled()
{
	while(!getButtonPress(buttonBack))
	{
		while(getButtonPress(buttonUp)){
			motor[motorA] = 50;}
		while(getButtonPress(buttonDown)){
			motor[motorA] = -50;}
		while(getButtonPress(buttonLeft)){
			motor[motorB] = 50;}
		while(getButtonPress(buttonRight)){
			motor[motorB] = -50;}
		/*	if(getButtonPress(buttonEnter)){
		time1[T1]=0;
		motor[motorC] = 5;
		while(time1[T1] < 1000 ) {}
		time1[T1]=0;
		motor[motorC] = -5;
		while(time1[T1] < 1000) {} }*/
		motor[motorA] = motor[motorB] = 0;
	}
}

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
	Dispense();
	Controlled();
	//simultaneous();

	SensorType[S2] = sensorEV3_Touch;

	while(!getButtonPress(buttonAny)){}

		recallHorSensors();
		wait1Msec(500);
		motor[motorB]= 25;
		while (nMotorEncoder[motorB] < ((180/(PI*2.1575))*11.08)){}
		motor[motorB] = 0;


}
