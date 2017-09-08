CUR_PATH = $(shell echo $(PWD))
OBJS = $(addprefix ykushcmd/objs/, ykushcmd.o commandParser.o ykushxs.o yk_usb_device.o ykush.o ykush_help.o)
LIBS = -lhidapi-libusb -lusb-1.0 -ludev
LOADPATHS = -L$(CUR_PATH)/ykushcmd/linux
INCLUDEPATHS= -I$(CUR_PATH)/ykushcmd/linux -I$(CUR_PATH)/ykushcmd/ykushxs -I$(CUR_PATH)/ykushcmd/ykush -I$(CUR_PATH)/ykushcmd -I$(CUR_PATH)/ykushcmd/help
PREPROCESSOR_DEFS = -DLINUX
CPP = g++

ykushcmd : $(OBJS)
	$(CPP) $(LOADPATHS) -o bin/ykushcmd $(OBJS) $(LIBS)

ykushcmd/objs/ykushcmd.o : ykushcmd/ykushcmd.cpp ykushcmd/commandParser.h ykushcmd/help/ykush_help.h
	$(CPP) $(PREPROCESSOR_DEFS) $(INCLUDEPATHS) -c ykushcmd/ykushcmd.cpp -o ykushcmd/objs/ykushcmd.o

ykushcmd/objs/commandParser.o : ykushcmd/commandParser.cpp ykushcmd/commandParser.h ykushcmd/ykushxs/ykushxs.h ykushcmd/ykush/ykush.h 
	$(CPP) $(PREPROCESSOR_DEFS) $(INCLUDEPATHS) -c ykushcmd/commandParser.cpp -o ykushcmd/objs/commandParser.o

ykushcmd/objs/ykushxs.o : ykushcmd/ykushxs/ykushxs.cpp ykushcmd/ykushxs/ykushxs.h ykushcmd/yk_usb_device.h
	$(CPP) $(PREPROCESSOR_DEFS) $(INCLUDEPATHS) -c ykushcmd/ykushxs/ykushxs.cpp -o ykushcmd/objs/ykushxs.o

ykushcmd/objs/ykush.o : ykushcmd/ykush/ykush.cpp ykushcmd/ykush/ykush.h ykushcmd/yk_usb_device.h
	$(CPP) $(PREPROCESSOR_DEFS) $(INCLUDEPATHS) -c ykushcmd/ykush/ykush.cpp -o ykushcmd/objs/ykush.o

ykushcmd/objs/yk_usb_device.o : ykushcmd/yk_usb_device.cpp ykushcmd/yk_usb_device.h
	$(CPP) $(PREPROCESSOR_DEFS) $(INCLUDEPATHS) -c ykushcmd/yk_usb_device.cpp -o ykushcmd/objs/yk_usb_device.o

ykushcmd/objs/ykush_help.o : ykushcmd/help/ykush_help.cpp ykushcmd/help/ykush_help.h
	$(CPP) $(PREPROCESSOR_DEFS) $(INCLUDEPATHS) -c ykushcmd/help/ykush_help.cpp -o ykushcmd/objs/ykush_help.o


clean :
	rm -f bin/ykushcmd $(OBJS)

