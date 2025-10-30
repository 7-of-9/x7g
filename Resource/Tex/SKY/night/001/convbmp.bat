path=%path%;C:\DXSDK\Bin\DXUtils
dir *.bmp /b > list.txt
pause
for /f %%1 in (list.txt) do dxtex %%1 %%1.dds
pause

