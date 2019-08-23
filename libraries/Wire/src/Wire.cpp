/*
  TwoWire.cpp - TWI/I2C library for Wiring & Arduino
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 
  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
  Modified 2017 by Chuck Todd (ctodd@cableone.net) to correct Unconfigured Slave Mode reboot
*/

extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
  #include "utility/twi.h"
}

#include "Wire.h"

#define DEFAULT_FREQUENCY 100000

// Initialize Class Variables //////////////////////////////////////////////////

uint8_t TwoWire::rxBuffer[BUFFER_LENGTH];
uint8_t TwoWire::rxBufferIndex = 0;			//head
uint8_t TwoWire::rxBufferLength = 0;		//tail

uint8_t TwoWire::txAddress = 0;
uint8_t TwoWire::txBuffer[BUFFER_LENGTH];
uint8_t TwoWire::txBufferIndex = 0;			//head
uint8_t TwoWire::txBufferLength = 0;		//tail

uint8_t TwoWire::transmitting = 0;
void (*TwoWire::user_onRequest)(void);
void (*TwoWire::user_onReceive)(int);

// Constructors ////////////////////////////////////////////////////////////////

TwoWire::TwoWire()
{
}

// Public Methods //////////////////////////////////////////////////////////////

void TwoWire::begin(void)
{
	rxBufferIndex = 0;
	rxBufferLength = 0;

	txBufferIndex = 0;
	txBufferLength = 0;

	TWI_MasterInit(DEFAULT_FREQUENCY);	
}

void TwoWire::begin(uint8_t address)
{
	rxBufferIndex = 0;
	rxBufferLength = 0;

	txBufferIndex = 0;
	txBufferLength = 0;
  
	TWI_SlaveInit(address);
	
	TWI_attachSlaveTxEvent(onRequestService, txBuffer); // default callback must exist
	TWI_attachSlaveRxEvent(onReceiveService, rxBuffer, BUFFER_LENGTH); // default callback must exist
	
}

void TwoWire::begin(int address)
{
	begin((uint8_t)address);
}

void TwoWire::end(void)
{
	TWI_Disable();
}

void TwoWire::setClock(uint32_t clock)
{
	TWI_MasterSetBaud(clock);
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool sendStop) {	
	if(quantity > BUFFER_LENGTH){
		quantity = BUFFER_LENGTH;
	}
	
	uint8_t bytes_read = TWI_MasterRead(address, rxBuffer, quantity, sendStop);
	
	/* Initialize read variables */
	rxBufferIndex = 0;
	rxBufferLength = bytes_read;

	return bytes_read;
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity)
{
	return requestFrom(address, quantity, true);
}

uint8_t TwoWire::requestFrom(int address, int quantity)
{
	return requestFrom((uint8_t)address, (size_t)quantity, true);
}

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop)
{
	return requestFrom((uint8_t)address, (size_t)quantity, (bool)sendStop);
}

void TwoWire::beginTransmission(uint8_t address)
{
  // indicate that we are transmitting
  transmitting = 1;
  // set address of targeted slave
  txAddress = address;
  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;
}

void TwoWire::beginTransmission(int address)
{
	beginTransmission((uint8_t)address);
}

//
//	Originally, 'endTransmission' was an f(void) function.
//	It has been modified to take one parameter indicating
//	whether or not a STOP should be performed on the bus.
//	Calling endTransmission(false) allows a sketch to 
//	perform a repeated start. 
//
//	WARNING: Nothing in the library keeps track of whether
//	the bus tenure has been properly ended with a STOP. It
//	is very possible to leave the bus in a hung state if
//	no call to endTransmission(true) is made. Some I2C
//	devices will behave oddly if they do not see a STOP.
//
uint8_t TwoWire::endTransmission(bool sendStop)
{
	// transmit buffer (blocking)
	uint8_t status = TWI_MasterWrite(txAddress, txBuffer, txBufferLength, sendStop);
	
	// reset tx buffer iterator vars
	txBufferIndex = 0;
	txBufferLength = 0;
	
	// indicate that we are done transmitting
	transmitting = 0;
	
	return status;
}

//	This provides backwards compatibility with the original
//	definition, and expected behaviour, of endTransmission
//
uint8_t TwoWire::endTransmission(void)
{
  return endTransmission(true);
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWire::write(uint8_t data)
{
	/* Check if buffer is full */
	if(txBufferLength >= BUFFER_LENGTH){
	  setWriteError();
	  return 0;
	}

	/* Put byte in txBuffer */
	txBuffer[txBufferIndex] = data;
	txBufferIndex++;

	/* Update buffer length */
	txBufferLength = txBufferIndex;
	 
	return 1;
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWire::write(const uint8_t *data, size_t quantity)
{

	for(size_t i = 0; i < quantity; i++){
		write(*(data + i));
	}

	return quantity;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::available(void)
{
	return rxBufferLength - rxBufferIndex;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::read(void)
{
	int value = -1;
  
	// get each successive byte on each call
	if(rxBufferIndex < rxBufferLength){
		value = rxBuffer[rxBufferIndex];
		rxBufferIndex++;
	}

	return value;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::peek(void)
{
	int value = -1;
  
	if(rxBufferIndex < rxBufferLength){
		value = rxBuffer[rxBufferIndex];
	}

	return value;
}

// can be used to get out of an error state in TWI module 
// e.g. when MDATA regsiter is written before MADDR
void TwoWire::flush(void)
{
// 	/* Clear buffers */
// 	for(uint8_t i = 0; i < BUFFER_LENGTH; i++){
// 		txBuffer[i] = 0;
// 		rxBuffer[i] = 0;
// 	}
// 	
// 	/* Clear buffer variables */
// 	txBufferIndex = 0;
// 	txBufferLength = 0;
// 	rxBufferIndex = 0;
// 	rxBufferLength = 0;
// 	
// 	/* Turn off and on TWI module */
// 	TWI_Flush();
}

// behind the scenes function that is called when data is received
void TwoWire::onReceiveService(int numBytes)
{
	// don't bother if user hasn't registered a callback
	if(!user_onReceive){
		return;
	}
	// don't bother if rx buffer is in use by a master requestFrom() op
	// i know this drops data, but it allows for slight stupidity
	// meaning, they may not have read all the master requestFrom() data yet
	if(rxBufferIndex < rxBufferLength){
		return;
	}

	// set rx iterator vars
	rxBufferIndex = 0;
	rxBufferLength = numBytes;
	
	// alert user program
	user_onReceive(numBytes);
}

// behind the scenes function that is called when data is requested
uint8_t TwoWire::onRequestService(void)
{
	// don't bother if user hasn't registered a callback
	if(!user_onRequest){
		return 0;
	}
	
	// reset slave write buffer iterator var
	txBufferIndex = 0;
	txBufferLength = 0;
  
	// alert user program
	user_onRequest();

	return txBufferLength;
}

// sets function called on slave write
void TwoWire::onReceive( void (*function)(int) )
{
	user_onReceive = function;
}

// sets function called on slave read
void TwoWire::onRequest( void (*function)(void) )
{
	user_onRequest = function;
}

// Preinstantiate Objects //////////////////////////////////////////////////////

TwoWire Wire = TwoWire();

