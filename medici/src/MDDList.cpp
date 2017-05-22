/*
 * MDDList.cpp
 *
 *  Created on: 27/gen/2013
 *      Author: Dario
 */
#include "header.h"

#include "MDDList.h"
#include "logger.hpp"
#include <sstream>

bool fSortWeight(vector<int> i, vector<int> j) {
	return (i[1] > j[1]);
}

MDDList::MDDList(forest *mdd) {
	this->mdd = mdd;
	//
	dd_edge all(mdd);
	dd_edge edge(mdd); //=new dd_edge(mdd);
	mdd->createEdge(true, edge);
	mdd->createEdge(true, all);
	//edge.show(stdout,3); //TODO debug
	//cout<<"***********"<<endl;
	nodoVuoto = all;
	nodoBase = edge;
	//nodoFase1 = edge;
	//all.show(stdout,3);

}

MDDList::~MDDList() {

}

void MDDList::printList(ostream& str) {
	for (uint z = 0; z < list.size() - 1; z++) {
		//	cout<<"***mdd index: "<<z<<" ****"<<endl;
		//apply(UNION,  *(mddList.at(0)),  *(mddList.at(z)),  *(mddList.at(0)));
		//list[z].clear();
		//list[z].show(stdout,2);
		printMdd(str,z);
	}
}

void MDDList::updateCovered(int mddIndex, TupleList &listT, int mode) {

	vector<cvalue> code = Operations::getCodeFromMdd(list[mddIndex].node);
	for (std::list<Tuple>::iterator it = listT.tList.begin(), end =
			listT.tList.end(); it != end; ++it) {

		//dd_edge node2 = it->node;
		if (!it->status) //Se è già coperta non serve
		{
			if (Operations::isIncludedTuple(code, it->code, it->param) == 1) {
				if (mode == 0) {
					listT.setCovered(*it);
				}
				list[mddIndex].nCoveredMdd++;
				list[mddIndex].tWeight += it->weight;
			}

		}
	}
//	cout<<"TIME DEBUG "<<Operations::GetTimeMs64()-now<<endl;
}

