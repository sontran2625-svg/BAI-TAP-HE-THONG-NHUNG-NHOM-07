#include <stdint.h>

// Khai báo đỉnh ngăn xếp từ Linker Script
extern uint32_t _estack;

// Khai báo hàm main
extern int main(void);

// Khai báo các trình phục vụ ngắt cơ bản
void Reset_Handler(void) __attribute__((used));
void NMI_Handler(void) __attribute__((weak));
void HardFault_Handler(void) __attribute__((weak));
void MemManage_Handler(void) __attribute__((weak));
void BusFault_Handler(void) __attribute__((weak));
void UsageFault_Handler(void) __attribute__((weak));

// Trình phục vụ ngắt mặc định
void Default_Handler(void) {
    while(1);
}

// Gán alias mặc định cho các ngắt
void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void) __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));

// Bảng Vector Table chuẩn cho Cortex-M3
__attribute__((section(".isr_vector")))
const uint32_t vector_table[] = {
    (uint32_t)&_estack,           // 0: Initial Stack Pointer
    (uint32_t)&Reset_Handler,     // 1: Reset Handler
    (uint32_t)&NMI_Handler,       // 2: NMI Handler
    (uint32_t)&HardFault_Handler, // 3: Hard Fault Handler
    (uint32_t)&MemManage_Handler, // 4: MPU Fault Handler
    (uint32_t)&BusFault_Handler,  // 5: Bus Fault Handler
    (uint32_t)&UsageFault_Handler // 6: Usage Fault Handler
};

__attribute__((used))
void Reset_Handler(void) {
    // Nhảy trực tiếp vào hàm main
    main();
    
    while(1);
}
