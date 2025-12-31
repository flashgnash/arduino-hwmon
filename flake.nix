{
  description = "Arduino hwmon sender";

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
    let
      systems = flake-utils.lib.defaultSystems;
    in
    {
      # ---------- NixOS module (TOP-LEVEL)
      nixosModules.default =
        {
          pkgs,
          lib,
          config,
          ...
        }:
        let
          cfg = config.services.hwmon;
        in
        {
          options.services.hwmon.enable = lib.mkEnableOption "hwmon sender service";

          config = lib.mkIf cfg.enable {
            systemd.services.hwmon = {
              description = "Arduino hwmon sender";

              bindsTo = [ "dev-arduino-hwmon.device" ];
              after = [ "dev-arduino-hwmon.device" ];
              wantedBy = [ "multi-user.target" ];

              serviceConfig = {
                ExecStart = "${self.packages.${pkgs.system}.default}/bin/hwmon_sender";
                Restart = "always";
                RestartSec = 2;
              };
            };

            services.udev.extraRules = ''
              SUBSYSTEM=="tty", ATTRS{idVendor}=="XXXX", ATTRS{idProduct}=="YYYY", SYMLINK+="arduino-hwmon"
            '';
          };
        };
    }
    // flake-utils.lib.eachDefaultSystem (
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
        # ---------- Package
        packages.default = pkgs.rustPlatform.buildRustPackage {
          pname = "hwmon_sender";
          version = "0.0.1";
          src = ./.;

          cargoLock.lockFile = ./Cargo.lock;

          nativeBuildInputs = [ pkgs.pkg-config ];
          buildInputs = [ pkgs.udev ];
        };

        # ---------- Dev shell (RESTORED)
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
              cachix

              (pkgs.writeShellScriptBin "runArduino" ''
                cd arduino
                arduino-cli core install arduino:avr
                arduino-cli lib install LedControl
                arduino-cli compile -b arduino:avr:uno
                arduino-cli upload -b arduino:avr:uno -p /dev/arduino-hwmon
              '')

              (pkgs.writeShellScriptBin "runSender" ''
                cargo run
              '')

              (pkgs.writeShellScriptBin "run" ''
                runArduino
                runSender
              '')
            ]
            ++ buildPkgs;

          OUT_DIR = "./src/db";
          RUST_BACKTRACE = "full";
        };
      }
    );
}