int MDDList::checkForParameter(vector<int> it2, Tuple &tupla, int* bounds,
		int mddIndex, TupleList &listT, int paramIt, bool step2Check,
		int &randomPar) {
	int nParams = mdd->getDomain()->getNumVariables();
	bool debug = false;
	int var; //= mdd->getDomain()->getVariableBound(paramI+1,false); //cardinalità parametro paramI
	int paramI = it2.at(0); //index originale, mentre a 1 c'è il peso associato
	var = bounds[nParams - paramI - 1];
#ifdef DEBUG2
	cout<<"PARAMI "; //TODO remove
	cout<<paramI<<" "<<it2.at(1)<<" bounds: "<<var<<endl;
#endif
	// cout<<"PARAM BOUND: "<<var<<endl;
	//  cout<<"DEBUG2"<<endl; //TODO remove
	//   string str;
	//   cin>>str;
//	if (listT.size()<10) debug=true;
	double maxVarStep1 = 0; //numero di tuple inseribili al massimo
	double maxVarStep2 = 0;
	double countVarStep1 = 0; //contatore temporaneo
	double countVarStep2 = 0;
//	int maxVarStep1Value = -1; //valore del parametro che inserisce più tuple
	int maxVarStep2Value = -1;
	vector<int> maxVarStep1ValueS;
//	vector<int> maxVarStep2ValueS;

	for (int i = 0; i < var; i++) //testo possibili valori dei parametri
			{

		// 	cout<<"TESTING "<<i<<endl;
		//cout<<"DEBUG2"<<endl; //TODO remove
		//aggiungo TEST compatibilita per MDD generato con questo parametro
		tupla.code[paramI] = i;
		dd_edge nodeTmp = Operations::getMDDFromTuple(tupla.code, mdd);

		tupla.code[paramI] = -1;

		//double cardDiff = Operations::getCardinalityDifference(nodeTmp,list[mddIndex].node);

		//	nodeTmp2.clear();
		countVarStep1 = 0;
		countVarStep2 = 0;

		bool toCheck = true;
		for (std::list<Tuple>::iterator it2 = listT.tList.begin(), end =
				listT.tList.end(); it2 != end; ++it2) {
			//Sfoglio lista tuple
			if (it2->status == 1)
				continue; //è già coperta
			if (it2->code[paramI] != i)
				continue; //non è compatibile o non include il parametro
			if (toCheck) {
				nodeTmp *= list[mddIndex].node;
				//	mdd->removeAllComputeTableEntries();
				if (nodeTmp.getCardinality() <= 0) {
					//parametro non valido per constraints
#ifdef DEBUG2
					cout<<"Parametro non valido per constraints"<<endl;
#endif
					//	nodeTmp2.clear();
					break;//Passo al prossimo

				}
				toCheck = false;
			}
			//ora potrebbe essere compatibile, testo con mdd

			if (debug) {
				//	Operations::printElements(stdout,node2);
				//Operations::printElements(std::cout, list[mddIndex].node);
			}

			//		countVar+=Operations::getIntersectionCardinality(node2,nodeTmp);
			//Questo test impiega troppo tempo node2 è la tupla e ok, nodeTmp è il modello con constraints, provare con uno vuoto solo con parametri fissati?
			tupla.code[paramI] = i; //cambio temporaneo per check
			int isIncluded = Operations::isIncludedTuple(tupla.code, it2->code,
					it2->param);
			tupla.code[paramI] = -1;
			if (isIncluded == 1) { //Già inclusa
				//if (countVarStep1<it2->weightMax) countVarStep1=it2->weightMax;
				countVarStep1 += (it2->weight); //*(it2->weight);//*(it2->weight)*(it2->weight);
			} else if (isIncluded == 2) {
				//Provo a tagliare questo pezzo per eseguirlo solo dopo?
				if (countVarStep1 <= 0) //non eseguo se c'è già step1 fissato, ottimizza però lo usavo in caso di parità di countvar1... qualche dato però mi resta perchè qualche volta lo eseguo lo stesso
						{

#ifdef DEBUG2
					cout<<"CHECKING PARAMETER: "<<i<<" Counvarstep1 "<<countVarStep1<<" Step2 "<<
					countVarStep2<<endl;
#endif
					if (paramIt == 0) {
						countVarStep2 += it2->weight;
					} //primo parametro è sempre compatibile
					else
					//if (paramIt==0) //controllo solo in caso di primo parametro
					{

						//TODO provo a togliere check e controllo efficienza
						//		dd_edge node2 = Operations::getMDDFromTuple(it2->code,mdd);

						double cardDiffTmp = 0;
						//	Operations::getCardinalityDifference(node2,nodeTmp);
						//	cout<<"CHIAMATA a getCARDDIFF"<<endl;

						if (cardDiffTmp >= 0)
							countVarStep2 += it2->weight; //1/cardDiffTmp;//it2->weight;
#ifdef DEBUG2
									else
									cout<<"Parametro non compatibile per constraints: "<<i<<" Counvarstep1 "<<countVarStep1<<" Step2 "<<
									countVarStep2<<endl;
#endif

					}
				}
			}
			//  double cardDiffTmp=Operations::getCardinalityDifference(node2,nodeTmp);
			//	double cardDiffTmp=Operations::getCardinalityDifference(node2,list[mddIndex].node);

		}

#ifdef DEBUG2
		cout<<"CHECKED PARAMETER: "<<i<<" Counvarstep1 "<<countVarStep1<<" Step2 "<<
		countVarStep2<<endl;
#endif
		if (countVarStep1 > 0) {
			if (countVarStep1 > maxVarStep1) {
#ifdef DEBUG2
				cout<<"SCELTO DA PRIMO GRADO "<<i<<" "<<countVarStep1<<endl;
#endif
				maxVarStep1 = countVarStep1;
				maxVarStep1ValueS.clear();
				maxVarStep1ValueS.push_back(i);
				//maxVarStep1Value = i;

			} else if (countVarStep1 == maxVarStep1) {
				//maxVarStep1Value=i;
				/*
				 if (countVarStep2 >= maxVarStep2) //scelta di secondo grado TODO prima c'era uguale
				 {
				 #ifdef DEBUG2
				 cout<<"SCELTO DA SECONDO GRADO  "<<i<<" ";
				 if ((countVarStep2==maxVarStep2))
				 cout<<"INDECIDIBILE"<<endl; else
				 cout<<countVarStep2<<endl;
				 #endif
				 maxVarStep2 = countVarStep2;
				 //maxVarStep1=countVarStep1; //non serve è già uguale per ipotesi
				 if (countVarStep2>maxVarStep2)
				 maxVarStep1ValueS.clear();
				 maxVarStep1ValueS.push_back(i);
				 //maxVarStep1Value = i;

				 }*/
				maxVarStep1ValueS.push_back(i);
			}
		} else if ((countVarStep2 >= maxVarStep2) && (maxVarStep1 <= 0)) { //TODO c'era =
#ifdef DEBUG2
				cout<<"SCELTO DA SECONDO GRADO PURO  "<<i<<" ";
				if ((countVarStep2==maxVarStep2))
				cout<<"INDECIDIBILE"<<endl; else
				cout<<countVarStep2<<endl;
#endif
			maxVarStep2 = countVarStep2;
			//	if (countVarStep2>maxVarStep2)
			//	maxVarStep2ValueS.clear();
			//	maxVarStep2ValueS.push_back(i);
			maxVarStep2Value = i;

		}
	}
	//uscito dal ciclo avrò un valore da fissare per il parametro
	//   cout<<"FISSO "<<maxVarValue<<" con n tuple compatibili: "<<maxVar<<endl;
	randomPar = maxVarStep1ValueS.size(); //da 2 in su ho fatto scelta random
	if (maxVarStep1ValueS.size() > 0) {
		//scelta in base a random che gli passo, non supera mai size
		//srand (time(NULL));
		int pos = rand() % maxVarStep1ValueS.size();
		//cout<<maxVarStep1ValueS.size()<<"****"<<pos<<endl;

		tupla.code[paramI] = maxVarStep1ValueS[pos]; //TODO pos
	} else {
		tupla.code[paramI] = maxVarStep2Value;
		//tupla.code[paramI] = maxVarStep2ValueS[randomPar % maxVarStep2ValueS.size()];
	}
#ifdef DEBUG2
	cout<<"Maxvar2/1: "<<maxVarStep2<<"/"<<maxVarStep1<<endl;
#endif
	if ((maxVarStep2 + maxVarStep1) == 0)
		return 0; //-1 se fissare quel parametro non copre e non coprirà mai niente
	if (maxVarStep1 > 0)
		return maxVarStep1;
	else
		return (0 - maxVarStep2);
	//	return tupla.code[paramI];
}

