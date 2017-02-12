void push_param(Token token) {
    if (token.type == 'W') {
        longjmp(_error_jmp_buf, ERR_UNKNOWN_WORD);
    }
    else {
        printf("TODO: Implement push_param\n\n");
    }
}
