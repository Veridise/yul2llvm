#include <fstream>
#include <iostream>
#include <libyul2llvm/TranslateYulToLLVM.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/WithColor.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Verifier.h>
#include <system_error>

namespace cl = llvm::cl;

/// @throw std::system_error if the file cannot be opened
/// @throw nlohmann::json::exception if the JSON cannot be parsed
void readJsonFile(const std::string &filename, nlohmann::json &json) {
  std::ifstream jsonFileStream;
  jsonFileStream.exceptions(std::ios::failbit | std::ios::badbit);
  jsonFileStream.open(filename);
  json = nlohmann::json::parse(jsonFileStream, nullptr, true, true);
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
                
  cl::opt<bool> disableVerifier("d", cl::desc("Disable verification of generated llvm module"),
                                cl::value_desc("disable-verifier"), cl::init(false));

  if (!cl::ParseCommandLineOptions(argc, argv)) {
    return EXIT_FAILURE;
  }

  json rawAST;

  try {
    readJsonFile(inputFile, rawAST);
  } catch (const nlohmann::json::exception &err) {
    llvm::WithColor::error()
        << "Failed to parse contract json file: " << err.what() << "\n";
    return EXIT_FAILURE;
  } catch (const std::system_error &err) {
    // the exception error message is not really helpful, so omit it
    llvm::WithColor::error() << "Failed to open json file\n";
    return EXIT_FAILURE;
  }

  yul2llvm::TranslateYulToLLVM translator(rawAST);
  if (!translator.run()) {
    return EXIT_FAILURE;
  }

  if (!disableVerifier && llvm::verifyModule(*translator.getModule(), &llvm::errs())) {
      llvm::errs() << "\n";
      llvm::WithColor::error() << "internal error: generated IR is broken!\n";
      return EXIT_FAILURE;
  }

  std::error_code fileOpeningError;
  if (outputFile == "-") {
    translator.dumpModule(llvm::outs());
    llvm::errs() << "llvm successfully generated\n";
  } else {
    llvm::raw_fd_ostream fstream(outputFile, fileOpeningError);
    if (fileOpeningError) {
      llvm::WithColor::error(llvm::errs())
          << "Could not open output file for writing: "
          << fileOpeningError.message() << "\n";
      return EXIT_FAILURE;
    } else {
      translator.dumpModule(fstream);
      llvm::errs() << "llvm successfully generated\n";
    }
  }

  return EXIT_SUCCESS;
}