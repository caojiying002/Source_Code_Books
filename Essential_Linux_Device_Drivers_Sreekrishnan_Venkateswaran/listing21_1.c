char buffer;

int
trojan_function()
{
    int *my_variable = 0xAB, i;

    /* ... */

    Point A:
        i = *my_variable; /* Kernel Panic: my_variable points
                             to bad memory */
    return(i);
}