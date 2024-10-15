#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL 5 
#define MAX_LINE_LENGTH 100
int input_arr_len = 0;



struct process {  // this struct denotes how will a process looks like
    int pid;
    int arrival_time;
    double burst_time;
};

// The test file is the "input.txt" in the same folder of this SJF_prediction.
// If you are trying to make a new test file make sure each line of the testfile is like this: "pid arrival_time burst_time"


double *read_to_arr() {  // This function reads data in input.txt into array.
    // this function has passed the test, it correctly read non-space element into array and return it.
    FILE *fp = fopen("./input.txt", "r");
    static double result[MAX_LINE_LENGTH] = {0};
    int temp[MAX_LINE_LENGTH] = {0};
    int i = 0;
    int rec = 0;
    if (fp == NULL) {
        printf("emty file or unable to open");
        return NULL;
    }
    while (i < MAX_LINE_LENGTH && !feof(fp)) {
        int ch = fgetc(fp);
        //printf("%d\n", ch);
        if (ch == EOF) {
            break;
        }
        if (ch != ' ') {
            ungetc(ch, fp);  // put read char back
            fscanf(fp, "%d", &temp[i]);
            //printf("temp %d: %d \n", i, temp[i]);
            i++;
        }
    }
    printf("read to array, finished\n");
    input_arr_len = i;
    fclose(fp);
    for (int n = 0; n<i; n++){
        rec = temp[n];
        result[n] = (double) rec /100.0 * 100.0;
        //printf("result %d: %f \n", n, result[n]);
    }
    return result;
}


// function to compare two processes based on their burst times
int compare_processes(const void* a, const void* b) {
    struct process* process_a = (struct process*) a;
    struct process* process_b = (struct process*) b;
    return process_a->burst_time - process_b->burst_time;
}

double Avg(double num_lst[], int len);
int* Slice(double lst[], int start, int end);


// the process input is like [pid, arrv_tm, brst_tm]
// pass in the form of [process1, process2, ..., processn]
double* Predict_burst_time(double burst_time_list[], double alpha, int method, int size){  
    // this function will return a list of prediction result of Burst time
    // method could be either 1 or 2; 1: average, 2: aging
    double temp = 0.0;
    double* result = (double*) malloc(sizeof(double) * size);
    if (!burst_time_list) {printf("empty list, aborting.\n"); return NULL;}
    if (method == 1) {
        printf("Choose to use method 1: average\n");
        // t_{n} = (sum(t_{0~n-1})/n-1)
        //result[1] = Avg(Slice(burst_time_list, 0, 1), 2);
        //printf("result[0]:%f\n", (double) burst_time_list[0]);
        for (int i=1; i < size; i++) {
            result[i] = Avg(Slice(burst_time_list, 0, i), i); 
        }
        //for (int i = 0; i<size; i++){printf("methd_1_arr_element %d: %f \n", i, result[i]);}
        return result;
    }
    else if (method == 2) { // T_{n+1} = alpha t_n + (1 - alpha) T_n , where t_n is the actual burst_time
        // aging have some problems
        printf("Choose to use method 2: aging\n");
        result[0] = burst_time_list[0];
        for (int i=1; i < size; i++) {
            result[i] = alpha * burst_time_list[i-1] + (1 - alpha) * result[i-1]; 
        }
        //for (int i = 0; i < size; i++){printf("methd_2_arr_element %d: %f \n", i, result[i]);}
        return result;
    }
}

double Avg(double num_lst[], int len) {
    // this function simply calculate the average of num_lst
    double sum = 0;
    for (int i=0; i < len; i++) {
        sum += num_lst[i];
    }
    return sum / len;
}

int* Slice(double lst[], int start, int end) { 
    // this function creates a slice of the array lst starts from x and ends at y
    // this function has passed the test for generating a slice of passed array.
    double* result = (double*) malloc((end-start) * sizeof(double));
    for (int i=start; i < end; i++) {
        result[i-start] = lst[i];
    }
    return result;
}

// getter and setter for prediction of burst time.

