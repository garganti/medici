/*
 * TupleList.cpp
 *
 *  Created on: 27/gen/2013
 *      Author: Dario
 */
#include "header.h"
#include "math.h"

#include "TupleList.h"
#include <sstream>
#include "logger.hpp"

// a predicate implemented as a function:
bool checkCovered(const Tuple::compType& value) {
	return (value.otherIndex->status);
}
bool checkCoveredL(const Tuple& value) {
	return (value.status);
}
bool checkUncoverableL(const Tuple& value) {
	return (!value.coverable);
}
bool orderTList(Tuple i, Tuple j) {
	return (i.weight > j.weight);
}
bool orderTListComp(Tuple i, Tuple j) {
	return (i.weight > j.weight);
}
bool orderTListWMax(Tuple i, Tuple j) {
	return (i.weightMax > j.weightMax);
}
bool orderIndexList(vector<int> i, vector<int> j) {
	return (i[1] > j[1]);
}

TupleList::TupleList(Settings s) {
	// TODO Auto-generated constructor stub
	nToCover = 0;
	nCovered = 0;
	nUncoverable = 0;
	nTotal = 0;
	nValues = 0;
	setting = s;
//	weightMax = 0;

}

TupleList::~TupleList() {
	// TODO Auto-generated destructor stub
}

void TupleList::setNValues(int* bounds, int nel) {
	for (int i = 0; i < nel; i++) {
		nValues += bounds[i];
	}
}
int TupleList::checkTuples(TupleList list) {

	uint nWise = list.tList.back().param.size();
	double i = 0;
	for (std::list<Tuple>::iterator it = list.tList.begin(), end =
			list.tList.end(); it != end; ++it) {
		i = 0;
		//controllo crosscorrelazione con successive
		for (std::list<Tuple>::iterator it2 = (it), end = list.tList.end();
				it2 != end; ++it2) {
			i++;
			if (i == 1)
				continue;
			vector<int> res;
			std::sort(it->paramCASA.begin(), it->paramCASA.end());
			std::sort(it2->paramCASA.begin(), it2->paramCASA.end());
			set_intersection(it->paramCASA.begin(), it->paramCASA.end(),
					it2->paramCASA.begin(), it2->paramCASA.end(),
					std::back_inserter(res));
			if (res.size() == nWise) //se sono identiche l'intersezione è uguale all'inizio
					{
				it->print(logcout(LOG_INFO));
				it2->print(logcout(LOG_INFO));
				return -1;
			} //Errore
		}
	}
	return 1;
}

TupleList TupleList::generateNWise(int* bounds, int nel, int nWise, Settings s) {
	TupleList list(s);
	int n, r, k;

	//vector<int> p(3, -1);
	vector<int> pc(nWise, -1);
	n = nel;
	r = nWise;
	const uint N = nWise; //n variabili o livelli, 3-wise
	int subBounds[N]; //definisco bounds

	//genera combinazioni 3 elementi
	std::vector<bool> v(n);
	std::fill(v.begin() + r, v.end(), true);
	vector<int> t(N);
	k = 0;
	int iClass=1;
	do {
		k = 0;
		for (int i = 0; i < n; ++i) {
			if (!v[i]) {
				t[k] = i;
				//			std::cout << t[k]<< " ";
				k++;
			}
		}
		//	std::cout << "\n";
		//qui ho la mia terna di indici di parametri da far permutare
		//assegno i bounds (3 nel caso 3-wise), vanno riempiti al contrario
		//	cout<<"Setting bounds "<<endl;
		for (int i = N - 1; i >= 0; i--) {

			//cout<<nel-t[i]-1<<"-"<<bounds[nel-t[i]-1]<<"-"<<t[i]<<"-"<<i<<endl;
			subBounds[N - 1 - i] = bounds[nel - t[i] - 1];
		}
		//creazione mdd meddly

		// Create a domain
		domain* d = createDomain();
		assert(d != 0);

		// Create variable in the above domain
		d->createVariablesBottomUp(subBounds, N);
		forest* mdd = d->createForest(false,           // this is not a relation
				forest::BOOLEAN,          // terminals are either true or false
				forest::MULTI_TERMINAL    // disables edge-labeling
				);
		assert(mdd != 0);
		dd_edge all(mdd);
		mdd->createEdge(true, all);
		for (enumerator iter(all); iter; ++iter) {
			const int* minterm = iter.getAssignments();
			vector<cvalue> El(nel, -1);
			for (int i = N; i > 0; i--) {
				//	fprintf(stdout, " %d", minterm[i]);
				El[t[N - i]] = minterm[i];
				//	cout<<"setting elment: "<<t[N-i]<<endl;
				pc[N - i] = Operations::toCASAValueConversion(t[N - i],
						minterm[i], bounds, nel);
			}

			//		cout<<endl;

			list.tList.push_back(Tuple(El, t, pc, iClass));

		}
		//calcolo combinazione tramite stati raggiungibili meddly
		//  exit(0);
		destroyDomain(d);
		iClass++;
	} while (std::next_permutation(v.begin(), v.end()));
	//exit(0);

	logcout(LOG_INFO) << "Generated tuples for " << nWise << "-wise with cardinality " << list.size() << endl;
	return list;

}

