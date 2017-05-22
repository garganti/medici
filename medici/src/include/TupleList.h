/*
 * TupleList.h
 *
 *  Created on: 27/gen/2013
 *      Author: Dario
 */

#ifndef TUPLELIST_H_
#define TUPLELIST_H_
#include <Tuple.h>
#include <Settings.h>

using namespace std;
using namespace MEDDLY;

/**
 * Store the list of Tuples to cover and operations on them
 * store also the list of constraints
 */
class TupleList {
public:
	Settings setting;

	list<Tuple> tList;
//	list<Tuple> tListTC; //list tuple covered
	//contiene il numero di tuple che interagiscono con un parametro, ha dimensione pari alla size di una tupla
	vector<vector<int> > indexList;
	vector<vector<int> > indexListCASA; //contiene il numero di tuple che interagiscono con un parametro, ha dimensione pari alla size di una tupla

	vector<int> constraintsIndexList;
	int nToCover;
	int nCovered;
	int nUncoverable;
	int nTotal;
	int nValues;
	double weightMax;

	/**
	 * Return the size of the stored list
	 * @return size
	 */
	int size();

	/**
	 * Print list
	 */
	void print(ostream&);

	/**
	 * Scan param list and calculate their weight based on tuples to cover
	 * it then update the single tuples on based on their parameters and order the param list
	 * @param base meddly forest
	 */
	void checkParamList(forest*);
	void checkParamListNew(forest*);
	/**
	 * Same as checkparamList but in CASA format
	 * @param
	 */
	void checkParamListCASA(forest*); //actually not used

	/**
	 * That's a validator function, it crosscheck tuples in order to determine if they all are differents
	 * @param list
	 * @return -1 error, 1 ok
	 */
	int checkTuples(TupleList list);

	/**
	 * Remove covered tuples from the list
	 */
	void removeCovered();

	/**
	 * Set a single tuple as covered
	 * @param tupla
	 */
	void setCovered(Tuple &tupla);
	/**
	 * Set a list of tuples as covered
	 * @param vector of type Tuple
	 */
	void setCovered(vector<Tuple>);

	/**
	 * Sort tuple list based on params
	 * @param mode 0->standard weight based
	 * 1->based on max weight
	 * 2->order the separated list of tuples to cover (not actually used)
	 * 3->order based on compatibility (not actually used)
	 * 4->find a parameter with max weight and move it to the first position
	 */
	void sortCompatibility(int mode = 0);

	/**
	 * Pairwise static generator
	 * @param bounds
	 * @param nel number of elements
	 * @return list TupleList
	 */
//	static TupleList generatePairWise(int[], int);

	/**
	 * N-Wise static generator
	 * @param bounds
	 * @param nel number of elements
	 * @param nWise wise parameter
	 * @return list TupleList
	 */
	static TupleList generateNWise(int[], int, int nWise, Settings s);

	/**
	 * Calculate the number of values of the model given bounds and set it in the class
	 * @param bounds
	 * @param nel
	 */
	void setNValues(int[], int);
	// void createNodes(forest *);
	/**
	 * Remove uncoverable tuples from the list
	 */
	void removeUncoverable();

	int checkCompatibility(forest *mdd, dd_edge nodoBase);

	/**
	 * Reorder index list giving priority to parameters that interact with the one given
	 * @param param index
	 */
	void reorderIndexList(int,vector<vector<int> > &);

	TupleList(Settings s);
	virtual ~TupleList();

private:

};

#endif /* TUPLELIST_H_ */
