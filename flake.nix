{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.11";

    flake-parts = {
      url = "github:hercules-ci/flake-parts";
      inputs.nixpkgs-lib.follows = "nixpkgs";
    };

    treefmt-nix = {
      url = "github:numtide/treefmt-nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };


    nix-github-actions = {
      url = "github:nix-community/nix-github-actions";
      inputs.nixpkgs.follows = "nixpkgs";
    };

    zephyr-nix = {
      url = "github:OSSystems/zephyr-nix/pending";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = inputs@{ flake-parts, ... }:
    let
      inherit (inputs.nixpkgs) lib;
      inherit (inputs) self;
    in
    flake-parts.lib.mkFlake { inherit inputs; } {
      systems = lib.systems.flakeExposed;

      imports = [
        inputs.treefmt-nix.flakeModule
      ];

      flake.githubActions = inputs.nix-github-actions.lib.mkGithubMatrix {
        checks = {
          inherit (self.checks) x86_64-linux;
        };
      };

      perSystem = { config, inputs', pkgs, ... }:
        let
          zephyr = inputs'.zephyr-nix.packages;
        in
        {
          treefmt.config = {
            projectRootFile = "flake.nix";

            programs = {
              black.enable = true;
              clang-format = {
                enable = true;
                package = pkgs.clang-tools_17;
              };
              nixpkgs-fmt.enable = true;
              shfmt.enable = true;
            };
          };

          devShells.default = pkgs.mkShell {
            inputsFrom = [
              config.treefmt.build.devShell
            ];

            packages = [
              (zephyr.sdk.override {
                targets = [
                  "arm-zephyr-eabi"
                ];
              })

              (zephyr.pythonEnv.override {
                extraLibs = [
                  zephyr.pythonEnv.pkgs.stringcase
                ];
              })

              zephyr.hosttools

              pkgs.cmake
              pkgs.ninja
            ];
          };
        };
    };
}