void TupleList::setCovered(Tuple &t) {
	t.status = 1;
	//proviamo a rimuoverla
	//tList.remove(t); //non va con iterator
	//TODO debug
	//cout<<"setting covered: ";
	//t.print();
//	cout<<"tupla: "<<t.getValue()<<" coperta"<<endl;
	/*cout<<"**Cercando TUPLA "<<&t<<endl;
	 for (std::list<Tuple>::iterator it = tList.begin(), end = tList.end(); it != end; ++it)
	 {
	 //nodo attuale
	 if (it->status>=0) //Se è già coperta non serve  //TODO invece se ci sono ancora tuple copribili va considerato
	 {
	 //sfoglio controllo ed eventualmente elimino
	 //it->compatible.remove(&it);
	 //		cout<<"cercando tupla "<<&(*it)<<endl;
	 std::list<Tuple::compType>::iterator it2 = it->compatible.begin();
	 while (it2 != it->compatible.end()) {
	 if (1) {//(it2->otherIndex==&t) {
	 cout<<"TROVATO TUPLA "<<&t;
	 t.print();
	 std::list<Tuple::compType>::iterator toErase = it2;
	 ++it2;
	 cout<<"CANCELLANDO DA  "<<&(*it)<<" ";
	 it->print();//<<it->code[0]<<it->code[1]<<it->code[2]<<endl;
	 it->compatible.erase(toErase);
	 //it->compatible.erase(it2++);
	 } else {
	 ++it2;
	 }
	 }
	 for (std::list<Tuple::compType>::iterator it2= it->compatible.begin(), end = it->compatible.end(); it2 != end; ++it2)
	 {
	 Tuple* tuplaC  =it2->otherIndex;
	 //Tuple* tuplaB = &(*it);
	 //	cout<<"cercando tupla "<<tuplaC<<" "<<tuplaB<<endl;
	 if (tuplaC==&t)
	 {
	 //	it->compatible.remove((*it2));
	 cout<<"TROVATO TUPLA"<<endl;
	 it->compatible.erase(it2);
	 //	it->compatible.remove()
	 }
	 }
	 //	Tuple* tuplaC  =it2->otherIndex;
	 }
	 }*/

	nToCover--;
	nCovered++;
}

void TupleList::setCovered(vector<Tuple> list) {
	for (std::vector<Tuple>::iterator it = list.begin(), end = list.end();
			it != end; ++it) {
		setCovered(*it);
	}
}

void TupleList::print(ostream& ss) {
	//Sfoglio lista tuple per stamparla
	for (std::list<Tuple>::iterator it = tList.begin(), end = tList.end();
			it != end; ++it) {
		//	cout<<"Stampa tupla indice: "<<endl; //<<indexList[i]<<endl;
		it->print(ss);
	}
	return;

}

