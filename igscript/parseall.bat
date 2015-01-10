set OUTDIR="output"

mkdir %OUTDIR%
for %%s in (*.s) do igscript.exe -p %%s %OUTDIR%\\%%~ns.txt