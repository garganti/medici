#include <header.h>
#include "boost/program_options.hpp"
#include <Settings.h>
#include <MEDICI.h>
#include <logger.hpp>

#define HELP "help"
#define AUTOVALIDATE "autovalidate"
#define SILENT "silent"
#define DONOTGENERATE "donotgenerate"

namespace po = boost::program_options;
using namespace std;
using namespace MEDDLY;

int main(int argc, char *argv[]) {
	Settings setting;
	vector<int> res(3);
	try {


		po::options_description desc("Allowed options");
		desc.add_options()
	            		(HELP, "produce help message")
	            		("casa",  "set input in casa format")
	            		("m", po::value<string>(), "set model file name")
	            		("o", po::value<string>(), "set output file name")
	            		("c", po::value<string>(), "set constraints file name, only for casa mode")
	            		("actsconv", "enable acts model conversion mode")
	            		("mname", po::value<string>(), "set model name for acts mode")
	            		("validate", "validating model mode")
	            		(AUTOVALIDATE, "autovalidate model")
	            		(SILENT, "be silent")
	            		(DONOTGENERATE, "build mdds but do not generate test suite")
	            		("mintries", po::value<int>(), "set mintries [1]")
	            		("tries", po::value<int>(), "set max tries [1]")
	            		("bettertries", po::value<int>(), "set number of better tries [1]")
	            		("noqueuecheck", "disable check the queue for early termination")
	            		("nit1", po::value<int>(), "set iterations over threshold [1]")
	            		("nit2", po::value<int>(), "set iterations under threshold [2]")
	            		("threshold", po::value<int>(), "set threshold [2000]" ) // Settings.THRESHOLD_DEFAULT
	            		("wexp", po::value<double>(), "set weight exponent over threshold [1]")
	            		("wexp2", po::value<double>(), "set weight exponent under threshold [1.5]")
	            		;
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count(HELP)) {
			cout << desc << "\n";
			return 0;
		}

		if (vm.count("casa")) {
			cout << "Casa mode enabled \n";
			setting.casa=true;
			// << vm["compression"].as<double>() << ".\n";
		}
		if (vm.count("validate")) {
			cout << "Validating mode enabled \n";
			setting.validate=true;
			MEDICI::validateMode(setting);

		}
		else if (vm.count("actsconv")) {
			cout << "ACTS model conversion enabled \n";
			setting.actsconv=true;
			if (vm.count("mname")) {
				strcpy(setting.mname,vm["o"].as<string>().c_str());
			}
			MEDICI::actsMode(setting);
		}
		else {
			//NORMAL MODE
			//cout<<"NORMAL MODE \n";
			if (!(vm.count("m"))) {
				cout<<"Model not set \n";
				return 0;
			}
			strcpy(setting.model,vm["m"].as<string>().c_str());
			if (vm.count("o")) {
				strcpy(setting.out,vm["o"].as<string>().c_str());
			}
			else
			{
				strcpy(setting.out,setting.model);
				strcat(setting.out, ".out");
			}
			if (setting.casa)
			{
				if (vm.count("c")) {
					strcpy(setting.constraint,vm["c"].as<string>().c_str());
				}
			}
			if (vm.count(AUTOVALIDATE)) {
				setting.autovalidate=true;
			}
			if (vm.count(SILENT)) {
				threshold = LOG_NOTHING;
			} else{
				threshold = LOG_INFO;
			}
			if (vm.count(DONOTGENERATE)) {
				setting.donotgenerate = true;
			} else{
				setting.donotgenerate = false;
			}

			if (vm.count("mintries")) {
				setting.mintries=vm["mintries"].as<int>();
			}
			if (vm.count("tries")) {
				setting.tries=vm["tries"].as<int>();
			}
			if (vm.count("bettertries")) {
				setting.bettertries=vm["bettertries"].as<int>();
			}
			if (vm.count("noqueuecheck")) {
				setting.queuecheck=false;
			}
			if (vm.count("nit1")) {
				setting.nit1=vm["nit1"].as<int>();
			}
			if (vm.count("nit2")) {
				setting.nit2=vm["nit2"].as<int>();
			}
			if (vm.count("threshold")) {
				setting.threshold=vm["threshold"].as<int>();
			}
			if (vm.count("wexp")) {
				setting.wexp=vm["wexp"].as<double>();
			}
			if (vm.count("wexp2")) {
				setting.wexp2=vm["wexp2"].as<double>();
			}


			MEDICI::normalMode(setting, res);
		}
	}
	catch(exception& e) {
		cerr << "error: " << e.what() << "\n";
		return 1;
	}
	catch(...) {
		cerr << "Exception of unknown type!\n";
	}

	//FINE PARSING PARAMETRI

   return 0;

}

