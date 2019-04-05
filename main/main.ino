#include "Pin_Defines.h"
#include "Marvin_Communication.h"
#include "Marvin_Motor.h"
#include "Druckpumpe.h"
#include <string.h>
// #define ENCODER_OPTIMIZE_INTERRUPTS
// #include <Encoder.h>

/*-------------------------------------------------------------------
=                         Switches                                  =
--------------------------------------------------------------------*/

#define ON 0x01
#define OFF 0x00

#define DEBUG_WHOLEMESSAGE	OFF
#define DEBUG_XYF			OFF
#define DEBUG_P				OFF
#define DEBUG_S				OFF
#define ENCODER				ON
#define ENCODER_ADVANCED	OFF
#define LIMIT_SWITCH1		OFF
#define LIMIT_SWITCH2		OFF
#define RPM_MITTELUNG		ON
#define EASY_REGLER			ON
#define P_REGLER			OFF
#define PI_REGLER			OFF

/*-------------------------------------------------------------------
=                         Message Queue                             =
--------------------------------------------------------------------*/
Commando MessageQueue[100];
Commando *EndOfMessageQueue;



/*-------------------------------------------------------------------
=                         Bresenham                                 =
--------------------------------------------------------------------*/

extern volatile int shortpin, longpin;
extern volatile float bcount;
volatile float bcounti = 0;
extern volatile uint16_t steps_x, steps_y;
extern volatile unsigned long pulses_x, pulses_y;
bool running_flag, REACHED = false;
volatile float nextX[100], nextY[100], nextF[100], Xreached, Yreached, Freached;
volatile uint8_t pnumber = 0;
volatile long steps_per_millimeter = (long)400;

/*-------------------------------------------------------------------
=                         Endschalter                               =
--------------------------------------------------------------------*/

uint8_t endschalter_flag_x = 0, endschalter_flag_y = 0;
extern uint8_t endschalter_flag_x;
extern uint8_t endschalter_flag_y;

/*-------------------------------------------------------------------
=                         Encoder                                    =
--------------------------------------------------------------------*/
volatile int encoder_pin = 2;
double rpm = 0.0;
volatile float velocity = 0;
volatile long pulses = 0;
volatile unsigned long timeold = 0, timeold_serial = 0;
volatile unsigned int pulsesperturn = 1;
const int wheel_diameter = 24;
static volatile unsigned long debounce = 0;

/*-------------------------------------------------------------------
=                         Motor Definitions                         =
--------------------------------------------------------------------*/

// Encoder encoder_spindel(RENC_A, RENC_B);
Marvin_Steppers stepper_motors(PWM1, PWM2, DIR1, DIR2);
Spindel spindel;
PressurePump pump;

/*-------------------------------------------------------------------
=                         Regler                                    =
--------------------------------------------------------------------*/
double Setpoint;
double rpwm = 0, rpwmmiddle = 0;
double rpwmarray[10];
int rpwmcount = 0;
int Max_RPM = 3000, Max_PWM = 150;
int kp = 1, kd = 1, ki = 1;
double ta = 0.1;
int esum = 0;


/*-------------------------------------------------------------------
=                         Others                                    =
--------------------------------------------------------------------*/
long positionLeft = -999;
volatile commEnum c = Wait;


/*-------------------------------------------------------------------
=                         ISR                                       =
--------------------------------------------------------------------*/

ISR(TIMER3_COMPA_vect)
{
	TCNT3 = 0;

	running_flag = true;
	if (longpin == PWM1)
	{
		PORTE ^= (1 << PE3);
	}
	else
	{
		PORTH ^= (1 << PH3);
	}

	bcounti++;
	if (bcounti >= bcount && bcount != 0)
	{
		bcounti = 0;
		if (shortpin == PWM1)
		{
			PORTE ^= (1 << PE3);
		}
		else
		{
			PORTH ^= (1 << PH3);
		}
	}

	pulses_x--;
	if (pulses_x <= 0)
	{
		pulses_x = 0;
		TCCR3B = 0;
		TCCR3A = 0;
		running_flag = false;
		pnumber = 0;
	}

}

