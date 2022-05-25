{
  description = "Windstille Input Engine";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-21.11";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "gitlab:grumbel/cmake-modules";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    logmich.url = "gitlab:logmich/logmich";
    logmich.inputs.nixpkgs.follows = "nixpkgs";
    logmich.inputs.flake-utils.follows = "flake-utils";
    logmich.inputs.tinycmmc.follows = "tinycmmc";

    priocpp.url = "gitlab:grumbel/priocpp";
    priocpp.inputs.nixpkgs.follows = "nixpkgs";
    priocpp.inputs.flake-utils.follows = "flake-utils";
    priocpp.inputs.tinycmmc.follows = "tinycmmc";
    priocpp.inputs.logmich.follows = "logmich";
    priocpp.inputs.sexpcpp.follows = "sexpcpp";

    sexpcpp.url = "gitlab:lispparser/sexp-cpp";
    sexpcpp.inputs.nixpkgs.follows = "nixpkgs";
    sexpcpp.inputs.flake-utils.follows = "flake-utils";
    sexpcpp.inputs.tinycmmc.follows = "tinycmmc";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc, logmich, priocpp, sexpcpp }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in rec {
        packages = flake-utils.lib.flattenTree {
          wstinput = pkgs.stdenv.mkDerivation {
            pname = "wstinput";
            version = "0.3.0";
            src = nixpkgs.lib.cleanSource ./.;
            nativeBuildInputs = [
              tinycmmc.defaultPackage.${system}

              pkgs.cmake
              pkgs.pkgconfig
            ];
            propagatedBuildInputs = [
              logmich.defaultPackage.${system}
              priocpp.defaultPackage.${system}
              sexpcpp.defaultPackage.${system}

              pkgs.SDL2
            ];
           };
        };
        defaultPackage = packages.wstinput;
      });
}
