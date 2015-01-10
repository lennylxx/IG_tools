set OUTDIR="output"

mkdir %OUTDIR%
for %%s in (*.s) do igscript.exe -x -p %%s %OUTDIR%\\%%~ns.txt