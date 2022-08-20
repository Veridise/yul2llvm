{
  inputs = {
    flake-utils = {
      url = "github:numtide/flake-utils/v1.0.0";
    };

    veridise-pkgs.url = "git+ssh://git@github.com/Veridise/veridise-nix-pkgs.git?ref=main";
    veridise-pkgs.inputs.nixpkgs.follows = "nixpkgs";
  };

  # Custom colored bash prompt
  nixConfig.bash-prompt = ''\[\e[0;32m\][yul2llvm]\[\e[m\] \[\e[38;5;244m\]\w\[\e[m\] % '';

  outputs = { self, nixpkgs, flake-utils, veridise-pkgs }:
    {
      # First, we define the packages used in this repository/flake
      overlays.default =
        let
          yul2llvm_srcs = final:
            final.lib.cleanSourceWith {
              filter = name: type: !(final.lib.strings.hasSuffix ".nix" name);
              src = final.lib.cleanSource ./.;
            };
        in
        final: prev: {
        # Fix the version of LLVM being used.
        yul2llvm_libllvm = final.llvmPackages_13.tools.libllvm;

        # Yul2LLVM C++ component
        yul2llvm_cpp = prev.stdenv.mkDerivation {
          name = "yul2llvm-cpp";
          version = "0.1.0";
          src = builtins.path {
            # Exclude python files
            path =
              let src0 = yul2llvm_srcs final;
              in
                final.lib.cleanSourceWith {
                  filter = path: type: !(
                    (type == "directory" && path == toString (src0.origSrc + "/src")) ||
                    (path == toString (src0.origSrc + "/pyproject.toml"))
                  );
                  src = src0;
                };
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
        yul2llvm = (final.poetry2nix.mkPoetryApplication {
          name = "yul2llvm";
          version = "0.1.0";
          projectDir = ./.;
          src = builtins.path {
            # Don't include C++ parts
            path =
              let src0 = yul2llvm_srcs final;
              in
                final.lib.cleanSourceWith {
                  filter = path: type: !(final.lib.lists.any (x: x) [
                    (baseNameOf path == "CMakeLists.txt")
                    (type == "directory" && path == toString (src0.origSrc + "/lib"))
                    (type == "directory" && path == toString (src0.origSrc + "/cmake"))
                  ]);
                  src = src0;
                };
            name = "yul2llvm-source";
          };

          overrides = final.poetry2nix.overrides.withDefaults (self: super: {
            # poetry2nix's mypy override doesn't work
            mypy = null;

            # cytoolz needs cython to build, and poetry2nix doesn't have an override
            cytoolz = super.cytoolz.overridePythonAttrs (attrs: {
              nativeBuildInputs = (attrs.nativeBuildInputs or []) ++ [ self.cython ];
            });
          });

          propagatedBuildInputs = [ final.solc_0_8_15 final.yul2llvm_cpp ];
        }).overridePythonAttrs (old: {
          doCheck = true;
          checkInputs = [
            old.passthru.python.pkgs.pytest
            final.lit
            final.yul2llvm_libllvm
            final.yul2llvm_cpp
          ];
          checkPhase = ''
            echo "Executing pytest"
            PATH="$out"/bin:"$PATH" pytest

            echo
            echo "Executing lit tests"
            PATH="$out"/bin:"$PATH" lit ./tests -v
          '';
        });
      };
    } //
    (flake-utils.lib.eachSystem ["aarch64-darwin" "x86_64-darwin" "x86_64-linux"] (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [ veridise-pkgs.overlays.default self.overlays.default ];
        };
      in
      {
        # Now, we can define the actual outputs of the flake

        packages = flake-utils.lib.flattenTree {
          # Copy the packages from the overlay.
          inherit (pkgs) yul2llvm yul2llvm_cpp yul2llvm_libllvm solc_0_8_15;
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
              solc_0_8_15

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
