{
  description = "MCAP Testing Flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
    utils.url = "github:numtide/flake-utils";
    nebs-packages.url = "github:RCMast3r/nebs_packages";
    nebs-packages.inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = { self, nixpkgs, utils, nebs-packages, ... }: 
  let 
    system = "x86_64-linux";

    # Overlay to include test_mcap from default.nix
    test_mcap_overlay = final: prev: {
      test_mcap = final.callPackage ./default.nix {};
    };

    # List of overlays, including nebs-packages
    my_overlays = [ 
      test_mcap_overlay
      nebs-packages.overlays.default
    ];

    # Define the package set
    pkgs = import nixpkgs {
      inherit system;
      overlays = my_overlays;
    };
  in
  {      
    packages.${system}.default = pkgs.test_mcap;

    overlays.default = nixpkgs.lib.composeManyExtensions [
      nebs-packages.overlays.default
      test_mcap_overlay
    ];

    # overlays.default = nixpkgs.lib.composeManyExtensions my_overlays;

    legacyPackages.${system} =
    import nixpkgs {
      inherit system;
      overlays = my_overlays;
    };
  };
}
