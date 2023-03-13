/*
 * Operations.cpp
 *
 *  Created on: 26/gen/2013
 *      Author: Dario
 */
#include "header.h"
#include "logger.hpp"
#include "ConstraintsParser.h"
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/predicate.hpp>

// Map for the parameters
parameter_bimap valuesMap;

// da FCC >= 4.7 uint non � pi� supportato: https://github.com/CRPropa/CRPropa3/issues/89
#ifndef uint
#define uint unsigned int
#endif

Operations::Operations() {
	// TODO Auto-generated constructor stub

}

Operations::~Operations() {
	// TODO Auto-generated destructor stub
}
// trim from start
static inline std::string& ltrim(std::string &s) {
	s.erase(s.begin(),
			std::find_if(s.begin(), s.end(),
					std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline std::string& rtrim(std::string &s) {
	s.erase(
			std::find_if(s.rbegin(), s.rend(),
					std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
			s.end());
	return s;
}

// trim from both ends
static inline std::string& trim(std::string &s) {
	return ltrim(rtrim(s));
}

/**
 * restituisce un caso di test che soddisfa mdd e
 */
vector<cvalue> Operations::getCodeFromMdd(dd_edge e) {
	int nLevels = e.getForest()->getDomain()->getNumVariables();
	vector<cvalue> code(nLevels, -1); //inizializzo codice con -1
	//ora trovo una soluzione di mdd
	if (e.getCardinality() == 0)
		return code; //errore non ci sono soluzioni
	for (enumerator iter(e); iter; ++iter) {
		const int *minterm = iter.getAssignments();
		//fprintf(strm, "[");
		for (int i = nLevels; i > 0; i--) {
			code[nLevels - i] = minterm[i];
			//	fprintf(strm, " %d", minterm[i]);
		}
		break; //altrimenti genera tutti i casi

	}
	return code;
}
vector<cvalue> Operations::getCodeFromParameter(int v, int *bounds, int N,
		int &in) {
	//conversione da modello CASA a questo

//  cout<<v<<" VALORE DA MODELLO CASA "<<N<<endl;
	vector<cvalue> code(N, -1); //inizializzo codice con -1
	int index = 0;
	// v--;
	for (int i = 0; i < N; i++) {

		if (v < (index + bounds[N - 1 - i])) {
			code[i] = v - index;
			in = i;
			break; //assegnato esco dal ciclo
		}
		index += bounds[N - 1 - i];
	}
	return code;
}

void Operations::getIndexAndValue(int casav, int *bounds, int N, int &index,
		int &value) {
	//conversione da modello CASA a questo

	//  cout<<v<<" VALORE DA MODELLO CASA "<<N<<endl;

	int in = 0;
	// v--;
	for (int i = 0; i < N; i++) {

		if (casav < (in + bounds[N - 1 - i])) {
			//code[i] = v - index;
			index = i;
			value = casav - in;
			break; //assegnato esco dal ciclo
		}
		in += bounds[N - 1 - i];
	}

	return;
}
vector<cvalue> Operations::getValueFromParameter(int v, int *bounds, int N) {
	//conversione da modello CASA a questo

//  cout<<v<<" VALORE DA MODELLO CASA "<<N<<endl;
	vector<cvalue> code(2, -1); //inizializzo codice con -1
	int index = 0;
	// v--;
	for (int i = 0; i < N; i++) {

		if (v < (index + bounds[N - 1 - i])) {
			code[1] = v - index;
			code[0] = i;
			break; //assegnato esco dal ciclo
		}
		index += bounds[N - 1 - i];
	}
	return code;
}
int Operations::testModelConstraintsFromFileCASA(
		vector<list<Operations::TestModelConstraint> > &testModel,
		char *filename) {
	ifstream myReadFile;
	string strRow;
	uint N = 0;
	uint nParam = 0;
	/* apre il file */
	myReadFile.open(filename);
	if (!myReadFile.is_open()) {
		// if( fd==NULL ) {
		perror("Errore in apertura del file");
		return -1; //errore
	}

	getline(myReadFile, strRow); // Prima riga indica numero di constraints
	N = atoi(strRow.c_str()); //numero contraints
	//sfoglio righe
	for (uint i = 0; i < N; i++) {
		getline(myReadFile, strRow);
		nParam = atoi(strRow.c_str()); //numero parametri in formula
		getline(myReadFile, strRow); //passo a seconda riga con definizione parametri
		vector<string> tokens = Operations::tokenize(strRow);
		if (tokens.size() != nParam * 2)
			return -1; //devono essere definite tutte
		list<Operations::TestModelConstraint> pTList;
		for (uint k = 0; k < tokens.size(); k++) {
			bool sign;
			if (strcmp(tokens[k].c_str(), "-") == 0) {
				sign = false;
			} else {
				sign = true;
			}
			k++;
			int value = atoi(tokens[k].c_str());
			Operations::TestModelConstraint pT(value, sign);
			pTList.push_front(pT);
		}
		//ho terminato la riga e quindi ho definito ci
		testModel.push_back(pTList);

	}

	myReadFile.close();
	return 0;
}

int Operations::testModelFromFileCTWedge(
		vector<Operations::TestModel> &testModel,
		vector<list<Operations::TestModelConstraint> > &testModelConstraints, char *filename) {
	ifstream myReadFile;
	string strRow;
	bool inParameters = false;
	bool inConstraints = false;
	// Parameter counter
	int counter = 0;

	// Open the CTWedge file
	myReadFile.open(filename);
	if (!myReadFile.is_open()) {
		perror("Errore in apertura del file");
		return -1; //errore
	}

	while (!myReadFile.eof()) {
		getline(myReadFile, strRow);
		// It's a comment
		if (strRow.rfind("//", 0) == 0)
			continue;
		if (strRow.rfind("Parameters", 0) == 0) {
			inParameters = true;
			continue;
		}
		if (strRow.rfind("Constraints", 0) == 0) {
			inParameters = false;
			inConstraints = true;
			continue;
		}
		if (inParameters) {
			// Remove all the white spaces
			strRow.erase(std::remove_if(strRow.begin(), strRow.end(), ::isspace),
					strRow.end());

			// Empty row
			if (strRow == "")
				continue;

			// It's a parameter. We need to tokenize the string
			boost::char_separator<char> sep(":");
			boost::tokenizer<boost::char_separator<char>> tokens(strRow, sep);
			string parameterName = "";
			string parameterDef = "";
			for (const auto& t : tokens) {
				if (parameterName == "")
					parameterName = t;
				else
					parameterDef = t;
			}
			Operations::TestModel pT;
			pT.parameters = 1;
			pT.name = parameterName;
			if (parameterDef == "Boolean" || parameterDef == "Boolean;") {
				// Boolean parameter
				pT.values = vector<string> {"false", "true"};
				valuesMap.insert({make_pair(parameterName, "false"), counter});
				counter++;
				valuesMap.insert({make_pair(parameterName, "true"), counter});
				counter++;
			} else {
				if (parameterDef.rfind("{", 0) == 0) {
					// Enumerative -> Remove possible ; at the end
					if (boost::algorithm::ends_with(parameterDef, ";"))
						parameterDef = parameterDef.substr(0, parameterDef.size() - 1);

					if(parameterDef.rfind(",", 0) > 0) {
						pT.values = vector<string>();
						parameterDef = parameterDef.substr(1, parameterDef.size() - 2);
						boost::char_separator<char> sep(",");
						boost::tokenizer<boost::char_separator<char>> tokens(parameterDef, sep);
						for (const auto& t : tokens) {
							valuesMap.insert({make_pair(parameterName, t), counter});
							counter++;
							pT.values.push_back(t);
						}
					} else if (parameterDef.rfind(" ", 0)) {
						pT.values = vector<string>();
						parameterDef = parameterDef.substr(1, parameterDef.size() - 2);
						boost::char_separator<char> sep(" ");
						boost::tokenizer<boost::char_separator<char>> tokens(parameterDef, sep);
						for (const auto& t : tokens) {
							valuesMap.insert({make_pair(parameterName, t), counter});
							counter++;
							pT.values.push_back(t);
						}
					}
				} else {
					cout << parameterDef << endl;
					perror("Unrecognized parameter type");
					return -1; //errore
				}
			}
			pT.card = pT.values.size();
			testModel.push_back(pT);
			continue;
		}
		if (inConstraints) {
			strRow = trim(strRow);

			// The constraint shall begin with #
			if (strRow.rfind("#", 0) != 0)
				continue;
			// Remove head and tail characters
			strRow = strRow.substr(1, strRow.size() - 2);
			strRow = trim(strRow) + ";";

			// Parse the row with a constraint parser
			parser<decltype(std::begin(strRow))> p;
			try {
				expr result;
				bool ok = qi::phrase_parse(std::begin(strRow), std::end(strRow), p > ';', qi::space, result);

				if (!ok) {
					perror("Error in parsing constraint");
					return -1; //errore
				} else {
					// Use the parsed string
					vector<string> tokens = Operations::tokenize(boost::apply_visitor(printer(), result));
					list<Operations::TestModelConstraint> pTList;
					for (uint k = 0; k < tokens.size(); k++) {
						if (strcmp(tokens[k].c_str(), "-") == 0) {
							// A value followed by a - add a negative
							pTList.push_front(
									Operations::TestModelConstraint::makeOperation('-'));
						} else if (strcmp(tokens[k].c_str(), "+") == 0) {
							pTList.push_front(
									Operations::TestModelConstraint::makeOperation('+'));
						} else if (strcmp(tokens[k].c_str(), "*") == 0) {
							pTList.push_front(
									Operations::TestModelConstraint::makeOperation('*'));
						} else {
							// It's a number
							int value = atoi(tokens[k].c_str());
							pTList.push_front(Operations::TestModelConstraint(value, true));
						}
					}
					// The constraint has been fully managed
					testModelConstraints.push_back(pTList);
				}
			} catch (const qi::expectation_failure<std::string> &e) {
				perror("Error in parsing constraint");
				return -1; //errore
			}
		}
	}
	return 0;
}

int Operations::testModelConstraintsFromFileMEDICI(
		vector<list<Operations::TestModelConstraint> > &testModel,
		char *filename) {
	ifstream myReadFile;
	string strRow;
	uint N = 0;
	/* apre il file */
	myReadFile.open(filename);
	if (!myReadFile.is_open()) {
		// if( fd==NULL ) {
		perror("Errore in apertura del file");
		return -1; //errore
	}
	//SALTO TRE RIGHE DI DEFINIZIONE MODELLO
	getline(myReadFile, strRow);
	getline(myReadFile, strRow);
	getline(myReadFile, strRow);
	//INIZIO LETTURA CONSTRAINTS
	getline(myReadFile, strRow); // Prima riga indica numero di constraints
	N = atoi(strRow.c_str()); //numero contraints
	//sfoglio righe
	for (uint i = 0; i < N; i++) {
		getline(myReadFile, strRow);
		vector<string> tokens = Operations::tokenize(strRow);
		list<Operations::TestModelConstraint> pTList;
		for (uint k = 0; k < tokens.size(); k++) {
			if (strcmp(tokens[k].c_str(), "-") == 0) {
				// a value followed by a - add a negative
				pTList.push_front(
						Operations::TestModelConstraint::makeOperation('-'));
			} else if (strcmp(tokens[k].c_str(), "+") == 0) {
				pTList.push_front(
						Operations::TestModelConstraint::makeOperation('+'));
			} else if (strcmp(tokens[k].c_str(), "*") == 0) {
				pTList.push_front(
						Operations::TestModelConstraint::makeOperation('*'));
			} else {
				//� un numero
				int value = atoi(tokens[k].c_str());
				pTList.push_front(Operations::TestModelConstraint(value, true));
			}
		}
#ifdef DEBUG
		cout << "added constraints ";
		for (list<Operations::TestModelConstraint>::iterator pTListit = pTList.begin(); pTListit != pTList.end(); pTListit++)
			cout << " " << *pTListit;
		cout << endl;
#endif
		//ho terminato la riga e quindi ho definito ci
		testModel.push_back(pTList);

	}

	myReadFile.close();
	return 0;
}

int Operations::testModelFromFile(vector<Operations::TestModel> &testModel,
		char *filename, int &nWise) {
	ifstream myReadFile;
	string strRow;
	uint N = 0;
	/* apre il file */
	myReadFile.open(filename);
	if (!myReadFile.is_open()) {
		// if( fd==NULL ) {
		perror("Errore in apertura del file");
		return -1; //errore
	}

	getline(myReadFile, strRow); // Prima riga indica nwise
	nWise = atoi(strRow.c_str()); //nWise
	getline(myReadFile, strRow);
	N = atoi(strRow.c_str()); //numero variabili, livelli
	//	cout<<N<<endl;
	getline(myReadFile, strRow); //definizione variabili
	vector<string> tokens = Operations::tokenize(strRow);
	//	cout<<tokens.size()<<endl;
	if (tokens.size() != N)
		return -1; //devono essere definite tutte
	Operations::TestModel pT;

	for (uint k = 0; k < tokens.size(); k++) {
		pT.card = atoi(tokens[k].c_str());
		pT.parameters = 1;
		testModel.push_back(pT);

	}

	myReadFile.close();
	return 0;
}

void Operations::generateBounds(vector<TestModel> model, int nel,
		int bounds[]) {
	//int bounds[nel]; //dimensione da definire
	int bIndex = 0;
	// for (std::list<int>::reverse_iterator i = s.rbegin(); i != s.rend(); ++i)
	for (std::vector<TestModel>::reverse_iterator it = model.rbegin(), end =
			model.rend(); it != end; ++it) {
		//*it[1] = n parametri
		//*it[0] = cardinality
		//	cout<<"Filling bounds"<<endl;
		//	cout<<it->parameters<<" "<<it->card<<endl;
		//for (int i=it->parameters-1;i>=0;i--)
		for (int i = 0; i < it->parameters; i++) {
			bounds[bIndex] = it->card;
			bIndex++;
			//	cout<<it->card<<endl;
		}

	}

	//return bounds;
}
/**
 * Controlla compatibilit� tra due nodi attraverso il calcolo della differenza di cardinalit� per l'interesezione
 * return:
 * -1 incompatibili
 *  altrimenti differenza cardinalit� tra nodo2 e intersezione, 0 significa che nodo2 include nodo1 senza modifiche
 */

double Operations::getCardinalityDifference(dd_edge edge, dd_edge mdd_edge) {
	// dd_edge tempElement(edge.getForest());

	//double tCard=(edge*mdd_edge).getCardinality();
	//edge.getForest()->removeAllComputeTableEntries();
	//if (tCard==0) return -1;
	//return tCard;

	dd_edge tempElement = edge * mdd_edge;
	// apply(INTERSECTION, edge,  mdd_edge, tempElement);
	//double tCard = tempElement.getCardinality();
	double tCard = tempElement.getCardinality();
//  tempElement.clear();
//  int64 partial2=Operations::GetTimeMs64();

//	tempElement.getForest()->removeAllComputeTableEntries();
	// tempElement.getForest()->removeStaleComputeTableEntries();
//  partial+=Operations::GetTimeMs64()-partial2;
//  edge.getForest()->garbageCollect(); //pulisce memoria
	if (tCard == 0) {

		return -1;
	}
//  return tCard;
	return (mdd_edge.getCardinality() - tCard); //per definizione >=0

}
double Operations::getIntersectionCardinality(dd_edge edge, dd_edge mdd_edge) {
	// dd_edge tempElement(edge.getForest());
	dd_edge tempElement = edge * mdd_edge;
	// apply(INTERSECTION, edge,  mdd_edge, tempElement);
	double tCard = tempElement.getCardinality();
//  double tCard =tempElement.getCardinality();
//  tempElement.clear();
	// tempElement.getForest()->removeStaleComputeTableEntries();
	tempElement.getForest()->removeAllComputeTableEntries();
//  edge.getForest()->garbageCollect(); //pulisce memoria
	/* if (tCard==0)
	 {

	 return -1;
	 }*/
	return tCard;
	// return (mdd_edge.getCardinality()-tCard); //per definizione >=0

}
// break a string into a vector tokens
vector<string> Operations::tokenize(string str) {
	vector<string> result;
	str = trim(str);
	string::const_iterator it = str.begin();

	while (it != str.end()) {
		while ((*it == ' ') && (it != str.end()))
			it++; // skips duplicated spaces
		string::const_iterator it2 = it;
		while ((*it2 != ' ') && (it2 != str.end()))
			it2++; // move to the position of the following space
		string tmp(it, it2); // copy range [it,it2) to tmp string

		//  cout<<"pasing: "<<tmp<<" "<<strlen(tmp.c_str())<<endl;
		// if (strlen(tmp.c_str())>0){
		result.push_back(tmp);
		//	cout<<"LETTO "<<tmp<<" "<<strlen(tmp.c_str())<<endl;
		//  }
		it = it2;
	}
	return result;
}

dd_edge Operations::getMDDFromTuple(vector<cvalue> tupla, forest *mdd) {
	const uint N = tupla.size(); //n variabili o livelli

	// Create an element to insert in the MDD
	// Note that this is of size (N + 1), since [0] is a special level handle
	// dedicated to terminal nodes.
	int *elementList[1]; //matrice pairwise

	elementList[0] = new int[N + 1];

#ifdef DEBUG
	printf("mdd per terna: [%d %d %d]\n",tupla[0],tupla[1],tupla[2]);
#endif
	int i = 1;
//	for (std::list<Tuple>::iterator it = listT.tList.begin(), end = listT.tList.end(); it != end; ++it)
	//	{
	for (std::vector<cvalue>::reverse_iterator it = tupla.rbegin(), end =
			tupla.rend(); it != end; ++it) {

		elementList[0][i++] = *it;	//contrario da C ad A
	}
	//creo nodo per elemento da coprire
	dd_edge element(mdd);
	mdd->createEdge(elementList, 1, element);
	delete elementList[0]; //libero memoria creata con new
	//Operations::printElements(stdout,element);
	return element;
}

void Operations::printElements(std::ostream &strm, dd_edge &e,
		int verbosity = 0) {
	int nLevels = ((e.getForest())->getDomain())->getNumVariables();
	for (enumerator iter(e); iter; ++iter) {
		const int *minterm = iter.getAssignments();
		strm << "[";
		for (int i = nLevels; i > 0; i--) {
			strm << " " << minterm[i];
		}
		switch ((e.getForest())->getRangeType()) {
		case forest::BOOLEAN:
			strm << " --> T]\n";
			break;
		case forest::INTEGER: {
			int val = 0;
			iter.getValue(val);
			strm << " --> " << val << "]\n";
		}
			break;
		case forest::REAL: {
			int val = 0;
			iter.getValue(val);
			strm << " --> " << val << "]\n"; //&&%0.3f
		}
			break;
		default:
			strm << "Error: invalid range_type\n";
		}
		if (verbosity == 0)
			break; //ne basta una per mdd
	}
}

int64 Operations::getTimeMs64() {

#ifdef WIN32
	/* Windows */
	FILETIME ft;
	LARGE_INTEGER li;

	/* Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it
	 * to a LARGE_INTEGER structure. */
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	uint64 ret = li.QuadPart;
	ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */
	ret /= 10000; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */

	return ret;
#else
	/* Linux */
	struct timeval tv;

	gettimeofday(&tv, NULL);

	uint64 ret = tv.tv_usec;
	/* Convert from micro seconds (10^-6) to milliseconds (10^-3) */
	ret /= 1000;

	/* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
	ret += (tv.tv_sec * 1000);

	return ret;
#endif
}

int Operations::isIncludedTuple(vector<cvalue> vCode, vector<cvalue> vCode2,
		vector<int> vParams, int paramZero) {
	int ret1 = 0;
	int ret2 = 0;
	bool paramZeroIncluded = false;
	if (paramZero == -1)
		paramZeroIncluded = true; //Check not needed
	for (std::vector<int>::iterator it = vParams.begin(), end = vParams.end();
			it != end; ++it) {
		if ((*it) == paramZero)
			paramZeroIncluded = true;
		if (vCode[*it] == -1) //caso parametro non ancora impostato, tupla potrebbe essere parzialmente compatibile
				{
			ret1 = 1;
		} else if (vCode[*it] == vCode2[*it]) //caso con parametro fissato ed uguale, tupla potrebbe essere completamente compatibile
				{
			ret2 = 1;
		} else if (vCode[*it] != vCode2[*it]) //tupla sicuramente non compatibile, non serve provare altri parametri
			return 0;
	}
	if (!paramZeroIncluded)
		return 0;
	if (ret1)
		return 2;
	if (ret2)
		return 1;

	return -1;
}

int Operations::toCASACodeConversion(vector<cvalue> &code, int *bounds) {
	uint size = code.size();
	int parB = 0;
	for (uint i = 0; i < size; i++) {
		code[i] = parB + code[i];
		parB += bounds[size - i - 1];
	}

	return 0;
}

int Operations::toCASAValueConversion(int index, int value, int *bounds,
		int size) {

	int parB = 0;
//	int parInd=index;

//	cout<<"CONVERSIONE: "<<index<<"|"<<value<<"|"<<size<<"-->";

	for (int i = 0; i < index; i++) {
		//code[i]=parB+code[i];
		//	cout<<"bound: "<<bounds[size-i-1]<<" "<<size-i-1<<endl;
		parB += bounds[size - i - 1];
	}
	//sono arrivato a index che mi serve
//	cout<<parB+value<<endl;
	return parB + value;
	//return -1;
}

int Operations::toACTSModelConversion(vector<Operations::TestModel> testModel,
		vector<list<Operations::TestModelConstraint> > testConstraints,
		int *bounds, int nel, char *filename, char *modelname) {
	ofstream fout;

	/* apre il file */
	fout.open(filename);
	if (!fout.is_open()) {
		// if( fd==NULL ) {
		perror("Errore in apertura del file");
		return -1; //errore
	}

	fout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	fout << "<System name=\"" << modelname << "\">" << endl;
	fout << " <Parameters>" << endl;
	int i = 0;
	for (std::vector<Operations::TestModel>::iterator it = testModel.begin(),
			end = testModel.end(); it != end; ++it) {
		fout << "  " << "<Parameter id=\"" << i << "\" name=\"p" << i
				<< "\" type=\"0\">" << endl;
		fout << "   <values>" << endl;
		for (int k = 0; k < it->card; k++) {
			fout << "    <value>" << k << "</value>" << endl;
		}
		fout << "   </values>" << endl;
		fout << "   <basechoices>" << endl;
		fout << "   <basechoice />" << endl;
		fout << "   </basechoices>" << endl;
		fout << " </Parameter>" << endl;

		i++;
	}
	fout << " </Parameters>" << endl;
	fout << "<OutputParameters />" << endl;
	fout << "<Relations />" << endl;
	fout << " <Constraints>" << endl;
	for (std::vector<list<Operations::TestModelConstraint> >::iterator itc =
			testConstraints.begin(), end = testConstraints.end(); itc != end;
			++itc) {
		//vector di constraints uniti con and
		fout << "<Constraint text=\"";
		int kz = 0;
		for (std::list<Operations::TestModelConstraint>::iterator itcs =
				itc->begin(), end = itc->end(); itcs != end; ++itcs) {
			//singolo valore della riga
			int index = -1;
			int value = -1;
			Operations::getIndexAndValue(itcs->getValue(), bounds, nel, index,
					value);
			if (kz > 0)
				fout << "||";
			fout << "(p" << index;
			if (itcs->isNegative())
				fout << "!";
			fout << "=" << value << ")";

			kz++;
		}
		fout << "\">" << endl;
		fout << "<Parameters>" << endl;
		for (std::list<Operations::TestModelConstraint>::iterator itcs =
				itc->begin(), end = itc->end(); itcs != end; ++itcs) {
			//singolo valore della riga
			int index = -1;
			int value = -1;
			Operations::getIndexAndValue(itcs->getValue(), bounds, nel, index,
					value);
			fout << "<Parameter name=\"p" << index << "\" />" << endl;

		}

		fout << "</Parameters>" << endl;
		fout << "</Constraint>" << endl;

	}
	fout << " </Constraints>" << endl;
	fout << "</System>" << endl;

	fout.close();
	return 0;
}
