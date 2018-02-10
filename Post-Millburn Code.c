#pragma config(Sensor, in2,    moGoPot,         sensorPotentiometer)
#pragma config(Sensor, in3,    barPot,         sensorPotentiometer)
#pragma config(Sensor, in4,    gyroscope,      sensorGyro)
#pragma config(Sensor, dgtl7,  lDriveQuad,     sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  rDriveQuad,     sensorQuadEncoder)
#pragma config(Motor,  port1,           barL,          tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port2,           leftB,         tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port3,           leftF,         tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           lLift,         tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           moGo,          tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           intake,        tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           rLift,         tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           rightB,        tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           rightF,        tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port10,          barR,          tmotorVex393HighSpeed_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX2)
#pragma competitionControl(Competition)
#include "Vex_Competition_Includes.c"

// Reduce task count
// LCD workaround
// Add manual barcontrol task (do not call)
// Test Push


/*
 *  Controller 1:
 *      Joystick Controls:
 *          Left Joystick : Left Drive
 *          Right Joystick : Right Drive
 *
 *      Bumpers:
 *          6U - Lift Up
 *          6D - Lift Down
 *          5U - AutoStack (Field)
 *          5D - AutoStack Cancel
 *
 *      Buttons:
 *          7U - Mobile Goal In
 *          7D - Mobile Goal Out
 *          7R - Intake In
 *          7L - Intake Out
 *
 *          8U - Bar Up
 *          8D - Bar Down
 *          8R - Toggle Field AutoStack
 *          8L - Toggle Driver AutoStack
 *
 *
 *      Commented Out:
 *          8R - (Bar Up Manual)
 *          8L - (Bar Down Manual)
 */


//VARIABLES - FOR POWER AND SENSOR VALUE POSITIONS

// Motor Powers
int lDrivePwr = 0, rDrivePwr = 0;
int rLiftPwr = 0, lLiftPwr = 0;
int barPwr = 0;
int mGoalPwr = 0;
int intakePwr = 0;

// Control Variables
int driveTarget = 0;
int liftTarget = 0;

// Toggle Variables
bool barIsUp = true;
bool barIsManual = false;

bool intakeHold = false;

bool autoStackIsOn = false;
bool isFieldControl = true;

bool moGoIsManual = true;
bool moGoIsUp = true;

// AutoStack Contorl Variables
int cones = 0;
int conesDriver = 0;

// Sensor Values - CONSTANTS
const int LIFT_MIN = 1426;
const int LIFT_MID = 0;
const int LIFT_MAX  = 0;
const int LIFT_DRIVER = 1742;

const int BAR_UP = 3350;
const int BAR_DOWN = 1870;

const int MOGO_UP = 0;
const int MOGO_DOWN = 0;

// Auton Function Variables
bool isOpposite = false;
bool isRunning = false;
int turnRange = 10;
int driveRange = 15;

// AutoStack arrays
int conesHeight[10];
int coneDown[10];

// Auton Variables
const int BlueStart =  0;
const int RedStart = 1365;
const int NoStart = 2730;

const int fivePtStart = 0;
const int tenPtStart = 1365;
const int twentyPtStart = 2730;


// Datalog Definitions
#define   DATALOG_SERIES_0    0
#define   DATALOG_SERIES_1    1
#define   DATALOG_SERIES_2    2
#define   DATALOG_SERIES_3    3
#define   DATALOG_SERIES_4    4
#define   DATALOG_SERIES_5    5
#define   DATALOG_SERIES_6    6
#define   DATALOG_SERIES_7    7

// Datalog Function
void display(int P, int I, int D, int error, int mpwr, int targ, int sensor, int other)
{
	//writeDebugStreamLine("Function: P: %d, I: %d, D: %d, Error: %d, MPWR: %d, Target: %d",  P, I, D, error, mpwr, targ);
	datalogDataGroupStart();
	datalogAddValue( DATALOG_SERIES_0, P );
	datalogAddValue( DATALOG_SERIES_1, I );
	datalogAddValue( DATALOG_SERIES_2, D );
	datalogAddValue( DATALOG_SERIES_3,error);
	datalogAddValue( DATALOG_SERIES_4,mpwr);
	datalogAddValue( DATALOG_SERIES_5,targ);
	datalogAddValue( DATALOG_SERIES_6,sensor);
	datalogAddValue( DATALOG_SERIES_7,other);
	datalogDataGroupEnd();
}

