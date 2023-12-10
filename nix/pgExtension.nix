{ stdenv, postgresql, bzip2, extensionName }:

stdenv.mkDerivation {
  name = extensionName;

  buildInputs = [ postgresql bzip2 ];

  src = ../.;

  installPhase = ''
    install -D *.so -t $out/lib
    install -D -t $out/share/postgresql/extension sql/*.sql
    install -D -t $out/share/postgresql/extension ${extensionName}.control
  '';
}
