
int main()
{
    int a = 5;
    int b = 10;
    int c = 1;
    int p = a  + c;

    if (0) 
    {
        printf("Never Visited\n");
    }

    int x = 10 + p + 9 * 7;

    for (int i = 0; i < 5; i++)
    {
        printf("loop unrolling");
    }

    if (1)
    {
        printf("Visited once.\n");
    }

    int d = a + 2 * 4;

    for (int i = 0; i < 3; i++)
    {
        printf("loop unrolling 2\n");
    }


printf("result : %d",a+b);
    return 0;
}