/*
 * MEDICI.cpp
 *
 *  Created on: 29/mar/2013
 *      Author: Dario
 */

#include "MEDICI.h"
#include "logger.hpp"

MEDICI::MEDICI() {
}

MEDICI::~MEDICI() {
}

int MEDICI::normalMode(Settings setting, vector<int> &res) {
	char outputStats[1000];
	srand(time(0));
	vector<int> resSize;
	settings s;
	s.computeTable.maxSize = 4000;
	strcpy(outputStats, setting.out);
	strcat(outputStats, ".stats");
	int64 now;
	int64 partial = 0;
	int64 partial2;
	initialize(s);

	now = Operations::getTimeMs64();
	TupleList tupleList(setting);

	vector<Operations::TestModel> testModel;
	vector<std::list<Operations::TestModelConstraint> > testModelC;
	int nWise;
	if (Operations::testModelFromFile(testModel, setting.model, nWise) == -1) {
		cout << "Error reading file " << setting.model << endl;
	}
	if (strlen(setting.constraint) && (setting.casa))
		Operations::testModelConstraintsFromFileCASA(testModelC,
				setting.constraint);
	else
		Operations::testModelConstraintsFromFileMEDICI(testModelC,
				setting.model);
	int nel = 0;
	//Trovo numero parametri
	for (std::vector<Operations::TestModel>::iterator it = testModel.begin(),
			end = testModel.end(); it != end; ++it) {
		nel += it->parameters;
	}

	const uint N = nel; //n variabili o livelli
	int bounds[N];
	int iterazioni = 0;

	Operations::generateBounds(testModel, nel, bounds);

	tupleList = TupleList::generateNWise(bounds, N, nWise, setting);

#ifdef TEST
	cout<<"CHECK tuples: "<<TupleList::CheckTuples(tupleList)<<endl;

#endif

	tupleList.setNValues(bounds, N);

	logcout(LOG_INFO) << "time execution tuple generator " << nWise << "-wise "
			<< Operations::getTimeMs64() - now << "ms" << endl;
	tupleList.nToCover = tupleList.size(); //init tutte da coprire
	tupleList.nCovered = 0;
	tupleList.nTotal = tupleList.size();
	//exit(0);
	cleanup(); //cleanup dopo uso per generazione nwise
	//INIT
	initialize(s);
	logcout(LOG_INFO) << getLibraryInfo() << endl;

	// Create a domain
	domain* d = createDomain();
	assert(d != 0);

	// Create variable in the above domain
	d->createVariablesBottomUp(bounds, N);

	logcout(LOG_INFO) << "Created domain with " << d->getNumVariables() << " variables\n";

	// Create a forest in the above domain
	forest* mdd = d->createForest(false,               // this is not a relation
			forest::BOOLEAN,          // terminals are either true or false
			forest::MULTI_TERMINAL    // disables edge-labeling
			);
	assert(mdd != 0);
	//init mddlist
	MDDList mddList(mdd);     //lista mdd creati
	MDDList mddListBest(mdd);     //lista mdd creati

	if (setting.casa)
		mddList.updateConstraints(testModelC, bounds, nel, tupleList);
	else
		mddList.updateConstraintsMEDICI(testModelC, bounds, nel, tupleList);

	// controlla che esita almeno una soluzione
	if (!mddList.nodoBase.getCardinality()) //constraints non compatibili con modello
	{
		cout << "ERRORE constraints generano modello sempre falso" << endl;
		return 0;
	}

	mddList.aggiungiInCoda();

	// Display forest properties
	logcout(LOG_INFO) << "Created forest in this domain with:" << "\n  Relation:\tfalse"
			<< "\n  Range Type:\tBOOLEAN" << "\n  Edge Label:\tMULTI_TERMINAL"
			<< "\n";

	logcout(LOG_INFO) << "time execution domain generator"
			<< Operations::getTimeMs64() - now << "ms" << endl;

	mdd->removeAllComputeTableEntries();

	logcout(LOG_INFO) << "Controllo preliminare di copertura per le tuple..." << endl;
	mddList.checkCoverable(mdd, tupleList);

	logcout(LOG_INFO) << "size dopo controllo copribilita " << tupleList.size() << endl;
	TupleList listBak = tupleList;
	vector<MDDList::Node> listMddBak = mddList.list;
	// numero di tentativo
	int tryN = 0;
	int minSize = -1;
	int minSizeI = -1;
	bool codeCheck = false;
	// in case do not generate
	if (setting.donotgenerate) return 0;
	// main algorithm
	for (tryN = 0; tryN < setting.tries; tryN++){
		tupleList = listBak;
		mddList.list = listMddBak;
		//mddList.list.clear();
		iterazioni = 0;
		logcout(LOG_INFO) << "Inizio algoritmo iterativo per generazione casi di test ... tentativo " << tryN+1 << endl;
		if (tupleList.size() <= setting.threshold) {
			tupleList.checkCompatibility(mdd, mddList.nodoBase);
			tupleList.checkParamListNew(mdd);
		} else {
			tupleList.checkParamList(mdd); //Se non lo faccio non converge neanche più
		}
		partial2 = Operations::getTimeMs64();
		tupleList.sortCompatibility();
		partial += Operations::getTimeMs64() - partial2;
		logcout(LOG_INFO) << "TIME INTERVAL " << Operations::getTimeMs64() - partial2 << endl;
		int nit;
		while (tupleList.size() > 0) //ripeto fino a convergenza
		{
			// set the right nit
			if (tupleList.size() <= setting.threshold) {
				nit = setting.nit2;
			} else{
				nit = setting.nit1;
			}
			//
			if (mddList.addOther2b(tupleList, bounds, nit) <= 0) {
					cout << "ERROR" << endl;
					break;
			}
			iterazioni++;
			//
			if (tupleList.size() <= setting.threshold) {
				tupleList.checkCompatibility(mdd, mddList.nodoBase);
				tupleList.checkParamListNew(mdd);
			} else
				tupleList.checkParamList(mdd); //Se non lo faccio non converge neanche più

			logcout(LOG_INFO) << "TUPLE RIMANENTI " << tupleList.size() << endl;
			if (tupleList.size() < 0) {
				// errore non dovrebbe accadere
				tupleList.print(logcout(LOG_INFO));
			}
		}
		// In caso di errore (non dovrebbe accadere)
		if (tupleList.size() > 0) {
			tupleList.print(logcout(LOG_INFO));
		}
		int currentTSSize = mddList.list.size() - 1;
		// store current size
		resSize.push_back(currentTSSize);
		// check if this is the best test suit
		if ((minSize == -1) || (currentTSSize < minSize)) {
			minSize = currentTSSize;
			minSizeI = tryN;
			mddListBest = mddList;
			// controllo code (AG)
			if (setting.queuecheck && mddList.list.size() >= 3) {
				int codeSizeLast =
						mddList.list[mddList.list.size() - 2].nCoveredMdd;
				int codeSizePrev =
						mddList.list[mddList.list.size() - 3].nCoveredMdd;
				if ((codeSizeLast > 2) && (codeSizePrev > 2))
					codeCheck = true;
				else
					codeCheck = false;
				//break; //non itero più se la coda è numerosa
			}
		}
		// se non ho raggiunto il minmi, continua comunque
		if (tryN < setting.mintries)
			continue;
		if (setting.queuecheck && codeCheck){
			logcout(LOG_INFO) << "exiting - per controllo code" << endl;
			break; //se il migliore ha passato il controllo code
		}

		//CHECK if result is improved in the last tries
		if (tryN > setting.bettertries) {
			int senzaMiglioramento = resSize.size() - minSizeI;
			if( senzaMiglioramento > setting.bettertries){
				logcout(LOG_INFO) << "exiting - non migliorata per " << senzaMiglioramento << " > " << setting.bettertries	<< endl;
				break;
			}
		}

	}

	logcout(LOG_INFO) << "***********************Elenco nodi risultati*************************************** \n";

	mddListBest.printList(logcout(LOG_INFO));

	logcout(LOG_INFO) << "Tuple coperte: " << tupleList.nCovered << " Tuple da coprire: "
			<< tupleList.nToCover << " Tuple non copribili: "
			<< tupleList.nUncoverable << endl;
	//cout << "Chiuso in : " << iterazioni-- << " iterazioni" << endl;
	logcout(LOG_INFO) << "Ottenuti: " << minSize << " mdd" << endl;
	logcout(LOG_INFO) << "Tempo per ordinamenti: " << partial << "ms" << endl;

	mdd->garbageCollect();
	//cout<<outputFile<<"***********"<<endl;
	mddListBest.listToFile(setting.out, bounds);
	//Salvataggio stats su file
	ofstream fout;

	fout.open(outputStats);

	if (!fout.is_open()) {

		perror("Errore in apertura del file per stats");
		return 0;
	}
	fout << "****Model name: " << setting.model << " *******" << endl;
	fout << "Tuple coperte: " << tupleList.nCovered << " Tuple da coprire: "
			<< tupleList.nToCover << " Tuple non copribili: "
			<< tupleList.nUncoverable << endl;
	//fout << "Chiuso in : " << iterazioni-- << " iterazioni" << endl;
	//fout << "Ottenuti: " << mddList.list.size() - 1 << " mdd" << endl;
	fout << "Ottenuti: " << minSize << " mdd" << endl;
	fout << "time execution script " << Operations::getTimeMs64() - now << "ms" << endl;
	fout << "Tempo per ordinamenti: " << partial << "ms" << endl;

	fout.close();

	destroyDomain(d);

	cleanup();

	logcout(LOG_INFO) << "time execution script " << Operations::getTimeMs64() - now << "ms"
			<< endl;
	res = vector<int>(3, 0);
	//result[0]=mddList.list.size() - 1;
	res[0] = minSize;
	res[1] = Operations::getTimeMs64() - now;
	res[2] = tryN;
	if (setting.autovalidate) //autovalidazione
		return validateMode(setting);
	return 0;
}