int MDDList::aggiungiInCoda() {

	list.push_back( { nodoBase, 0, 0 }); //inserimento in coda di un mdd completo con constraints

	return 0;
}

int MDDList::aggiungiAt(int index, vector<cvalue> code) {
	dd_edge tempEdge = Operations::getMDDFromTuple(code, mdd);
	aggiungiAt(index, tempEdge);

	return 0;
}

int MDDList::aggiungiAt(int index, dd_edge mddToAdd) {

	apply(INTERSECTION, mddToAdd, (list.at(index).node), (list.at(index).node));
	mddToAdd.clear();
	//list[index].nCoveredMdd++;
#ifdef DEBUG
	cout<<"Aggiungo mdd a mdd: "<<index<<endl;
#endif
	// printMdd(index);
	return 0;

}

/*int MDDList::aggiungiTemp(dd_edge mddToMerge, dd_edge mddBase, dd_edge result) {

 apply(INTERSECTION, mddBase, mddToMerge, mddToMerge);

 // printMdd(index);
 return 0;

 }*/

void MDDList::printMdd(std::ostream& ss, int index) {
	dd_edge edge = list[index].node;
	ss << "CARD: " << edge.getCardinality() << " ";
	ss << list.at(index).nCoveredMdd << " tuples ";
	ss << "weight: " << list.at(index).tWeight << " ";
	Operations::printElements(ss, edge,0);
}

void MDDList::updateConstraints(
		vector<std::list<Operations::TestModelConstraint> > tMC, int* bounds,
		int N, TupleList &tList) {

	int i = 0;
	logcout(LOG_INFO) << "Cardinalita' di partenza " << nodoBase.getCardinality() << endl; //TODO debug

	for (std::vector<std::list<Operations::TestModelConstraint> >::iterator it =
			tMC.begin(), end = tMC.end(); it != end; ++it) {

		dd_edge c(mdd);
		vector<int> params;
		while (!it->empty()) {
			Operations::TestModelConstraint it2 = it->back();
			it->pop_back();
			//sfoglio parametri della singola formula disgiuntiva (or)
			//	cout<<"CONTROLLO CODICE "<<it2.value<<endl;
			dd_edge tP;
			int index;
			vector<cvalue> code = Operations::getCodeFromParameter(
					it2.getValue(), bounds, N, index);
			vector<int> param(1, index);
			params.push_back(index); //aggiunge a elenco parametri constraint
			Tuple tuplaTest = Tuple(code, param);
			//tuplaTest.print();
			tList.constraintsIndexList.push_back(index);
			//	cout<<"INDEX "<<index<<endl;
//					nodoVuoto.show(stdout,3);
			//	cout<<"traduzione in tupla: "<<tuplaTest.getValue()<<endl; //tuplaTest.print();
			tP = Operations::getMDDFromTuple(code, mdd);
			//tP.show(stdout,3); //TODO debug
			//	cout<<"------------"<<endl;
			//cout<<"Cardinalità codice "<<it2->value<<" uguale a: "<<tP.getCardinality()<<endl;
			if (it2.isNegative()) {

				//	cout<<"segno negativo "<<endl;
				//	cout<<"Cardinalità nodo vuoto "<<nodoVuoto.getCardinality()<<endl;
				//			nodoVuoto.show(stdout,3);
				tP = nodoVuoto - tP; //fare il complementare
				//	cout<<"Cardinalità codice "<<it2->value<<" modificata da not uguale a: "<<tP.getCardinality()<<endl;
			}
			//ho finito di crearlo unisco a C
			c += tP;

		}
		//ho il mio constraint
		//c.show(stdout,3);
		logcout(LOG_INFO) << "Cardinalità del constraint " << c.getCardinality() << endl;

		listConstraints.push_back( { c, params }); //params è la lista dei parametri che include il constraint
		nodoBase *= c; //AND con nodobase per attivare constraint
		logcout(LOG_INFO) << "Cardinalita ridotta con constraint " << i++ << " "
				<< nodoBase.getCardinality() << endl; //TODO debug
		//	nodoBase.show(stdout,3);

		//	mdd->createEdge(true, edge);
	}
	logcout(LOG_INFO) << "Cardinalita finale "  << nodoBase.getCardinality() << endl;

	//elimino duplicati e ordino lista costraints in tlist
	//in listConstraints ho una lista di nodi uno per ogni CNF e i suoi parametri
	//mergeConstraints();
	logcout(LOG_INFO) << "Numero constraints di partenza: " << listConstraints.size()
			<< endl;
	while (mergeConstraints())
		;
	logcout(LOG_INFO) << "Numero constraints frammentati: " << listConstraints.size()
			<< endl;

	//printConstraints();
	sort(tList.constraintsIndexList.begin(), tList.constraintsIndexList.end());
	tList.constraintsIndexList.erase(
			unique(tList.constraintsIndexList.begin(),
					tList.constraintsIndexList.end()),
			tList.constraintsIndexList.end());
	//nodoBase=nodoBase-c; //proviamo differenza
	logcout(LOG_INFO) << "Creo lista semplice" << endl;
	//uso la constraintsIndexList per sapere gli mdd da generare per i constraints
//	createConstraintsMddList(tList.constraintsIndexList); //TODO tolto non cambiava i tempi, la frammentazione è il massimo
//	nodoBase.show(stdout,3);

}