double* get_burst_time(struct process process_list[], int size) {
    // the getter has passed the test to get the burst time from a process_list
    double* result = (double*) malloc(sizeof(double) * size);
    for (int i=0; i < size; i++) {
        result[i] = process_list[i].burst_time;
        //printf("burst_time %d: %d\n", i, result[i]);
    }
    return result;
}

void set_burst_time(struct process process_list[], double burst_time_list[], int size) {
    // set_burst_time passed the set test
    for (int i=0; i<size; i++) {
        process_list[i].burst_time = burst_time_list[i];
    }
    /*for (int i=0; i< size; i++) {
        printf("Struct %d: pid: %d, arrival time: %d, burst time: %d \n", i, process_list[i].pid, process_list[i].arrival_time, process_list[i].burst_time);
    }*/
}

double* SJF (struct process process_list[], double alpha, int method, int size) {
    // TODO: modify the function for aging method
    double* waiting_time = (double*) malloc(sizeof(double) * size);
    double* turnaround_time = (double*) malloc(sizeof(double) * size);
    double* burst_time_list = get_burst_time(process_list, size);
    double* predicted_burst_time = Predict_burst_time(burst_time_list, alpha, method, size);  // return 
    //for (int i=0; i < size; i++) {printf("Predicted list item %d: %f \n", i, predicted_burst_time[i]);}
    set_burst_time(process_list, predicted_burst_time, size);
    
    /*for (int i=0; i< size; i++) {
        printf("Struct %d: pid: %d, arrival time: %d, burst time: %f \n", i, process_list[i].pid, process_list[i].arrival_time, process_list[i].burst_time);
    }*/
    // sort the processes based on their burst times using qsort function
    qsort(process_list, size, sizeof(struct process), compare_processes);

    // calculate waiting time for each process
    int total_waiting_time = 0;
    for (int i=0; i < size; i++) {
        if (i == 0) {
            waiting_time[i] = 0;
        } else {
            waiting_time[i] = waiting_time[i-1] + process_list[i-1].burst_time;
        }
        total_waiting_time += waiting_time[i];
    }
    // calculate turnaround time for each process
    for (int i=0; i < size; i++) {
        turnaround_time[i] = waiting_time[i] + process_list[i].burst_time;
    }

    // calculate average waiting time and turnaround time
    double* result = (double*) malloc(sizeof(double) * 2);
    result[0] = (double) total_waiting_time / size;
    result[1] = Avg(turnaround_time, size);
    return result;
}

void init_process_list(double arr[], struct process process_list[], int size) { 
    // this function takes a int array and initialize a process array
    // this function has passed the test to intitialize a process array.
    int i;
    //printf("test for process list intialization\n");
    for (i = 0; i < size; i++) {
        int index = i * 3;
        process_list[i].pid = arr[index];
        process_list[i].arrival_time = arr[index + 1];
        process_list[i].burst_time = arr[index + 2];
        //printf("Struct %d: pid: %d, arrival time: %d, burst time: %d \n", i, process_list[i].pid, process_list[i].arrival_time, process_list[i].burst_time);
    }
}


int main() {
    double *input = read_to_arr();
    int size = input_arr_len / 3;  // calculate the size of process_list
    //for (int i = 0; i<size; i++){printf("arr_element %d: %d\n", i, input[i]);}
    printf("length of input array: %d\n", size);
    //printf("size of new struct list: %d\n", size);
    struct process process_list[size];
    init_process_list(input, process_list, size);
    /*for (int i=0; i< size; i++) {
        printf("Struct %d: pid: %d, arrival time: %d, burst time: %d \n", i, process_list[i].pid, process_list[i].arrival_time, process_list[i].burst_time);
    }*/
    double* result1 = SJF(process_list, 0.2, 1, size);
    printf("The average waiting time and trunaround time of method 1 is: %f and %f.\n", result1[0], result1[1]);
    double* result2 = SJF(process_list, 0.2, 2, size);
    printf("The average waiting time and trunaround time of method 2 is: %lf and %f.\n", result2[0], result2[1]);
}


