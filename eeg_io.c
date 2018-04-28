#include <stdio.h>
#include <ctype.h>
int main(void)
{
    FILE *file  = NULL;
    int symbol  = 0,
        columns = 0,
        rows    = 0;
    // Attempt to open the file "file.txt" with reading persmissions.
    file = fopen("file.txt", "r");
    // It succeeded!
    if (file != NULL)
    {
        do
        {
            // Obtain a single character from the file.
            symbol = fgetc(file);
            // Read character by character to determine the amount of columns on the first row.
            // Because every number is followed by a whitespace this equals the amount of items on a row.
            if (rows == 0 && (isspace(symbol) || feof(file)))
            {
                columns++;
            }
            // Every row is followed by a newline. But the last one needn't be.
            if (symbol == '\n' || feof(file))
            {
                rows++;
            }
        }
        while (symbol != EOF);
        // Some error occurred during reading.
        if (ferror(file))
        {
            printf("Error on reading from file.\n");
        }
        // No errors occurred, print the results.
        else
        {
            printf("The file contains %d row(s) and %d column(s).\n", rows, columns);
        }
        // Close the file.
        fclose(file);
    }
    // Opening the input file failed.
    else
    {
        perror("Error on opening the input file");
    }
    return 0;
}