/*
 * MEDICI.h
 *
 *  Created on: 29/mar/2013
 *      Author: Dario
 */

#ifndef MEDICI_H_
#define MEDICI_H_


#include <header.h>
#include <MDDList.h>
#include <TupleList.h>
#include <Tuple.h>
#include <Settings.h>


using namespace std;
using namespace MEDDLY;

class MEDICI {
public:
	static int normalMode(Settings setting, vector<int> &res);
	static int validateMode(Settings setting);
	static int actsMode(Settings setting);
	MEDICI();
	virtual ~MEDICI();
};

#endif /* MEDICI_H_ */
