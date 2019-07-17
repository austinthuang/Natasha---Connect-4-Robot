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

task main()
{
	//while(!getButtonPress(buttonBack)){
	Dispense();
	Controlled();
	//simultaneous();


}
