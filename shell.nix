with import (builtins.fetchTarball {
  name = "2023-09-16";
  url = "https://github.com/NixOS/nixpkgs/archive/ae5b96f3ab6aabb60809ab78c2c99f8dd51ee678.tar.gz";
  sha256 = "11fpdcj5xrmmngq0z8gsc3axambqzvyqkfk23jn3qkx9a5x56xxk";
}) {};
mkShell {
  buildInputs =
    let
      extensionName = "bzip";
      supportedPgVersions = [
        postgresql_16
        postgresql_15
        postgresql_14
        postgresql_13
        postgresql_12
      ];
      pgWExtension = { postgresql }:
      postgresql.withPackages (p: [
        (callPackage ./nix/pgExtension.nix { inherit postgresql extensionName; })
        (callPackage ./nix/pgsql-fio.nix { inherit postgresql; }) # only used for manual tests where writing to a file is required
      ]);
      extAll = map (x: callPackage ./nix/pgScript.nix { postgresql = pgWExtension { postgresql = x;}; }) supportedPgVersions;
    in
    extAll;

  shellHook = ''
    export HISTFILE=.history
  '';
}
