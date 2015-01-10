set INDIR="output"
set OUTDIR="new"

mkdir %OUTDIR%
for %%s in (*.s) do igscript.exe -x -c %%s %INDIR%\\%%~ns.txt %OUTDIR%\\%%~ns.s