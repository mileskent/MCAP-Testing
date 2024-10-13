{ stdenv, cmake, protobuf, lz4, zstd, abseil-cpp }:

stdenv.mkDerivation rec {
  pname = "hub";
  version = "0.1.0";
  src = ./.;
  nativeBuildInputs = [ cmake ];
  buildInputs = [ protobuf lz4 zstd ];
  
}