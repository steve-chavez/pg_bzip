{ stdenv, lib, fetchFromGitHub, postgresql }:

stdenv.mkDerivation rec {
  name = "pgsql-fio";

  buildInputs = [ postgresql ];

  src = fetchFromGitHub {
    owner  = "csimsek";
    repo   = name;
    rev    = "9f6133c7ac4c50a14cf983943cb9916f994034bd";
    hash   = "sha256-uoWoFfm8iM/FzBtIH5SF6TPRhDXDMVftueWjMYggiJY=";
  };

  installPhase = ''
    install -D fio.so         -t $out/lib
    install -D fio--1.0.sql   -t $out/share/postgresql/extension
    install -D fio.control    -t $out/share/postgresql/extension
  '';
}
