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
void readContract(const std::string &filename, nlohmann::json &rawAST) {
  std::ifstream jsonFileStream;
  jsonFileStream.exceptions(std::ios::failbit | std::ios::badbit);
  jsonFileStream.open(filename);
  json = nlohmann::json::parse(jsonFileStream, nullptr, true, true);
}

void readStorageLayout(const std::string &storageLayoutFilename, nlohmann::json &storageLayout) {
  std::ifstream jsonFileStream;
  jsonFileStream.exceptions(std::ios::failbit | std::ios::badbit);
  jsonFileStream.open(storageLayoutFilename);
  storageLayout = nlohmann::json::parse(jsonFileStream, nullptr, true, true);
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

  cl::opt<std::string> storageLayoutFile("s", cl::desc("Storage layout files"),
                                  cl::value_desc("storage-layout"), cl::init("-"), cl::ValueOptional);

  if (!cl::ParseCommandLineOptions(argc, argv)) {
    return EXIT_FAILURE;
  }

  json rawAST;
  json storageLayout;

  try {
    readContract(inputFile, rawAST);
  } catch (const nlohmann::json::exception &err) {
    llvm::WithColor::error()
        << "Failed to parse contract json file: " << err.what() << "\n";
    return EXIT_FAILURE;
  } catch (const std::system_error &err) {
    // the exception error message is not really helpful, so omit it
    llvm::WithColor::error() << "Failed to open json file\n";
    return EXIT_FAILURE;
  }

  if(storageLayoutFile != "-"){
    try {
      readStorageLayout(storageLayoutFile, storageLayout);
    } catch (const nlohmann::json::exception &err) {
      llvm::WithColor::error()
          << "Failed to parse storageLayout json file: when provided" << err.what() << "\n";
      return EXIT_FAILURE;
    } catch (const std::system_error &err) {
      // the exception error message is not really helpful, so omit it
      llvm::WithColor::error() << "Failed to open json file\n";
      return EXIT_FAILURE;
    }
  }
  yul2llvm::TranslateYulToLLVM translator(rawAST, storageLayout);
  translator.run();

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