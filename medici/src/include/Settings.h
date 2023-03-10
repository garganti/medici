/*
 * Settings.h
 *
 *  Created on: 28/mar/2013
 *      Author: Dario
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_



class Settings {
public:
	// threshold should be >=0
    int threshold;
    int mintries;
    // maximum number of tries
    int tries;
    int bettertries;
    //
    int nit1;
    //
    int nit2;
    //
    int strength = 0;
    double wexp;
    double wexp2;
    bool casa;
    bool stdOut;
    bool actsconv;
    bool useCTWedge;
    bool validate;
    bool autovalidate;
    bool donotgenerate; // build MDD but do not generate
    // controllo code
    bool queuecheck;
    char out[1000];
    char model[1000];
    char constraint[1000];
    char mname[1000];
	Settings();
	virtual ~Settings();
	// default values
	static const int THRESHOLD_DEFAULT = 2000;
};

#endif /* SETTINGS_H_ */
