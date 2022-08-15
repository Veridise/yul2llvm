{
  inputs = {
    flake-utils = {
      url = "github:numtide/flake-utils/v1.0.0";
    };
  };

  # Custom colored bash prompt
  nixConfig.bash-prompt = ''\[\e[0;32m\][yul2llvm]\[\e[m\] \[\e[38;5;244m\]\w\[\e[m\] % '';

  outputs = { self, nixpkgs, flake-utils }:
    {
      # First, we define the packages used in this repository/flake
      overlays.default = final: prev: {
        # TODO: package solc

        # Fix the version of LLVM being used.
        yul2llvm_libllvm = final.llvmPackages_13.tools.libllvm;

        # Yul2LLVM C++ component
        yul2llvm_cpp = prev.stdenv.mkDerivation {
          name = "yul2llvm-cpp";
          version = "0.1.0";
          src = builtins.path {
            path = ./.;
            name = "yul2llvm-source";
          };

          nativeBuildInputs = with final; [ cmake ninja ];
          buildInputs = with final; [
            yul2llvm_libllvm
            nlohmann_json
          ];

          doCheck = true;
          checkTarget = "check";
          checkInputs = with final; [ gtest python3 lit ];
        };

        # Yul2LLVM python component
        # Note that this is not really packaged in a nix-compatible way;
        # this is merely a stopgap for now. The right way should be to use
        # something like poetry2nix
        yul2llvm = final.python3.pkgs.buildPythonPackage {
          name = "yul2llvm";
          version = "0.1.0";
          src = builtins.path {
            path = ./.;
            name = "yul2llvm-source";
          };
          format = "pyproject";
          buildInputs = [ final.yul2llvm_cpp ];

          # Currently, we're using the files generated for Eurus's Yul parser.
          # Will probably need to add ANTLR if we need to modify the grammar.
          # nativeBuildInputs = [ final.antlr4_8 ];
        };
      };
    } //
    (flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [ self.overlays.default ];
        };
      in
      {
        # Now, we can define the actual outputs of the flake

        packages = flake-utils.lib.flattenTree {
          # Copy the packages from the overlay.
          inherit (pkgs) yul2llvm yul2llvm_cpp yul2llvm_libllvm;
        };

        devShells = flake-utils.lib.flattenTree {
          # Because `nix develop` is used to set up a dev shell for a given
          # derivation, we just need to extend the derivation with any
          # extra tools we need.
          #
          # The default shell should support both the Python and C++ components.
          # We can't extend yul2llvm (python) directly since that would
          # require us to build yul2llvm_cpp first. The easiest way is to start
          # with the C++ component and then add the Python parts on top.
          default = pkgs.yul2llvm_cpp.overrideAttrs (old: {
            nativeBuildInputs = old.nativeBuildInputs ++ (with pkgs; [
              # Python component dependencies
              (python3.withPackages (p: [
                p.venvShellHook
              ]))

              # C++ dev tools
              doxygen

              # git-clang-format
              libclang.python
            ]);

            # Use Debug by default so assertions are enabled by default.
            cmakeBuildType = "Debug";

            shellHook = ''
              # needed to get accurate compile_commands.json
              export CXXFLAGS="$NIX_CFLAGS_COMPILE";

              # set up virtualenv
              source ${pkgs.python3Packages.venvShellHook}/nix-support/setup-hook
              export venvDir="$PWD"/.venv
              runHook venvShellHook
            '';

            postVenvCreation = ''
              pip install --upgrade pip
              pip install -e '.[dev]'
            '';
          });
        };
      }
    ));
}
