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

// 复位技巧
void dut_reset(Vtop *dut, vluint64_t &sim_time)
{
    dut->rst = 0;
    if (sim_time >= 3 && sim_time < 6) //设定复位的指定跳变数
        dut->rst = 1;
}


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
        dut_reset(top,sim_time);
		top->clk ^= 1;

		top->eval();

		if (sim_time == 8){
			top->Q_in = 5;
			top->en = 1;
		} else if (sim_time == 10){
			top->en = 0;
		}

        tfp->dump(contextp->time());
        contextp->timeInc(1);

		++sim_time;
    }

    delete top;
    tfp->close();
    delete contextp;
    return 0;
}
