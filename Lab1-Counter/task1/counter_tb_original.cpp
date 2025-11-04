#include "VcounterQ2.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

int main(int argc, char** argv, char** env) {
    int i;
    int clk;

    // Pass command-line args to Verilator (e.g., +trace)
    Verilated::commandArgs(argc, argv);

    // Create top-level Verilated model (DUT)
    VcounterQ2* top = new VcounterQ2;

    // --- enable VCD tracing ---
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);                // trace depth
    tfp->open("counter.vcd");           // waveform file

    // --- initial stimulus ---
    top->clk = 1;
    top->rst = 1;
    top->en  = 0;


    // --- main simulation loop (clock cycles) ---
    for (i = 0; i < 300; ++i) {

        // change inputs over time (sync-style driving)
        top->rst = (i < 2) || (i == 15);  // assert for first 2 cycles and once at i==15
        top->en  = (i>=4);              // enable after a few cycles

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
