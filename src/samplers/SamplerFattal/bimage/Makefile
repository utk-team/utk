CC = g++
AR = ar
RANLIB = ranlib
MSG = @echo
NEWL = @echo
CLEAR = clear
LIB_NAME = bimage
TEST = btest
HDRS = bimage.hh
LIB_SRCS = $(LIB_NAME:=.cc) 		#Replacing nothing with .cc
LIB_OBJS = $(subst .cc,.o,$(LIB_SRCS)) 	#substituting .cc -> .o
LIB_OBJS1 = $(LIB_SRCS:.cc=.o) 		#Replacing .cc with .o
TEST_SRCS = $(TEST:=.cc)
TEST_OBJS = $(subst .cc,.o,$(TEST_SRCS))
LINK_FLAGS = 
INCLUDE = 

COMP_FLAGS = -O4 -Wall -fPIC $(INCLUDE) -Wno-deprecated

.cc.o: $(LIB_SRCS) btest.cc
	$(MSG) Compiling $<...
	$(CC) $(COMP_FLAGS) -c $<
	$(NEWL)

all: static shared $(TEST)

static: lib$(LIB_NAME).a 

shared: lib$(LIB_NAME).so.1.0

lib$(LIB_NAME).a: $(LIB_OBJS)
	$(MSG) Creating static library...
	$(AR) rcu lib$(LIB_NAME).a $<
	$(RANLIB) lib$(LIB_NAME).a 
	$(NEWL)

lib$(LIB_NAME).so.1.0: $(LIB_OBJS)
	$(MSG) Creating shared library...
	$(CC) -shared -Wl,-soname,lib$(LIB_NAME).so.1 \
	-o lib$(LIB_NAME).so.1.0 $< $(LINK_FLAGS)
	rm -f lib$(LIB_NAME).so.1 lib$(LIB_NAME).so
	ln -s lib$(LIB_NAME).so.1.0 lib$(LIB_NAME).so.1 
	ln -s lib$(LIB_NAME).so.1 lib$(LIB_NAME).so
	$(NEWL)
        # 'setenv' LD_LIBRARY_PATH `pwd`:$LD_LIBRARY_PATH 

$(TEST): bimage.h bimage.cc btest.o
	$(MSG) Linking test file...
	$(CC) btest.cc -o $(TEST) -L. -l$(LIB_NAME) $(LINK_FLAGS) -Wl,--rpath -Wl,LIBDIR -Wno-deprecated
tar:
	tar cvf bimage.tar Makefile *.cc *.hh *.h ascii4.bmp --ignore-failed-read

backup: tar
	cp bimage.tar /backup/

clean:
	/bin/rm -f *.o a.out *core *~ $(TEST) \
	lib$(LIB_NAME).a lib$(LIB_NAME).so.1 \
	lib$(LIB_NAME).so.1.0 lib$(LIB_NAME).so 
	$(CLEAR)

depend: $(LIB_SRCS) $(TEST_SRCS)
	makedepend $(LIB_SRCS) $(TEST_SRCS) $(INCLUDE)
# DO NOT DELETE

bimage.o: bimage.hh /usr/include/math.h /usr/include/features.h
bimage.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
bimage.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
bimage.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
bimage.o: /usr/include/bits/mathcalls.h /usr/include/string.h
bimage.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
bimage.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
bimage.o: /usr/include/bits/confname.h /usr/include/getopt.h
bimage.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
bimage.o: /usr/include/endian.h /usr/include/bits/endian.h
bimage.o: /usr/include/sys/select.h /usr/include/bits/select.h
bimage.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
bimage.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
bimage.o: /usr/include/alloca.h /usr/include/stdio.h /usr/include/libio.h
bimage.o: /usr/include/_G_config.h /usr/include/wchar.h
bimage.o: /usr/include/bits/wchar.h /usr/include/gconv.h
bimage.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
btest.o: bimage.hh /usr/include/math.h /usr/include/features.h
btest.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
btest.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
btest.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
btest.o: /usr/include/bits/mathcalls.h /usr/include/stdlib.h
btest.o: /usr/include/sys/types.h /usr/include/bits/types.h
btest.o: /usr/include/bits/typesizes.h /usr/include/time.h
btest.o: /usr/include/endian.h /usr/include/bits/endian.h
btest.o: /usr/include/sys/select.h /usr/include/bits/select.h
btest.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
btest.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
btest.o: /usr/include/alloca.h
