{
  description = "Rust application";

  inputs = {
    nixpkgs.url = "nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";

  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        build-and-debug = pkgs.writeShellScriptBin "build-and-debug" (''cargo run'');

        rustPkgs = import ./Cargo.nix { inherit pkgs; };

      in
      {
        devShells.default = pkgs.mkShell {
          nativeBuildInputs = with pkgs; [
            pkg-config
          ];
          buildInputs = with pkgs; [
            rustc
            cargo
            rustfmt
            rust-analyzer
            clippy
            pkg-config
            udev

            arduino-cli

            (pkgs.writeShellScriptBin "runArduino" (''
              cd arduino
              arduino-cli core install arduino:avr
              arduino-cli lib install LedControl
              arduino-cli compile -b arduino:avr:uno
              arduino-cli upload -b arduino:avr:uno -p /dev/ttyACM0

            ''))
            (pkgs.writeShellScriptBin "runSender" (''
              cargo run
            ''))

            (pkgs.writeShellScriptBin "run" (''
              runArduino
              runSender
            ''))

            cachix

            crate2nix

            openssl.dev

            build-and-debug

          ];

          PKG_CONFIG_PATH = "${pkgs.openssl.dev}/lib/pkgconfig";
          OUT_DIR = "./src/db";
          RUST_BACKTRACE = "full";
        };

        packages.default = rustPkgs.workspaceMembers."ordis".build.overrideAttrs (old: rec {
          PKG_CONFIG_PATH = "${pkgs.openssl.dev}/lib/pkgconfig";
          OUT_DIR = "./src/db";
          RUST_BACKTRACE = "full";

          # preBuild = ''
          #   pkgs.crate2nix generate
          # '';

          buildInputs = old.buildInputs or [ ] ++ [
            pkgs.sqlite
            pkgs.openssl.dev
          ];
        });

      }
    );
}