int MEDICI::validateMode(Settings setting) {
	srand(time(0));
	settings s;
	s.computeTable.maxSize = 4000;
	int64 now;

	initialize(s);

	now = Operations::getTimeMs64();
	TupleList tupleList(setting);

	vector<Operations::TestModel> testModel;
	vector<std::list<Operations::TestModelConstraint> > testModelC;
	int nWise;
	if (Operations::testModelFromFile(testModel, setting.model, nWise) == -1) {
		cout << "Error reading file " << setting.model << endl;
	}
	if (strlen(setting.constraint) && (setting.casa))
		Operations::testModelConstraintsFromFileCASA(testModelC,
				setting.constraint);
	else
		Operations::testModelConstraintsFromFileMEDICI(testModelC,
				setting.model);
	int nel = 0;
	//Trovo numero parametri
	for (std::vector<Operations::TestModel>::iterator it = testModel.begin(),
			end = testModel.end(); it != end; ++it) {
		nel += it->parameters;
	}

	const uint N = nel; //n variabili o livelli
	int bounds[N];

	Operations::generateBounds(testModel, nel, bounds);

	tupleList = TupleList::generateNWise(bounds, N, nWise, setting);

#ifdef TEST
	cout<<"CHECK tuples: "<<TupleList::CheckTuples(tupleList)<<endl;

#endif

	tupleList.setNValues(bounds, N);

	logcout(LOG_INFO) << "time execution generato " << nWise << "-wise "
			<< Operations::getTimeMs64() - now << "ms" << endl;
	tupleList.nToCover = tupleList.size(); //init tutte da coprire
	tupleList.nCovered = 0;
	tupleList.nTotal = tupleList.size();
	//exit(0);
	cleanup(); //cleanup dopo uso per generazione nwise
	//INIT
	initialize(s);
	logcout(LOG_INFO) << getLibraryInfo() << endl;

	// Create a domain
	domain* d = createDomain();
	assert(d != 0);

	// Create variable in the above domain
	d->createVariablesBottomUp(bounds, N);

	logcout(LOG_INFO) << "Created domain with " << d->getNumVariables() << " variables\n";

	// Create a forest in the above domain
	forest* mdd = d->createForest(false,               // this is not a relation
			forest::BOOLEAN,          // terminals are either true or false
			forest::MULTI_TERMINAL    // disables edge-labeling
			);
	assert(mdd != 0);
	//init mddlist
	MDDList mddList = MDDList::listFromFile(setting.out, bounds, nel, mdd);

	if (setting.casa)
		mddList.updateConstraints(testModelC, bounds, nel, tupleList);
	else
		mddList.updateConstraintsMEDICI(testModelC, bounds, nel, tupleList);

	if (!mddList.nodoBase.getCardinality()) //constraints non compatibili con modello
	{
		cout << "ERRORE constraints generano modello sempre falso" << endl;
		return 0;
	}

	int size = mddList.list.size();

	for (int i = 0; i < size; i++) {
		tupleList.checkParamList(mdd);
		tupleList.removeCovered();
		mddList.updateCovered(i, tupleList, 0);
	}
	mddList.aggiungiInCoda(); //service
	tupleList.removeCovered();
	mddList.printList(logcout(LOG_INFO));
	logcout(LOG_INFO) << "Tuple covered: " << tupleList.nCovered << endl;
	logcout(LOG_INFO) << "Tuple not coverable: " << tupleList.nTotal - tupleList.nCovered
			<< endl;
	logcout(LOG_INFO) << "Mdd number: " << mddList.list.size() - 1 << endl;

	mddList.checkCoverable(mdd, tupleList);
	int retSize = tupleList.size();
	destroyDomain(d);
	cleanup();
	if (retSize == 0) {
		logcout(LOG_INFO) << "MODEL VALIDATED CORRECTLY" << endl;
		return 0;
	}
	logcout(LOG_INFO) << "ERROR TUPLE STILL TO COVER: " << retSize << endl;

	return retSize;
}

int MEDICI::actsMode(Settings setting) {
	if (setting.casa == false) {
		logcout(LOG_INFO) << "actually implemented only for casa model file" << endl;
		return 0;
	}

	//	TupleList tupleList;
	//	initialize();

	vector<Operations::TestModel> testModel;
	vector<std::list<Operations::TestModelConstraint> > testModelC;
	int nWise;
	if (Operations::testModelFromFile(testModel, setting.model, nWise) == -1) {
		cout << "Error reading file " << setting.model << endl;
	}
	if (strlen(setting.constraint) && (setting.casa))
		Operations::testModelConstraintsFromFileCASA(testModelC,
				setting.constraint);
	else
		Operations::testModelConstraintsFromFileMEDICI(testModelC,
				setting.model);

	int nel = 0;
	//Trovo numero parametri
	for (std::vector<Operations::TestModel>::iterator it = testModel.begin(),
			end = testModel.end(); it != end; ++it) {
		nel += it->parameters;
	}

	const uint N = nel; //n variabili o livelli
	int bounds[N];

	Operations::generateBounds(testModel, nel, bounds);
	Operations::toACTSModelConversion(testModel, testModelC, bounds, N,
			setting.out, setting.mname);

	return 0; //should be 0
}
