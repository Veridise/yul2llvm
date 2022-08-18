#include <iostream>
#include <fstream>
#include <libyul2llvm/TranslateYulToLLVM.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/WithColor.h>
#include <llvm/Support/raw_ostream.h>

namespace cl = llvm::cl;

int readJsonData(std::string filename, json &rawAST) {
  std::ifstream jsonFileStream(filename);
  try {
    rawAST = nlohmann::json::parse(jsonFileStream, nullptr, true, true);
    return 0;
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
  cl::opt<std::string> outputFile("o", cl::desc("Output file location"),
                                  cl::value_desc("filename"), cl::init("-"));

  if (!cl::ParseCommandLineOptions(argc, argv)) {
    return EXIT_FAILURE;
  }

  json rawAST;

  if (readJsonData(inputFile, rawAST)) {
    return EXIT_FAILURE;
  }

  yul2llvm::TranslateYulToLLVM translator(rawAST);
  translator.run();

  std::error_code fileOpeningError;
  if (outputFile == "-") {
    translator.dumpFunctions(llvm::outs());
  } else {
    llvm::raw_fd_ostream fstream(outputFile, fileOpeningError);
    if (fileOpeningError) {
      llvm::WithColor::error(llvm::errs())
          << "Could not open output file for writing: "
          << fileOpeningError.message() << "\n";
      return EXIT_FAILURE;
    } else {
      translator.dumpFunctions(fstream);
    }
  }

  llvm::errs() << "llvm successfully generated\n";
  return EXIT_SUCCESS;
}