void TupleList::checkParamList(forest* mdd) {
	//Aggiungo peso a tuple
	bool debug = false;
	if (tList.size() < 0)
		debug = true;
	int nParams = mdd->getDomain()->getNumVariables();
	indexList.clear();
	//vector<int[2]> newIndexList(nParams);// = new vector<int>(0,nParams);

	for (int paramI = 0; paramI < nParams; paramI++) {
		vector<int> param(2, 0);

		param[1] = 0;
		param[0] = paramI;

		indexList.push_back(param);
	}

//	for( int paramI=0;paramI<nParams;paramI++)
//	{
	for (std::list<Tuple>::iterator it = tList.begin(), end = tList.end();
			it != end; ++it) {
		for (std::vector<int>::iterator it2 = it->param.begin(), end =
				it->param.end(); it2 != end; ++it2) {

			if (debug) {
				cout << "CHECKING " << it->getValue() << endl;
			}
			indexList[*it2][1]++; //incrementa contatore per il parametro

		}

		//		}
	}
	//fino a qui è ordinato per index parametri
	weightMax = 0;
	for (std::list<Tuple>::iterator it = tList.begin(), end = tList.end();
			it != end; ++it) {
		//assegno peso a tuple
		it->weight = 0;
		it->weightMax = 0; //peso del parametro più pesante
		for (std::vector<int>::iterator it2 = it->param.begin(), end =
				it->param.end(); it2 != end; ++it2) {
			it->weight += indexList[*it2][1];//+((double)it->code[*it2])/100;
			//TODO vedere se mantenere modifica

			if (indexList[*it2][1] > it->weightMax) {
				it->weightMax = indexList[*it2][1]; //NB in [][0] c'è l'indice del parametro che prima del riordino è guauale a quello del vettore
			}
		}
		if (it->weight > weightMax)
			weightMax = it->weight;

	}

	if (setting.wexp!=1){
		for (std::list<Tuple>::iterator it = tList.begin(), end = tList.end();
					it != end; ++it) {
			it->weight=pow(it->weight,setting.wexp);
		}
		}

	//Ordino
	sort(indexList.begin(), indexList.end(), orderIndexList);
#ifdef DEBUG
	//indexList=newIndexList;
	cout<<"massimo is "<<indexList[0][1]<<endl;
	cout<<"minimo is "<<indexList.at(indexList.size()-1)[1]<<endl;
#endif

//	cout << "Max val is " << *max_element(indexList.begin(),indexList.end()) << endl;

}

void TupleList::removeCovered() {
	tList.remove_if(checkCoveredL);
}
void TupleList::removeUncoverable() {
	tList.remove_if(checkUncoverableL);
}
int TupleList::size() {
	return this->tList.size();
}

void TupleList::sortCompatibility(int mode) {
	//cout<<"ordino lista "<<tList.size()<<endl;
	if (mode == 0) {
		tList.sort(orderTList);
	}
	if (mode == 4) {
		int size=tList.size(); //TODO debug levare
		//provo a mettere solo in cima il più pesante
		list<Tuple>::iterator first = tList.begin();
		//	cout<<"CERCANDO WEIGHTMAX "<<weightMax<<endl;
		for (std::list<Tuple>::iterator it = tList.begin(), end = tList.end();
				it != end; ++it) {
			//	cout<<"ACTUAL WEIGHT "<<it->weight<<endl;
			if (it->weight == weightMax) {
				//trovata tupla da spostare in cima
				//	cout<<"TROVATA"<<endl;
				Tuple temp = *it;
				*it = *first;
				*first = temp;
				break;
			}
		}

		//tList.sort(orderTList);
	}
	if (mode == 1)
		tList.sort(orderTListWMax);
//	if (mode == 2)
//		tListTC.sort(orderTList);
	if (mode == 3)
		tList.sort(orderTListComp);

	return;

}

