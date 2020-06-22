#include <avr/io.h>

uint8_t* co_psp;
uint8_t* co_ksp;

void coswitch(uint8_t* sktop) {
    co_psp = sktop;
}

void costart(uint8_t* sktop, void* coroutine) {
    co_psp = sktop;
    asm volatile(
        "; save the kernel stack pointer\n\t"
        "in __tmp_reg__, %[_SPL_]\n\t"
        "sts co_ksp, __tmp_reg__\n\t"
        "in __tmp_reg__, %[_SPH_]\n\t"
        "sts co_ksp+1, __tmp_reg__\n\t"
        "; enable the proc stack\n\t"
        "lds __tmp_reg__,co_psp\n\t"
        "out %[_SPL_],__tmp_reg__\n\t"
        "lds __tmp_reg__,co_psp+1\n\t"
        "out %[_SPH_],__tmp_reg__\n\t"
        "; push the coroutine address onto proc stack\n\t"
        "push %A0\n\t"
        "push %B0\n\t"
        "; jump to the coroutine on ret at end of function\n\t"
        : :
        "e" (coroutine),
        [_SPL_] "i" _SFR_IO_ADDR(SPL),
        [_SPH_] "i" _SFR_IO_ADDR(SPH)
    );
    return;
}

void coyield() {
    asm volatile(
        "; save the proc stack pointer\n\t"
        "in __tmp_reg__, %[_SPL_]\n\t"
        "sts co_psp, __tmp_reg__\n\t"
        "in __tmp_reg__, %[_SPH_]\n\t"
        "sts co_psp+1, __tmp_reg__\n\t"
        "; restore the kernel stack\n\t"
        "lds __tmp_reg__,co_ksp\n\t"
        "out %[_SPL_],__tmp_reg__\n\t"
        "lds __tmp_reg__,co_ksp+1\n\t"
        "out %[_SPH_],__tmp_reg__\n\t"
        : :
        [_SPL_] "i" _SFR_IO_ADDR(SPL),
        [_SPH_] "i" _SFR_IO_ADDR(SPH)
    );
    return;
}

void coresume() {
    asm volatile(
        "; save the kernel stack pointer\n\t"
        "in __tmp_reg__, %[_SPL_]\n\t"
        "sts co_ksp, __tmp_reg__\n\t"
        "in __tmp_reg__, %[_SPH_]\n\t"
        "sts co_ksp+1, __tmp_reg__\n\t"
        "; restore the proc stack\n\t"
        "lds __tmp_reg__,co_psp\n\t"
        "out %[_SPL_],__tmp_reg__\n\t"
        "lds __tmp_reg__,co_psp+1\n\t"
        "out %[_SPH_],__tmp_reg__\n\t"
        : :
        [_SPL_] "i" _SFR_IO_ADDR(SPL),
        [_SPH_] "i" _SFR_IO_ADDR(SPH)
    );
}

void coend() {
    co_psp = 0;
    asm volatile(
        "; restore the kernel stack\n\t"
        "lds __tmp_reg__,co_ksp\n\t"
        "out %[_SPL_],__tmp_reg__\n\t"
        "lds __tmp_reg__,co_ksp+1\n\t"
        "out %[_SPH_],__tmp_reg__\n\t"
        : :
        [_SPL_] "i" _SFR_IO_ADDR(SPL),
        [_SPH_] "i" _SFR_IO_ADDR(SPH)
    );
    return;
}

uint8_t* cogetstack() {
    return co_psp;
}
