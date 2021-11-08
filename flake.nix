{
  description = "Windstille Input Engine";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
    tinycmmc.url = "gitlab:grumbel/cmake-modules";
    logmich.url = "gitlab:logmich/logmich";
    priocpp.url = "gitlab:grumbel/priocpp";
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
