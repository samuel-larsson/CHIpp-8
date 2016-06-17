#include "chip8.h"
#include <iostream>

void chip8::initialize(){
  pc = 0x200;   //PC starts at 0x200
  opcode = 0;   //Reset current opcode
  I = 0;        //Reset index reg.
  sp = 0;       //Reset stack pointer

  //Clear memory
  for(int i = 0; i <= 4096; i++){
    memory[i] = 0;
  }
  //Clear registers V0-VF
  for(int i = 0; i <= 16; i++){
    V[i] = 0;
  }
  //Clear stack
  for(int i = 0; i <= 16; i++){
    stack[i] = 0;
  }
  //Clear display
  for(int i = 0; i <= 64*32; i++){
    gfx[i] = 0;
  }

  //Load fontset
  for(int i = 0; i < 80; ++i){
    memory[i] = chip8_fontset[i];
  }

  //Reset timers
  delay_timer = 0;
  sound_timer = 0;

}

void chip8::emulateCycle(){
  //Fetch opcode
  opcode = memory[pc] << 8 | memory[pc + 1];    // XX00 || 00YY == XXYY

  //Decode opcode
  switch(opcode & 0xF000){
    case 0x0000:
      switch(opcode & 0x000F){
        case 0x0000:  //0x00E0: Clears the screen
        //EXECUTE
        break;
        case 0x000E:  //0x00EE: Returns from a subroutine
        //EXECUTE
        break;

        default:
          std::cout << "Unknown opcode [0x0000]: 0x" << opcode << "\n";
          break;
      }
      break;

    case 0x1000:    //0x1NNN Jump to location NNN
      pc = opcode & 0x0FFF;
      break;

    case 0x2000:    //0x2NNN Call subroutine at NNN
      stack[sp] = pc;
      sp++;
      pc = opcode & 0x0FFF;
      break;

    case 0x3000:    //0x3XNN  Skips next instruction if VX == NN
      if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
        pc += 4;
      }
      else {
        pc += 2;
      }
      break;

    case 0x4000:    //0x4XNN Skips next instruction if VX != NN
      if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)){
        pc += 4;
      } else {
        pc += 2;
      }
      break;

    case 0x5000:    //0x5XY0 Skips next intr. if VX == VY
      if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]){
        pc += 4;
      } else {
        pc += 2;
      }
      break;

    case 0x6000:    //0x6XNN Loads NN to VX
      V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
      pc += 2;
      break;

    case 0x7000:    //0x7XNN Loads NN+VX to VX
      V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
      pc += 2;
      break;

    case 0x8000:    //0x8XYN has 9 different branches depending on last 4 bits
      switch(opcode & 0x000F){

        case 0x0000: //0x8XY0 Move VY to VX
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;

        case 0x0001: //0x8XY1 Set VX to (VX OR VY)
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] | V[(opcode & 0x0F00) >> 8];
          pc += 2;
          break;

        case 0x0002: //0x8XY2 Set VX to (VX AND VY)
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] & V[(opcode & 0x0F00) >> 8];
          pc += 2;
          break;

        case 0x0003: //0x8XY3 Set VX to (VX XOR VY)
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] ^ V[(opcode & 0x0F00) >> 8];
          pc += 2;
          break;

        case 0x0004: //0x8XY4 Set VX = VX + VY, VF = carry
          if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])){
            V[0xF] = 1;
          } else {
            V[0xF] = 0;
          }
          V[(opcode & 0x0F00) >> 8] += V[opcode & 0x00F0 >> 4];
          pc += 2;
          break;

        case 0x0005: //0x8XY5 Set VX = VX - VY, VF = NOT borrow
          if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]){
            V[0xF] = 1;
          }
          else {
            V[0xF] = 0;
          }
          V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
          break;

          
          //CONTINUE HERE

        default:
          std::cout << "Unknown opcode [0x8000]: 0x" << opcode << "\n";
          break;

      }
      break;

    case 0xA000:    //0xANNN: Sets I to the address NNN
      I = opcode & 0x0FFF;
      pc += 2;
      break;

    default:
      std::cout << "Unknown opcode: 0x" << opcode << "\n";
      break;
  }

  //Update timers
  if(delay_timer > 0)
    --delay_timer;

  if(sound_timer > 0){
    if(sound_timer == 1){
      std::cout << "BEEP\n";
      --sound_timer;
    }
  }
}
