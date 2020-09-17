# development shell, includes all dependencies, debug, style tools
{
  pkgs ? import <nixpkgs> {}
}:
with pkgs;
pkgs.mkShell {
        name = "mnemo";
	nativeBuildInputs = [ autoreconfHook pkgconfig ];
        buildInputs = [
        ];
        CFLAGS = "-std=c99 -pedantic -Wall -Wextra";
}
