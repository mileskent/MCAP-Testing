{
  description = "MCAP Testing Flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, utils }: 
  let 
    system = "x86_64-linux";

    mcap_test_overlay = final: prev: {
      mcap_test = final.callPackage ./default.nix { };
    };
    my_overlays = [ mcap_test_overlay ];

    pkgs = import nixpkgs {
      system = "x86_64-linux";
      overlays = [ self.overlays.default ];
    };  
  in
  {      
    packages.x86_64-linux.default = pkgs.mcap_test;
    overlays.default = nixpkgs.lib.composeManyExtensions my_overlays;

    legacyPackages.x86_64-linux =
    import nixpkgs {
      inherit system;
      overlays = [
        (final: _: { mcap_test = final.callPackage ./default.nix { }; })
      ];
    };
  };
}
