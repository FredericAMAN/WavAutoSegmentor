# Commands aliases
CD=cd
RM = rm -fr
MAKE=make
ECHO=@echo
DOCCC = `which doxygen`

# Rules
default: all
	

all: make_main make_tools

clean_all: clean_main clean_tools clean_lib

# generate the documentation (there is no argument, everything is in the Doxygen file)
doc:
	$(DOCCC) 


# Main cirdox code
make_main: make_tools
	$(CD) ./src && $(MAKE)
clean_main:
	$(CD) ./src && $(MAKE) clean

# Tools libs
make_tools:
	$(CD) ./src/tools && $(MAKE)
clean_tools:
	$(CD) ./src/tools && $(MAKE) clean
clean_lib:
	$(RM) ./lib/plugins/*
