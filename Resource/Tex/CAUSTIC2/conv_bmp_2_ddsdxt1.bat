dir *.bmp /b > list.txt
pause
for /f %%1 in (list.txt) do dxtex %%1 -m DXT1 %%1.dds
pause

