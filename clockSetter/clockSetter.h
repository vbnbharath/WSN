/*
 * clockSetter.h
 *
 *  Created on: Oct 11, 2015
 *      Author: cgoss
 */

#ifndef CLOCKSETTER_H_
#define CLOCKSETTER_H_
enum clockEnum
{
	s12k,
	s1M,
	s8M,
	s12M,
	s16M
} clockSpeed;

void setClock(clockSpeed speed)


#endif /* CLOCKSETTER_H_ */
