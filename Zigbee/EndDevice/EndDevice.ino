/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Example code for Xbee IO Pro Tutorial
        
        Using the Xbee Module, is posible change the relays states
        sending serials commands, and through others serials commands
        obtain the value of each optocoupler
        
        |RCV| ACTION |
        | 1 | Rele 1 |
        | 2 | Rele 2 |
        | 3 | Rele 3 |
        | 4 | Rele 4 |
        | - | ------ |
        | a | Opto 1 |
        | b | Opto 2 |
        | c | Opto 3 |
        | d | Opto 4 |

This file has been developed by Cesar Munoz for MCI www.olimex.cl, version 1.0 10/2012
The uses and changes are allowed, including the line above or the paragraph
"This file is partially based on the Example code for Xbee IO Pro Tutorial, 
developed by Cesar Munoz for MCI www.olimex.cl on 10/2012"
This code not provide extra guaranty and no limit the original warranty adquired for the buyer
Any hardware modification without authoritation from MCI void this warranty. 
Modified by Alvaro Alonso, 10/07/2014
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/


/*
 * Para comunicarse con el Xbee
 */
#include <SoftwareSerial.h>
SoftwareSerial	nss(2, 3); // rx,tx
char inByte;

void setup()
{
	pinMode(7, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);

	pinMode(4, INPUT);
	pinMode(5, INPUT);
	pinMode(6, INPUT);
	pinMode(11, INPUT);

	Serial.begin(9600);
        delay(100);

        nss.begin(9600);
        delay(100);
}


void loop()
{
	while(nss.available() > 0)
	{
	  inByte = nss.read();
	  if(inByte == '1')
	  {
	    toogleRelay(7);
            if(digitalRead(7) == HIGH) nss.println("1:N;");
            else nss.println("1:F;");
            //delay(100);
            
	  }
	  else if(inByte == '2')
          {
	    toogleRelay(8);
            if(digitalRead(8) == HIGH) nss.println("2:N;");
            else nss.println("2:F;");
            //delay(100);
	  }
	  else if(inByte == '3')
	  {
	    toogleRelay(9);
            if(digitalRead(9) == HIGH) nss.println("3:N;");
            else nss.println("3:F;");
            //delay(100);
	  }
	  else if(inByte == '4')
	  {
	    toogleRelay(10);
            if(digitalRead(10) == HIGH) nss.println("4:N;");
            else nss.println("4:F;");
            //delay(100);
	  }
          else if(inByte == 'E')
	  {
	    EstadoActual();
	  }
	}
}


void toogleRelay(int p1)
{
        if( (p1 == 7) || (p1 == 8) || (p1 == 9) || (p1 == 10) ) //solo activa los pines conectados a los reles
        	digitalWrite(p1, digitalRead(p1) ^ 1);

}

void EstadoActual()
{
        if(digitalRead(7) == HIGH) nss.println("1:N;");
            else nss.println("1:F;");
        if(digitalRead(8) == HIGH) nss.println("2:N;");
            else nss.println("2:F;");
        if(digitalRead(9) == HIGH) nss.println("3:N;");
            else nss.println("3:F;");
        if(digitalRead(10) == HIGH) nss.println("4:N;");
            else nss.println("4:F;");
}
o

