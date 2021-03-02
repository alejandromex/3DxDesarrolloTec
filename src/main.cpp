#include <Arduino.h>
#include <math.h>

#define num_of_read 4 //READ THE SENSOR NUM NUMBER OF TIMES..SHOULD THE SUCCESIVE READINGS VARY
const int Rx = 7400;  //fixed resistor attached in series to the sensor and ground...the same value repeated for all WM and Temp Sensor.
const long default_TempC=24;
const long open_resistance=35000; //check the open resistance value by replacing sensor with an open and replace the value here...this value might vary slightly with circuit components 
const long short_resistance=200; // similarly check short resistance by shorting the sensor terminals and replace the value here.
const long short_CB=240, open_CB=255 ;

int i, j=0;                            
int SupplyV=3.9; // Cambio de voltaje a 3.3 - - - Assuming 5V output, this can be measured and replaced with an exact value if required
int WM1_CB=0;

double SenV10K=0, SenVTempC=0, SenVWM1=0, ARead_13=0 ;

#define constant 3.9;
float readVoltage()
{
  float ADCvoltage=(float)analogRead(13)/4095*constant;
  return ADCvoltage*constant;

}

void setup() 
{
  Serial.begin(9600); 
  
    pinMode(26, OUTPUT); 
    pinMode(25, OUTPUT);  
    pinMode(33, OUTPUT);  
    pinMode(12, OUTPUT);  
    pinMode(32, OUTPUT);  
    pinMode(14, OUTPUT);  
    
    delay(100);  
}

void loop()
{   
    while(j==0)
    {
    
    //**********LETS READ THE TEMPERATURE SENSOR ON CHANNEL 1**************
    // first take reading through path A

    ARead_13=0;
	
	for (i=0; i<num_of_read; i++)   //the num_of_read initialized above, controls the number of read successive read loops that is averaged. 
	{

		digitalWrite(26, HIGH);   //s0=1; s1=s2=0                 
		digitalWrite(25, LOW);
		digitalWrite(33, LOW);
		delay(10); //Make sure the MUX channels for sensor on Channel 0 is ready for activation
		// sensor on channel 1 is the Temperature Sensor 
		
		digitalWrite(12, HIGH);   //Set pin 5 as Vs
		
		delay(0.09); //wait 90 micro seconds and take sensor read...do not exceed 100uS
		ARead_13+=analogRead(13);   // read the sensor voltage right after the sensor and right before the series resistor Rx-7870
		
    
    	digitalWrite(12, LOW);      //set the excitation voltage to OFF/LOW
		delay(100); //0.1 second wait before moving to next channel or switching MUX
		
		// Now lets swap polarity

		digitalWrite(14, HIGH); //Set pin 11 as Vs

		delay(0.09); 
		ARead_13+=analogRead(13);   
		digitalWrite(14, LOW);      

	}
	

	Serial.println((String)ARead_13 + "Lectura analago 13");
	SenVTempC=((ARead_13/4095)*SupplyV) / (2 * num_of_read); //get the average of the readings and convert to volts
	double TempC_Resistance = (Rx*(SupplyV-SenVTempC)/SenVTempC)-140; //do the voltage divider math, using the Rx variable representing the known resistor
	
	Serial.println(ARead_13/4095);
	double y = (ARead_13/4095)*SupplyV;
	int x = 2 * num_of_read;
	double result = y/x;
	Serial.println(result);

	Serial.println((String)SenVTempC + " - SenVTempC");

	Serial.println((String)TempC_Resistance + " - TempC_Resistance"); 

    //Convert the resistance value of 10K temp sensor to actual Temperature value in Degree C 
    double TempC=(-23.89*(log(TempC_Resistance)))+246.00;

	Serial.println("====================");


		Serial.println(log(TempC_Resistance));


	Serial.println("====================");

    if (TempC_Resistance<0)
    {
		Serial.print("Temperature Sensor absent or open circuit assuming: \n");  
		TempC=default_TempC;
    }
    delay(100); //0.1 second wait before moving to next channel or switching MUX
    




    delay(5000);
   }
}    
