#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#define CSV "nyc_pop.csv"
#define DATA "nyc_pop.data"

struct pop_entry {
  int year;
  int population;
  char boro[15];
};

long get_file_size(char *file) {
    struct stat info;
    int err = stat(file, &info);
    if (err == -1) {
        printf("Error while trying to get file size of %s: %s\n", file, strerror(errno));
        return err;
    }
    return info.st_size;
}

void read_csv() {
    // Helpful Print Statement
    printf("-read_csv started\n");

    // Opening CSV File
    int fd = open(CSV, O_RDONLY);
    if (fd == -1) {
        printf("Error while trying to open %s: %s\n", CSV, strerror(errno));
        return;
    }

    // Reading In Data
    int size = get_file_size(CSV);
    if (size == -1) return;
    char data[size];
    int err = read(fd, data, size);
    if (err == -1) {
        printf("Error while reading data from %s: %s\n", CSV, strerror(errno));
        return;
    }
    close(fd);

    // Counting Lines
    char *cp = data;
    int cnt = 0;
    while (cp = strchr(cp, '\n')) {cnt++; cp++;}

    // Parsing Data
    int i, q;
    char *header[5] = {"Manhattan","Brooklyn","Queens","Bronx","Staten Island"}, *st = data;
    struct pop_entry pop_data[5*(cnt-1)];
    for (i = 0; i < cnt; i++) {
        if (i != 0) {
            int year, nums[5];
            sscanf(st, "%d,%d,%d,%d,%d,%d\n", &year, &nums[0], &nums[1], &nums[2], &nums[3], &nums[4]);
            for (q = 0; q < 5; q++) {
                pop_data[5*(i-1)+q].year = year;
                pop_data[5*(i-1)+q].population = nums[q];
                strncpy(pop_data[5*(i-1)+q].boro, header[q], sizeof(pop_data[5*(i-1)+q].boro));
            }
        }
        st = strchr(st, '\n')+1;
    }

    // Opening Data File
    int fd2 = open(DATA, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd2 == -1) {
        printf("Error while opening %s: %s\n", DATA, strerror(errno));
        return;
    }

    // Writing Data
    err = write(fd2, pop_data, sizeof(pop_data));
    if (err == -1) {
        printf("Error while writing to %s: %s\n", DATA, strerror(errno));
        return;
    }

    // Ending Function
    printf("-read_csv completed, wrote %d bytes of data to %s\n\n", sizeof(pop_data), DATA);
    close(fd2);
    return;
}

void read_data() {
    // Helpful Print Statement
    printf("-read_data started\n");

    // Opening Data File
    int fd = open(DATA, O_RDONLY);
    if (fd == -1) {
        printf("Error while opening %s: %s\n", DATA, strerror(errno));
        return;
    }

    // Reading In Data
    int size = get_file_size(DATA), cnt = size / sizeof(struct pop_entry);
    struct pop_entry *pop_data = calloc(cnt, sizeof(struct pop_entry));
    int err = read(fd, pop_data, size);

    // Print Data
    int i;
    for (i = 0; i < cnt; i++) {
        printf("%3d:\t y: %d\t boro: %-16s\t population: %d\n", i, pop_data[i].year, pop_data[i].boro, pop_data[i].population);
    }

    // Ending Function
    printf("-read_data completed, displayed %d entries of %s\n\n", cnt, DATA);
    close(fd);
    return;
}

