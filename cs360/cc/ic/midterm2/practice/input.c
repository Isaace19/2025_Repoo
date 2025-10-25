if (argc < 2) {
    printf("usage: %s <command> [args]\n", argv[0]);
    return 1;
}


// argument execv example
 execv(argv[1], argv + 1);
perror(execv);
return 111;


/*
 * this should be given as the input to the actual program to run to error check out the file to be executed
 * args in square brackets are interpreted as argument array with square brackets, which is when to use execv
 *
 *
 *
 * */