void setup()
{
	/*Timer 3 Setup*/
	TCCR3A = 0;
	TCCR3B = 0;
	TCNT3 = 0;

	/*Wait for Serial to connect*/
	Serial.begin(115200);
	while (!Serial);

	/*Stepper Motor 1 PWM Output*/
	pinMode(PWM1, OUTPUT);

	/*Stepper Motor 2 PWM Output*/
	pinMode(PWM2, OUTPUT);

	/*Spindel PWM Output*/
	pinMode(PWM3, OUTPUT);

	/*Direction Pin for Stepper Motor 1*/
	pinMode(DIR1, OUTPUT);

	/*Direction Pin for Stepper Motor 2*/
	pinMode(DIR2, OUTPUT);

	/*Direction Pins for Spindel*/
	pinMode(DIR3, OUTPUT);
	pinMode(DIR4, OUTPUT);

	/*Direction Pins for Pump*/
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);

	/*Relay Output*/
	pinMode(RELAY_IN4, OUTPUT);

	/*Limit Switches Input*/
	pinMode(LIM1, INPUT);
	pinMode(LIM2, INPUT);

	// stepper_motors.stopTimer3();
	// stepper_motors.stopTimer4();
	spindel.setRichtung(keine);
	digitalWrite(RELAY_IN4, LOW);

	/*Just to be sure*/
	pulses = 0;
	rpm = 0;
	timeold = 0;
	timeold_serial = 0;

	/*Encoder Interrupt Pin Input*/
	pinMode(ENCODER_PIN, INPUT);

	/*Encoder Interrupt Pin Attach Interrupt on rising edge*/
	attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), counter, RISING);

	/*Set Last Message Pointer to first Element of Message Queue
	'cause it's still empty*/
	EndOfMessageQueue = &MessageQueue[0];
	Commando com;
	com.Command = NO_VALID_MESSAGE;
	com.T1 = -1.0;
	com.T2 = -1.0;
	com.T3 = -1.0;
	for (int i = 0; i < sizeof(MessageQueue) / sizeof(MessageQueue[0]); i++)
	{
		MessageQueue[i] = com;
	}
}



