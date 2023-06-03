struct Module
{
    
};

void print_string(char* str, int len)
{
    printf("len: %d\n", len);
    for (int i = 0; i < len; i++)
    {
        printf("%d, ", str[i]);
    }
    printf("\n");
}

void add_file_path(char** paths, u64* current_character, u64 index)
{

}

void modulize(char* mod_path)
{
    u64 file_size;
    char* file = load_file(mod_path, &file_size);

    // first, traverse to determine how many lines (and therefore, file names) there are

    u64 number_of_files = 1;

    // TODO: uhm later I will want to have linking to other modules and they shouldn't contribute to num of files
    // but i will just rewrite this when I add that functionality tbh or something
    for (u64 i = 0; i < file_size; i++)
    {
        char character = file[i];

        if (character == '\n')
            number_of_files++;
    }

    printf("Num files: %llu\n", number_of_files);

    char** file_paths = malloc(sizeof(char*) * number_of_files);

    u64 current_character = 0;
    u64 file_index = 0;

    while (current_character < file_size)
    {

    }
}