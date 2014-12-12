/*
  ElipseMobile.cpp

  The MIT License (MIT)

  Copyright (c) 2014 Elipse Software. All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "ElipseMobile.h"
#define SIZEOF1(x) (sizeof(x)/sizeof(x[0]))

#define DefaultDigTags 10
#define DefaultAnalogTags 10

enum Commands
{
  GET_TAG_LIST = 0,
  DIGITAL_WRITE_CMD = 1,
  DIGITAL_READ_CMD = 2,
  ANALOG_WRITE_CMD = 3,
  ANALOG_READ_CMD = 4,
};

/***************************************************************************************************************/
void SendDigitalResponse(uint8_t  r, uint8_t result)
{
  uint8_t response[2];
  response[0] = (r != 0) ? 1 : 0;
  response[1] = result;
  Serial.write(response, SIZEOF1(response));
}
/***************************************************************************************************************/
void SendAnalogResponse(uint8_t  r, uint16_t response)
{
  r = (r != 0) ? 1 : 0;
  Serial.write((const uint8_t*)&r,1);
  Serial.write((const uint8_t*)&response,2);
}
/***************************************************************************************************************/
void SendStringResponse(uint8_t  r, const char* string, const uint16_t strSize)
{
  r = (r != 0) ? 1 : 0;
  Serial.write((const uint8_t*)&r,1);

  uint8_t response[strSize];
  for (int i=0; i < strSize; i++)
  {
    response[i] = string[i];
  }

  Serial.write(response, strSize);
}
/***************************************************************************************************************/
void ElipseMobile::GetTagList()
{
  while(Serial.available() <= 0)
  {
  }
  uint16_t strSize = (numberDigTags*7 + numberDigTags*3) + (numberAnalogTags*6 + numberAnalogTags*3) + 1;
  char tagstr[strSize];

  int pos = 0;
  for (int k=0; k < numberDigTags; k++)
    pos += sprintf(&tagstr[pos], "Digital%02i/", k);
  for (int k=0; k < numberAnalogTags; k++)
    pos += sprintf(&tagstr[pos], "Analog%02i/", k);

  tagstr[strSize-1] = '\0';

  SendStringResponse(1, tagstr, strSize);
}
/***************************************************************************************************************/
void DigitalWriteCommand()
{
  //Receives 2 characters
  while(Serial.available() <= 0)
  {
  }
  char port  = Serial.read(); //-1 if no data is available
  pinMode(port,OUTPUT);
  while(Serial.available() <= 0)
  {
  }
  char state = Serial.read();
  digitalWrite(port, state);
}
/***************************************************************************************************************/
void DigitalReadCommand()
{
  //Receives 1 characters and sends 2 characters
  while(Serial.available() <= 0)
  {
  }
  char port  = Serial.read(); //-1 if no data is available
  //pinMode(port,INPUT);
  uint8_t readResult = digitalRead(port);

  SendDigitalResponse(1, readResult);
}
/***************************************************************************************************************/
void AnalogWriteCommand()
{
  //Receives 2 characters
  while(Serial.available() <= 0)
  {
  }
  char port  = Serial.read(); //-1 if no data is available
  pinMode(port, OUTPUT);
  while(Serial.available() <= 0)
  {
  }
  char state = Serial.read();
  analogWrite(port, state);
}
/***************************************************************************************************************/
void AnalogReadCommand()
{
  //Receives 1 characters and sends 2 characters
  while(Serial.available() <= 0)
  {
  }
  char port  = Serial.read(); //-1 if no data is available
  uint16_t readResult = analogRead(port);

  SendAnalogResponse(1, readResult);
}
/***************************************************************************************************************/
/***************************************************************************************************************/

ElipseMobile::ElipseMobile()
{
}

bool ElipseMobile::ProcessCommands(char *command)
{
  if(Serial.available() > 0)
  {
    char commandRead = Serial.read();
    if(!numberDigTags)
    {
      numberDigTags = DefaultDigTags;
    }
    if(!numberAnalogTags)
    {
      numberAnalogTags = DefaultAnalogTags;
    }
    switch(commandRead)
    {
    case GET_TAG_LIST:
      GetTagList();
      break;
    case DIGITAL_WRITE_CMD:
      DigitalWriteCommand();
      break;
    case DIGITAL_READ_CMD:
      DigitalReadCommand();
      break;
    case ANALOG_WRITE_CMD:
      AnalogWriteCommand();
      break;
    case ANALOG_READ_CMD:
      AnalogReadCommand();
      break;
    default:
      break;
    }

    if(command != NULL)
      *command = commandRead;
  }
  return true;
}

bool ElipseMobile::DigitalTags(uint8_t dTags)
{
  if(dTags == NULL)
    return false;

  numberDigTags = dTags;
  return true;
}

bool ElipseMobile::AnalogTags(uint8_t aTags)
{
  if(aTags == NULL)
    return false;

  numberAnalogTags = aTags;
  return true;
}
