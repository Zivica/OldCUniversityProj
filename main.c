#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Constants file name */
#define CONSTANTS_FILE "constants.txt"

/* Structure to hold simulation parameters */
typedef struct {
    double birth_rate;      // Rabbit birth rate
    double death_rate;      // Rabbit death rate
    double predation_rate;  // Predation rate (fox eats rabbits)
    double fox_birth_rate;  // Fox birth rate per rabbit eaten
    double fox_death_rate;  // Fox natural death rate
} SimulationData;

/* Structure to hold population records */
typedef struct {
    int time_steps;
    int *rabbit_population;
    int *fox_population;
} PopulationRecord;

/* Function prototypes */
void load_constants(SimulationData *data);
void save_constants(SimulationData *data);
void display_constants(SimulationData *data);
void modify_constants(SimulationData *data);
void run_simulation(SimulationData *data, PopulationRecord *record);
void display_menu();
void display_populations(PopulationRecord *record);
void display_population_vs_time(PopulationRecord *record);
void display_fox_vs_rabbit(PopulationRecord *record);
void free_population_record(PopulationRecord *record);
int get_integer_input(const char *prompt);
double get_double_input(const char *prompt);

int main() {
    SimulationData data;
    PopulationRecord record = {0, NULL, NULL};
    int choice;

    /* Load simulation constants from file */
    load_constants(&data);

    while (1) {
        display_menu();
        choice = get_integer_input("Enter your choice: ");

        switch (choice) {
            case 1:
                display_constants(&data);
                break;
            case 2:
                modify_constants(&data);
                break;
            case 3:
                run_simulation(&data, &record);
                break;
            case 4:
                display_populations(&record);
                break;
            case 5:
                display_population_vs_time(&record);
                break;
            case 6:
                display_fox_vs_rabbit(&record);
                break;
            case 7:
                free_population_record(&record);
                printf("Exiting program.\n");
                exit(0);
                break;
            default:
                printf("Invalid choice. Please select a valid option.\n");
        }
    }

    return 0;
}

/* Function to load simulation constants from a file */
void load_constants(SimulationData *data) {
    FILE *file = fopen(CONSTANTS_FILE, "r");
    if (file == NULL) {
        /* If file doesn't exist, create with default values */
        file = fopen(CONSTANTS_FILE, "w");
        if (file == NULL) {
            printf("Error creating constants file.\n");
            exit(1);
        }
        /* Default values */
        data->birth_rate = 0.1;
        data->death_rate = 0.05;
        data->predation_rate = 0.02;
        data->fox_birth_rate = 0.01;
        data->fox_death_rate = 0.1;
        /* Write defaults to file */
        fprintf(file, "%.4lf\n%.4lf\n%.4lf\n%.4lf\n%.4lf\n",
                data->birth_rate, data->death_rate,
                data->predation_rate, data->fox_birth_rate,
                data->fox_death_rate);
        fclose(file);
        printf("Constants file created with default values.\n");
    } else {
        /* Read values from file */
        if (fscanf(file, "%lf\n%lf\n%lf\n%lf\n%lf",
                   &data->birth_rate,
                   &data->death_rate,
                   &data->predation_rate,
                   &data->fox_birth_rate,
                   &data->fox_death_rate) != 5) {
            printf("Error reading constants file. Using default values.\n");
            data->birth_rate = 0.1;
            data->death_rate = 0.05;
            data->predation_rate = 0.02;
            data->fox_birth_rate = 0.01;
            data->fox_death_rate = 0.1;
        }
        fclose(file);
    }
}

/* Function to save simulation constants to a file */
void save_constants(SimulationData *data) {
    FILE *file = fopen(CONSTANTS_FILE, "w");
    if (file == NULL) {
        printf("Error opening constants file for writing.\n");
        return;
    }
    fprintf(file, "%.4lf\n%.4lf\n%.4lf\n%.4lf\n%.4lf\n",
            data->birth_rate, data->death_rate,
            data->predation_rate, data->fox_birth_rate,
            data->fox_death_rate);
    fclose(file);
    printf("Constants saved successfully.\n");
}

