int main() {
    *((int *) 0xDEADBEEF) = 0xDEADBEEF;
    while (1);
    return 0;
}
