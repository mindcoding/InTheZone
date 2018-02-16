#pragma config(Sensor, in1,    liftPot,        sensorPotentiometer)
#pragma config(Sensor, in2,    moGoPot,        sensorPotentiometer)
#pragma config(Sensor, in3,    barPot,         sensorPotentiometer)
#pragma config(Sensor, in4,    gyroscope,      sensorGyro)
#pragma config(Sensor, in5,    liftPot2,       sensorPotentiometer)
#pragma config(Sensor, dgtl7,  lDriveQuad,     sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  rDriveQuad,     sensorQuadEncoder)
#pragma config(Motor,  port1,           barL,          tmotorVex393HighSpeed_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           leftB,         tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port3,           leftF,         tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           lLift,         tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           moGo,          tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           intake,        tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           rLift,         tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           rightB,        tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           rightF,        tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port10,          barR,          tmotorVex393HighSpeed_HBridge, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// Prevents recursive includes
#ifndef MAIN_H
#define MAIN_H

// All includes
#pragma platform(VEX2)
#pragma competitionControl(Competition)
#include "Vex_Competition_Includes.c"

#include "User Control Tasks.c"

#include "Variables.c"
#include "GyroLib.c"
#include "AutoStack.c"
#include "LCD Code.c"
#include "Autonomous.c"


void place();

task controller()
{
	while (true)
	{
		// Added toggles for all aspects of the robot so that they could be shut off one at a time
		// since they are no longer tasks (you can no longer "stopTask()")

		// Drive
		if (driveIsActive)
		{
			lDrivePwr = vexRT[Ch3];
			rDrivePwr = vexRT[Ch2];

			// Introduces deadzone
			if (abs(lDrivePwr) < 5)
				lDrivePwr = 0;
			else if (abs(rDrivePwr) < 5)
				rDrivePwr = 0;

			motor[leftB] = motor[leftF] = lDrivePwr;
			motor[rightB] = motor[rightF] = rDrivePwr;
		}

		// Mobile Goal
		if (moGoIsManual)
		{
			stopTask(moGoSet);

			if (moGoIsActive)
			{
				if (vexRT[Btn7U])
				{
					if (SensorValue(moGoPot) >= MOGO_UP)
						mGoalPwr = 0;
					else
						mGoalPwr = 100;
				}
				else if (vexRT[Btn7D] && cones < 9)
					mGoalPwr = -100;
				else if (vexRT[Btn7D] && cones >= 9)
					place();
				else
					mGoalPwr = 0;

				// Setting mobile goal motor power
				motor[moGo] = mGoalPwr;
			}
		}
		else
		{
			startTask(moGoSet);

			if (moGoIsActive)
			{
				if (vexRT[Btn7U])
					moGoIsUp = true;
				else if (vexRT[Btn7D] && cones < 9)
					moGoIsUp = false;
				else if (vexRT[Btn7D] && cones >= 9)
					place();
			}
		}

		// Lift
		if (liftIsActive)
		{
			if(vexRT[Btn6U])
				lLiftPwr = rLiftPwr = 100;
			else if(vexRT[Btn6D])
				lLiftPwr = rLiftPwr = -100;
			else
			{
				if(SensorValue(liftPot) > LIFT_MIN + 100)
					lLiftPwr = rLiftPwr = -2;
				else
					lLiftPwr = rLiftPwr = -10;
			}

			motor[lLift] = lLiftPwr;
			motor[rLift] = rLiftPwr;
		}

		// Four bar
		if (barIsManual)
		{
			stopTask(barSet);

			if (barIsActive)
			{
				if (vexRT[Btn8U])
					barPwr = 100;
				else if (vexRT[Btn8D])
					barPwr = -100;
				else
					barPwr = 0;

				motor[barR] = motor[barL] = barPwr;
			}
		}
		else
		{
			startTask(barSet);
			if (barIsActive)
			{
				if (vexRT[Btn8U])
					barIsUp = true;
				else if (vexRT[Btn8D])
					barIsUp = false;
			}
		}

		// Intake
		if (intakeIsActive)
		{
			if (vexRT[Btn7L])
			{
				intakePwr = -100;
				intakeHold = false;
			}
			else if (vexRT[Btn7R])
			{
				intakePwr = 100;
				intakeHold = true;
			}
			else if (intakeHold)
				intakePwr = 20;
			else if (!intakeHold)
				intakePwr = -10;

			motor[intake] = intakePwr;
		}

	}
}

// Place mobile goal
void place()
{
	// Check to make sure there aren't conflicting motor power sets with task Controller
	if (!barIsManual)
		stopTask(barSet);

	moGoIsUp = false;
	while (SensorValue(moGoPot) > MOGO_DOWN)
		motor[barL] = motor[barR] = -30;

	intakeCone(0);

	if (!barIsManual)
		startTask(barSet);

	barIsUp = true;
}



// Pre-Auton
void pre_auton()
{
	bDisplayCompetitionStatusOnLcd = false;
	bStopTasksBetweenModes = false;

	// Initializes the cones arrays
	autoConeInitVals();

	gyroIsCalibrating = true;
	writeDebugStreamLine("%d", SensorValue[liftPot]);
	startTask(runLCD);

	// Calibrates Gyroscope
	GyroInit(in4);
	wait1Msec(1000);
	wait1Msec(2000);
	SensorFullCount[in4] = 3600 * 100;
	SensorScale[in4] = 130;
	gyroIsCalibrating = false;
}

// User Control
task usercontrol()
{
	clearDebugStream();
	datalogClear();
	stopTask(runLCD);
	clearLCDLine(0);
	clearLCDLine(1);

	// Starting Tasks
	startTask(controller, 130);
	startTask(autoStack);
	startTask(stabilizeLift);

	while (true)
	{
		int leftPot = SensorValue(liftPot);
		int rightPot = SensorValue(liftPot2);
		//writeDebugStreamLine("%f %f", GyroGetAngle(), GyroAngleAbsGet());
	}
}

#endif
