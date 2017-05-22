/*
 * Operations.h
 *
 *  Created on: 26/gen/2013
 *      Author: Dario
 */

#ifndef OPERATIONS_H_
#define OPERATIONS_H_
#include <assert.h>
#include <ostream>
#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <ctime>
#endif

using namespace std;
using namespace MEDDLY;
/* Returns the amount of milliseconds elapsed since the UNIX epoch. Works on both
 * windows and linux. */
typedef long long int64;
typedef long long uint64;

typedef short cvalue;

/**
 * Static class for operations
 */
class Operations {
public:
	struct TestModel {
		int card;
		int parameters;
	};

	class TestModelConstraint {
		//NB per constraints medici + è operatore binario
		// sign - can be an operation
		char op; //0 false (-), 1 true (+), 2 (*)
		int value;
		TestModelConstraint(char op, int val)
			:op(op),value(val){}
	public:
		// to build a single asignment (/possibly negated)
		TestModelConstraint(int val, bool pos){
			assert(val >= 0);
			value = val;
			// param = val
			if (pos) op = '+';
			// param != val
			else op = '-';
		}
		// return true if it represents an operation
		bool isOperation(){
			return !(value>=0);
		}
		bool isNegative(){
			assert(value != -1);
			assert(op != '*');
			return op == '-';
		}

		int getValue(){
			assert(value >= 0);
			return value;
		}
		// return the operation
		char getOp(){
			assert(value < 0);
			return op;
		}
		// operation
		static TestModelConstraint makeOperation(char op){
			assert (op == '+' ||op == '-' ||op == '*');
			return TestModelConstraint(op,-1);
		}

		friend ostream& operator<< (ostream &out, TestModelConstraint &tmCon){
			if (tmCon.isOperation())
				out << tmCon.op;
			else
				out << tmCon.op << tmCon.value;
			return out;
		}


	};
	/**
	 * Get cardinality difference of the intersection between two edges
	 *
	 * @param edge 1
	 * @param mdd_edge 2
	 * @return -1 if intersection is empty, otherwise the cardinality difference between the second one and the intersection
	 */
	static double getCardinalityDifference(dd_edge edge, dd_edge mdd_edge);
	/**
	 * Get the cardinality of the intersection
	 * @param edge
	 * @param mdd_edge
	 * @return Cardinality of edge*mdd_edge
	 *
	 */
	static double getIntersectionCardinality(dd_edge edge, dd_edge mdd_edge);

	/**
	 * Given the code (true table row) return an mdd edge
	 * @param tupla
	 * @param mdd
	 * @return mdd edge
	 */
	static dd_edge getMDDFromTuple(vector<cvalue> tupla, forest* mdd);
	/**
	 * Generate bounds in the meddly's format given a testmodel
	 * @param 	Test model
	 * @param 	number of elements
	 * @param 	bounds
	 */
	static void generateBounds(vector<TestModel>, int, int*);
	/**
	 * Print a mdd edge
	 * @param strm out format
	 * @param e edge
	 * @param verbosity 0 print only the fist possible test case, otherwise all possible test cases
	 */
	static void printElements(ostream& strm, dd_edge& e, int verbosity);

	/**
	 * A simple string tokenizer
	 * @param str	string
	 * @return vector of splitted strings
	 */
	static vector<string> tokenize(string str);
	/**
	 * Return the actual time in ms
	 * @return ms time
	 */
	static int64 getTimeMs64();

	/**
	 * Generate testmodel from file, given the file path
	 * @param empty testmodel to compile
	 * @param file name
	 * @return -1 error, 0 ok
	 */
	static int testModelFromFile(vector<TestModel> &, char *, int &nWise);
	/**
	 * Generate constraints model from file, given the file path
	 * @param empty testmodelconstraints to compile
	 * @param file name
	 * @return -1 error, 0 ok
	 */
	static int testModelConstraintsFromFileCASA(
			vector<list<TestModelConstraint> > &, char *); //ogni constraint è definito da una serie di parametri
	/**
	 * Generate constraints model from file, given the file path
	 * @param empty testmodelconstraints to compile
	 * @param file name
	 * @return -1 error, 0 ok
	 */
	static int testModelConstraintsFromFileMEDICI(
			vector<list<TestModelConstraint> > &, char *); //ogni constraint è definito da una serie di parametri

	/**
	 * Return the code (row of true table) given a single parameter and its value
	 * @param v param value
	 * @param bounds array
	 * @param Number of elements/variables
	 * @param param index (returned)
	 * @return vector of code generated
	 */
	static vector<cvalue> getCodeFromParameter(int, int*, int, int &);
	static void getIndexAndValue(int,int*,int,int &,int &);
	/**
	 * Return value in format (paraindex,value) from CASA value
	 * @param v
	 * @param bounds
	 * @param N
	 * @return
	 */


	static vector<cvalue> getValueFromParameter(int v, int* bounds, int N);
	/**
	 * Check if parameters in vector 2 are set in vector1 and if they're the same
	 * @param vector1
	 * @param vector2
	 * @param vector2 param indexes
	 * @param paramZero variant that give more value to the first param
	 * @return 0->incompatible; 1->completely included; 2->partially included (may be compatible or not depending on implicit constraints)
	 */
	static int isIncludedTuple(vector<cvalue>, vector<cvalue>, vector<int>,
			int paramZero = -1);



	/**
	 * Get a true table row from an Mdd, if it has cardinality>1 it returns the first valid test case
	 * @param e mdd edge
	 * @return	code
	 */
	static vector<cvalue> getCodeFromMdd(dd_edge e);

	/**
	 * convert a code row to CASA format, result is stored in the same array
	 * @param vector code
	 * @param bounds
	 * @return 0 ok
	 */
	static int toCASACodeConversion(vector<cvalue> &, int*);

	/**
	 * Given a couple of values in the format [paramindex,value] return the CASA value (no need of couple)
	 * @param couple
	 * @param bounds
	 * @param size	number of levels variables
	 * @return -1 error otherwise value
	 */
	static int toCASAValueConversion(int, int, int*, int);

	static int toACTSModelConversion(vector<TestModel>, vector<list<Operations::TestModelConstraint> >, int*,int,char * , char *);

	Operations();
	virtual ~Operations();

private:

};

#endif /* OPERATIONS_H_ */