void add_data() {
    // Helpful Print Statement
    printf("-add_data started\n");

    // Getting New Pop_Entry
    struct pop_entry temp;
    printf("Enter new entry in format [int year] [string borough] [int population]:\n");
    char input[1024], boro[1024];
    fgets(input, 1024, stdin);
    if (sscanf(input, "%d %s %d\n", &(temp.year), &boro, &(temp.population)) != 3) {
        printf("Incorrect formatting of inputted elements\n");
        return;
    }
    strncpy(temp.boro, boro, sizeof(temp.boro));

    // Opening Data File
    int fd = open(DATA, O_WRONLY | O_APPEND);
    if (fd == -1) {
        printf("Error while opening %s: %s\n", DATA, strerror(errno));
        return;
    }

    // Appending To Data File
    int err = write(fd, &temp, sizeof(temp));
    if (err == -1) {
        printf("Error while writing to %s: %s\n", DATA, strerror(errno));
        return;
    }

    // Ending Function
    printf("-add_data completed, appended \"y: %d\t boro: %-16s\t population: %d\" to file %s\n\n", temp.year, temp.boro, temp.population, DATA);
    close(fd);
    return;
}

void update_data() {
    // Helpful Print Statement
    printf("-update_data started\n");

    // Opening Data File
    int fd = open(DATA, O_RDONLY);
    if (fd == -1) {
        printf("Error while opening %s: %s\n", DATA, strerror(errno));
        return;
    }

    // Reading In Data
    int size = get_file_size(DATA), cnt = size / sizeof(struct pop_entry);
    struct pop_entry *pop_data = calloc(cnt, sizeof(struct pop_entry));
    int err = read(fd, pop_data, size);
    close(fd);

    // Print Data
    int i;
    for (i = 0; i < cnt; i++) {
        printf("%3d:\t y: %d\t boro: %-16s\t population: %d\n", i, pop_data[i].year, pop_data[i].boro, pop_data[i].population);
    }

    // Asking For Specific Entry
    int ind;
    printf("Enter the number of the entry you want to update:\n");
    char input[1024];
    fgets(input, 1024, stdin);
    if (sscanf(input, "%d", &ind) != 1) {
        printf("Please input an integer next time\n");
        return;
    }
    if (ind < 0 || ind >= cnt) {
        printf("Integer not within entry range\n");
        return;
    }

    // Printing Out Entry And Asking For Information
    printf("%3d:\t y: %d\t boro: %-16s\t population: %d\n", ind, pop_data[ind].year, pop_data[ind].boro, pop_data[ind].population);
    struct pop_entry temp;
    printf("Enter updated entry in format [int year] [string borough] [int population]:\n");
    char input2[1024], boro[1024];
    fgets(input2, 1024, stdin);
    if (sscanf(input2, "%d %s %d\n", &(temp.year), &boro, &(temp.population)) != 3) {
        printf("Incorrect formatting of inputted elements\n");
        return;
    }
    strncpy(temp.boro, boro, sizeof(temp.boro));

    // Opening Data File
    int fd2 = open(DATA, O_WRONLY);
    if (fd2 == -1) {
        printf("Error while opening %s: %s\n", DATA, strerror(errno));
        return;
    }

    // Writing Data
    lseek(fd2, ind*sizeof(struct pop_entry), SEEK_SET);
    int err2 = write(fd2, &temp, sizeof(struct pop_entry));
    if (err2 == -1) {
        printf("Error while writing to %s: %s\n", DATA, strerror(errno));
        return;
    }

    // Ending Function
    printf("-update_data completed, updated entry %d to \"y: %d\t boro: %-16s\t population: %d\" in file %s\n\n", ind, temp.year, temp.boro, temp.population, DATA);
    close(fd2);
    return;
}

int main(int argc, char* argv[]) {
    // Error Checking
    if (argc == 1) {
        printf("Program requires at least one of the following command line arguments\n");
        printf("-read_csv, -read_data, -add_data, -update_data\n");
        return 0;
    }

    // Running Arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-read_csv") == 0) {read_csv(); continue;}
        if (strcmp(argv[i], "-read_data") == 0) {read_data(); continue;}
        if (strcmp(argv[i], "-add_data") == 0) {add_data(); continue;}
        if (strcmp(argv[i], "-update_data") == 0) {update_data(); continue;}
        printf("Command line argument %d, %s, is undefined\n", i-1, argv[i]);
    }

    return 0;
}
