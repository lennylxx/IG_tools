set INDIR="output"
set OUTDIR="new"

mkdir %OUTDIR%
for %%s in (*.s) do igscript.exe -c %%s %INDIR%\\%%~ns.txt %OUTDIR%\\%%~ns.s