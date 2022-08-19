#include <fstream>
#include <iostream>
#include <libyul2llvm/TranslateYulToLLVM.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/WithColor.h>
#include <llvm/Support/raw_ostream.h>
#include <system_error>

namespace cl = llvm::cl;

/// @throw std::system_error if the file cannot be opened
/// @throw nlohmann::json::exception if the JSON cannot be parsed
void readJsonData(const std::string &filename, nlohmann::json &rawAST) {
  std::ifstream jsonFileStream;
  jsonFileStream.exceptions(std::ios::failbit | std::ios::badbit);
  jsonFileStream.open(filename);
  rawAST = nlohmann::json::parse(jsonFileStream, nullptr, true, true);
}

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({});

  // If this is null, read from stdin
  // @todo There should be a CommandLine API that does this...
  cl::opt<std::string> inputFile(
      cl::Positional,
      cl::desc("<filename> : Input json file name conatining preprocessed AST"),
      cl::Required);

  // By default, print to stdout. Otherwise, write to this file.
  // @todo There should be a CommandLine API that does this...
  cl::opt<std::string> outputFile("o", cl::desc("Output file location"),
                                  cl::value_desc("filename"), cl::init("-"));

  if (!cl::ParseCommandLineOptions(argc, argv)) {
    return EXIT_FAILURE;
  }

  json rawAST;

  try {
    readJsonData(inputFile, rawAST);
  } catch (const nlohmann::json::exception &err) {
    llvm::WithColor::error()
        << "Failed to parse json file: " << err.what() << "\n";
    return EXIT_FAILURE;
  } catch (const std::system_error &err) {
    // the exception error message is not really helpful, so omit it
    llvm::WithColor::error() << "Failed to open json file\n";
    return EXIT_FAILURE;
  }

  yul2llvm::TranslateYulToLLVM translator(rawAST);
  translator.run();
  translator.dumpFunctionsToFile(outputFile);
  llvm::outs() << "llvm successfully generated";
  return EXIT_SUCCESS;
}
