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
#ifndef AUTONOMOUS_FUNCTIONS_H
#define AUTONOMOUS_FUNCTIONS_H

#include "Variables.c"
#include "Vex_Competition_Includes.c"
#include "Debug.c"

// Auton Function Variables
bool isOpposite = false;
bool isRunning = false;
int turnRange = 10;
int driveRange = 15;

// Auton Variables
const int BlueStart =  0;
const int RedStart = 1365;
const int NoStart = 2730;

const int fivePtStart = 0;
const int tenPtStart = 1365;
const int twentyPtStart = 2730;

task lDrivePID()
{
	SensorValue(lDriveQuad) = 0;
	clearTimer(T2);

	float P, I , D;
	P=0;
	I=0;
	D=0;

	float kp;
	float ki;
	float kd;
	int range;

	if(!isopposite)
	{
		kp = .7;
		ki = 0.15;
		kd = 1.25;
		range = 100;
	}
	else
	{
		kp = .35;
		ki = 0.03;//.1;
		kd = .4;
		range = 75;
	}

	int target = 0;
	float error = 0;
	int lastError = 0;
	const float gyrostart = SensorValue(GyroGetAngle());
	int debugval = 0;


	while (true)
	{
		float g = GyroGetAngle();
		//if(isOpposite)
		//display( (int)P , (int)I , (int)D ,error,lDrivePwr,driveTarget,g);
		display( (int)P , (int)I , (int)D, error, lDrivePwr, driveTarget, debugval, rDrivePwr);

		target = driveTarget;

		if(!isOpposite)
		{
			error = target - SensorValue(lDriveQuad);
			debugval = SensorValue(lDriveQuad);
		}
		else
		{
			error = target - (g - gyrostart);
			error *=-1;
			debugval = g - gyrostart;
		}

		P = error * kp;
		D = (error - lastError) * kd;

		if (error > -range && error < range)
			I += error * ki;
		else
			I=0;

		lastError = error;
		lDrivePwr = P + D;
		lDrivePwr += I;

		if(lDrivePwr > 100)
			lDrivePwr = 100;
		if(lDrivePwr < -100)
			lDrivePwr = -100;

		if(time1[T2] < 200){
			if(lDrivePwr > 50)
				lDrivePwr = 50;
			if(lDrivePwr < -50)
				lDrivePwr = -50;
		}

		motor[leftB] = motor[leftF] = lDrivePwr;

		sleep (50);
	}
}

task rDrivePID()
{
	SensorValue(rDriveQuad) = 0;
	clearTimer(T3);

	float P, I, D;
	P = 0;
	I = 0;
	D = 0;

	float kp;
	float ki;
	float kd;
	int range;

	if(!isopposite)
	{
		kp = .9;//.6;
		ki = 0;//0.02;
		kd = .1;//.06;
		range = 500;
	}
	else
	{
		kp = .5;
		ki = 0.1;
		kd = 1.5;
		range = 50;
	}

	int target = 0;
	int error = 0;
	int lastError = 0;
	int debugval = 0;
	int startangle = GyroGetAngle();

	while (true)
	{
		//if(!isOpposite)
		//display( (int)P , (int)I , (int)D ,error,rDrivePwr,target,debugval , lDrivePwr);
		//display( (int)P , (int)I , (int)D ,error,rDrivePwr,target,debugval);

		if(!isOpposite)
		{
			error = startangle - GyroGetAngle();
			target = startangle;
			debugval = GyroGetAngle();
		}
		else
		{
			error = - (SensorValue(lDriveQuad) + SensorValue(rDriveQuad));
			target = -SensorValue(lDriveQuad);
			debugval = SensorValue(rDriveQuad);
		}

		//if(isOpposite)
		//    error*=-1;

		P = error * kp;
		if (lDrivePwr > -range && lDrivePwr < range)
			I += (error * ki);
		else
			I=0;
		D = (error - lastError) * kd;

		lastError = error;

		if(!isOpposite)
			rDrivePwr = lDrivePwr + P + I + D;
		else
			rDrivePwr = -1 * (lDrivePwr) + P + I + D;

		if(rDrivePwr > 100)
			rDrivePwr = 100;
		if(rDrivePwr < -100)
			rDrivePwr = -100;

		if(time1[T3] < 200){
			if(rDrivePwr > 55)
				rDrivePwr = 55;
			if(rDrivePwr < -55)
				rDrivePwr = -55;
		}

		motor[rightB] = motor[rightF] = rDrivePwr;

		//display( (int)P , (int)I , (int)D ,error,rDrivePwr,target,SensorValue(rDriveQuad));
		//writeDebugStreamLine("%d , %d", rDrivePwr, lDrivePwr);

		sleep (50);
	}
}