void loop()
{

#if LIMIT_SWITCH1
	if (!digitalRead(LIM1))
	{
		stopAll();
	}
#endif

#if LIMIT_SWITCH2
	if (!digitalRead(LIM2))
	{
		stopAll();
	}
#endif


	/*Read Encoder a lot faster than once per second. This may be too slow
	for PID*/
	if (millis() - timeold >= 100)
	{
		rpm = (pulses * 60.0 * 0.5 * 10);	// * 100 'cause 10 Milliseconds NOT Seconds
		velocity = rpm * 3.1416 * wheel_diameter * 60.0 / 1000000.0;
		timeold = millis();

		pulses = 0;

		rpwmarray[0] = rpm;

		/*Push RPM Array values one place back*/
		double *pl, *ph;
		pl = &rpwmarray[0];
		ph = &rpwmarray[1];
		for (int i = 0; i < 10; i++)
		{
			*pl++ = *ph++;
		}

#if EASY_REGLER

#if RPM_MITTELUNG

		/*Regler Mittelung*/
		for (int i = 0; i < 10-1; i++)
		{
			rpwmmiddle += rpwmmiddle;
		}
		rpwmmiddle = rpwmmiddle / 10.0;

#else
		rpwmmiddle = rpm;
#endif	// End of RPM_MITTELUNG

		rpwmmiddle = round(rpwmmiddle / 30.0) * 30.0;

		/*Regler*/
		if (rpwmmiddle < Setpoint)
		{
			if (rpwm < 255)
			{
				rpwm++;
			}
		}
		else if (rpwmmiddle > Setpoint)
		{
			if (rpwm > 0)
			{
				rpwm--;
			}
		}

#elif P_REGLER
		// Regelabweichung
		int e = Setpoint - rpwmmiddle;

		// Reglergleichung
		int y =  kp*e;
		
		// Y Limits
		if(y>Max_RPM){y = Max_RPM;}
		if(y<-Max_RPM){y = -Max_RPM;}

		// PWM 
		double anteil = (double)y / Max_RPM;
		rpwm += Max_PWM * anteil;

#elif PI_REGLER
		// Regelabweichung
		int e = Setpoint - rpwmmiddle;

		// Integralanteil
		esum += e;

		if (esum < -400) {esum = -400;}	        //Begrenzung I-Anteil
	  	if (esum > 400) {esum = 400;}
		
		// Reglergleichung
		int y =  kp*e + ki*Ta*esum;
		
		// Y Limits
		if(y>Max_RPM){y = Max_RPM;}
		if(y<-Max_RPM){y = -Max_RPM;}

		// PWM 
		double anteil = (double)y / Max_RPM;
		rpwm += Max_PWM * anteil;

#endif	// End Of EASY_REGLER & P_REGLER

		spindel.startMotorRPM(rechts, rpwm);

	}//	End Of Timer Encoder



	/*Send Encoder Data*/
	if (millis() - timeold_serial >= 1000)
	{
		Serial.print(millis() / 100);
		Serial.print("       ");
		Serial.print(rpm, DEC);
		Serial.print("   ");
		Serial.print(pulses, DEC);
		Serial.print("     ");
		Serial.println(velocity, 2);
		timeold_serial = millis();
	}


	/*Message Queue*/
	String m;
	char arr[10];
	char *token;
	int count = 0;
	struct StringArray xm;
	uint8_t comval = 0;
	while (checkConnection() > 0)
	{
		/*Read New Message Until it reaches "@" or
		Timeout after one second*/
		m = Serial.readStringUntil('@');

		/*Check if and which command was sent*/
		c = GetCommunicationEnum(m);

		m.toCharArray(arr, 20);

		/*Split Message in Parts of ; and save it in
		Container String_Array*/
		token = strtok(arr, ";");	// Get first Token

		xm.count = 0;
		while (token != NULL && xm.count < 20)
		{
			strcpy(xm.str_array[xm.count], token);
			xm.count++;
			token = strtok(NULL, ";");
		}

		/*From Received Message we create a Command*/
		Commando com = createCommand(c, xm);

		/*Send Appropriate ACK*/
		sendACK(c, com);

		/*Skip if No Valid Message Was Sent*/
		if (c == NO_VALID_MESSAGE)
		{
			continue;
		}

		/*Add it to Message Queue*/
		*(EndOfMessageQueue++) = com;	// Write to End of Message Queue and increment pointer
	}

	/*Manage Commands*/
	if (MessageQueue[0].Command != NO_VALID_MESSAGE)
	{
		comval = runCommand(MessageQueue[0], &pump, &spindel, &stepper_motors);

		if (comval == 1)
		{
			Commando *pl = &MessageQueue[0], *ph = &MessageQueue[1];
			for (int i = 0; i < sizeof(MessageQueue) / sizeof(MessageQueue[0]); i++)
			{
				*pl++ = *ph++;
			}

			EndOfMessageQueue--;
			Commando com;
			com.Command = NO_VALID_MESSAGE;
			com.T1 = -1.0;
			com.T2 = -1.0;
			com.T3 = -1.0;
			*EndOfMessageQueue = com;

		}
	}

}

void counter()
{
	pulses++;
}

void stopAll()
{
	// Stop Spindel
	// Stop Steppers
	// Stop Pump
	spindel.stopMotor();
	stepper_motors.stopMotors();
	pump.stopMotor();
	// Clear Point Queue
	for (size_t i = 1; i < 100; i++)
	{
		nextX[i] = 0;
		nextY[i] = 0;
		nextF[i] = 0;
	}
	// Reset Pointer
	pnumber = 0;
	// Reset Running Flag
	running_flag = false;
	// Reset REACHED Flag
	REACHED = false;
}


