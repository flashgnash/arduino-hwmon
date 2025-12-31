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
      # ---------- NixOS module (TOP-LEVEL, NOT per-system)
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
              wantedBy = [ "multi-user.target" ];
              after = [ "network.target" ];

              serviceConfig = {
                ExecStart = "${self.packages.${pkgs.system}.default}/bin/hwmon_sender";
                Restart = "always";
              };
            };
          };
        };
    }
    // flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
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

        # ---------- Dev shell
        devShells.default = pkgs.mkShell {
          buildInputs = [
            pkgs.rustc
            pkgs.cargo
            pkgs.rustfmt
            pkgs.rust-analyzer
            pkgs.clippy
            pkgs.pkg-config
            pkgs.udev
            pkgs.arduino-cli
          ];

          RUST_BACKTRACE = "full";
        };
      }
    );
}
