// #include <chrono>
#include <tuple>
#include <vector>

#include "chardev.hpp"
#include "mem-dev.hpp"
#include "pia1.hpp"
#include "plugin-callback.hpp"
#include "via.hpp"

plugin_callback_t plugin_callback;

Chardev *chardev;
Pia1 *pia1;
Via *via;

extern "C" int plugin_load() {
  chardev = new Chardev();
  if (!chardev->load_success) {
    delete chardev;
    return -1;
  }

  pia1 = new Pia1();

  via = new Via();

  return 0;
}

extern "C" int plugin_init(std::vector<std::pair<MemoryMappedDevice *, Word>> &devs, plugin_callback_t callback) {
  plugin_callback = callback;

  chardev->init_sdl();
  devs.push_back({chardev, 0x8000});
  devs.push_back({chardev, 0x8400});
  devs.push_back({chardev, 0x8800});
  devs.push_back({chardev, 0x8c00});

  devs.push_back({pia1, 0xe810});
  devs.push_back({pia1, 0xe814});
  devs.push_back({pia1, 0xe818});
  devs.push_back({pia1, 0xe81c});

  devs.push_back({via, 0xe840});

  return 0;
}

extern "C" int plugin_destroy() {
  if (chardev)
    delete chardev;

  if (pia1)
    delete pia1;

  return 0;
}

extern "C" int plugin_update(int cycles_elapsed) {
  if (via) {
    for (int i = 0; i < cycles_elapsed; i++)
      via->update();
  }

  return 0;
}
