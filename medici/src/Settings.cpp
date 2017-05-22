/*
 * Settings.cpp
 *
 *  Created on: 28/mar/2013
 *      Author: Dario
 */

#include "Settings.h"

// build setteing with default values
Settings::Settings() {
	threshold = THRESHOLD_DEFAULT;
	mintries=1;
	tries=1;
	bettertries=1;
	nit1=1;
	nit2=2;
	wexp=1;
	wexp2=1.5;
	casa=false;
	actsconv=false;
	validate=false;
	autovalidate=false;
	queuecheck = true;
}

Settings::~Settings() {
	// TODO Auto-generated destructor stub
}

