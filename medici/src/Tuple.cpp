/*
 * Tuple.cpp
 *
 *  Created on: 27/gen/2013
 *      Author: Dario
 */

#include "Tuple.h"
#include <sstream>
Tuple::Tuple(vector<cvalue> code, vector<int> param, vector<int> paramC, uint compClass) {
	this->code = code;
	this->param = param;
	this->paramCASA = paramC;
	this->compClass=compClass;
	status = 0;

	coverable = true;

	weight = 0;
	weightMax = 0;
	compCheck = false;
}
Tuple::Tuple(vector<cvalue> code, vector<int> param) {
	this->code = code;
	this->param = param;
	status = 0;

	coverable = true;

	weight = 0;
	weightMax = 0;
	compClass = 0;
	compCheck = false;

}
Tuple::Tuple(int nParams) {
	vector<cvalue> code(nParams, -1);
	vector<int> param(2, 0);
	this->code = code;
	this->param = param;
	status = 0;

	coverable = true;

	weight = 0;
	weightMax = 0;
	compClass = 0;
	compCheck = false;
}

Tuple::~Tuple() {
	// TODO Auto-generated destructor stub
}

/**
 * toString of tuple in format [ParamIndex-ParamValue,....]
 * @return
 */
string Tuple::getValue() {
	ostringstream streamString;
	int index = 0;
	for (std::vector<cvalue>::iterator it = code.begin(), end = code.end();
			it != end; ++it) {
		if (*it != -1)
			streamString << index << "-" << *it << ",";
		index++;
		//ret+=*it+",";
	}
	return streamString.str();
}
void Tuple::print(ostream& ss) {
	ss << "valori:[";
	for (uint i = 0; i < code.size(); i++) {
	//	if (code[i] != -1)
			ss << code[i] << ",";
	}
	ss << "] ";
	ss << this;
	/*cout<<" compatibilita: [";
	 for (std::list<Tuple::compType>::iterator it2= compatible.begin(), end = compatible.end(); it2 != end; ++it2)
	 {
	 // cout<<it2->otherIndex->getValue()<<"*|"<<"it2->cardDiff"<<"|"<<it2->relComp<<",";
	 cout<<"["<<it2->otherIndex->getValue()<<"]|"<<it2->otherIndex->compIndex<<",";
	 //  cout<<it2->otherIndex<<" ";
	 }*/

	ss << "] stato: " << status << " weight: " << weight << " compClass: "
			<< compClass << " parametri: ";
	for (std::vector<int>::iterator it = param.begin(), end = param.end();
			it != end; ++it) {
		ss << *it << ",";
	}
	ss << " ParametriCasa: ";
	for (std::vector<int>::iterator it = paramCASA.begin(), end =
			paramCASA.end(); it != end; ++it) {
		ss << *it << ",";
	}
	ss << endl;
}

Tuple Tuple::mergeTuple(Tuple t1, Tuple t2) {
	//compatibili per definizione, non faccio check
	for (std::vector<int>::iterator it = t2.param.begin(), end = t2.param.end();
			it != end; ++it) {
		t1.code[*it] = t2.code[*it];
	}
	return t1;
}

int Tuple::isIncludedTuple(Tuple *t1, Tuple *t2) {

	if (t1->compClass==t2->compClass) return 0; //stessa classe quindi incompatibili
	int nP=t1->param.size();
	int pC = 0;
	vector<int> res;
//	std::sort(it->paramCASA.begin(), it->paramCASA.end());
//	std::sort(it2->paramCASA.begin(), it2->paramCASA.end());
	set_intersection(t1->param.begin(), t2->param.end(),
			t2->param.begin(), t2->param.end(),
			std::back_inserter(res));
	pC = res.size();
	if (pC==0) return 1; //sic compatibili
	res.clear();
	set_intersection(t1->paramCASA.begin(), t2->paramCASA.end(),
				t2->paramCASA.begin(), t2->paramCASA.end(),
				std::back_inserter(res));
	//per essere compatibili devono avere un numero di valori uguali pari alla size dei
	//parametri uguali
	if (pC==res.size()) return 2;
	//altrimenti incompatibili
	return 0;

}
