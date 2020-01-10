unsigned short screen_width = 32;
unsigned short screen_length = 64;

unsigned short opcode;

unsigned char mem[4096];
unsigned char V[16];

unsigned short I;
unsigned short PC;

unsigned char graphics[64 * 32];

unsigned char delay_timer;
unsigned char sound_timer;

unsigned short stack[16];
unsigned short stack_pointer;

unsigned char key[16];

void init() {
  // programs start at 0x200
  PC = 0x200;
  opcode = 0;
  I = 0;
  stack_pointer = 0;

  // load font set
  // for (int i = 0; i < 80; ++i) {
  //   mem[i] = fontset[i];
  // }
}

void emulate() {
  // Fetch opcode
  opcode = mem[PC] << 8 | mem[PC + 1];

  unsigned char x = (opcode & 0x0F00) >> 8;
  unsigned char y = (opcode & 0x00F0) >> 4;
  switch (opcode & 0xF000) {
    case 0x0000:
      switch (opcode & 0x000F) {
        case 0x00E0:
          for (int i = 0; i < screen_width * screen_length; ++i) {
            graphics[i] = 0;
          }
          PC += 2;
          break;
        case 0x000E:
          PC = stack[--stack_pointer];
          break;
      }
      break;
    case 0x1000:
      PC = opcode & 0x0FFF;
      break;
    case 0x2000:
      stack[stack_pointer++] = PC;
      PC = opcode & 0x0FFF;
      break;
    case 0x3000:
      if (V[x] == (opcode & 0x00FF)) {
        PC += 4;
      } else {
        PC += 2;
      }
      break;
    case 0x4000:
      if (V[x] != (opcode & 0x00FF)) {
        PC += 4;
      } else {
        PC += 2;
      }
      break;
    case 0x5000:
      if (V[x] == V[y]) {
        PC += 4;
      } else {
        PC += 2;
      }
      break;
    case 0x6000:
      V[x] = opcode & 0x00FF;
      break;
    case 0x7000:
      V[x] += opcode & 0x00FF;
      break;
    case 0x8000:
      switch (opcode & 0x000F) {
        case 0x0000:
          V[x] = V[y];
          PC += 2;
          break;
        case 0x0001:
          V[x] |= V[y];
          PC += 2;
          break;
        case 0x0002:
          V[x] &= V[y];
          PC += 2;
          break;
        case 0x0003:
          V[x] ^= V[y];
          PC += 2;
          break;
        case 0x0004:
          if ((V[x] + V[y]) > 0xFF) {
            V[0xF] = 1;
          } else {
            V[0xF] = 0;
          }
          V[x] += V[y];
          PC += 2;
          break;
        case 0x0005:
          if (V[x] < V[y]) {
            V[0xF] = 1;
          } else {
            V[0xF] = 0;
          }
          V[x] -= V[y];
          PC += 2;
          break;
        case 0x0006:
          V[0xF] = V[x] & 0x1;
          V[x] >>= 1;
          PC += 2;
          break;
        case 0x0007:
          if (V[x] > V[y]) {
            V[0xF] = 0;
          } else {
            V[0xF] = 1;
          }
          V[x] = V[y] - V[x];
          PC += 2;
          break;
        case 0x000E:
          V[0xF] = V[x] >> 7;
          V[x] <<= 1;
          PC += 2;
          break;
      }
      break;
    case 0x9000:
      if (V[x] != V[y]) {
        PC += 4;
      } else {
        PC += 2;
      }
    case 0xA000:
      I = opcode & 0x0FFF;
      PC += 2;
      break;
    case 0xB000:
      PC = V[0] + (opcode & 0x0FFF);
      break;
    case 0xC000:
      V[x] = (rand() % 0xFF) & (opcode & 0x00FF);
      PC += 2;
      break;
    case 0xD000: {
      unsigned short height = opcode & 0x000F;
      V[0xF] = 0;
      for (int r = 0; r < V[x]; ++r) {
        unsigned short sprite_row = mem[I + r];
        for (int c = 0; c < 8; ++c) {
          if (sprite_row & (0x80 >> c) != 0) {
            if (graphics[V[x] + r + ((V[y] + r) * screen_length)] == 1) {
              V[0xF] = 1;
            }
            graphics[V[x] + r + ((V[y] + r) * screen_length)] ^= 1;
          }
        }
      }

      PC += 2;
      break;
    }
    case 0xE000:
      switch (opcode & 0x00FF) {
        case 0x009E:
          if (key[V[x]] != 0) {
            PC += 4;
          } else {
            PC += 2;
          }
          break;
        case 0x00A1:
          break;
      }
    case 0xF000:
      switch (opcode & 0x00FF) {
        case 0x0007:
          break;
        case 0x000A:
          break;
        case 0x0015:
          break;
        case 0x0018:
          break;
        case 0x001E:
          break;
        case 0x0029:
          break;
        case 0x0033:
          break;
        case 0x0055:
          break;
        case 0x0065:
          break;
      }
      break;
  }
}