#include "RtMidi.h"
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <optional>
#include <pwd.h>
#include <signal.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <thread>
#include <toml++/impl/node.hpp>
#include <toml++/toml.hpp>
#include <unistd.h>
#include <vector>

#define APP_VERSION "1.0.0"

bool done;
static void finish(int ignore) { done = true; }

int listMidiIO() {

	RtMidiIn *midiin = 0;
	RtMidiOut *midiout = 0;

	// RtMidiIn constructor
	try {
		midiin = new RtMidiIn();
	} catch (RtMidiError &error) {
		error.printMessage();
		exit(EXIT_FAILURE);
	}

	// Check inputs.
	unsigned int nPorts = midiin->getPortCount();
	std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";
	std::string portName;
	for (unsigned int i = 0; i < nPorts; i++) {
		try {
			portName = midiin->getPortName(i);
		} catch (RtMidiError &error) {
			error.printMessage();
			goto cleanup;
		}
		std::cout << "  Input Port #" << i + 1 << ": " << portName << '\n';
	}

	// RtMidiOut constructor
	try {
		midiout = new RtMidiOut();
	} catch (RtMidiError &error) {
		error.printMessage();
		exit(EXIT_FAILURE);
	}

	// Check outputs.
	nPorts = midiout->getPortCount();
	std::cout << "\nThere are " << nPorts << " MIDI output ports available.\n";
	for (unsigned int i = 0; i < nPorts; i++) {
		try {
			portName = midiout->getPortName(i);
		} catch (RtMidiError &error) {
			error.printMessage();
			goto cleanup;
		}
		std::cout << "  Output Port #" << i + 1 << ": " << portName << '\n';
	}
	std::cout << '\n';

	// Clean up
cleanup:
	delete midiin;
	delete midiout;

	return 0;
}