/* Function to display current simulation constants */
void display_constants(SimulationData *data) {
    printf("\nCurrent Simulation Constants:\n");
    printf("1. Rabbit Birth Rate: %.4lf\n", data->birth_rate);
    printf("2. Rabbit Death Rate: %.4lf\n", data->death_rate);
    printf("3. Predation Rate (Fox eats Rabbits): %.4lf\n", data->predation_rate);
    printf("4. Fox Birth Rate per Rabbit Eaten: %.4lf\n", data->fox_birth_rate);
    printf("5. Fox Natural Death Rate: %.4lf\n\n", data->fox_death_rate);
}

/* Function to modify simulation constants */
void modify_constants(SimulationData *data) {
    int choice;
    double value;
    while (1) {
        display_constants(data);
        printf("Select the constant to modify (1-5) or 6 to exit: ");
        choice = get_integer_input("");
        if (choice == 6)
            break;
        switch (choice) {
            case 1:
                value = get_double_input("Enter new Rabbit Birth Rate: ");
                data->birth_rate = value;
                break;
            case 2:
                value = get_double_input("Enter new Rabbit Death Rate: ");
                data->death_rate = value;
                break;
            case 3:
                value = get_double_input("Enter new Predation Rate: ");
                data->predation_rate = value;
                break;
            case 4:
                value = get_double_input("Enter new Fox Birth Rate per Rabbit Eaten: ");
                data->fox_birth_rate = value;
                break;
            case 5:
                value = get_double_input("Enter new Fox Natural Death Rate: ");
                data->fox_death_rate = value;
                break;
            default:
                printf("Invalid choice. Please select a valid option.\n");
        }
        save_constants(data);
    }
}

