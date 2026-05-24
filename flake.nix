{
  description = "Ambiente di sviluppo per PPSE-LAB Firmware (RP2040)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        
        # Definizione dei pacchetti Python necessari per tools/api-python
        pythonEnv = pkgs.python3.withPackages (ps: with ps; [
          pip
          pyserial
          requests
          pandas      # Utile per l'analisi dei file telemetry_log.csv citati [cite: 33]
          pytest      # Per i test in tools/api-python/tests
        ]);
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            # Core Development [cite: 63, 65]
            platformio
            gnumake
            
            # Python Environment per i tool in tools/api-python [cite: 68]
            pythonEnv
            
            # Documentation [cite: 3, 49]
            doxygen
            graphviz # Necessario per i grafi di dipendenza citati nel PDF [cite: 172, 194]
            
            # Utilità per RP2040
            picotool
          ];

          shellHook = ''
            echo "--- PPSE-LAB DEVELOPMENT ENVIRONMENT ---"
            echo "Target: RP2040 (GPS & Environment Logger)"
            echo "Firmware Version: 5c622b6" 
            echo "Documentazione: Doxygen 1.16.1 pronta"
            
            # Configurazione automatica per PlatformIO
            export PLATFORMIO_CORE_DIR=$PWD/.pio
            
            # Alias utile per il controllo conformità 
            alias check='make check'
          '';
        };
      }
    );
}
