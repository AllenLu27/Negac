#include <fstream>
#include <string.h>

#include "errors.hpp"
#include "scanner.hpp"
#include "symbol_table.hpp"
#include "3ac.hpp"
#include "ast.hpp"

using namespace negatron;

static void usageAndDie(){
	std::cerr << "Usage: negac <infile> <options>"
	<< " [-t <tokensFile>]"
	<< " [-u <unparseFile>]"
	<< " [-c]"
	<< " [-a <3ACFile>]"
	<< "\n"
	;
	std::cout << std::flush;
	std::cerr << std::flush;
	exit(1);
}

static void doTokenization(std::ifstream * input, const char * outPath){
	negatron::Scanner scanner(input);
	if (strcmp(outPath, "--") == 0){
		scanner.outputTokens(std::cout);
	} else {
		std::ofstream outStream(outPath);
		if (!outStream.good()){
			std::string msg = "Bad output file ";
			msg += outPath;
			throw new negatron::InternalError(msg.c_str());
		}
		scanner.outputTokens(outStream);
	}
}

static negatron::ProgramNode * getAST(std::ifstream * input){
	negatron::ProgramNode * root = nullptr;

	negatron::Scanner scanner(input);
	negatron::Parser parser(scanner, &root);
	int errCode = parser.parse();
	if (errCode != 0) { 
		return nullptr; 
	}
	
	return root;
}

static bool doUnparsing(std::ifstream * input, const char * outPath){
	negatron::ProgramNode * ast = getAST(input);
	if (ast == nullptr){ 
		std::cerr << "No AST built\n";
		return false;
	}
	if (input == nullptr){ 
		return false; 
	}

	if (strcmp(outPath, "--") == 0){
		ast->unparse(std::cout, 0);
	} else {
		std::ofstream outStream(outPath);
		if (!outStream.good()){
			std::string msg = "Bad output file ";
			msg += outPath;
			throw new negatron::InternalError(msg.c_str());
		}
		ast->unparse(outStream, 0);
	}
	return true;
}

static negatron::ProgramNode * doSemanticAnalysis(std::ifstream * input){
	negatron::ProgramNode * ast = getAST(input);
	if (ast == nullptr){ 
		return ast;
	}

	bool res = ast->semanticAnalysis();
	if (!res){
		std::cerr << "Semantic analysis failed\n";
		return nullptr;
	}
	return ast;
}

static void write3AC(negatron::IRProgram * prog, const char * outPath){
	if (outPath == nullptr){
		throw new InternalError("Null 3AC flat file given");
	}
	std::string flatProg = prog->toString();
	if (strcmp(outPath, "--") == 0){
		std::cout << flatProg << std::endl;
	} else {
		std::ofstream outStream(outPath);
		outStream << flatProg << std::endl;
		outStream.close();
	}
}

static IRProgram * do3AC(std::ifstream * input){
	negatron::ProgramNode * ast = doSemanticAnalysis(input);
	if (ast == nullptr){ return nullptr; }
	
	return ast->to3AC();
}

int main(int argc, char * argv[]){
	if (argc <= 1){ usageAndDie(); }
	std::ifstream * input = new std::ifstream(argv[1]);
	if (input == NULL){ usageAndDie(); }
	if (!input->good()){
		std::cerr << "Bad path " <<  argv[1] << std::endl;
		usageAndDie();
	}

	const char * tokensFile = nullptr; // Output file if 
	                                   // printing tokens
	const char * unparseFile = NULL;   // Output file if 
	                                   // unparsing
	bool checkSemantics = false;	   // Flag set if doing 
					   // semantic analysis
	const char * threeACFile = NULL;   // Output file for
					   // 3AC representation
	bool useful = false; // Check whether the command is 
                         // a no-op
	for (int i = 1; i < argc; i++){
		if (argv[i][0] == '-'){
			if (argv[i][1] == 't'){
				i++;
				tokensFile = argv[i];
				useful = true;
			} else if (argv[i][1] == 'u'){
				i++;
				unparseFile = argv[i];
				useful = true;
			} else if (argv[i][1] == 'c'){
				checkSemantics = true;
				useful = true;
			} else if (argv[i][1] == 'a'){
				i++;
				threeACFile = argv[i];
				useful = true;
			} else {
				std::cerr << "Unknown option"
				  << " " << argv[i] << "\n";
				usageAndDie();
			}
		}
	}

	if (useful == false){
		std::cerr << "You didn't specify an operation to do!\n";
		usageAndDie();
	}

	if (tokensFile != nullptr){
		doTokenization(input, tokensFile);
	}
	if (unparseFile != nullptr){
		try {
			doUnparsing(input, unparseFile);
		} catch (negatron::InternalError * e){
			std::cerr << "InternalError: " << e->msg();
			exit(5);
		}
	}
	if (checkSemantics){
		try {
			doSemanticAnalysis(input);
		} catch (negatron::ToDoError * e){
			std::cerr << "ToDoError: " << e->msg() << "\n";
		} catch (negatron::InternalError * e){
			std::cerr << "InternalError: " << e->msg() << "\n";
		}
	}

	if (threeACFile != NULL){
		try {
			auto prog = do3AC(input);
			if (prog != nullptr){
				write3AC(prog, threeACFile);
			}
		} catch (negatron::ToDoError * e){
			std::cerr << "ToDoError: " << e->msg() << "\n";
		} catch (negatron::InternalError * e){
			std::cerr << "InternalError: " << e->msg() << "\n";
		}
	}

	return 0;
}
