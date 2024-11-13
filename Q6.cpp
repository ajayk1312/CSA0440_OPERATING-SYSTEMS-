#include <stdio.h>

#define MAX 100

struct Process {
    int id;
    int burst_time;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
};

void round_robin(struct Process p[], int n, int quantum) {
    int time = 0;  
    int completed = 0;  

    for (int i = 0; i < n; i++) {
        p[i].remaining_time = p[i].burst_time;
    }

    while (completed < n) {
        for (int i = 0; i < n; i++) {
            if (p[i].remaining_time > 0) {
                if (p[i].remaining_time > quantum) {
                    time += quantum;
                    p[i].remaining_time -= quantum;
                } else {
                    time += p[i].remaining_time;
                    p[i].waiting_time = time - p[i].burst_time;
                    p[i].turnaround_time = time;
                    p[i].remaining_time = 0;
                    completed++;
                }
            }
        }
    }
}

void display_process_info(struct Process p[], int n) {
    printf("Process ID\tBurst Time\tWaiting Time\tTurnaround Time\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t\t%d\t\t%d\t\t%d\n", p[i].id, p[i].burst_time, p[i].waiting_time, p[i].turnaround_time);
    }
}

int main() {
    int n, quantum;
    struct Process processes[MAX];

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        processes[i].id = i + 1;
        printf("Enter burst time for process %d: ", i + 1);
        scanf("%d", &processes[i].burst_time);
    }

    printf("Enter time quantum: ");
    scanf("%d", &quantum);

    round_robin(processes, n, quantum);

    display_process_info(processes, n);

    float total_waiting_time = 0, total_turnaround_time = 0;
    for (int i = 0; i < n; i++) {
        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;
    }

    printf("\nAverage Waiting Time: %.2f\n", total_waiting_time / n);
    printf("Average Turnaround Time: %.2f\n", total_turnaround_time / n);

    return 0;
}
