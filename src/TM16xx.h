/*
TM16xx.h - Library for TM1637, TM1638 and similar chips.

Copyright (C) 2011 Ricardo Batista <rjbatista at gmail dot com>
Modified by Maxint R&D. See https://github.com/maxint-rd/

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TM16XX_h
#define TM16XX_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#if !defined(__max)
// MMOLE 180325: min, max are no macro in ESP core 2.3.9 libraries, see https://github.com/esp8266/Arduino/issues/398
// MMOLE 211229: Redefining min/max has issues in newer ESP cores with certain wifi libraries.
//               See definition as function template below.
#define __min(a,b) ((a)<(b)?(a):(b))
#define __max(a,b) ((a)>(b)?(a):(b))
#endif

#if !defined(_BV)
// MMOLE 220909: _BV is not defined in Raspberry Pi Pico RP2040 core v2.4.0 or v2.5.2 by Earle F. Philhower
// TM16xx uses it in some classes to determine button presses (TM1628/30/37/38/38Anode
// Note that _BV works on int values (16-bit in Arduino). For that reason using bit() would be better.
// This is how _BV is defined in the AVR libraries used within Arduino:
#define _BV( x )( 1<<(x))
#endif

#define TM16XX_CMD_DATA_AUTO 0x40
#define TM16XX_CMD_DATA_READ 0x42			// command to read data used on two wire interfaces of TM1637
#define TM16XX_CMD_DATA_FIXED 0x44
#define TM16XX_CMD_DISPLAY 0x80
#define TM16XX_CMD_ADDRESS 0xC0

#include "TM16xxFonts.h"

class TM16xx
{
  public:
    /**
     * Instantiate a TM16xx module specifying data, clock and strobe pins (no strobe on some modules),
     * the maximum number of displays supported by the chip (as provided by derived chip specific class), 
     * the number of digits used to display numbers or text, 
     * display state and the starting intensity (0-7).
     */
    TM16xx(byte dataPin, byte clockPin, byte strobePin, byte maxDisplays, byte nDigitsUsed, boolean activateDisplay=true,	byte intensity=7);

    /** Set the display (segments and LEDs) active or off and intensity (range from 0-7). */
    virtual void setupDisplay(boolean active, byte intensity);

    /** Clear the display */
		virtual void clearDisplay();

    /** Set segments of the display */
	  virtual void setSegments(byte segments, byte position);
	  virtual void setSegments16(uint16_t segments, byte position);   // some modules support more than 8 segments
	  
	  // Basic display functions. For additional display features use the TM16xxDisplay class
    /** Set a single display at pos (starting at 0) to a digit (left to right) */
    virtual void setDisplayDigit(byte digit, byte pos=0, boolean dot=false, const byte numberFont[] = TM16XX_NUMBER_FONT);
		/** Set the display to a decimal number */
	  virtual void setDisplayToDecNumber(int nNumber, byte bDots=0);
		/** Clear  a single display at pos (starting at 0, left to right) */ 
    virtual void clearDisplayDigit(byte pos, boolean dot=false);
    /** Set the display to the values (left to right) */
    virtual void setDisplay(const byte values[], byte size=8);

    /** Set the display to the string (defaults to built in font) */
		virtual void setDisplayToString(const char* string, const word dots=0, const byte pos=0, const byte font[] = TM16XX_FONT_DEFAULT);
		virtual void sendChar(byte pos, byte data, boolean dot); // made public to allow calling from TM16xxDisplay
		virtual byte getNumDigits(); // added as public menthod to allow calling from TM16xxDisplay
		virtual void sendAsciiChar(byte pos, char c, boolean dot); // made public to allow calling from TM16xxDisplay

		// Key-scanning functions
		// Note: not all TM16xx chips support key-scanning and sizes are different per chip
		// Up to 32 key states are supported, but specific chips may support less keys or less combinations
		// The chip specific derived class method will return a 32-bit value representing the state of each key, containing 0 if no key is pressed
		virtual uint32_t getButtons();  // return state of up to 32 keys.

  protected:
//		virtual void sendChar(byte pos, byte data, boolean dot);
	  virtual void bitDelay();
    virtual void start();
    virtual void stop();
    virtual void send(byte data);
		virtual void sendCommand(byte led);
    virtual void sendData(byte add, byte data);
    virtual byte receive();

#if !defined(max)
// MMOLE 211229: use c++ function templates to implement our own min/max, as redefining them wont work in newer ESP cores when using certain wifi libraries
// NOTE: min, max are no macro in ESP core 2.3.9 libraries, see https://github.com/esp8266/Arduino/issues/398
// See also
//   https://www.cplusplus.com/doc/oldtutorial/templates/
//   https://www.alltestanswers.com/c-templates-for-the-two-functions-minimum-and-maximum/
//   https://www.learncpp.com/cpp-tutorial/function-templates-with-multiple-template-types/
// MMOLE 220814: Arduino IDE 1.8.12 for LGT328P required "#if !defined(max)" to prevent compilation errors
template <typename T, typename U>
auto max(T x, U y) -> decltype(x>y ? x : y)
{
  return x>y ? x : y;
}
template <typename T, typename U>
auto min(T x, U y) -> decltype(x>y ? x : y)
{
  return x<y ? x : y;
}
#endif  // !defined(max)

    byte _maxDisplays=2;		// maximum number of digits (grids), chip-dependent
    byte _maxSegments=8;		// maximum number of segments per display, chip-dependent

    byte digits;		// number of digits in the display, module dependent
    byte dataPin;
    byte clockPin;
    byte strobePin;
};

#endif
