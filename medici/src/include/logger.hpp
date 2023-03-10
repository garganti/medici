/*
 * logger.hpp
 *
 *  Created on: 26/dic/2013
 *      Author: garganti
 */

#ifndef LOGGER_HPP_
#define LOGGER_HPP_
#include <iostream>

// A class which does not print anything
struct nullstream: std::ostream {
	nullstream() :
			std::ios(0), std::ostream(0) {
	}
};

enum log_level_t {
	LOG_NOTHING, LOG_CRITICAL, LOG_ERROR, LOG_WARNING, LOG_INFO, LOG_DEBUG
};

extern log_level_t threshold;

std::ostream& logcout(log_level_t x);

#endif /* LOGGER_HPP_ */