void MDDList::updateConstraintsMEDICI(
		vector<std::list<Operations::TestModelConstraint> > tMC, int* bounds,
		int N, TupleList &tList) {

	int i = 0;
	logcout(LOG_INFO) << "Cardinalita di partenza " << nodoBase.getCardinality() << endl; //TODO debug

	for (std::vector<std::list<Operations::TestModelConstraint> >::iterator it =
			tMC.begin(), end = tMC.end(); it != end; ++it) {

		dd_edge c(mdd);
		vector<int> params;
		std::list<dd_edge> tPList;
		while (!it->empty()) {
#ifdef DEBUG2
			cout << "current list";
			for (std::list<Operations::TestModelConstraint>::iterator pTListit = it->begin(); pTListit != it->end(); pTListit++)
			    cout << " " << *pTListit;
			cout << endl;
#endif
			// comincio dal fondo
			Operations::TestModelConstraint it2 = it->back();
			it->pop_back();
			//sfoglio parametri della singola formula disgiuntiva (or)
			//	cout<<"CONTROLLO CODICE "<<it2.value<<endl;
			//	cout<<"CONTROLLO SEGNO "<<it2.sign<<endl;
#ifdef DEBUG2
			cout << "analyzing " << it2<< endl;
#endif
			dd_edge tP;
			dd_edge tP1;
			int index;
			if (!it2.isOperation()) {
				vector<cvalue> code = Operations::getCodeFromParameter(
						it2.getValue(), bounds, N, index);
				vector<int> param(1, index);
				params.push_back(index); //aggiunge a elenco parametri constraint
				Tuple tuplaTest = Tuple(code, param);
				//tuplaTest.print();
				//cout<<code.size()<<endl;
				tList.constraintsIndexList.push_back(index);
				//	cout<<"INDEX "<<index<<endl;
				//					nodoVuoto.show(stdout,3);
				//	cout<<"traduzione in tupla: "<<tuplaTest.getValue()<<endl; //tuplaTest.print();
				tP = Operations::getMDDFromTuple(code, mdd);
				tPList.push_front(tP);
				//tP.show(stdout,3); //TODO debug
			} else {
				// it2 is an operation
				switch (it2.getOp()) {
				case '-': //- unario
					//cambio tp ma non lo rimuovo da lista
					tPList.front() = nodoVuoto - tPList.front();
					//tPList.front().show(stdout,3); //TODO debug
					break;
				case '+': //+ binario
					//Sommo ultimi due della lista, salvo nel penultimo e rimuovo ultimo
					/* AG tP1 = tPList.back();
					tPList.pop_back();
					tPList.back() = tPList.back() + tP1;*/
					tP1 = tPList.front();
					tPList.pop_front();
					tPList.front() = tPList.front() + tP1;
					break;
				case '*': //* binario
					/*tP1 = tPList.back();
					tPList.pop_back();
					tPList.back() = tPList.back() * tP1;*/
					tP1 = tPList.front();
					tPList.pop_front();
					tPList.front() = tPList.front() * tP1;
					break;
				default:
					break;

				}
			}
			//nodoVuoto.show(stdout,3);
			//	cout<<"------------"<<endl;
			//	cout<<"Cardinalità codice "<<it2->value<<" uguale a: "<<tP.getCardinality()<<endl;
		}
		//ho finito di crearlo unisco a C
		//controllo che sia stato definito bene, lista deve avere un elemento solo
		//cout<<tPList.size()<<"******\n";
		if (tPList.size() != 1) {
			cout << "ERROR CONSTRAINTS DEFINITION \n";
			return;
		}

		c = tPList.back();

		//ho il mio constraint
		//c.show(stdout,3);
		//cout<<"Cardinalità del constraint "<<c.getCardinality()<<endl;

		listConstraints.push_back( { c, params }); //params è la lista dei parametri che include il constraint
		nodoBase *= c; //AND con nodobase per attivare constraint
		logcout(LOG_INFO) << "Cardinalita ridotta con constraint " << i++ << " "
				<< nodoBase.getCardinality() << endl; //TODO debug
		//	nodoBase.show(stdout,3);

		//	mdd->createEdge(true, edge);

	}
	logcout(LOG_INFO) << "Cardinalita finale "  << nodoBase.getCardinality() << endl;
	//elimino duplicati e ordino lista costraints in tlist
	//in listConstraints ho una lista di nodi uno per ogni CNF e i suoi parametri
	//mergeConstraints();
	logcout(LOG_INFO) << "Numero constraints di partenza: " << listConstraints.size()
			<< endl;
	while (mergeConstraints())
		;
	logcout(LOG_INFO) << "Numero constraints frammentati: " << listConstraints.size()
			<< endl;

	//printConstraints();
	sort(tList.constraintsIndexList.begin(), tList.constraintsIndexList.end());
	tList.constraintsIndexList.erase(
			unique(tList.constraintsIndexList.begin(),
					tList.constraintsIndexList.end()),
			tList.constraintsIndexList.end());
	//nodoBase=nodoBase-c; //proviamo differenza
	logcout(LOG_INFO) << "Creo lista semplice" << endl;
	//uso la constraintsIndexList per sapere gli mdd da generare per i constraints
//	createConstraintsMddList(tList.constraintsIndexList); //TODO tolto non cambiava i tempi, la frammentazione è il massimo
//	nodoBase.show(stdout,3);

}

