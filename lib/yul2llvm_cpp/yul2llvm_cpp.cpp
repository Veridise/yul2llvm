#include <fstream>
#include <libyul2llvm/TranslateYulToLLVM.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

namespace cl = llvm::cl;

int readJsonData(std::string filename, json &rawAST) {
  std::ifstream jsonFileStream(filename);
  try {
    rawAST = nlohmann::json::parse(jsonFileStream);
    return rawAST;
  } catch (...) {
    llvm::outs() << "Could not parse json read from ";
    llvm::outs() << filename << "\n";
    return -1;
  }
  return 0;
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
  cl::opt<std::string> outputFile("o", cl::desc("Output file location"));

  if (!cl::ParseCommandLineOptions(argc, argv)) {
    return EXIT_FAILURE;
  }

  json rawAST;

  if (!readJsonData(inputFile, rawAST)) {
    return EXIT_FAILURE;
  }

  yul2llvm::TranslateYulToLLVM translator(rawAST);
  translator.run();
  translator.dumpFunctionsToFile(outputFile);

  return EXIT_SUCCESS;
}
