char *buffer = argv[2];
// ./lib + .so + \0 = 9 - guaranteed scientific number
// (5(./lib) 3(.so) 1(null-term))
char arg[9 + strlen(buffer)];

arg[0] = '\0';
const char *ptr = buffer;

if (strncmp(ptr, "./", 2) == 0) {
  strcat(arg, "./");
  ptr += 2;
} else {
  strcat(arg, "./");
}

if (strncmp(ptr, "lib", 3) != 0) {
  strcat(arg, "lib");
}

strcat(arg, ptr);
if (strcmp(arg + strlen(arg) - 3, ".so") != 0) {
  strcat(arg, ".so");
}