bool MDDList::mergeConstraints() {
	//Iterator su lista constraints
	vector<Constraint> lCNew;
	bool comp = false;

//	lCNew.push_back(*listConstraints.begin()); //inizializzo mettendo il primo elemento
	for (std::vector<MDDList::Constraint>::iterator it =
			listConstraints.begin(), end = listConstraints.end(); it != end;
			++it) {
		comp = false;
		//iterator su lista parametri, questo posso evitarlo facendo il merge dei vector e vedendo se size>0
		//for (std::vector < int >::iterator it2 = it->params.begin(), end = it->params.end(); it2 != end; ++it2)
		//{
		//controllo se ho già inserito un mdd compatibile -> iterator su lCNew NB il primo lo mette pure se c'è già ma fa niente
		std::sort(it->params.begin(), it->params.end());
		it->params.erase(std::unique(it->params.begin(), it->params.end()),
				it->params.end());

		for (std::vector<MDDList::Constraint>::iterator it3 = lCNew.begin(),
				end = lCNew.end(); it3 != end; ++it3) {
			std::vector<int> res; //Intersection of V1 and V2
			std::sort(it3->params.begin(), it3->params.end());
			it3->params.erase(
					std::unique(it3->params.begin(), it3->params.end()),
					it3->params.end());
			set_intersection(it->params.begin(), it->params.end(),
					it3->params.begin(), it3->params.end(),
					std::back_inserter(res));
			//in res ho l'intersezione dei parametri, 0 se sono indipendenti

			if (res.size() > 0) {
				comp = true;
				//	 cout<<"TROVATA intersezione valida di dimensione "<<res.size()<<endl;
				//è compatibile aggiungo qui
				//NB potrebbe essere compatibile con più di uno e in quel caso devo mischiarli di nuovo, ripassando il ciclo
				it3->node *= it->node; //faccio intersezione per nodo e merge per vector prametri
				//DEBUG
				//  	 cout<<"set union tra "<<it->params.size()<<" e "<<it3->params.size()<<endl;
				//	    	 for (std::vector < int >::iterator it2 = it->params.begin(), end = it->params.end(); it2 != end; ++it2)
				//	    	 {
				//	    		 cout<<*it2<<" - ";
				//	    	 }
				std::vector<int> v(it->params.size() + it3->params.size());
				std::vector<int>::iterator itTmp;
				itTmp = std::set_union(it->params.begin(), it->params.end(),
						it3->params.begin(), it3->params.end(), v.begin());
				v.resize(itTmp - v.begin());
				//v.swap(it->params);
				//	 it->params[0]=200;
				it3->params = v;
				//DEBUG
				// cout<<"ottenuto "<<it->params.size()<<endl;
				// for (std::vector < int >::iterator it2 = it->params.begin(), end = it->params.end(); it2 != end; ++it2)
				// {
				//	 cout<<*it2<<" - ";
				// }
				// cout<<endl;
				std::sort(it->params.begin(), it->params.end());
				// it->params.erase(std::unique(it->params.begin(), it->params.end()), it->params.end());
				//RIORDINO ed elimino doppi, merge elimina già doppi e probabilmente riordina anche
				break;//non mi serve andare avanti ho trovato compatibile
			}

		}
		if (!comp) //non è compatibile con nessuno, aggiungo nuovo
		{

			//cout<<"NON è compatibile con nessuno creo nuovo"<<endl;
			lCNew.push_back(*it);
		}
		//}
	}
	lCNew.swap(listConstraints); //Swap ora tengo solo il vector ridotto
//	cout<<"FINE ITERAZIONE DIM "<<lCNew.size()<<" passata a "<<listConstraints.size()<<endl;
//	printConstraints();
	if (lCNew.size() == listConstraints.size())
		return false;
	return true;
	/**
	 * Ordina ed elimina duplicati, da usare alla fine
	 */
//	sort( tList.constraintsIndexList.begin(), tList.constraintsIndexList.end() );
//	tList.constraintsIndexList.erase( unique( tList.constraintsIndexList.begin(), tList.constraintsIndexList.end() ), tList.constraintsIndexList.end() );
}

void MDDList::printConstraints() {
	for (std::vector<MDDList::Constraint>::iterator it =
			listConstraints.begin(), end = listConstraints.end(); it != end;
			++it) {
		cout << "Stampa constraint size: " << it->params.size() << " group: "
				<< endl;
		for (std::vector<int>::iterator it2 = it->params.begin(), end =
				it->params.end(); it2 != end; ++it2) {
			cout << *it2 << " - ";
		}
		cout << endl;

	}
}

