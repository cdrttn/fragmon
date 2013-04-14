#make debug build? if DEBUG=1, debug mode wxWidgets and Python libraries are used
#and Fragmon is built with debug symbols.
DEBUG = 1

#Installation Paths
#
# Fragmon checks first for gamequery/ and fragmon.htb in the same directory
# of the fragmon executable.
#
# Fragmon also recognizes data files in these directories: 
# /usr/lib/fragmon/ 
# /usr/local/lib/fragmon/
# /opt/fragmon/
# ~/.fragmon-data/
#
# So long as Fragmon's data (query module directory and help docs) is in
# one of those locations, the fragmon executable can be installed anywhere
FRAGMON_DATA = ~/fragmon-data
FRAGMON_BIN = ~/bin

#wx-config path for release and debug builds of wxWidgets
WXCONFIG_R = /home/chrisd/wx/gtk-rel/bin/wx-config
WXCONFIG_D = wx-config

#paths for release mode python
PYHEADER_R = /usr/include/python2.4
PYLIB_R = -lpython2.4

#paths for debug mode python
PYHEADER_D = /home/chrisd/wx/pdbg/include/python2.4
#when using static linkage, some extra linker options may need to be added
PYLIB_D = -Wl,-E /home/chrisd/wx/pdbg/lib/libpython2.4.a -lm -lutil -ldl 

CPP = g++
MKRELEASE = mkrelease.sh unix

ifeq ($(DEBUG), 1)
	WXCONFIG = $(WXCONFIG_D)
	GDB = -ggdb
	PYHEADER = $(PYHEADER_D)
	PYLIB = $(PYLIB_D)
else
	WXCONFIG = $(WXCONFIG_R)
	GDB = 
	PYHEADER = $(PYHEADER_R)	
	PYLIB = $(PYLIB_R) 
endif

CFLAGS = $(GDB) `$(WXCONFIG) --cxxflags` -I$(PYHEADER)
LDFLAGS = $(GDB) -pthread $(PYLIB) `$(WXCONFIG) --libs`

OBJ = Fragmon.o fragmon_wdr.o pyemb.o QueryInput.o ServerEntry.o \
RuleListCtrl.o DebugFrame.o MainFrame.o PlayerListCtrl.o flipper.o PassLock.o \
ExecInput.o varstr.o CopyFmt.o ipcserv.o OptionsDlg.o QueryOpts.o findapp.o \
CompOutput.o pydataconv.o pywrappers.o IRCInput.o irc.o scorebot.o \
BroadcastFmt.o ScorebotInput.o ircbase.o ChannelInput.o pygamequery.o \
pyscorebot.o

WRAPTEST = pyemb.cpp pydataconv.cpp pywrappers.cpp ircbase.cpp

fragmon: $(OBJ)
	$(CPP) -o fragmon $(OBJ) $(LDFLAGS)

wraptest: $(WRAPTEST)
	$(CPP) -shared -o pywrappers.so -DWRAPTEST $(WRAPTEST) $(CFLAGS) $(LDFLAGS)

%.o: %.cpp
	$(CPP) -c $< $(CFLAGS)

clean:
	rm -f fragmon *.o *.core core.* core *.so
	rm -rf ../fragmon-rel

release: fragmon
	strip fragmon
	sh $(MKRELEASE)

helpfile:
	cd doc && $(MAKE)

install: fragmon helpfile
	test -d $(FRAGMON_DATA) || mkdir -p $(FRAGMON_DATA)

	cp -R gamequery $(FRAGMON_DATA)
	cp LICENSE $(FRAGMON_DATA)
	cp fragmon.htb $(FRAGMON_DATA)
	rm -f $(FRAGMON_DATA)/gamequery/*test*    

	install -c -s fragmon $(FRAGMON_BIN)
