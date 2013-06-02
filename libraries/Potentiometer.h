/*
||
|| @file Potentiometer.h
|| @version 1.2
|| @author Alexander Brevig
|| @contact alexanderbrevig@gmail.com
||
|| @description
|| | Provide an easy way of making/using potentiometerss
|| #
||
|| @license
|| | This library is free software; you can redistribute it and/or
|| | modify it under the terms of the GNU Lesser General Public
|| | License as published by the Free Software Foundation; version
|| | 2.1 of the License.
|| |
|| | This library is distributed in the hope that it will be useful,
|| | but WITHOUT ANY WARRANTY; without even the implied warranty of
|| | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
|| | Lesser General Public License for more details.
|| |
|| | You should have received a copy of the GNU Lesser General Public
|| | License along with this library; if not, write to the Free Software
|| | Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
|| #
||
*/

#ifndef Potentiometer_H
#define Potentiometer_H
 
#include <WProgram.h>

class Potentiometer {
	public:
		Potentiometer(byte potPin);
		Potentiometer(byte potPin, uint16_t sectors);
		uint16_t getValue();
		uint16_t getSector();
		void setSectors(uint16_t sectors);
	private:
		byte pin;
		uint16_t sectors;
};

#endif

/*
|| @changelog
|| | 1.2 2009-07-21 - Alexander Brevig : Changed Lisence
|| | 1.0 2009-04-13 - Alexander Brevig : Initial Release
|| #
*/