/*
 * MDDList.h
 *
 *  Created on: 27/gen/2013
 *      Author: Dario
 */

#ifndef MDDLIST_H_
#define MDDLIST_H_

#include "TupleList.h"
#include <ostream>

using namespace std;
using namespace MEDDLY;
/**
 * MDDList class, manage the list of created MDDs that will in the end become test-cases
 */
class MDDList {
public:
	struct Node {
		dd_edge node; //puntatore a tupla
		int nCoveredMdd;
		double tWeight;

	};
	struct Constraint {
		dd_edge node;
		vector<int> params;
	};

	vector<Node> list;
	vector<Constraint> listConstraints;
	vector<Constraint> listConstraintsSimple;

	/**
	 * Second phase, generate the best possible Test-Case given the list of tuples to cover
	 * @param listT		List of tuples to cover
	 * @param bounds	Bounds of variables in domain
	 * @return			-1 error, 0 ok
	 */
	int addOther2b(TupleList &listT, int*, int);

	forest* mdd;
	dd_edge nodoBase;
	dd_edge nodoVuoto;
//	dd_edge nodoFase1;

	/**
	 * Print the List of generated Test-Cases
	 */
	void printList(ostream& str);

	/**
	 * Add the constraints acquired from the TestModel to the base node
	 * Generate the constraints list added as an element on tuplelist class
	 * Fragment the constraints in the list in order to optimize check by other functions
	 *
	 * @param tMC	Constraints model
	 * @param bounds	Bounds of variable in domain
	 * @param N			Number of variables
	 * @param tList		List of tuples to cover, it contain the list of constraints used in other functions
	 */
	void updateConstraints(vector<std::list<Operations::TestModelConstraint> >,
			int*, int, TupleList &);

	/**
	 * Add the constraints acquired from the TestModel to the base node
	 * Generate the constraints list added as an element on tuplelist class
	 * Fragment the constraints in the list in order to optimize check by other functions
	 *
	 * @param tMC	Constraints model
	 * @param bounds	Bounds of variable in domain
	 * @param N			Number of variables
	 * @param tList		List of tuples to cover, it contain the list of constraints used in other functions
	 */
	void updateConstraintsMEDICI(vector<std::list<Operations::TestModelConstraint> >,
			int*, int, TupleList &);
	/**
	 * Save the test cases list to file, based on CASA model: http://cse.unl.edu/~citportal/tools/casa/documentation2.php
	 * @param filename
	 * @param bounds
	 * @return -1 error, 0 ok
	 */
	int listToFile(char *, int*);

	/**
	 * Add an MDD to the list, constraints are added, if any
	 * @return 0 ok
	 */
	int aggiungiInCoda();
	/**
	 * Scan TupleList and check if they are compatible with constraints
	 * @param mdd Meddly forest
	 * @param tupleList List of tuples to cover
	 * @return 0 ok
	 */
	int checkCoverable(forest*, TupleList &);
	/**
	 * Check TupleList to find the ones covered by the generated testcase
	 * @param mddIndex  The index of the Test-Case in the list
	 * @param listT	    The list of Tuples
	 * @param mode		0 standard, 1 do not update tuplelist
	 */
	void updateCovered(int, TupleList &, int mode = 0); //0 esegue veloce, 1 aggiorna anche lista tuple da coprire quando aggiorna status


	static MDDList listFromFile(char *, int*, int, forest *);

	MDDList(forest *);
	virtual ~MDDList();
private:
	/**
	 * Merge Constraints in the list, generating a reduced list considering dependences
	 * @return true ok, false error
	 */
	bool mergeConstraints();
	/**
	 * Print constraints list
	 */
	void printConstraints();
	/**
	 * Check the best choice for the given parameters based on optimization goal
	 * @param it2		Parameter data, index and weight
	 * @param tupla		Partial TestCase
	 * @param bounds	Bounds of Variables/Levels in domain
	 * @param mddIndex	Index of the testcase in the list
	 * @param listT		List of Tuples still to cover
	 * @return			Weight of the parameter fixed, based on optimization goal, will be up to the caller to decide if fix it in the testcase or not
	 */
	int checkForParameter(vector<int>, Tuple &, int*, int, TupleList &, int,
			bool, int &);

	/**
	 * Print the Mdd at the given index, used to print a complete test-case (mdd with cardinality=1),
	 * otherwise it'll print only the first valid case of the mdd
	 * @param index	index of mdd to print
	 */
	void printMdd(ostream&, int);

	/**
	 * Update an MDD with the parameters in code, code should be compatible with the MDD otherwise it'll generate an invalid case
	 * (always false).
	 * @param index Index of the mdd
	 * @param code	list of parameters to set (-1 in case it is not fixed)
	 * @return 0 ok
	 */
	int aggiungiAt(int index, vector<cvalue> code);
	/**
	 * Intersect an mdd with the one pointed by the index
	 * @param index	index of existing mdd in the list
	 * @param mddToAdd	mdd to intersect
	 * @return	0 ok
	 */
	int aggiungiAt(int index, dd_edge mddToAdd);



};

#endif /* MDDLIST_H_ */
