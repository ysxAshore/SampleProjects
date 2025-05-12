VERILATOR = verilator
VERILATOR_FLAGS = --cc --exe --trace --x-assign unique --x-initial unique
SIM_TOP = sim_top.c
TOP_MODULE = top.v
SIMULATION ?= top
OBJ_DIR = obj_dir

default: compile

$(OBJ_DIR)/V$(SIMULATION): $(TOP_MODULE) $(SIM_TOP)
	$(VERILATOR) $(VERILATOR_FLAGS) $^
	cd $(OBJ_DIR) && make -f V$(SIMULATION).mk

compile: $(OBJ_DIR)/V$(SIMULATION) 

run: compile
	@./$(OBJ_DIR)/V$(SIMULATION) +verilator+rand+reset+2

clean:
	rm -rf $(OBJ_DIR) *.vcd
