int main() {
    double bottom_tail = gsl_cdf_gaussian_P(-1.96, 1);
    printf("Area between [-1.96, 1.96]: %g\n", 1-2*bottom_tail);
}

void test1() {
    char *repstext = getenv("reps");
    int reps = repstext ? atoi(repstext) : 10;
    char *msg = getenv("msg");
    if (!msg) msg = "Hello.";

    for (int i=0; i < reps; i++) {
        printf("%s\n", msg);
    }
}