int listenToMidiPort(int port) {
	RtMidiIn *midiin = new RtMidiIn();
	std::vector<unsigned char> message;
	int nBytes, i;
	double stamp;

	// Check if there any ports just in case
	unsigned int nPorts = midiin->getPortCount();
	if (nPorts == 0) {
		std::cout << "No ports available!\n";
		goto cleanup;
	}
	if (nPorts < port) {
		std::cout << "Port does not exist!\n";
		std::cout << "See help page (midi-to-key {--help|h}) for more info.";
	}

	midiin->openPort(port - 1);

	midiin->ignoreTypes(false, false, false);

	done = false;
	(void)signal(SIGINT, finish);

	// Periodically check input queue.
	std::cout << "Reading MIDI from port " << port << " -- quit with Ctrl-C.\n";
	while (!done) {
		stamp = midiin->getMessage(&message);
		nBytes = message.size();
		for (i = 0; i < nBytes; i++)
			std::cout << "Byte " << i << " = " << (int)message[i] << ", ";
		if (nBytes > 0)
			std::cout << "stamp = " << stamp << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

cleanup:
	delete midiin;

	return 0;
}

void helpMessage() {
	std::cout << "Midi To Key - Version " << APP_VERSION << "\n";
	std::cout << "Made by Jack Mechem -- Project Github: "
				 "https://github.com/JackMechem/midi-to-key \n\n";
	std::cout << "Usage:\n";
	std::cout << "  Help and List:\n";
	std::cout << "    midi-to-key {--help|-h} | Shows Help Page\n";
	std::cout << "    midi-to-key {--list-io|-lio} | Lists midi inputs and "
				 "outputs\n";
	std::cout
		<< "    midi-to-key {--input-port|ip} <Port-Number> {--listen|-ln} "
		   "| Listens "
		   "to specified input port and displays midi note registered - Note: "
		   "The {--input-port|-ip} flag must "
		   "be typed before the {--listen|-ln} flag\n\n";
	std::cout << "    Examples:\n";
	std::cout << "      midi-to-key -lio | Lists io\n";
	std::cout << "      midi-to-key -h | Shows help page\n";
	std::cout
		<< "      midi-to-key -ip 2 -ln | Lists input from specified port #2\n";
	std::cout << std::endl << std::endl;
	std::cout << "  Running The Program:\n";
	std::cout
		<< "    midi-to-key run [{--config|-c} </path/to/config>] - Note: "
		   "Default config is $HOME/.config/midi-to-key/config.toml\n";
}

int listenAndMap(std::string configLocation) {

	RtMidiIn *midiin = new RtMidiIn();
	std::vector<unsigned char> message;
	int nBytes, i;
	double stamp;

	struct conf_config {
		std::optional<int> inputPort;
	};

	struct conf_mapping {
		std::string name;
		int byte0;
		int byte1;
		std::string key;
	};
	try {
		toml::table parsedToml = toml::parse_file(configLocation);

		// Config Section
		conf_config configData;
		configData.inputPort = parsedToml["config"]["inputPort"].value<int>();
		if (!configData.inputPort.has_value()) {
			std::cout << "No input value provided!\n";
			return 1;
		}
		std::cout << "Provided midi input port: "
				  << configData.inputPort.value() << std::endl
				  << std::endl;

		// Mapping Section
		std::vector<conf_mapping> parsedMappingData;

		auto mappingTableArray = parsedToml["mapping"];

		toml::array mappingData = *mappingTableArray.as_array();

		std::cout << "Mapping data provided: \n";
		for (const toml::node &mapping : mappingData) {
			toml::table mappingData = *mapping.as_table();
			std::cout << mappingData << std::endl << std::endl;
			conf_mapping parsedMapping;
			parsedMapping.name = mappingData["name"].value_or("NAN");
			parsedMapping.byte0 = mappingData["byte0"].value_or(128);
			parsedMapping.byte1 = mappingData["byte1"].value_or(0);
			parsedMapping.key = mappingData["key"].value_or("f");

			parsedMappingData.push_back(parsedMapping);
		}

		// Check if there any ports just in case
		unsigned int nPorts = midiin->getPortCount();
		if (nPorts == 0) {
			std::cout << "No ports available!\n";
			goto cleanup;
		}
		if (nPorts < configData.inputPort.value()) {
			std::cout << "Port does not exist!\n";
			std::cout
				<< "See help page (midi-to-key {--help|h}) for more info.";
		}

		midiin->openPort(configData.inputPort.value() - 1);

		midiin->ignoreTypes(false, false, false);

		done = false;
		(void)signal(SIGINT, finish);

		// Periodically check input queue.
		std::cout << "Reading MIDI from port " << configData.inputPort.value()
				  << " -- quit with Ctrl-C.\n";
		while (!done) {
			stamp = midiin->getMessage(&message);
			nBytes = message.size();
			for (i = 0; i < nBytes; i++) {
				std::cout << "Byte " << i << " = " << (int)message[i] << ", ";
			}
			if (nBytes > 0)
				std::cout << "stamp = " << stamp << std::endl;
			if (nBytes > 0) {
				for (int v = 0; v <= parsedMappingData.size() - 1; v++) {
					conf_mapping newMapping = parsedMappingData[v];
					if (newMapping.byte0 == (int)message[0] &&
						newMapping.byte1 == (int)message[1]) {
						std::cout
							<< "Triggering mapping named: " << newMapping.name
							<< std::endl
							<< std::endl;
						system((newMapping.key + " &").c_str());
					}
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

	cleanup:
		delete midiin;

		return 0;

	} catch (const toml::parse_error &err) {
		std::cerr << "Error parsing file '" << err.source().path << "':\n"
				  << err.description() << "\n (" << err.source().begin << ")\n";
		return 1;
	}

	return 0;
}

int main(int argc, char *argv[]) {

	int inPort = 0;
	struct passwd *pw = getpwuid(getuid());

	const std::string homedir = pw->pw_dir;

	std::string config = homedir + "/.config/midi-to-key/config.toml";

	if (argc > 1) {
		for (int i = 0; i <= argc - 1; i++) {
			std::string arg = argv[i];

			if (arg == "run") {
				// Get config argument
				for (int u = 0; u <= argc - 1; u++) {
					std::string argtwo = argv[u];
					if (argtwo == "--config" || argtwo == "-c") {
						config = argv[u + 1];
						std::cout << "Provided config location: " << config
								  << std::endl;
						break;
					}
				}
				listenAndMap(config);
			}

			// Prints help message and exits program
			if (arg == "--help" || arg == "-h") {
				helpMessage();
				return 0;
			}

			// Lists all midi inputs and outputs and exits the program
			if (arg == "--list-io" || arg == "-lio") {
				listMidiIO();
				return 0;
			}

			// Sets the input midi port number
			if (arg == "--input-port" || arg == "-ip") {

				// Next argument
				std::string narg = argv[i + 1];

				try {
					std::size_t pos;
					inPort = std::stoi(narg, &pos);
					if (pos < narg.size()) {
						std::cerr << "Trailing characters after input port "
									 "number: "
								  << narg << '\n';
						return 1;
					}
				} catch (std::invalid_argument const &ex) {
					std::cerr << "Invalid input port number: " << narg << '\n';
					return 1;
				} catch (std::out_of_range const &ex) {
					std::cerr << "Input port number out of range: " << narg
							  << '\n';
					return 1;
				}

				std::cout << "Input port number provided: " << inPort
						  << std::endl;
				// Go to next argument as to skip the value given
				i++;
			}

			if (arg == "--listen" || arg == "-ln") {
				listenToMidiPort(inPort);
			}
		}
	} else {
		helpMessage();
	}

	return 0;
}