void TupleList::checkParamListCASA(forest* mdd) {
	//Aggiungo peso a tuple

	bool debug = false;

//	int nParams = mdd->getDomain()->getNumVariables();
	indexListCASA.clear();
	//vector<int[2]> newIndexList(nParams);// = new vector<int>(0,nParams);
	//TODO cambiare il numero di parametri con quello casa totale (somma di tutti i valori nel vettore bounds)
	for (int paramI = 0; paramI < nValues; paramI++) {
		vector<int> param(2, 0);

		param[1] = 0;
		param[0] = paramI;

		indexListCASA.push_back(param);
	}
	logcout(LOG_DEBUG) << indexListCASA.size() << endl;

//	for( int paramI=0;paramI<nParams;paramI++)
//	{
	for (std::list<Tuple>::iterator it = tList.begin(), end = tList.end();
			it != end; ++it) {

		for (std::vector<int>::iterator it2 = it->paramCASA.begin(), end =
				it->paramCASA.end(); it2 != end; ++it2) {

			if (debug) {
				logcout(LOG_DEBUG) << "CHECKING " << it->getValue() << endl;
			}

			indexListCASA[*it2][1]++; //incrementa contatore per il parametro

		}

		//		}
	}
	//fino a qui è ordinato per index parametri
	for (std::list<Tuple>::iterator it = tList.begin(), end = tList.end();
			it != end; ++it) {
		//assegno peso a tuple
		it->weight = 0;
		it->weightMax = 0; //peso del parametro più pesante
		for (std::vector<int>::iterator it2 = it->paramCASA.begin(), end =
				it->paramCASA.end(); it2 != end; ++it2) {
			it->weight += indexListCASA[*it2][1];
			if (indexListCASA[*it2][1] > it->weightMax) {
				it->weightMax = indexListCASA[*it2][1]; //NB in [][0] c'è l'indice del parametro che prima del riordino è guauale a quello del vettore
			}
		}

	}

	//Ordino
//	sort(indexList.begin(), indexList.end(), orderIndexList);
#ifdef DEBUG
	//indexList=newIndexList;
	cout<<"massimo is "<<indexList[0][1]<<endl;
	cout<<"minimo is "<<indexList.at(indexList.size()-1)[1]<<endl;
#endif

//	cout << "Max val is " << *max_element(indexList.begin(),indexList.end()) << endl;

}

int TupleList::checkCompatibility(forest *mdd, dd_edge nodoBase)
{
//	int nchecks=0;
	int size=tList.size();
	for (std::list<Tuple>::iterator it = tList.begin(), end = tList.end();
			it != end; ++it) {
		it->weight=1;
	/*	for (std::vector<int>::iterator it2 = it->param.begin(), end =
				it->param.end(); it2 != end; ++it2) {
			it->weight+=((double)it->code[*it2])/100;


		}*/

	}
	for (std::list<Tuple>::iterator it = tList.begin(), end = tList.end();
			it != end; ++it) {
		if ((it->status==0)) //Se è già coperta non serve
		{
			//dd_edge node = Operations::getMDDFromTuple(it->code,mdd);
			//confronto con successive escludendo se stessa
			for (std::list<Tuple>::iterator it2 = it, end = tList.end();
						it2 != end; ++it2) {
				if ((it2->status==0)&&(it!=it2)){ //se è già coperta non serve, idem non la confronto con se stessa
					//dd_edge node2 = Operations::getMDDFromTuple(it2->code,mdd);
					//dd_edge node2 = Operations::getMDDFromTuple({0,0,-1},mdd);
					//-1 se non compatibili, >0 se compatibili, nota che potrei controllare che ogni altra combinazione è impossibile, per ottimizzare
					int compatible = Tuple::isIncludedTuple(&(*it),&(*it2));
					//double cardinality = Operations::getCardinalityDifference(node,node2); //TODO da rimettere
					//nchecks++;
					//Caso 1/2 devo controllare contraints impliciti, ma peggiora troppo performance
					if (size<0){ //disabilitato
						if (compatible>0){
							int card = Operations::getCardinalityDifference(Operations::getMDDFromTuple(it->code,mdd),Operations::getMDDFromTuple(it2->code,mdd)*(nodoBase));
							if (card==-1)
							{
								compatible=0;
								//cout<<"incomp step2"<<endl;
							}
						}
					}
						//sono incompatibili, aggiungo in entrambi i vector i due indici
					if (compatible==0)
					{
						it->weight++;

						it2->weight++;
					}


				}
			}

		}
	}
	if (setting.wexp2!=1){
	for (std::list<Tuple>::iterator it = tList.begin(), end = tList.end();
				it != end; ++it) {
		it->weight=pow(it->weight,setting.wexp2);
	}
	}
//	cout<<"*******"<<nchecks<<"****"<<endl;
	return 0;
}

