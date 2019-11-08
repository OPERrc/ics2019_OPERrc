#include <am.h>
#include <x86.h>
#include <klib.h>

static _Context* (*user_handler)(_Event, _Context*) = NULL;

void __am_irq0();
void __am_vecsys();
void __am_vectrap();
void __am_vecnull();

_Context* __am_irq_handle(_Context *c) {
  _Context *next = c;
  /*printf("eax = 0x%x\n", c->eax);
  printf("ecx = 0x%x\n", c->ecx);
  printf("edx = 0x%x\n", c->edx);
  printf("ebx = 0x%x\n", c->ebx);
  printf("esp = 0x%x\n", c->esp);
  printf("ebp = 0x%x\n", c->ebp);
  printf("esi = 0x%x\n", c->esi);
  printf("edi = 0x%x\n", c->edi);
  printf("eflags = 0x%x\n", c->eflags);
  printf("cs = 0x%x\n", c->cs);
  printf("pc = 0x%x\n", c->eip);
  printf("irq = 0x%x\n", c->irq);
  printf("as = 0x%x\n", c->as);*/

  if (user_handler) {
    _Event ev = {0};
    switch (c->irq) {
      case 32:
      case 0x80:
      case 0x81: ev.event = _EVENT_YIELD; break; 
      default: ev.event = _EVENT_ERROR; break;
    }

    next = user_handler(ev, c);
    if (next == NULL) {
      next = c;
    }
  }

  return next;
}

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  static GateDesc idt[NR_IRQ];

  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i] = GATE(STS_TG32, KSEL(SEG_KCODE), __am_vecnull, DPL_KERN);
  }

  // ----------------------- interrupts ----------------------------
  idt[32]   = GATE(STS_IG32, KSEL(SEG_KCODE), __am_irq0,   DPL_KERN);
  // ---------------------- system call ----------------------------
  idt[0x80] = GATE(STS_TG32, KSEL(SEG_KCODE), __am_vecsys, DPL_USER);
  idt[0x81] = GATE(STS_TG32, KSEL(SEG_KCODE), __am_vectrap, DPL_KERN);

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;

  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
  return NULL;
}

void _yield() {
  asm volatile("int $0x81");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}
