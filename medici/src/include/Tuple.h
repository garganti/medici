/*
 * Tuple.h
 *
 *  Created on: 27/gen/2013
 *      Author: Dario
 */

#ifndef TUPLE_H_
#define TUPLE_H_
#include "header.h"

using namespace std;

/**
 * Class that define and store single Tuples to cover
 */
class Tuple {

public:
	struct compType {
		Tuple *otherIndex; //puntatore a tupla
		//int cardDiff;
		int relComp;
	};

	bool status; //0 to cover, 1 covered
	bool compCheck; //controllo se è stato generato vettore compatibility
	bool coverable; //0 uc
	/**
	 * Print Tuple
	 */
	void print(ostream&);
//	list < compType > compatible; //forse meglio vector, da vedere
	vector<cvalue> code;
	vector<int> param; //include gli indici dei parametri, se pairwise è di due elementi
	vector<int> paramCASA; //param index in casa format
	//list < Tuple > inserted;
//	dd_edge node;
	unsigned int compClass;
//	int compCard;
//	double cardDiff;
	double weight; //valore pesato sui parametri che contiene che ne decreta la sua importanza
	double weightMax;

//	void sortCompatible();
//	void resetCompatible();
	/**
	 * Merge two tuples, static function
	 * @param t1 first tuple
	 * @param t2 second tuple
	 * @return merged tuple
	 */
	static Tuple mergeTuple(Tuple, Tuple);
	/**
	 * ToString
	 * @return string
	 */
	string getValue();

	/**
	 * Constructor from a code and his parameters
	 * @param code
	 * @param params
	 */
	Tuple(vector<cvalue>, vector<int>); //passa codice e indice parametri

	/**
	 * Constructor from code, parameters, and casa codes
	 * @param code
	 * @param params
	 * @param casa codes
	 */
	Tuple(vector<cvalue>, vector<int>, vector<int>,unsigned int compClass=0); //passa codice e indice parametri

	/**
	 * Constructor from the number of parameters, it create an empty tuple with n values
	 * @param n
	 */
	Tuple(int); //crea una tupla vuota con n parametri

	static int isIncludedTuple(Tuple*,Tuple*);

	virtual ~Tuple();
};

#endif /* TUPLE_H_ */