int MDDList::checkCoverable(forest* mdd, TupleList &tupleList) {

	dd_edge base = nodoVuoto;
	int i = 0;

	int64 now = Operations::getTimeMs64();

	int i2 = 0;
	for (std::list<Tuple>::iterator it = tupleList.tList.begin(), end =
			tupleList.tList.end(); it != end; ++it) {
		//Aggiungo check preliminare che può evitare il resto del controllo

		vector<int> res;
		i2 = 0;
		set_intersection(tupleList.constraintsIndexList.begin(),
				tupleList.constraintsIndexList.end(), it->param.begin(),
				it->param.end(), std::back_inserter(res));
		if (res.size() != 0) {
			//controllo constraints frammentati
			//partial2=Operations::GetTimeMs64();
			dd_edge nodeTmp = base;
			//in res ho i parametri che sono nei constraints

			dd_edge node = Operations::getMDDFromTuple(it->code, mdd);

			//	partial=partial+(Operations::GetTimeMs64()-partial2);
			//Sfoglio lista constraints frammentati per ottenere quello ridotto che contiene tutti i parametri
			uint sizeT = res.size();
			//int sizeTF = sizeT;
			bool changed = false;
			for (std::vector<MDDList::Constraint>::iterator it =
					listConstraints.begin(), end = listConstraints.end();
					it != end; ++it) {
				std::vector<int> res2; //V1-V2
				set_difference(res.begin(), res.end(), it->params.begin(),
						it->params.end(), std::back_inserter(res2));
				res = res2;
				//cout<<res.size()<<endl;

				if (res.size() < sizeT) {
					if ((res.size() == 0) && (!changed)) {
						nodeTmp = it->node;
					} else {
						//cout<<"mischiati "<<++i2<<"constraints"<<endl;
						nodeTmp *= it->node;
						//	cout<<it->node.getCardinality()<<" card node constraint "<<endl;
						//	cout<<base.getCardinality()<<" card ridotta"<<endl;
					}
					//c'è qualche parametro in comune
					changed = true;

				}
				sizeT = res.size();
				if (sizeT == 0)
					break; //non serve continuare

			}
			//ORA ho il constraints ridotto
			//	partial2=Operations::GetTimeMs64();

			double cardDiff = Operations::getCardinalityDifference(nodeTmp,
					node);
			//	partial=partial+(Operations::GetTimeMs64()-partial2);
			if (cardDiff < 0) {
				it->coverable = false;

			}
			//it->cardDiff=cardDiff;
		}
		i++;
		if ((i % 1000) == 0) //Se no stampa troppo
				{
			//	mdd->removeAllComputeTableEntries();
			logcout(LOG_INFO) << "parziale " << i << " " << Operations::getTimeMs64() - now
					<< "ms" << endl;
			//	cout<<"parziale generazione "<<i<<" "<<partial<<"ms"<<endl;
		}
	}

	tupleList.removeUncoverable();
	tupleList.nUncoverable = tupleList.nToCover - tupleList.tList.size();
	tupleList.nToCover = tupleList.tList.size();
	tupleList.nTotal = tupleList.tList.size();
	return 0;
}

int MDDList::listToFile(char* filename, int *bounds) {
	ofstream fout;
	fout.open(filename);
	if (!fout.is_open()) {
		// if( fd==NULL ) {
		perror("Errore in apertura del file");
		return -1; //errore
	}
	fout << list.size() - 1 << endl;
	//printValue(lista)<<endl; //scrive valore e va a capo

	for (uint z = 0; z < list.size() - 1; z++) //ultimo è sempre uno vuoto
			{
		vector<cvalue> code = Operations::getCodeFromMdd(list[z].node);
		Operations::toCASACodeConversion(code, bounds);
		for (uint k = 0; k < code.size(); k++) //ultimo è sempre uno vuoto
				{
			fout << code[k] << " ";
		}
		fout << endl;

	}
	fout.close();
	return 0;
}

MDDList MDDList::listFromFile(char* filename, int *bounds, int nel,
		forest * mdd) {
	ifstream fin;
	uint nMdd;
	string strRow;
	MDDList mddList(mdd);
	vector<short> code(nel, -1);
	vector<short> pvalue(2, -1);
	int v = -1;
	fin.open(filename);
	if (!fin.is_open()) {
		// if( fd==NULL ) {
		perror("Errore in apertura del file");
		return NULL; //errore
	}

	getline(fin, strRow); // Prima riga indica numero di risultati
	nMdd = atoi(strRow.c_str()); //nWise

	for (uint i = 0; i < nMdd; i++) {
		getline(fin, strRow);
		code = vector<short>(nel, -1);
		vector<string> tokens = Operations::tokenize(strRow);
		if (tokens.size() != nel)
			return NULL;
		for (uint k = 0; k < tokens.size(); k++) {
			v = atoi(tokens[k].c_str()); //casa parameter
			pvalue = Operations::getValueFromParameter(v, bounds, nel);
			code[pvalue[0]] = pvalue[1];
		}
		//code completato
		mddList.list.push_back(
				{ Operations::getMDDFromTuple(code, mddList.mdd), 0, 0 });

	}

	fin.close();
	return mddList;
}

