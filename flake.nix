{
  description = "Windstille Input Engine";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    logmich.url = "git+https://github.com/logmich/logmich.git";
    logmich.inputs.nixpkgs.follows = "nixpkgs";

    priocpp.url = "git+https://github.com/grumbel/priocpp.git";
    priocpp.inputs.nixpkgs.follows = "nixpkgs";
    priocpp.inputs.logmich.follows = "logmich";

    sexpcpp.url = "git+https://github.com/lispparser/sexp-cpp.git";
    sexpcpp.inputs.nixpkgs.follows = "nixpkgs";

    SDL2-win32.url = "git+https://github.com/grumnix/SDL2-win32.git";
    SDL2-win32.inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = { self, nixpkgs, flake-utils, logmich, priocpp, sexpcpp, SDL2-win32 }:
    let
      versionBase = nixpkgs.lib.strings.removeSuffix "\n" (builtins.readFile ./VERSION);
      gitRev = "${self.shortRev or self.dirtyShortRev or "dirty"}";
      isDev = nixpkgs.lib.strings.hasInfix "-dev" versionBase;
      version =
        if isDev then
          "${versionBase}.${toString (self.revCount or 0)}+g${gitRev}"
        else
          versionBase;

      eachSystem = flake-utils.lib.eachSystem (flake-utils.lib.defaultSystems ++ [ "x86_64-windows" "i686-windows" ]);
      pkgsFromSystem = system:
        if system == "x86_64-windows" then nixpkgs.legacyPackages.x86_64-linux.pkgsCross.mingwW64
        else if system == "i686-windows" then nixpkgs.legacyPackages.x86_64-linux.pkgsCross.mingw32
        else nixpkgs.legacyPackages.${system};
    in
    eachSystem (system:
      let
        pkgs = pkgsFromSystem system;
      in
      {
        packages = rec {
          default = wstinput;

          wstinput = pkgs.stdenv.mkDerivation {
            pname = "wstinput";
            inherit version;

            src = nixpkgs.lib.cleanSource ./.;

            cmakeFlags = [
              "-DPROJECT_VERSION_FULL=${version}"
            ];

            nativeBuildInputs = [
              pkgs.buildPackages.cmake
              pkgs.buildPackages.pkg-config
            ];

            propagatedBuildInputs = [
              logmich.packages.${pkgs.stdenv.hostPlatform.system}.default
              priocpp.packages.${pkgs.stdenv.hostPlatform.system}.default
              sexpcpp.packages.${pkgs.stdenv.hostPlatform.system}.default

              (if pkgs.stdenv.hostPlatform.isWindows
               then SDL2-win32.packages.${pkgs.stdenv.hostPlatform.system}.default
               else pkgs.SDL2)
            ];
          };
        };
      }
    );
}
