{
  description = "Windstille Input Engine";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
    nix.inputs.nixpkgs.follows = "nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "gitlab:grumbel/cmake-modules";
    tinycmmc.inputs.nix.follows = "nix";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    logmich.url = "gitlab:logmich/logmich";
    logmich.inputs.nix.follows = "nix";
    logmich.inputs.nixpkgs.follows = "nixpkgs";
    logmich.inputs.flake-utils.follows = "flake-utils";
    logmich.inputs.tinycmmc.follows = "tinycmmc";

    priocpp.url = "gitlab:grumbel/priocpp";
    priocpp.inputs.nix.follows = "nix";
    priocpp.inputs.nixpkgs.follows = "nixpkgs";
    priocpp.inputs.flake-utils.follows = "flake-utils";
    priocpp.inputs.tinycmmc.follows = "tinycmmc";
    priocpp.inputs.logmich.follows = "logmich";
  };

  outputs = { self, nix, nixpkgs, flake-utils, tinycmmc, logmich, priocpp }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in rec {
        packages = flake-utils.lib.flattenTree {
          wstinput = pkgs.stdenv.mkDerivation {
            pname = "wstinput";
            version = "0.0.0";
            src = nixpkgs.lib.cleanSource ./.;
            nativeBuildInputs = [
              pkgs.cmake
              pkgs.ninja
              pkgs.gcc
              pkgs.pkgconfig
              tinycmmc.defaultPackage.${system}
            ];
            buildInputs = [
              logmich.defaultPackage.${system}
              priocpp.defaultPackage.${system}
              pkgs.glm
              pkgs.jsoncpp

              pkgs.SDL2
            ];
           };
        };
        defaultPackage = packages.wstinput;
      });
}
