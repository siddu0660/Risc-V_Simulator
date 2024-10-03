COMPILER = g++

SRC = assembler.cpp encoder.cpp memory.cpp parser.cpp simulator.cpp

OBJ = $(SRC:.cpp=.o)

MAIN = main.cpp

EXEC = riscv_sim

all: $(EXEC)

%.o: %.cpp
	@echo "Compiling $< into object file..."
	$(COMPILER) -c $< -o $@

$(EXEC): $(MAIN) $(OBJ)
	@echo "Linking object files into the executable $(EXEC)..."
	$(COMPILER) $(OBJ) $(MAIN) -o $(EXEC)
	@echo "Build complete."
	@echo "Cleaning up: Removing object files..."
	rm -f $(OBJ)
	@echo "Cleanup complete."

clean:
	@echo "Cleaning up: Removing object files..."
	rm -f $(OBJ)
	@echo "Cleanup complete."

wipe:
	@echo "Wiping files..."
	rm -f $(OBJ) $(EXEC) error.log output.hex
	@echo "Wiped the files"