#include "Vcounter.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

int main(int argc, char** argv, char** env) {
    int i;
    int clk;

    // Pass command-line args to Verilator (e.g., +trace)
    Verilated::commandArgs(argc, argv);

    // Create top-level Verilated model (DUT)
    Vcounter* top = new Vcounter;

    // --- enable VCD tracing ---
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);                // trace depth
    tfp->open("counter.vcd");           // waveform file

    // --- initial stimulus ---
    top->clk = 1;
    top->rst = 1;
    top->en  = 0;

    static int tmp = 0;
    static int tmp_target = 3;


    // --- main simulation loop (clock cycles) ---
    for (i = 0; i < 300; ++i) {

        // change inputs over time (sync-style driving)
        top->rst = (i < 2) || (i == 20);  // assert for first 2 cycles and once at i==15
        top->en  = (i>=4);              // enable after a few cycles

        if (top->count==0x9){

            if (tmp<tmp_target){
                top-> en =0;
                tmp++;
            }
            else{
                top->en=1;
                tmp =0;
            }
            
        }

        // produce one full clock cycle (two half toggles),
        // dump time steps, and evaluate the model at each edge
        for (clk = 0; clk < 2; ++clk) {
            tfp->dump(2*i + clk);    // timestamp (arbitrary units)
            top->clk = !top->clk;    // toggle clock
            top->eval();             // evaluate DUT on this edge
        }

        if (Verilated::gotFinish()) break;  // model requested finish
    }

    // --- shutdown ---
    tfp->close();
    delete tfp;
    delete top;
    return 0;
}
