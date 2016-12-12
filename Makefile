CMPI = mpic++ -Wall -O3 -fopenmp

CPPFLAGS=-Iinc

LDFLAGS=-lboost_program_options -lm

FILES=export forward init memory parse_args shalw
OBJECTS=$(addsuffix .o, $(FILES))
BIN=bin/shalw

all : $(BIN)

$(BIN) : $(addprefix obj/, $(OBJECTS))
	$(CMPI) -o $@ $^ $(LDFLAGS)

obj/%.o : src/%.cpp
	$(CMPI) -c -o $@ $^ $(CPPFLAGS)

obj/%.o : src/%.c
	$(CMPI) -c -o $@ $^ $(CPPFLAGS)

exec:
	mpirun -n 2  $(BIN) -x 512 -y 512 -t 400

clean :
	rm -f bin/* obj/*

proper :
	rm -f bin/* obj/* saved/*