// Task/Function declarations needed to run in the controller task
task barSet();
task moGoSet();
void place();

// Tasks
task controller()
{
	while (true)
	{
		// Controller

		// Drive - with deadzone
		if (vexRT[Ch3] < 5 && vexRT[Ch3] > -5)
			lDrivePwr = 0;
		else if (vexRT[Ch2] < 5 && vexRT[Ch2] > -5)
			rDrivePwr = 0;
		else
		{
			lDrivePwr = vexRT[Ch3];
			rDrivePwr = vexRT[Ch2];
		}

		// Setting the drive motor power
		motor[leftB] = motor[leftF] = lDrivePwr;
		motor[rightB] = motor[rightF] = rDrivePwr;

		// Mobile Goal
		if (moGoIsManual)
		{
			stopTask(moGoSet);
			if (vexRT[Btn7U])
					mGoalPwr = 100;
			else if (vexRT[Btn7D] && cones < 9)
					mGoalPwr = -100;
			else if (vexRT[Btn7D] && cones >= 9)
				place();
			else
					mGoalPwr = 0;

			// Setting mobile goal motor power
			motor[moGo] = mGoalPwr;
		}
		else
		{
			startTask(moGoSet);
			if (vexRT[Btn7U])
				moGoIsUp = true;
			else if (vexRT[Btn7D] && cones < 9)
				moGoIsUp = false;
			else if (vexRT[Btn7D] && cones >= 9)
				place();
		}

		// Lift
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

		// Four bar
		if (barIsManual)
		{
			stopTask(barSet);
			if (vexRT[Btn8U])
				barPwr = 100;
			else if (vexRT[Btn8D])
				barPwr = -100;
			else
				barPwr = 0;

			motor[barR] = motor[barL] = barPwr;
		}
		else
		{
			startTask(barSet);
			if (vexRT[Btn8U])
				barIsUp = true;
			else if (vexRT[Btn8D])
				barIsUp = false;
		}

		// Intake
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

		// Setting Motors
		motor[intake] = intakePwr;
	}
}

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
    const float gyrostart = SensorValue(gyroscope);
    int debugval = 0;


    while (true)
    {
        float g = SensorValue(gyroscope);
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
    int startangle = SensorValue(gyroscope);

    while (true)
    {
        //if(!isOpposite)
        //display( (int)P , (int)I , (int)D ,error,rDrivePwr,target,debugval , lDrivePwr);
        //display( (int)P , (int)I , (int)D ,error,rDrivePwr,target,debugval);

        if(!isOpposite)
        {
            error = startangle - SensorValue(gyroscope);
            target = startangle;
            debugval = SensorValue(gyroscope);
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

task barSet()
{
    while (true)
    {
        int lBound = 200;
        int uBound = 10;

        const int UPPER_PWR = 100;
        const int LOWER_PWR = 40;
        const int HOLD_PWR = 5;

        // Bang-bang the bar
        if (barIsUp)
        {
            if (SensorValue(barPot) < BAR_UP - lBound)
                barPwr = UPPER_PWR;
            else if (SensorValue(barPot) < BAR_UP - uBound)
                barPwr = LOWER_PWR;
            else
                barPwr = HOLD_PWR;
        }
        else
        {
            if (SensorValue(barPot) > BAR_DOWN + lBound)
                barPwr = -UPPER_PWR;
            else if (SensorValue(barPot) > BAR_DOWN + uBound)
                barPwr = -LOWER_PWR;
            else
                barPwr = -HOLD_PWR;
        }

				// Commented out because replaced in controller task
        motor[barL] = motor[barR] = barPwr;

        sleep(50);
    }
}

task moGoSet()
{
	while (true)
	{
			int lBound = 200;
			int uBound = 10;

			const int UPPER_PWR = 100;
			const int LOWER_PWR = 40;
			const int HOLD_PWR = 0;

			// Bang-bang the mobile goal
			if (moGoIsUp)
			{
					if (SensorValue(moGoPot) < MOGO_UP - lBound)
							barPwr = UPPER_PWR;
					else if (SensorValue(moGoPot) < MOGO_UP - uBound)
							barPwr = LOWER_PWR;
					else
							barPwr = HOLD_PWR;
			}
			else
			{
					if (SensorValue(moGoPot) > MOGO_DOWN + lBound)
							barPwr = -UPPER_PWR;
					else if (SensorValue(moGoPot) > MOGO_DOWN + uBound)
							barPwr = -LOWER_PWR;
					else
							barPwr = -HOLD_PWR;
			}

			// Commented out because replaced in controller task
			motor[moGo] = barPwr;

			sleep(50);
	}
}



// AutoStack Functions
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
        if (vexRT(Btn5U) && !autoStackIsOn) // Runs autoStack
        {
            tasksStarted = false;
            stopTask(lift);
            stopTask(barControl);
            stopTask(intakeControl);

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

            startTask(lift);
            startTask(barControl);
            startTask(intakeControl);
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
	int startangle = SensorValue(gyroscope);
	isOpposite = true;
	driveTarget = -len;
	startTask(lDrivePID);
	startTask(rDrivePID);
	while((SensorValue(gyroscope) - startangle) > ((- len) + turnRange))
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
	int startangle = SensorValue(gyroscope);
	isOpposite = true;
	driveTarget = len;
	startTask(lDrivePID);
	startTask(rDrivePID);
	while((SensorValue(gyroscope) - startangle) < (len - turnRange));
	sleep(500);
	stopTask(lDrivePID);
	stopTask(rDrivePID);
	motor[rightB] = motor[rightF] = motor[leftB] = motor[leftF] = 0;
}

void turnTo(int angle)
{
	const int range = 5;
	const int pwr = 30;
	if(angle < SensorValue(gyroscope))
	{
		motor[leftB] = motor[leftF] = pwr;
		motor[rightB] = motor[rightF] = -pwr;
	}
	else
	{
		motor[leftB] = motor[leftF] = -pwr;
		motor[rightB] = motor[rightF] = pwr;
	}
	while(abs(SensorValue(gyroscope) - angle) > range){}
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


// Normal in game functions

// Place mobile goal
void place()
{
	// Check to make sure there aren't conflicting motor power sets with task Controller
	if (!barIsManual)
		stopTask(barSet);

	moGoIsUp = false;
	while (SensorValue(moGoPot) > MOGO_DOWN)
		motor[barL] = motor[barR] = -50;

	intakeCone(0);

	if (!barIsManual)
		startTask(barSet);

	barIsUp = true;
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



// Autons (Name these better @MAX)
void skillsauton1()
{
	liftTarget = LIFT_MIN + 600;
	startTask(lLiftPID);
	deployMGoal(1);

	forward(1120);

	motor[leftB] = motor[leftF] = motor[rightB] = motor[rightF] = 100;
	sleep(200);
	motor[leftB] = motor[leftF] = motor[rightB] = motor[rightF] = 0;

	deployMGoal(0);
	right(1800);
	forward(1000);
	right(900);
	sleep(500);
	forward(430);
	sleep(500);
	left(900);
	sleep(500);

	motor[moGo] = 100;
	sleep(400);
	motor[moGo] = 0;

	timeforward(200,500);
	sleep(800);
	int angle = SensorValue(gyroscope);
	motor[leftB] = motor[leftF] = motor[rightB] = motor[rightF] = 100;
	sleep(1000);
	motor[leftB] = motor[leftF] = motor[rightB] = motor[rightF] = 0;

	timebackward(700,2000);
	deployMGoal(0);

	turnTo(angle);

	right(900);
	timebackward(1100,2000);
	motor[rightB] = motor[rightF] = -50;
	sleep(2000);
	motor[rightB] = motor[rightF] = 0;
	sleep(500);
	pre_auton();
	forward(1200);
	right(450);
	deployMGoal(1);
	forward(1000);

	motor[leftB] = motor[leftF] = motor[rightB] = motor[rightF] = 100;
	sleep(200);
	motor[leftB] = motor[leftF] = motor[rightB] = motor[rightF] = 0;

	deployMGoal(0);

	forward(700);

	left(900);
	sleep(500);
	forward(450);
	sleep(500);
	right(900);
	sleep(500);

	motor[barL] = 100;
	sleep(400);
	motor[barL] = 0;

	timeforward(200,500);
	sleep(800);
	angle = SensorValue(gyroscope);
	motor[leftB] = motor[leftF] = motor[rightB] = motor[rightF] = 100;
	sleep(1000);
	motor[leftB] = motor[leftF] = motor[rightB] = motor[rightF] = 0;

	timebackward(700,2000);
	deployMGoal(0);

}

void grabMogo()
{
	motor[intake] = 20;
	liftTarget = LIFT_MIN + 200;
	startTask(lLiftPID);
	//startTask(barSet);
	startTask(MGoalDown);
	//barIsUp = true;
	sleep(500);
	forward(1350);

	startTask(MGoalUp);
	intakeCone(0);

	motor[leftB] = motor[leftF] = motor[rightB] = motor[rightF] = 100;
	sleep(200);
	motor[leftB] = motor[leftF] = motor[rightB] = motor[rightF] = 0;
	sleep(600);

	writeDebugStreamLine("%d",SensorValue(gyroscope));
	turnTo(0);
	writeDebugStreamLine("%d",SensorValue(gyroscope));
	backward(1750);
	writeDebugStreamLine("%d",SensorValue(gyroscope));
}

void grabMogoOtherSide()
{
	motor[intake] = 20;
	liftTarget = LIFT_MIN + 200;
	startTask(lLiftPID);
	//startTask(barSet);
	startTask(MGoalDown);
	//barIsUp = true;
	sleep(500);
	forward(1350);

	startTask(MGoalUp);
	intakeCone(0);
	motor[leftB] = motor[leftF] = motor[rightB] = motor[rightF] = 100;
	sleep(400);
	motor[leftB] = motor[leftF] = motor[rightB] = motor[rightF] = 0;
	sleep(1000);

	writeDebugStreamLine("%d",SensorValue(gyroscope));
	turnTo(0);
	writeDebugStreamLine("%d",SensorValue(gyroscope));
	backward(1750);
	writeDebugStreamLine("%d",SensorValue(gyroscope));
}

void matchAutonRightRed()
{
	grabMogo();

	right(450);
	backward(750);
	right(900);
	startTask(delayMGoalThrow);
	timeforward(650,1200);
	deployMGoal(1);
	backward(100);
	startTask(MGoalUp);
	sleep(800);
	backward(500);
}

void matchAutonLeftBlue()
{
	grabMogo();

	left(450);
	backward(750);
	left(900);
	startTask(delayMGoalThrow);
	timeforward(650,1200);
	deployMGoal(1);
	backward(100);
	startTask(MGoalUp);
	sleep(800);
	backward(500);

}

void matchAuton10PtBlue()
{
	grabMogoOtherSide();

	left(1350);
	forward(300);
	left(900);
	timeforward(200,600);
	deployMGoal(1);
	backward(800);
}

void matchAuton10PtRed()
{
	grabMogoOtherSide();

	right(1350);
	forward(300);
	right(900);
	timeforward(200,600);
	deployMGoal(1);
	backward(800);
}

void matchAuton5PtBlue()
{
	grabMogoOtherSide();

	forward(200);
	left(2000);
	deployMGoal(1);
	backward(500);
}

void matchauton5PtRed()
{
	grabMogoOtherSide();

	forward(200);
	right(2000);
	deployMGoal(1);
	backward(500);
}











// Pre-Auton
void pre_auton()
{
    bStopTasksBetweenModes = true;

    // Initializes the cones arrays
    initConeVals();

    // Calibrates Gyroscope
    SensorType[in4] = sensorNone;
    wait1Msec(1000);
    SensorType[in4] = sensorGyro;
    wait1Msec(2000);
    SensorFullCount[in4] = 3600 * 100;
    SensorScale[in4] = 130;

		// while(bIfiRobotDisabled == true)
		// 	autonomousSelection();
}

// Auton
task autonomous()
{
    clearDebugStream();
    datalogClear();

    //skillsauton1();
    //matchautonleftblue();
    //matchautonrightred();
    //matchauton10PtRed();
    // matchauton5PtRed();
}

// User Control
task usercontrol()
{
    clearDebugStream();
    datalogClear();

    // Starting Tasks
		startTask(controller);
    // startTask(autoStack);
    // startTask(autoStackControl);


    while (true)
    {
        //writeDebugStreamLine("%f %f", SensorValue(gyroscope), GyroAngleAbsGet());
    }
}