/* Function to run the simulation */
void run_simulation(SimulationData *data, PopulationRecord *record) {
    int initial_rabbits, initial_foxes, duration, i;
    double rabbits = 0.0, foxes = 0.0;
    double new_rabbits, new_foxes;

    printf("\n--- Run Simulation ---\n");
    initial_rabbits = get_integer_input("Enter initial number of rabbits: ");
    initial_foxes = get_integer_input("Enter initial number of foxes: ");
    duration = get_integer_input("Enter simulation duration (time steps): ");

    if (duration <= 0) {
        printf("Duration must be a positive integer.\n");
        return;
    }

    /* Allocate memory for population records */
    record->time_steps = duration + 1;
    record->rabbit_population = (int *)malloc(sizeof(int) * record->time_steps);
    record->fox_population = (int *)malloc(sizeof(int) * record->time_steps);
    if (record->rabbit_population == NULL || record->fox_population == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    /* Initialize populations */
    record->rabbit_population[0] = initial_rabbits;
    record->fox_population[0] = initial_foxes;

    /* Simulation loop */
    for (i = 1; i < record->time_steps; i++) {
        rabbits = (double)record->rabbit_population[i - 1];
        foxes = (double)record->fox_population[i - 1];

        /* Calculate new populations based on Lotka-Volterra equations */
        new_rabbits = rabbits + (data->birth_rate * rabbits) - (data->predation_rate * rabbits * foxes);
        new_foxes = foxes + (data->fox_birth_rate * rabbits * foxes) - (data->fox_death_rate * foxes);

        /* Ensure populations don't go negative */
        if (new_rabbits < 0)
            new_rabbits = 0;
        if (new_foxes < 0)
            new_foxes = 0;

        record->rabbit_population[i] = (int)new_rabbits;
        record->fox_population[i] = (int)new_foxes;
    }

    printf("Simulation completed successfully.\n\n");
}

/* Function to display the main menu */
void display_menu() {
    printf("===== Fox-Hare Population Simulation =====\n");
    printf("1. Display Simulation Constants\n");
    printf("2. Modify Simulation Constants\n");
    printf("3. Run Simulation\n");
    printf("4. Display Population Over Time\n");
    printf("5. Display Population vs. Time Diagram\n");
    printf("6. Display Fox Population vs. Rabbit Population Diagram\n");
    printf("7. Exit\n");
    printf("===========================================\n");
}

/* Function to display populations over time */
void display_populations(PopulationRecord *record) {
    int i;
    if (record->rabbit_population == NULL || record->fox_population == NULL) {
        printf("No simulation data available. Please run a simulation first.\n\n");
        return;
    }

    printf("\nTime\tRabbits\tFoxes\n");
    printf("----------------------------\n");
    for (i = 0; i < record->time_steps; i++) {
        printf("%d\t%d\t%d\n", i, record->rabbit_population[i], record->fox_population[i]);
    }
    printf("\n");
}

/* Function to display population vs. time diagram (text-based) */
void display_population_vs_time(PopulationRecord *record) {
    int i;
    int max_time = record->time_steps - 1;
    int max_rabbits = 0, max_foxes = 0;
    int scale;

    if (record->rabbit_population == NULL || record->fox_population == NULL) {
        printf("No simulation data available. Please run a simulation first.\n\n");
        return;
    }

    /* Find maximum populations for scaling */
    for (i = 0; i < record->time_steps; i++) {
        if (record->rabbit_population[i] > max_rabbits)
            max_rabbits = record->rabbit_population[i];
        if (record->fox_population[i] > max_foxes)
            max_foxes = record->fox_population[i];
    }

    /* Determine scale */
    scale = (max_rabbits > max_foxes) ? max_rabbits : max_foxes;
    scale = scale / 50 + 1; // Scale to fit within 50 characters

    printf("\nPopulation vs. Time Diagram (Text-Based)\n");
    printf("Each '*' represents approximately %d individuals.\n", scale);
    printf("Time\tRabbits\tFoxes\n");
    printf("---------------------------------------\n");
    for (i = 0; i < record->time_steps; i++) {
        printf("%d\t", i);
        int j;
        for (j = 0; j < record->rabbit_population[i] / scale; j++)
            printf("*");
        printf("\t");
        for (j = 0; j < record->fox_population[i] / scale; j++)
            printf("*");
        printf("\n");
    }
    printf("\n");
}

/* Function to display fox population vs. rabbit population diagram (text-based) */
void display_fox_vs_rabbit(PopulationRecord *record) {
    int i;
    int max_rabbits = 0, max_foxes = 0;
    int scale_rabbits, scale_foxes;

    if (record->rabbit_population == NULL || record->fox_population == NULL) {
        printf("No simulation data available. Please run a simulation first.\n\n");
        return;
    }

    /* Find maximum populations for scaling */
    for (i = 0; i < record->time_steps; i++) {
        if (record->rabbit_population[i] > max_rabbits)
            max_rabbits = record->rabbit_population[i];
        if (record->fox_population[i] > max_foxes)
            max_foxes = record->fox_population[i];
    }

    /* Determine scale */
    scale_rabbits = max_rabbits / 50 + 1; // Scale to fit within 50 characters
    scale_foxes = max_foxes / 50 + 1;

    printf("\nFox Population vs. Rabbit Population Diagram (Text-Based)\n");
    printf("Each '*' represents approximately %d Rabbits and %d Foxes.\n", scale_rabbits, scale_foxes);
    printf("Rabbits\tFoxes\t\n");
    printf("---------------------------------------\n");
    for (i = 0; i < record->time_steps; i++) {
        printf("%d\t", record->rabbit_population[i]);
        int j;
        for (j = 0; j < record->fox_population[i] / scale_foxes; j++)
            printf("*");
        printf("\n");
    }
    printf("\n");
}

/* Function to free allocated memory for population records */
void free_population_record(PopulationRecord *record) {
    if (record->rabbit_population != NULL) {
        free(record->rabbit_population);
        record->rabbit_population = NULL;
    }
    if (record->fox_population != NULL) {
        free(record->fox_population);
        record->fox_population = NULL;
    }
    record->time_steps = 0;
}

/* Function to get integer input with prompt */
int get_integer_input(const char *prompt) {
    int value;
    char buffer[100];
    while (1) {
        if (strlen(prompt) > 0)
            printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%d", &value) == 1)
                return value;
        }
        printf("Invalid input. Please enter an integer.\n");
    }
}

/* Function to get double input with prompt */
double get_double_input(const char *prompt) {
    double value;
    char buffer[100];
    while (1) {
        if (strlen(prompt) > 0)
            printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%lf", &value) == 1)
                return value;
        }
        printf("Invalid input. Please enter a number.\n");
    }
}