// Lift PID (not specifically lLift)
task lLiftPID()
{
	float P, I, D;
	// period = .8
	float kp = 0.3; //.5
	float ki = 0.075;
	float kd = 0.4;

	int error = 0;
	int lastError = 0;
	int range = 150;

	// 16.38 ticks per degree
	while (true)
	{
		error = liftTarget - SensorValue(liftPot);

		P = error * kp;
		D = (error - lastError) * kd;

		if (error > -range && error < range)
			I += error * ki;
		else
			I=0;

		lastError = error;
		lLiftPwr = P + I + D;
		motor[lLift] = motor[rLift] = lLiftPwr;

		//display( (int)P , (int)I , (int)D ,error,lLiftPwr,liftTarget,SensorValue(rLiftPot));
		sleep (100);
	}
}





void intakeCone(int pos)
{
	if (pos == 1)
	{
		motor[intake] = 100;
		sleep(500);
		motor[intake] = 30;
	}
	else if (pos == 0)
	{
		motor[intake] = -100;
		sleep(500);
		motor[intake] = 0;
	}
}







// Drive Auton Functions
void forward(int len)
{
	isOpposite = false;
	driveTarget = len;
	startTask(lDrivePID);
	startTask(rDrivePID);
	while(SensorValue(lDriveQuad) < (len - driveRange));
	sleep(500);
	stopTask(lDrivePID);
	stopTask(rDrivePID);
	motor[rightB] = motor[rightF] = motor[leftB] = motor[leftF] = 0;
}

void timeforward(int len, int time)
{
	isOpposite = false;
	driveTarget = len;
	startTask(lDrivePID);
	startTask(rDrivePID);
	clearTimer(T1);
	while(SensorValue(lDriveQuad) < (len - driveRange))
	{
		if(time1[T1] > time){
			break;
		}
	};
	sleep(500);
	stopTask(lDrivePID);
	stopTask(rDrivePID);
	motor[rightB] = motor[rightF] = motor[leftB] = motor[leftF] = 0;
}

void backward(int len)
{
	isOpposite = false;
	driveTarget = -len;
	startTask(lDrivePID);
	startTask(rDrivePID);
	while(SensorValue(lDriveQuad) > (-len + driveRange));
	sleep(500);
	stopTask(lDrivePID);
	stopTask(rDrivePID);
	motor[rightB] = motor[rightF] = motor[leftB] = motor[leftF] = 0;
}

void timebackward(int len, int time)
{
	isOpposite = false;
	driveTarget = -len;
	startTask(lDrivePID);
	startTask(rDrivePID);
	clearTimer(T1);
	while(SensorValue(lDriveQuad) > (-len + driveRange))
	{
		if(time1[T1] > time){
			break;
		}
	};
	sleep(500);
	stopTask(lDrivePID);
	stopTask(rDrivePID);
	motor[rightB] = motor[rightF] = motor[leftB] = motor[leftF] = 0;
}

void right(int len)
{
	int startangle = GyroGetAngle();
	isOpposite = true;
	driveTarget = -len;
	startTask(lDrivePID);
	startTask(rDrivePID);
	while((GyroGetAngle() - startangle) > ((- len) + turnRange))
	{
		//writeDebugStreamLine("%f",GyroAngleAbsGet());
	}
	sleep(500);
	stopTask(lDrivePID);
	stopTask(rDrivePID);
	motor[rightB] = motor[rightF] = motor[leftB] = motor[leftF] = 0;
}

void left(int len)
{
	int startangle = GyroGetAngle();
	isOpposite = true;
	driveTarget = len;
	startTask(lDrivePID);
	startTask(rDrivePID);
	while((GyroGetAngle() - startangle) < (len - turnRange));
	sleep(500);
	stopTask(lDrivePID);
	stopTask(rDrivePID);
	motor[rightB] = motor[rightF] = motor[leftB] = motor[leftF] = 0;
}

void turnTo(int angle)
{
	const int range = 5;
	const int pwr = 30;
	if(angle < GyroGetAngle())
	{
		motor[leftB] = motor[leftF] = pwr;
		motor[rightB] = motor[rightF] = -pwr;
	}
	else
	{
		motor[leftB] = motor[leftF] = -pwr;
		motor[rightB] = motor[rightF] = pwr;
	}
	while(abs(GyroGetAngle() - angle) > range){}
}

void deployMGoal(int position)
{
	// Deploys MGoal
	if (position == 1)
	{
		motor[moGo] = 100;
		sleep(1500);
		motor[moGo] = 0;
	}
	// Brings in MGoal
	if (position == 0)
	{
		motor[moGo] = -100;
		sleep(1000);
		motor[moGo] = 0;
	}
}

// Auton Tasks
task delayMGoalThrow()
{
	sleep(500);
	motor[moGo] = 100;
	sleep(400);
	motor[moGo] = 0;
}

task MGoalDown()
{
	motor[moGo] = 100;
	sleep(1500);
	motor[moGo] = 0;
}

task MGoalUp()
{
	motor[moGo] = -100;
	sleep(1000);
	motor[moGo] = 0;
}

#endif
