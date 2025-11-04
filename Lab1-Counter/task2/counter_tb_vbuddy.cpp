#include "Vcounter.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"

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

    //initialization of Vbuddy
    if(vbdOpen()!=1) return -1;
    vbdHeader("Lab1: Counter");


    // --- initial stimulus ---
    top->clk = 1;
    top->rst = 1;
    top->en  = 1;


    // --- main simulation loop (clock cycles) ---
    for (i = 0; i < 1000; ++i) {

        // change inputs over time (sync-style driving)
        top->rst = (i < 2) || (i == 15);  // assert for first 2 cycles and once at i==15
        top->en  = vbdFlag();              // enable after a few cycles

        // produce one full clock cycle (two half toggles),
        // dump time steps, and evaluate the model at each edge
        for (clk = 0; clk < 2; ++clk) {
            tfp->dump(2*i + clk);    // timestamp (arbitrary units)
            top->clk = !top->clk;    // toggle clock
            top->eval();             // evaluate DUT on this edge
        }

        // Send count value to Vbuddy
        // Send count value to Vbuddy
        vbdPlot(top->count, 0, 255);
        vbdCycle(i+1);

        if (Verilated::gotFinish()) break;  // model requested finish
    }

    // --- shutdown ---
    vbdClose();
    tfp->close();
    delete tfp;
    delete top;
    return 0;
}
