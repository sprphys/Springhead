################################################################
#
# 	Makefile for CDTestStage
#
################################################################

################################################################
#	common rules
################################################################

TOP_DIR 	= ../../../..
include 	$(TOP_DIR)/make.header
include 	$(TOP_DIR)/make.func

$(eval MAKE_DIR := $(subst $(SP),\$(SP),$(shell env pwd)))
MAKE_DIRNAME	= $(shell env basename $(MAKE_DIR))
BIN_NAME  	= $(MAKE_DIRNAME)

SRCS		= $(wildcard *.cpp)
OBJS 		= ${SRCS:.cpp=.o}
REALCLEANS	= $(wildcard log/*)

CXXFLAGS	:= $(CXXFLAGS) \
		$(call add_inc,${SPR_GL_INC}) \
		-D__cdecl= \
		-std=gnu++11

LDFLAGS		:= \
		$(call add_lib,${SPR_LINUX_LIB}) \
		$(LDFLAGS) \
		-llapack


################################################################
#	compile, link and go
################################################################
		
all:		compile test

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

compile:	$(OBJS)
	$(CXX) $(OBJS) -o $(BIN_NAME) $(LDFLAGS)

test:	
	-$(FORCE_TOMEOUT) ./$(BIN_NAME)


################################################################
#	clean up
################################################################

clean:
	@echo "entering $(MAKE_DIRNAME)"
	-$(RM) $(OBJS) $(BIN_NAME)
	
realclean:	clean
	-$(RM) $(REALCLEANS)
			
			
################################################################
#	THE END
################################################################
