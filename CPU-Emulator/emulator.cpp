// emulate executing Hack CPU instructions
#include "iobuffer.h"
#include "symbols.h"
#include "hack_computer.h"
#include <bitset>
#include <vector>
#include <iostream>
#include <math.h>

// Author: Yuanchen Xiang
// Student ID: a1815395

// to make out programs a bit neater
using namespace std ;

using namespace CS_IO_Buffers ;
using namespace CS_Symbol_Tables ;
using namespace Hack_Computer ;

// This program must emulate the execution of Hack CPU instructions
//
// To read from registers A, D or PC, use read_A(), read_D() or read_PC()
// To write to registers A, D or PC, use write_A(value), write_D(value) or write_PC(value)
//
// To read from ROM use read_ROM(address)
// To read from RAM use read_RAM(address)
// To write to RAM use write_RAM(address,value)
//
// All values returned by a read_*() function are 16-bit unsigned integers
// All parameters passed to a write_*() function are 16-bit unsigned integers
//

/*****************   REPLACE THE FOLLOWING CODE  ******************/


// disassemble an instruction - convert binary to symbolic form
// A instructions should be "@" followed by the value in decimal
// C instructions should be dest=alu;jump
// omit dest= if all destination bits are 0
// omit ;jump if all jump bits are 0
string disassemble_instruction(uint16_t instruction)
{
    int binaryArray[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    if (instruction > 65535)
    {
        instruction = instruction - pow(2,16);
    }

    // turn instruction to int array
    for (int i = 15; i >= 0; i--){
        binaryArray[i] = instruction % 2;
        instruction = instruction / 2;
    }

    bool isContinue = true;

    // if (binaryArray[0] == 0)
    // {
    //     int val = binaryArray[0] + binaryArray[1] + binaryArray[2];
    //     if (val > 2 || val < 0)
    //     {
    //         return "** illegal instruction **";
    //     }
    // }

    if (binaryArray[0] == 1)
    {
        int val = binaryArray[0] + binaryArray[1] + binaryArray[2];

        if (val != 3)
        {
            isContinue = false;
        }

    }

    if (isContinue == true)
    {

        string binaryString = "";

        // turn int array to string
        for (int i = 0; i < 16; i++)
        {
            binaryString = binaryString + to_string(binaryArray[i]);
        }

        // A-instruction
        if (binaryArray[0] == 0)
        {
            int num = 0;

            for (int i = 15; i >= 0; i--)
            {

                if (binaryArray[i] == 1)
                {
                    num += pow(2, 15-i);
                }
            }

            // make the num to string
            string Astr = to_string(num);

            // return the string
            return "@"+Astr;
        }

        else if (binaryArray[0] == 1)
        {
            uint16_t jump_bits = 0;
            uint16_t dest_bits = 0;
            uint16_t comp_bits = 0;

            int index_jump = 0;

            for( int i = 15; i >= 13; i--)
            {
                if (binaryArray[i] == 1)
                {
                    jump_bits += pow(2, index_jump);
                }
                index_jump++;
            }

            int index_dest = 0;

            for( int i = 12; i >= 10; i--)
            {
                if (binaryArray[i] == 1)
                {
                    dest_bits += pow(2, index_dest);
                }
                index_dest++;
            }

            int index_comp = 0;

            for( int i = 9; i >= 3; i--)
            {
                if (binaryArray[i] == 1)
                {
                    comp_bits += pow(2, index_comp);
                }
                index_comp++;
            }

            string comp_string = aluop(comp_bits);
            string dest_string = destination(dest_bits);
            string jump_string = jump(jump_bits);
            string output = dest_string + comp_string + jump_string;
            return output;
        }
    }
    return "** illegal instruction **";
}

// emulate a cpu instruction - the Hack Computer has been initialised
// the PC contains the address in ROM of the instruction to execute
// if you want to produce additional output, use write_to_traces()
static void emulate_instruction()
{
    uint16_t pc_address = read_PC();

    if (pc_address > 32767)
    {
        write_PC(pc_address + 1);
        return;
    }

    uint16_t rom_instruction = read_ROM(pc_address);

    int rom_binaryArray[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    for (int i = 15; i >= 0; i--)
    {
        rom_binaryArray[i] = rom_instruction % 2;
        rom_instruction = rom_instruction / 2;
    }

    bool isValid = true;

    // if (rom_binaryArray[0] == 0)
    // {
    //     int val = rom_binaryArray[0] + rom_binaryArray[1] + rom_binaryArray[2];
    //     if (val <= 2 && val >= 0)
    //     {
    //         isValid = true;
    //     }
    // }

    if (rom_binaryArray[0] == 1)
    {
        int val = rom_binaryArray[0] + rom_binaryArray[1] + rom_binaryArray[2];

        if (val != 3)
        {
            isValid = false;
        }

    }

    if (isValid == true){

        write_PC(pc_address + 1);

        // A-instruction
        if (rom_binaryArray[0] == 0)
        {
            uint16_t number = 0;

            for (int i = 15; i >= 0; i--)
            {

                if (rom_binaryArray[i] == 1)
                {
                    number += pow(2, 15-i);
                }

            }

            write_A(number);
        }

        // C-instruction
        if (rom_binaryArray[0] == 1)
        {
            // x = input_D and y = input_A
            uint16_t input_A = read_A();
            uint16_t input_D = read_D();
            uint16_t ALUoutput = 0;

            int ng = 0;
            int zr = 0;

            // this should be binaryArray[12], however instruction in cpu.hdl read backwards therefore in here it should be binaryArray[3]
            if (rom_binaryArray[3] == 1)
            {
                input_A = read_RAM(input_A);
            }

            // next we need to formulate ALU calculation, also in reverse order with cpu.hdl
            // zx : x = 0
            if (rom_binaryArray[4] == 1)
            {
                input_D = 0;
            }

            // nx : x = !x
            if (rom_binaryArray[5] == 1)
            {
                input_D = ~input_D;
            }

            // then zy and ny
            if (rom_binaryArray[6] == 1)
            {
                input_A = 0;
            }

            if (rom_binaryArray[7] == 1)
            {
                input_A = ~input_A;
            }

            // f
            if (rom_binaryArray[8] == 0)
            {
                ALUoutput = input_A&input_D;
            }

            else
            {
                ALUoutput = input_A + input_D;

                if (ALUoutput > 65535)
                {
                    ALUoutput = ALUoutput - pow(2,16);
                }

            }

            // no
            if (rom_binaryArray[9] == 1)
            {
                ALUoutput = ~ALUoutput;
            }

            // ng and zr
            if (ALUoutput == 0)
            {
                zr = 1;
            }

            if (ALUoutput > 32767) // means ALUoutput is negative
            {
                ng = 1;
            }

            // write in A-register. it depends on instruction[5]. in my case, it is instruction[10]
            if (rom_binaryArray[10] == 1)
            {
                write_A(ALUoutput);
            }

            // write in D, it depends on instruction[4], in my case, it is instruction[11]
            if (rom_binaryArray[11] == 1)
            {
                write_D(ALUoutput);
            }

            // write in RAM, it is And(instruction[15], instruction[3]), instruction[15] is always 1 because it is C-instruction.
            // Therefore, instruction[3] must be 1, in my case, reverse order is 12
            if (rom_binaryArray[12] == 1)
            {
                write_RAM(read_A(), ALUoutput);
            }

            // decide whether jump or not, it depends on the jump bit (last three bit) and ng / zr according to cpu.hdl
            bool isJump = false;

            if (rom_binaryArray[13] == 1 && ng == 1)
            {
                isJump = true;
            }

            if (rom_binaryArray[14] && zr == 1)
            {
                isJump = true;
            }

            int orngzr = zr|ng;

            if (rom_binaryArray[15] && orngzr == 0)
            {
                isJump = true;
            }

            if (isJump == true)
            {
                write_PC(read_A());
            }
        }
    }
}


/*****************        DOWN TO HERE         ******************/

// if the first command line argument is "D" the test output must display disassembled instructions
bool display_disassembled_instructions = false ;

// this function only calls disassemble_instruction if required
// it is called from more_cpu_tests() and is passed the instruction being emulated
string disassemble(uint16_t instruction)
{
    if ( display_disassembled_instructions ) return disassemble_instruction(instruction) ;

    // default is to display instruction in binary
    return "\"" + std::bitset<16>(instruction).to_string() + "\"" ;
}


// main program
int main(int argc,char **argv)
{
    // force all output to be immediate - the test scripts will override this
    config_output(iob_immediate) ;
    config_errors(iob_immediate) ;

    // is the first command line argument "D", ie does the test output include disassembly of the instruction being emulated
    display_disassembled_instructions = argc > 1 && string(argv[1]) == "D" ;

    // more_cpu_tests() will initialise the next test and return true
    // if there are no more tests to be run, it returns false
    // more_cpu_tests() also outputs the results of the previous test
    while ( more_cpu_tests(disassemble) )
    {
        emulate_instruction() ;
    }

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}

