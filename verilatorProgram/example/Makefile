VERILATOR = verilator
VERILATOR_FLAGS = --cc --exe -j 0 -Wall --trace --x-assign unique --x-initial unique
SIM_TOP = sim_main.c
TOP_MODULE = our.v
SIMULATION ?= our
OBJ_DIR = obj_dir

default: compile

$(OBJ_DIR)/V$(SIMULATION): $(TOP_MODULE) $(SIM_TOP)
	$(VERILATOR) $(VERILATOR_FLAGS) $^
	cd $(OBJ_DIR) && make -f V$(SIMULATION).mk

compile: $(OBJ_DIR)/V$(SIMULATION) 

run: compile
	@./$(OBJ_DIR)/V$(SIMULATION) +verilator+rand+reset+2

clean:
	rm -rf $(OBJ_DIR)
