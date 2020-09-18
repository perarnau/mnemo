# development shell, includes all dependencies, debug, style tools
{
  pkgs ? import <nixpkgs> {}
}:
with pkgs;
pkgs.mkShell {
        name = "mnemo";
	nativeBuildInputs = [ autoreconfHook pkgconfig ];
        buildInputs = [
          python3
        ];
        CFLAGS = "-std=c99 -pedantic -Wall -Wextra";
}
