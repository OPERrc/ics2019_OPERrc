#include <ndl.h>

// ======================= I/O Extension (IOE) =======================

void _ioe_init();
size_t _io_read(uint32_t dev, uintptr_t reg, void *buf, size_t size);
size_t _io_write(uint32_t dev, uintptr_t reg, void *buf, size_t size);

uint32_t uptime() {
  _DEV_TIMER_UPTIME_t uptime;
  _io_read(_DEV_TIMER, _DEVREG_TIMER_UPTIME, &uptime, sizeof(uptime));
  return uptime.lo;
}

int read_key() {
  _DEV_INPUT_KBD_t key;
  _io_read(_DEV_INPUT, _DEVREG_INPUT_KBD, &key, sizeof(_DEV_INPUT_KBD_t));
  int ret = key.keycode;
  if (key.keydown) ret |= 0x8000;
  return ret;
}

void draw_rect(uint32_t *pixels, int x, int y, int w, int h) {
  _DEV_VIDEO_FBCTL_t ctl = (_DEV_VIDEO_FBCTL_t) {
    .pixels = pixels,
    .x = x, .y = y, .w = w, .h = h,
    .sync = 0,
  };
  _io_write(_DEV_VIDEO, _DEVREG_VIDEO_FBCTL, &ctl, sizeof(ctl));
}

void draw_sync() {
  _DEV_VIDEO_FBCTL_t ctl;
  ctl.pixels = NULL;
  ctl.x = ctl.y = ctl.w = ctl.h = 0;
  ctl.sync = 1;
  _io_write(_DEV_VIDEO, _DEVREG_VIDEO_FBCTL, &ctl, sizeof(ctl));
}

int screen_width() {
  _DEV_VIDEO_INFO_t info;
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, &info, sizeof(info));
  return info.width;
}

int screen_height() {
  _DEV_VIDEO_INFO_t info;
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, &info, sizeof(info));
  return info.height;
}