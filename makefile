# BOOST libraries
BOOST_LIB=-lboost_mpi -lboost_serialization -lboost_system -lboost_filesystem -lboost_date_time -lboost_program_options
BOOST_INCLUDE=-I/usr/include/boost
BOOST_LIBDIR=-L/usr/lib/x86_64-linux-gnu

# CPLEX library
CPLEX_PATH=/opt/ibm/ILOG/CPLEX_Studio1263/cplex
CONCERT_PATH=/opt/ibm/ILOG/CPLEX_Studio1263/concert
CPLEX_INCLUDE=-I$(CPLEX_PATH)/include
CONCERT_INCLUDE=-I$(CONCERT_PATH)/include
CPLEX_LIBDIR=-L$(CPLEX_PATH)/lib/x86-64_linux/static_pic
CONCERT_LIBDIR=-L$(CONCERT_PATH)/lib/x86-64_linux/static_pic
CPLEX_LIB=-lconcert -lilocplex -lcplex -lm -lpthread

# YAML library
YAML_LIB=-lyaml-cpp
YAML_INCLUDE=-I/usr/include
YAML_LIBDIR=-L/usr/lib/x86_64-linux-gnu

RM=rm -rf
LDLIBS=-lstdc++ -lm ${BOOST_LIB} ${YAML_LIB}
LDFLAGS=-L/usr/local/lib ${BOOST_LIBDIR} ${YAML_LIBDIR}
CXXFLAGS=-I. ${BOOST_INCLUDE} ${YAML_INCLUDE}
CXX=g++ -std=c++17 -DIL_STD -Wall -g #

# to use the CPLEX libs, compile with CPLEX=true
ifdef CPLEX
	LDLIBS+=${CPLEX_LIB} 
	LDFLAGS+=${CPLEX_LIBDIR} ${CONCERT_LIBDIR} 
	CXXFLAGS+=${CPLEX_INCLUDE} ${CONCERT_INCLUDE} 
	CXX+=-D_CPLEX #
endif

SRCDIR=src
OBJDIR=obj
BINDIR=bin
EXEC=main

# filter out cplex algorithm when not specified
SRC_TMP=$(foreach sdir, $(SRCDIR), $(wildcard $(sdir)/*.cpp))
SRC_OUT=
ifndef CPLEX
	SRC_OUT+=${SRCDIR}/ca_cplex.cpp
	SRC_OUT+=${SRCDIR}/ca_cplex_SS.cpp
	SRC_OUT+=${SRCDIR}/ca_cplex_rlps.cpp
endif
SRC=$(filter-out $(SRC_OUT), $(SRC_TMP))
OBJ=$(patsubst src/%.cpp, obj/%.o, $(SRC))

################

.PHONY: all checkdirs clean test

all: checkdirs $(BINDIR)/$(EXEC)

$(BINDIR)/$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(BINDIR)/$(EXEC) $(OBJ) $(LDLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp 
	$(CXX) -c -o $@ $< $(CXXFLAGS)


checkdirs: $(OBJDIR) $(BINDIR)

$(OBJDIR):
	@mkdir -p $@

$(BINDIR):
	@mkdir -p $@

clean:
	$(RM) $(OBJDIR) $(BINDIR)

### unit tests defs and targets

TEST_SRCDIR=test
TEST_EXEC=test
TEST_LIBS=-lcppunit

# filter out tests for cplex algorithm when not specified
TEST_SRC_TMP=$(foreach sdir, $(TEST_SRCDIR), $(wildcard $(sdir)/*.cpp))
TEST_SRC_OUT=
ifndef CPLEX
	TEST_SRC_OUT+=${TEST_SRCDIR}/test_ca_cplex.cpp
	TEST_SRC_OUT+=${TEST_SRCDIR}/test_ca_cplex_SS.cpp
	TEST_SRC_OUT+=${TEST_SRCDIR}/test_ca_cplex_rlps.cpp
endif
TEST_SRC=$(filter-out $(TEST_SRC_OUT), $(TEST_SRC_TMP))
TEST_OBJ=$(patsubst $(TEST_SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(TEST_SRC))
OBJ_NO_MAIN=$(filter-out obj/main.o, $(OBJ))

################

test: checkdirs $(BINDIR)/$(TEST_EXEC)
	./$(BINDIR)/$(TEST_EXEC)

$(BINDIR)/$(TEST_EXEC): $(OBJ_NO_MAIN)  $(TEST_OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) $(TEST_LIBS)

$(OBJDIR)/%.o: $(TEST_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

