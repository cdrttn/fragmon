
test -d ../fragmon-rel || mkdir ../fragmon-rel
test -d ../fragmon-rel/gamequery || mkdir ../fragmon-rel/gamequery
test -d ../fragmon-rel/gamequery/minilib || mkdir ../fragmon-rel/gamequery/minilib

if [ "$1" == "win32" ]
then
    cp /cygdrive/c/WINDOWS/system32/python24.dll ../fragmon-rel 
    cp /cygdrive/c/WINDOWS/system32/msvcr71.dll ../fragmon-rel
    cp fragmon.exe ../fragmon-rel
    cp fmlaunch.mrc ../fragmon-rel
    cp fragmon-inno.iss ../fragmon-rel
    cp icon.ico ../fragmon-rel

else
    cp fragmon ../fragmon-rel
fi

cp gamequery/*.py ../fragmon-rel/gamequery
cp gamequery/minilib/* ../fragmon-rel/gamequery/minilib
rm ../fragmon-rel/gamequery/*test*

cp fragmon.htb ../fragmon-rel
cp LICENSE ../fragmon-rel
cp README.txt ../fragmon-rel

