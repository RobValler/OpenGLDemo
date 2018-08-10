/*
 * report.cpp
 *
 *  Created on: Dec 28, 2012
 *      Author: robert
 */


#include <stdio.h>
#include <stdarg.h>


void report(const char *fmt, ...)
{

	va_list arg;
	va_start(arg, fmt);
	vprintf(fmt, arg);
	va_end(arg);

}