int MDDList::addOther2b(TupleList &listT, int* bounds, int tries) //bounds non doveva servire ma ci deve essere un bug in meddly;
		{
	int mddIndex = 0;
	int nParams = mdd->getDomain()->getNumVariables();
	int paramIt = 0;
	int nTupleInit = 1;
	int bestParam = -1;

	bool debug = false;
	Tuple tupla = Tuple(nParams);
	//int nParams = mdd->getDomain()->getNumVariables();
	if (listT.size() <= 0)
		debug = true;
	//Provo modifica per creazione tupla iniziale
	//OK itero per tuple
	int tI = 0;

	aggiungiInCoda();
	mddIndex = list.size() - 2;
	for (std::list<Tuple>::iterator it = listT.tList.begin(), end =
			listT.tList.end(); it != end; ++it) {
		if (!nTupleInit)
			break; //arrivato a 0

		if (it->status == 1)
			continue; //è già coperta
		//non è coperta quindi da questa genero sicuramente un caso di test
		if (tI == 0) {
			//Aggiungo mdd
			//prima ne aggiungo uno vuoto in coda

			//ora aggiungo a quello che è ora il penultimo elemento la tupla
			aggiungiAt(mddIndex, it->code); //TODO da rimettere
			listT.setCovered(*it); //è in mdd quindi setto coperta
			list[mddIndex].nCoveredMdd++;
			list[mddIndex].tWeight += it->weight;
			double wmax = 0;
			//trovo parametro best da cui ordinare dopo index
			for (std::vector<int>::iterator it2 = it->param.begin(), end =
					it->param.end(); it2 != end; ++it2) {

				for (std::vector<vector<int> >::iterator iti =
						listT.indexList.begin(), end = listT.indexList.end();
						iti != end; ++iti) {
					if (iti->at(0) != *it2)
						continue;
					//trovato index
					if (iti->at(1) >= wmax) {
						wmax = iti->at(1);
						bestParam = *it2;
					}

				}
			}
			tupla = Tuple(it->code, it->param, it->paramCASA); //tupla che completo mano a mano
			logcout(LOG_INFO) << "Aggiungo tupla: " << it->getValue() << " Weight: "
					<< it->weight << endl;
			//	int64 now = Operations::getTimeMs64();

			listT.reorderIndexList(bestParam, listT.indexList);
			//	cout << "reorder in: " << Operations::getTimeMs64() - now << "ms" << endl;
			nTupleInit--;
			tI++;
		} else { //non usato
			//ho già una tupla inserita, test compatibilità
			dd_edge node2 = Operations::getMDDFromTuple(it->code, mdd);
			double cardDiffTmp = Operations::getCardinalityDifference(node2,
					list[mddIndex].node);
			//		cout<<"CHIAMATA a getCARDDIFF"<<endl;
			//MODIFICA, DECIDO IN BASE A QUELLO CHE CAMBIA MENO LA CARDINALITA
			if (cardDiffTmp > 0) {
				//è compatibile
				aggiungiAt(mddIndex, it->code);
				logcout(LOG_INFO) << "Aggiungo tupla: " << it->getValue() << endl;
				tupla = Tuple::mergeTuple(tupla, *it);
				//vector<cvalue> code = Operations::getCodeFromMdd(list[mddIndex].node);
				//	tupla = Tuple(code, it->param); //param sono sbagliati ma non mi interessano
				logcout(LOG_INFO) << "Risultato tupla: " << tupla.getValue() << endl;
				nTupleInit--;
				tI++;
			}
		}

		if (debug)
			cout << "tupla di partenza: " << tupla.getValue() << endl; //TODO debug da togliere

	}
	//OK itero per tuple

	//	Tuple tupla = Tuple(nParams);
	//sfoglio tuple parametri ordinata

	dd_edge nodoInit = list.at(mddIndex).node;
	int nMddInit = list[mddIndex].nCoveredMdd;
	double weightInit = list[mddIndex].tWeight;
	Tuple tInit = tupla;
	double tWeightMax = 0;
	int tIndex = -1;
	vector<vector<cvalue> > codes;
	vector<vector<int> > indexListTMP = listT.indexList;

	bool randomCheck = true;
	for (int z = 0; z < tries; z++) {
		if (!randomCheck) {
#ifdef DEBUG2
			cout<<"DEBUG SALTO NO RANDOM"<<endl;
#endif
			break; //se non c'era random nell'iterazione precedente inutile farne un'altra
		}
		randomCheck = false;
		tupla = tInit;
		indexListTMP = listT.indexList;
		list.at(mddIndex).node = nodoInit;
		list.at(mddIndex).nCoveredMdd = nMddInit;
		list.at(mddIndex).tWeight = weightInit;
		vector<vector<int> > indexListTC;		// = listT.indexList;
		bool coverable = true;
		int paramZero = 0;
		indexListTC.push_back(indexListTMP.back());
		//int iterazioni=0;

		int randomPar = 0;
		bool restart = false;

		while (((indexListTC.size() > 0) && (coverable == true))
				|| (restart == true)) {

			restart = false;
			indexListTC.clear();
			/**
			 * RIORDINARE indexListTMP mettendo prima i parametri che interagiscono con i già fissati
			 ultimo dei fissati, il più pesante, solo una volta?
			 */

			coverable = false;
			paramIt = 0;
			//	paramZero=0; //TEST

#ifdef DEBUG2
			cout<<"NUOVA ITERAZIONE"<<endl;
#endif
			for (std::vector<vector<int> >::iterator it2 = indexListTMP.begin(),
					end = indexListTMP.end(); it2 != end; ++it2) {

				//devo modificare per passare ad altri parametri in base a quello che decido per primo
				int paramI = it2->at(0);
				if (tupla.code[paramI] > -1)
					continue; //parametro già fissato
				//trovato parametro non fissato
				if (it2->at(1) == 0) {
					//non coinvolge nessuna tupla e quindi essendo ordinati è così
					//anche per i successivi, esco
					break;
				}
				/*	if (paramIt==0)
				 {paramZero=paramI;
				 cout<<"PARAM0 "<<paramZero<<endl;
				 }*/
				//iterazioni++;
				//int64 now = Operations::getTimeMs64();
				bool step2Check = false;
				if (((paramIt < 1) && (paramZero > 0)))
					step2Check = true;

				int nCoverable = checkForParameter(*it2, tupla, bounds,
						mddIndex, listT, paramIt + nTupleInit, step2Check,
						randomPar); //paramit=0 solo quando tupla non init

				if (randomPar > 1)
					randomCheck = true; //fatto scelta random
				//partial+=Operations::getTimeMs64()-now;
				if ((nCoverable == 0)) {
#ifdef DEBUG2

					cout<<"SALTO PARAMETRO CHE NON INFLUISCE: "<<paramI<<"-"<<tupla.code[paramI]<<endl;
#endif
					tupla.code[paramI] = -1;
					continue;

				}		//salto senza riprenderlo, fissarlo è controproducente
				if ((nCoverable > 0) || ((paramIt < 1) && (paramZero > 0))) {
					//cout<<"AGGIORNO"<<endl;
#ifdef DEBUG2
					if (paramIt==0)
					cout<<"PRIMO PARAMETRO\n";
					cout<<"FISSO PARAMETRO-VALORE: "<<paramI<<"-"<<tupla.code[paramI]<<endl;
#endif
					//aggiorno mdd con questa tupla

					aggiungiAt(mddIndex, tupla.code);
#ifdef DUBLESORT
					if ((paramIt==0)&&(nCoverable<=0)&&(paramZero>0))
					{
						//cout<<"RESTART"<<endl;
						listT.reorderIndexList(paramI,indexListTMP);
						coverable = true;
						restart=true;
						break;//esco ho riordinato indexlist e riparto da capo
					}
#endif
					//
					coverable = true;
				} else {
					//quelli che salto li devo riprendere
					it2->at(1) = (0 - nCoverable); //assegno preso secondario e le riodino così
					indexListTC.push_back(*it2);
					//ordino per un parametro che gli ho associato, cambio it2->at(1) che indica il peso
#ifdef DEBUG2
					cout<<"SALTO PARAMETRO-VALORE: "<<paramI<<"-"<<tupla.code[paramI]<<endl;
#endif
					tupla.code[paramI] = -1;
					coverable = true; //quelli che proprio non vanno bene sono già esclusi
				}
				paramIt++;

			}
			//cout<<"ITERAZIONI "<<partial<<"ms"<<endl;
			if (indexListTC.size()) {
				//sort(indexListTC.begin(),indexListTC.end(),fSortWeight);
				indexListTMP.swap(indexListTC);
			}
			paramZero++;
		}

		//TODO debug stampo codice ridotto
		logcout(LOG_INFO) << mddIndex << endl;
		tupla.print(logcout(LOG_INFO));
		//saturo mdd a un caso di test valido anche nel caso abbia cardinalità maggiore
		vector<cvalue> code = Operations::getCodeFromMdd(list[mddIndex].node);
		codes.push_back(code);
		list[mddIndex].node = Operations::getMDDFromTuple(code, mdd);
		updateCovered(mddIndex, listT, 1); //ciclo su tuplelist per testare quali tuple ho coperto col caso generato
		//listT.removeCovered(); //posso farlo meglio

		logcout(LOG_INFO) << "COPERTI: " << list.at(mddIndex).nCoveredMdd << " WEIGHT: "
				<< list.at(mddIndex).tWeight << endl;

		if ((list.at(mddIndex).nCoveredMdd) == 0)
			return -1;
		//TEST peso specifico
		if ((list.at(mddIndex).tWeight) > tWeightMax) {
			tWeightMax = list.at(mddIndex).tWeight;	///list.at(mddIndex).nCoveredMdd;
			tIndex = z;
			logcout(LOG_INFO) << "Scelto prov" << endl;
		}
		/*	if ((list.at(mddIndex).nCoveredMdd)>tWeightMax)
		 {
		 tWeightMax=list.at(mddIndex).nCoveredMdd;///list.at(mddIndex).nCoveredMdd;
		 tIndex=z;
		 cout<<"Scelto prov"<<endl;
		 }*/
	}
	//reset
	list.at(mddIndex).node = nodoInit;
	list.at(mddIndex).nCoveredMdd = nMddInit;
	list.at(mddIndex).tWeight = weightInit;

	list[mddIndex].node = Operations::getMDDFromTuple(codes[tIndex], mdd);
	updateCovered(mddIndex, listT); //ciclo su tuplelist per testare quali tuple ho coperto col caso generato
	listT.removeCovered(); //posso farlo meglio
	logcout(LOG_INFO) << "*SCELTO*" << endl;
	printMdd(logcout(LOG_INFO), mddIndex);
	logcout(LOG_INFO) << "COPERTI: " << list.at(mddIndex).nCoveredMdd << " WEIGHT: "
			<< list.at(mddIndex).tWeight << endl;

	return 1; //todo provo ciclo completo qui

}
