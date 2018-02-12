#pragma config(Sensor, in1,    liftPot,        sensorPotentiometer)
#pragma config(Sensor, in2,    moGoPot,        sensorPotentiometer)
#pragma config(Sensor, in3,    barPot,         sensorPotentiometer)
#pragma config(Sensor, in4,    gyroscope,      sensorGyro)
#pragma config(Sensor, dgtl7,  lDriveQuad,     sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  rDriveQuad,     sensorQuadEncoder)
#pragma config(Motor,  port1,           barL,          tmotorVex393HighSpeed_HBridge, openLoop)
#pragma config(Motor,  port2,           leftB,         tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port3,           leftF,         tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           lLift,         tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           moGo,          tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           intake,        tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           rLift,         tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           rightB,        tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           rightF,        tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port10,          barR,          tmotorVex393HighSpeed_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// Stop recursive includes
#ifndef AUTOSTACK_H
#define AUTOSTACK_H

#include "Vex_Competition_Includes.c"
#include "Variables.c"
#include "Autonomous Control.c"


void moveDownField()
{
	// Moves lift up to get out of stack
	motor[lLift] = motor[rLift] = 60;
	int tempy = SensorValue[liftPot];
	while(SensorValue[liftPot] <  tempy + 100) {}
	motor[lLift] = motor[rLift] = 0;

	// Moves the bar to bottom position
	barIsUp = false;
	while(SensorValue[barPot] > BAR_DOWN) {} // Tentative for change - change BAR_DOWN to middle position

	// Moves the lift to field position
	motor[lLift] = motor[rLift] = -100;
	while(SensorValue[liftPot] > LIFT_MIN + 200) {}
	motor[lLift] = motor[rLift] = -60;
	while(SensorValue[liftPot] > LIFT_MIN) {}
	motor[lLift] = motor[rLift] = 0;
}

void moveDownDriver(int height)
{
	// Moves lift up to get out of stack
	motor[lLift] = motor[rLift] = 100;
	while(SensorValue[liftPot] < height - 100) {}
	motor[lLift] = motor[rLift] = 0;

	// Moves bar down
	barIsUp = false;
	while(SensorValue[barPot] > BAR_DOWN) {} // Tentative for change - change BAR_DOWN to middle position

	// Moves lift down to driver load stack
	motor[lLift] = motor[rLift] = -100;
	while(SensorValue[liftPot] > LIFT_DRIVER + 200) {}
	motor[lLift] = motor[rLift] = -60;
	while(SensorValue[liftPot] > LIFT_DRIVER) {}
	motor[lLift] = motor[rLift] = 0;
}

void autoStackInit(int height, int down, bool driver)
{
	// Moves the lift up to the height
	motor[lLift] = motor[rLift] = 100;
	while(SensorValue[liftPot] < height) {}
	motor[lLift] = motor[rLift] = 0;

	// Moves the bar up
	barIsUp = true;
	while(SensorValue[barPot] < BAR_UP) {}

	// Moves the lift down to stack
	motor[lLift] = motor[rLift] = -70;
	while(SensorValue[liftPot] > down) {}
	motor[lLift] = motor[rLift] = 0;

	// Releases the cone
	intakeCone(0);

	if(!driver)
		moveDownField();
	else
		moveDownDriver(height);
}

void initConeVals()
{
	conesHeight[0] = 1418;
	conesHeight[1] = 1524;
	conesHeight[2] = 1593;
	conesHeight[3] = 1593 + 181;
	conesHeight[4] = 1903;
	conesHeight[5] = conesHeight[4] + 191;
	conesHeight[6] = conesHeight[5] + 191;
	conesHeight[7] = 2082;
	conesHeight[8] = 2251;
	conesHeight[9] = 0;

	coneDown[0] = 1250;
	coneDown[1] = 1361;
	coneDown[2] = 1544;
	coneDown[3] = 1544 + 181;
	coneDown[4] = 1774;
	coneDown[5] = coneDown[4] + 225;
	coneDown[6] = coneDown[5] + 252;
	coneDown[7] = 2020;
	coneDown[8] = 2091;
	coneDown[9] = 0;
}

task runAutoStack()
{
	if (isFieldControl)
	{
		autoStackIsOn = true;
		intakeCone(1);
		autoStackInit(conesHeight[cones], coneDown[cones], false);
		cones++;
		autoStackIsOn = false;
	}
	else
	{
		autoStackIsOn = true;

		intakeCone(1);
		autoStackInit(conesHeight[cones], coneDown[cones], true);
		conesDriver++;

		autoStackIsOn = false;
	}
}

task autoStack()
{
	bool tasksStarted = true;
	while(true)
	{
		if (vexRT(Btn5U) && !autoStackIsOn) // Runs AutoStack
		{
			tasksStarted = false;

      intakeIsActive = false;
      barIsActive = false;
      liftIsActive = false;


			startTask(runAutoStack);
		}
		else if (vexRT(Btn5D) && autoStackIsOn) // Cancels autoStack
		{
			stopTask(runAutoStack);

			autoStackIsOn = false;
		}
		else if (!tasksStarted && !autoStackIsOn) // Restarts all tasks in the event that they are not running
		{
			tasksStarted = true;

      intakeIsActive = true;
      barIsActive = true;
      liftIsActive = true;

		}

		sleep (100);
	}
}

task autoStackControl()
{
	while (true)
	{
		if (vexRT[Btn8L])
			isFieldControl = true;
		else if (vexRT[Btn8R])
			isFieldControl = false;

		if (!vexRT[Btn8LXmtr2] || !vexRT[Btn8UXmtr2] || !vexRT[Btn8DXmtr2] || !vexRT[Btn8RXmtr2])
		{
			if (vexRT[Btn7LXmtr2])
				cones = 1;
			else if (vexRT[Btn7UXmtr2])
				cones = 2;
			else if (vexRT[Btn7DXmtr2])
				cones = 3;
			else if (vexRT[Btn7RXmtr2])
				cones = 4;
		}
		else if (vexRT[Btn8LXmtr2])
		{
			if (vexRT[Btn7LXmtr2])
				cones = 5;
			else if (vexRT[Btn7UXmtr2])
				cones = 6;
			else if (vexRT[Btn7DXmtr2])
				cones = 7;
			else if (vexRT[Btn7RXmtr2])
				cones = 8;
		}
		else if (vexRT[Btn8RXmtr2])
		{
			if (vexRT[Btn7LXmtr2])
				cones = 9;
			else if (vexRT[Btn7UXmtr2])
				cones = 10;
			else if (vexRT[Btn7DXmtr2])
				cones = 11;
			else if (vexRT[Btn7RXmtr2])
				cones = 12;
		}

		if (vexRT[Btn5UXmtr2])
			cones = 0;
		else if (vexRT[Btn5DXmtr2])
			cones++;

		sleep (100);
	}
}

#endif
