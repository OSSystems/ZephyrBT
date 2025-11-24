{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.05";

    flake-parts = {
      url = "github:hercules-ci/flake-parts";
      inputs.nixpkgs-lib.follows = "nixpkgs";
    };

    treefmt-nix = {
      url = "github:numtide/treefmt-nix/246639a1ec081bb40941a25e9eb8481a66d71b49";
      inputs.nixpkgs.follows = "nixpkgs";
    };

    nix-github-actions = {
      url = "github:nix-community/nix-github-actions";
      inputs.nixpkgs.follows = "nixpkgs";
    };

    zephyr-nix = {
      url = "github:nix-community/zephyr-nix";
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
                package = pkgs.clang-tools_19;
              };
              nixpkgs-fmt.enable = true;
              shfmt.enable = true;
            };

            settings.formatter = {
              clang-format = {
                options = [ "-i" "-style=file:${inputs.zephyr-nix.inputs.zephyr}/.clang-format" ];
                excludes = [ "build/*" "twister-out*/*" ];
              };
            };
          };

          devShells.default = pkgs.mkShell {
            inputsFrom = [
              config.treefmt.build.devShell
            ];

            packages = [
              (zephyr.sdk-0_17.override {
                targets = [
                  "arm-zephyr-eabi"
                ];
              })

              (zephyr.pythonEnv.override {
                extraPackages = pkgs: [
                  pkgs.stringcase
                ];
              })

              zephyr.hosttools

              pkgs.cmake
              pkgs.ninja
            ];

            shellHook = ''
              export LOCALE_ARCHIVE="${pkgs.glibcLocales}/lib/locale/locale-archive";
              export LC_ALL="C.UTF-8";
              export LANG="C.UTF-8";
            '';
          };
        };
    };

  nixConfig = {
    extra-substituters = [
      "https://cache.freedom.ind.br"
    ];
    extra-trusted-public-keys = [
      "cache.freedom.ind.br:4+Tt+AZreSw+P7xP0d6eHtIHhSAlkFbSa/9ugOkiMSM="
    ];
  };
}
