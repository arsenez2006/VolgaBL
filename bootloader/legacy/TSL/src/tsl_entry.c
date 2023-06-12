void _start() {
    asm("cli\nmov eax, 0xCCCCCCCC\nhalt:\nhlt\njmp short halt\n");
}
