TARGET = master_scada

CPP = g++
FLAGS = -02 -C
LIBS = -lrf24

SRC = $(shell find ./src/master/scada -name '*.cpp')
OBJ = $(SRC:.c=.o)

final: $(TARGET)

$(TARGET) : $(OBJ)
	$(CPP) $^ -o $@ $(LIBS)

%.o: %.cpp
	$(CPP) $(FLAGS) $< -o $@

clean:
	rm -f $(OBJ)