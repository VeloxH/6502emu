#pragma once
#include "common.hpp"

struct MemoryMappedDevice {
  MemoryMappedDevice(bool ro, SByte mapc) : read_only(ro), num_mapped_regs(mapc) { mapped_regs = new Byte *[mapc]; }
  const bool read_only = false;
  const SByte num_mapped_regs;
  Byte **mapped_regs;

  virtual int pre_update() = 0;
  virtual int post_update() = 0;
};
