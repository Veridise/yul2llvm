#include <libyul2llvm/TranslateYulToLLVM.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>
#include <fstream>


namespace cl = llvm::cl;

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({});

  // If this is null, read from stdin
  // TODO: There should be a CommandLine API that does this...
  cl::opt<std::string> inputFile(cl::Positional, cl::desc("<filename>"), cl::Optional);

  // By default, print to stdout. Otherwise, write to this file.
  // TODO: There should be a CommandLine API that does this...
  cl::opt<std::string> outputFile("o", cl::desc("Output file location"));

  if (!cl::ParseCommandLineOptions(argc, argv)) {
    return EXIT_FAILURE;
  }

  // TODO: placeholder
  yul2llvm::TranslateYulToLLVM translator("/Users/ajinkya/Workspace/yul-playground/function-add.json");
  
  translator.run(NULL);

  llvm::outs() << "Hello world!\n";
  return EXIT_SUCCESS;
}