void TupleList::checkParamListNew(forest* mdd) {
	//Aggiungo peso a tuple
	bool debug = false;
	if (tList.size() < 0)
		debug = true;
	int nParams = mdd->getDomain()->getNumVariables();
	indexList.clear();
	//vector<int[2]> newIndexList(nParams);// = new vector<int>(0,nParams);

	for (int paramI = 0; paramI < nParams; paramI++) {
		vector<int> param(2, 0);

		param[1] = 0;
		param[0] = paramI;

		indexList.push_back(param);
	}

//	for( int paramI=0;paramI<nParams;paramI++)
//	{
	weightMax = 0;
	for (std::list<Tuple>::iterator it = tList.begin(), end = tList.end();
			it != end; ++it) {
		if (it->weight>weightMax) weightMax=it->weight;
		for (std::vector<int>::iterator it2 = it->param.begin(), end =
				it->param.end(); it2 != end; ++it2) {

			if (debug) {
				cout << "CHECKING " << it->getValue() << endl;
			}
			indexList[*it2][1]+=it->weight; //incrementa contatore per il parametro

		}

		//		}
	}
	//fino a qui è ordinato per index parametri



	//Ordino
	sort(indexList.begin(), indexList.end(), orderIndexList);
#ifdef DEBUG
	//indexList=newIndexList;
	cout<<"massimo is "<<indexList[0][1]<<endl;
	cout<<"minimo is "<<indexList.at(indexList.size()-1)[1]<<endl;
#endif

//	cout << "Max val is " << *max_element(indexList.begin(),indexList.end()) << endl;

}

void TupleList::reorderIndexList(int bestParam, vector<vector<int> > &indexListToOrder)
{
	//return;
	int size=indexListToOrder.size();
	vector< vector < int > > indexListNew;// = indexList;
	vector< vector < int > > indexListNewInv;
	for (int i=0; i<size;i++)
	{
		if (indexListToOrder.at(i).at(0)==bestParam)
			{
				indexListNew.push_back(indexListToOrder.at(i));
				break;
			}
		//inserito in testa
	}
	//int64 partial=Operations::getTimeMs64();
	for (int i=0; i<size;i++)
	{
		if (indexListToOrder.at(i).at(0)==bestParam)
		{
			continue;
		}
		bool valid=false;
		for (std::list<Tuple>::iterator it = tList.begin(), end = tList.end();
				it != end; ++it) {
			bool check1=false;
			bool check2=false;
			for (std::vector<int>::iterator it2 = it->param.begin(), end =
					it->param.end(); it2 != end; ++it2) {

				if (*it2==bestParam) check1=true; //contiene bestparam
				if (*it2==indexListToOrder.at(i).at(0)) check2=true; //contiene param che sto passando


			}
			valid=check1&&check2;
			if (valid) break;
		}
		if (valid) indexListNew.push_back(indexListToOrder.at(i));
		else
			indexListNewInv.push_back(indexListToOrder.at(i));

	}
	//cout<<"Partial reorder: "<<Operations::getTimeMs64()-partial<<"ms"<<endl;
	//merge inv in coda a new
	//cout<<"DEBUG INDEXNEW "<<indexListNew.size()<<endl;
	for (std::vector< vector < int > >::iterator it = indexListNewInv.begin(), end = indexListNewInv.end();
					it != end; ++it) {
		indexListNew.push_back(*it);
	}

	if (indexListNew.size()!=indexListToOrder.size()) {
		cout<<"ERROR"; exit(0);
	}
	indexListToOrder=indexListNew;

}
