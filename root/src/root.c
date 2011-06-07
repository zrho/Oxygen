int main() {
    asm volatile (
        "mov $0x0,%rax; \
        mov $0xDEADB000,%r8; \
        mov $0x1000,%r9; \
        mov $0x0,%r10; \
        mov $0x3,%r11; \
        int $0x80");
    //*((int *) 0xDEADBEEF) = 0x0;
    asm volatile ("mov $0x1,%rax;int $0x80");
    while (1);
    return 0;
}
