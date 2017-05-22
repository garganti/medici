/*
 * logger.cpp
 *
 *  Created on: 26/dic/2013
 *      Author: garganti
 */


#include "logger.hpp"
#include <iostream>


static nullstream lognullstream;

log_level_t threshold = LOG_DEBUG;

std::ostream& logcout(log_level_t x) {
	return ((x <= threshold) ? std::cout : lognullstream);
}
