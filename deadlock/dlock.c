#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0

typedef struct System_Statement {
  int **Max;
  int **Allocation;
  int **Q;
  int *Available;
} statement;

typedef struct Request {
  int *req;
  int pnum;
} request;

statement System_s;
// request rq;
int process, resources, dead_process;
int *marked;

void print_m(int **s, int r, int c) {
  for (int i = 0; i < r; i++) {
    for (int j = 0; j < c; j++) {
      printf("%d", s[i][j]);
      if (j != c - 1) {
        printf(" ");
      } else {
        printf("\n");
      }
    }
  }
}

void Init_System() {
  printf("请输入进程数目: ");
  scanf("%d", &process);
  printf("请输入资源数目: ");
  scanf("%d", &resources);
  System_s.Max = (int **)malloc(sizeof(int *) * process);
  printf("请输入Max矩阵: \n");
  for (int i = 0; i < process; i++) {
    System_s.Max[i] = (int *)malloc(sizeof(int) * resources);
    for (int j = 0; j < resources; j++) {
      scanf("%d", &System_s.Max[i][j]);
    }
  }
  System_s.Allocation = (int **)malloc(sizeof(int *) * process);
  printf("请输入Allocation矩阵: \n");
  for (int i = 0; i < process; i++) {
    System_s.Allocation[i] = (int *)malloc(sizeof(int) * resources);
    for (int j = 0; j < resources; j++) {
      scanf("%d", &System_s.Allocation[i][j]);
    }
  }
  System_s.Q = (int **)malloc(sizeof(int *) * process);
  printf("请输入请求矩阵Q: \n");
  for (int i = 0; i < process; i++) {
    System_s.Q[i] = (int *)malloc(sizeof(int) * resources);
    for (int j = 0; j < resources; j++) {
      scanf("%d", &System_s.Q[i][j]);
    }
  }
  System_s.Available = (int *)malloc(sizeof(int) * resources);
  printf("请输入Available矩阵: \n");
  for (int j = 0; j < resources; j++) {
    scanf("%d", &System_s.Available[j]);
  }
}

int check_deadlock() {
  marked = realloc(marked, sizeof(int) * process);
  for (int i = 0; i < process; i++) {
    marked[i] = 0;
  }
  int *W = (int *)malloc(sizeof(int) * resources);
  for (int i = 0; i < resources; i++) {
    W[i] = System_s.Available[i];
  }
  int len = 0, has_dlock = false, end_flag = false;
  // 先将Alloction中无分配的进程标记
  for (int i = 0; i < process; i++) {
    int z_flag = true;
    for (int j = 0; j < resources; j++) {
      if (System_s.Allocation[i][j] != 0) {
        z_flag = false;
        break;
      }
    }
    if (z_flag == true) {
      marked[i] = i + 1;
      len++;
    }
  }
  while (!end_flag) {
    int old_len = len;
    for (int i = 0; i < process; i++) {
      if (marked[i] != 0) {
        continue;
      }
      int pass_flag = true;
      for (int j = 0; j < resources; j++) {
        if (System_s.Q[i][j] > W[j]) {
          pass_flag = false;
          break;
        }
      }
      if (pass_flag) {
        marked[i] = i + 1;
        len++;
      }
    }
    if (old_len == len && len < process) {
      has_dlock = true;
      end_flag = true;
    } else if (len == process) {
      end_flag = true;
    }
  }
  free(W);
  return has_dlock;
}

void cancel(int pid) {
  for (int i = 0; i < resources; i++) {
    System_s.Available[i] += System_s.Allocation[pid][i];
    System_s.Q[pid][i] += System_s.Allocation[pid][i];
    System_s.Allocation[pid][i] = 0;
  }
}

void release() {
  int *dead = (int *)malloc(sizeof(int) * dead_process);
  int count = 0;
  for (int i = 0; i < process; i++) {
    if (marked[i] == 0) {
      dead[count++] = i;
      cancel(i);
    }
    if (!check_deadlock()) {
      printf("取消了死锁进程：\n");
      for (int j = 0; j < count; j++) {
        printf("P%d ", dead[j]);
      }
      printf("\n");
      printf("死锁已经解除.\n");
      break;
    }
  }
}

int edit_Q() {
  int tmp_id;
  int *rq = (int *)malloc(sizeof(int) * resources);
  printf("请输入你要分配的进程: ");
  scanf("%d", &tmp_id);
  printf("请输入你要分配的资源数: \n");
  for (int i = 0; i < resources; i++) {
    scanf("%d", &rq[i]);
  }
  for (int i = 0; i < resources; i++) {
    if (rq[i] + System_s.Allocation[tmp_id][i] > System_s.Max[tmp_id][i]) {
      free(rq);
      printf("超出最大请求\n");
      return -1; //超出最大请求，不合理
    } else if (rq[i] > System_s.Available[i]) {
      printf("超出可分配资源数\n");
      free(rq);
      return -2; //超出可分配资源，不可行
    }
  }
  for (int i = 0; i < resources; i++) {
    System_s.Q[tmp_id][i] -= rq[i];
    System_s.Allocation[tmp_id][i] += rq[i];
    System_s.Available[i] -= rq[i];
  }
  free(rq);
  return true;
}

// int alc() {
//   for (int i = 0; i < process; i++) {
//     for (int j = 0; j < resources; j++) {
//       System_s.Allocation[i][j] += System_s.Q[i][j];
//       System_s.Available[j] -= System_s.Q[i][j];
//       System_s.Q[i][j] = 0;
//     }
//   }
//   return 0;
// }

int main() {
  Init_System();
  // rq.req = (int *)malloc(sizeof(int) * resources);
  marked = (int *)calloc(process, sizeof(int));
  int end_flag = false;
  while (end_flag == false) {
    int opt_flag = 0;
    printf("请选择您需要的操作：\n");
    printf("1. 修改请求矩阵Q\n");
    printf("2. 检测死锁\n");
    printf("3. 死锁解除\n");
    // printf("4. 分配资源\n");
    printf("5. 查看当前系统状态\n");
    printf("6. 退出\n");
    scanf("%d", &opt_flag);
    switch (opt_flag) {
    case 1:
      printf("请输入请求矩阵Q: \n");
      edit_Q();
      break;
    case 2:
      dead_process = 0;
      if (check_deadlock()) {
        printf("存在死锁,死锁进程为：\n");
        for (int i = 0; i < process; i++) {
          if (marked[i] == 0) {
            dead_process++;
            printf("P%d ", i);
          }
        }
        printf("\n");
      } else {
        printf("不存在死锁.\n");
      }
      break;
    case 3:
      release();
      break;
    // case 4:
    //   alc();
    //   break;
    case 5:
      printf("Allocation矩阵:\n");
      print_m(System_s.Allocation, process, resources);
      printf("\n");
      printf("Request矩阵Q:\n");
      print_m(System_s.Q, process, resources);
      printf("\n");
      printf("Available向量:\n");
      for (int i = 0; i < resources; i++) {
        printf("%d ", System_s.Available[i]);
      }
      printf("\n");
      break;
    case 6:
      end_flag = true;
      break;
    default:
      printf("输入错误，请重新输入\n\n");
      break;
    }
  }
}