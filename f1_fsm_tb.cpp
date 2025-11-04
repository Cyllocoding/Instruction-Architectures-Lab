#include "Vf1_fsm.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"

int main(int argc, char** argv, char** env) {
    int i;
    int clk;

    // Pass command-line args to Verilator (e.g., +trace)
    Verilated::commandArgs(argc, argv);

    // Create top-level Verilated model (DUT)
    Vf1_fsm* top = new Vf1_fsm;

    // --- enable VCD tracing ---
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);                  // trace depth
    tfp->open("f1_fsm.vcd");              // waveform file

    // Initialization of Vbuddy
    if (vbdOpen() != 1) return -1;
    vbdHeader("Lab3: F1 Lights FSM");
    vbdSetMode(1);                        // rotary switch -> vbdFlag()

    // --- initial stimulus ---
    top->clk = 1;
    top->rst = 1;
    top->en  = 0;

    // --- main simulation loop (clock cycles) ---
    for (i = 0; i < 300000; ++i) {

        // change inputs over time (sync-style driving)
        top->rst = (i < 2);               // assert reset for first 2 cycles
        top->en  = vbdFlag();             // advance one state when switch is pressed

        // produce one full clock cycle (two half toggles),
        // dump time steps, and evaluate the model at each edge
        for (clk = 0; clk < 2; ++clk) {
            tfp->dump(2*i + clk);         // timestamp (arbitrary units)
            top->clk = !top->clk;         // toggle clock
            top->eval();                  // evaluate DUT on this edge
        }

        // Show the 8-bit light pattern on the neopixel bar
        vbdBar(top->data_out & 0xFF);
        vbdCycle(i + 1);

        if (Verilated::gotFinish()) break;  // model requested finish
        if (vbdGetkey('q')) break;          // optional quick exit
    }

    // --- shutdown ---
    vbdClose();
    tfp->close();
    delete tfp;
    delete top;
    return 0;
}
