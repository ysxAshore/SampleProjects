#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Vtop.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

// 终止时间
#define MAX_SIM_TIME 200
// 全局仿真时间
vluint64_t sim_time = 0;

int main(int argc,char **argv){
    srand(time(NULL));

    VerilatedContext* contextp = new VerilatedContext; 
    contextp->commandArgs(argc, argv);  

    Vtop * top = new Vtop{contextp};

    VerilatedVcdC * tfp = new VerilatedVcdC;
    contextp->traceEverOn(true);
    top->trace(tfp,0);
    tfp->open("wave.vcd");

    while(sim_time < MAX_SIM_TIME){

		if (sim_time == 0){
			top->data_in = 23;
			top->shift_amount = 3;
			top->direction = 1;
			top->shift_type = 1;
		} else if (sim_time == 10){
			top->data_in = 23;
			top->shift_amount = 4;
			top->direction = 0;
			top->shift_type = 1;
			
		}

		top->eval();

        tfp->dump(contextp->time());
        contextp->timeInc(1);

		++sim_time;
    }

    delete top;
    tfp->close();
    delete contextp;
    return 0;
}
