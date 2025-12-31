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

        buildPkgs = with pkgs; [
          rustc
          cargo
          pkg-config
          udev
        ];
      in
      {
        devShells.default = pkgs.mkShell {
          nativeBuildInputs = with pkgs; [
            pkg-config
          ];
          buildInputs =
            with pkgs;
            [

              rustfmt
              rust-analyzer
              clippy

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

            ]
            ++ buildPkgs;

          OUT_DIR = "./src/db";
          RUST_BACKTRACE = "full";
        };

        nixosModules.default =
          { pkgs, ... }:
          {
            environment.systemPackages = [ self.packages.${pkgs.system}.default ];
          };

        packages.default = pkgs.rustPlatform.buildRustPackage {
          pname = "hwmon_sender";
          version = "0.0.1";
          src = ./.;
          cargoBuildFlags = "-p hwmon_sender";

          cargoLock = {
            lockFile = ./Cargo.lock;
          };

          nativeBuildInputs = with pkgs; [
            pkg-config
          ];
          buildInputs = buildPkgs;
          PKG_CONFIG_PATH = "${pkgs.dbus.dev}/lib/pkgconfig";

        };

      }
    );
}
