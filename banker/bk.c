#include <stdio.h>
#include <stdlib.h>
//#include <sys/types.h>
//#include "op.h"
#define POSSIBLE 1
#define IMPOSSIBLE 0
#define true 1
#define false 0

typedef struct State {
  int **MAX;
  int **Allocation;
  int *Available;
  int *Resource;
} state;

typedef struct Request {
  int *req;
  int pnum;
} request;

state *System;
state *TmpSystem;

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

void InitState(state *s, int col, int row) {
  s->Allocation = (int **)calloc(row, sizeof(int *));
  for (int i = 0; i < row; i++) {
    s->Allocation[i] = (int *)calloc(col, sizeof(int));
  }
  s->MAX = (int **)calloc(row, sizeof(int *));
  for (int i = 0; i < row; i++) {
    s->MAX[i] = (int *)calloc(col, sizeof(int));
  }
  s->Available = (int *)calloc(col, sizeof(int));
  s->Resource = (int *)calloc(col, sizeof(int));
}

void InitSystemState(state *s, int col, int row) {
  printf("Enter the Max Matrix: \n");
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      scanf("%d", &s->MAX[i][j]);
    }
  }
  printf("Your MAX Matrix is: \n");
  print_m(s->MAX, row, col);
  printf("Enter the Allocation Matrix: \n");
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      scanf("%d", &s->Allocation[i][j]);
    }
  }
  printf("Your Allocation Matrix is: \n");
  print_m(s->Allocation, row, col);
  printf("Enter the Available Vector: \n");
  for (int i = 0; i < col; i++) {
    scanf("%d", &s->Available[i]);
  }
  printf("Your Available Vector is: \n");
  for (int i = 0; i < col; i++) {
    printf("%d", s->Available[i]);
    if (i == col - 1) {
      printf("\n");
    } else {
      printf(" ");
    }
  }
}

void state_cp(int col, int row) {
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      TmpSystem->Allocation[i][j] = System->Allocation[i][j];
      TmpSystem->MAX[i][j] = System->MAX[i][j];
      if (i == 0) {
        TmpSystem->Available[j] = System->Available[j];
        TmpSystem->Resource[j] = System->Resource[j];
      }
    }
  }
}

void state_apply(int col, request rq) {
  for (int i = 0; i < col; i++) {
    System->Allocation[rq.pnum][i] += rq.req[i];
    System->Available[i] -= rq.req[i];
  }
}

int count_z(int *a, int len) {
  int count = 0;
  for (int i = 0; i < len; i++) {
    if (a[i] == 0) {
      count++;
    }
  }
  return count;
}

int is_safe(state *S, int col, int row, int *safe) { //输入临时状态TmpState
  int *visited = (int *)calloc(row, sizeof(int));
  int count = 0, count_old = 0;
  int possible = POSSIBLE;
  // int found = true;
  while (
      possible) { //如果扫描一次后visited无变化并且未所有元素进组，则证明寻路径失败
    for (int i = 0; i < row; i++) {
      int count_f = 0;
      if (visited[i]) {
        continue;
      }
      for (int j = 0; j < col; j++) {
        if (S->MAX[i][j] - S->Allocation[i][j] <= S->Available[j]) {
          count_f++;
        }
      }
      // count_old++;
      if (count_f == col) {
        safe[count++] = i;
        visited[i] = true;
        for (int j = 0; j < col; j++) {
          S->Available[j] = S->Available[j] + S->Allocation[i][j];
        }
      }
    }
    if (count_old == count && count_z(visited, row) != 0) {
      possible = IMPOSSIBLE;
      break;
    }
    if (count_z(visited, row) == 0) {
      break;
    }
    count_old = count;
  }
  free(visited);
  return possible;
}

int is_ration(int r, request rq) {
  for (int i = 0; i < r; i++) {
    if (TmpSystem->Allocation[rq.pnum][i] + rq.req[i] >
        TmpSystem->MAX[rq.pnum][i]) {
      return false;
    }
  }
  return true;
}

int is_practice(request rq, int r) {
  for (int i = 0; i < r; i++) {
    if (rq.req[i] > TmpSystem->Available[i]) {
      return false;
    }
  }
  return true;
}

int main() {
  //初始化系统状态
  int p_am, r_am;
  System = (state *)calloc(1, sizeof(state));
  TmpSystem = (state *)calloc(1, sizeof(state));
  printf("Please Enter the Amount of procces: ");
  scanf("%d", &p_am);
  printf("Please Enter the Amount of Resource Types: ");
  scanf("%d", &r_am);
  InitState(System, r_am, p_am);
  InitState(TmpSystem, r_am, p_am);
  InitSystemState(System, r_am, p_am);
  int *safe_queue = (int *)malloc(sizeof(int) * p_am);
  // int safe_count = 0;

  //初次判断
  state_cp(r_am, p_am);
  if (!is_safe(TmpSystem, r_am, p_am, safe_queue)) {
    printf("ERROR !\n");
    return 0;
  } else {
    printf("It's Safe in T0\n");
    printf("The safe queue is :\n");
    for (int i = 0; i < p_am; i++) {
      printf("P%d ", safe_queue[i]);
    }
    printf("\n");
  }

  //按需求进行分配
  while (true) {
    //输入需求
    // int flag = true;
    safe_queue = (int *)malloc(sizeof(int) * p_am);
    // safe_count = 0;
    request rq;
    rq.req = (int *)malloc(sizeof(int) * r_am);
    rq.pnum = -1;
    printf("please Enter your process number: (enter -1 to check the states)\n");
    scanf("%d", &rq.pnum);
    if (rq.pnum == -1) {
      printf("MAX Matrix: \n");
      print_m(System->MAX, p_am, r_am);
      printf("\n");
      printf("ALLOCATION Matrix: \n");
      print_m(System->Allocation, p_am, r_am);
      printf("\n");
      printf("AVAILABLE Matrix: \n");
      for (int i = 0; i < r_am; i++) {
        printf("%d", System->Available[i]);
        if (i == r_am - 1) {
          printf("\n");
        } else {
          printf(" ");
        }
      }
      printf("\n");
      continue;
    }
    printf("Please Enter the amount of resources requested:\n");
    for (int i = 0; i < r_am; i++) {
      scanf("%d", &rq.req[i]);
    }
    //进行判断
    state_cp(r_am, p_am);
    if (!is_ration(r_am, rq)) {
      printf("ERROR\n");
      continue;
    } else if (!is_practice(rq, r_am)) {
      printf("Suspend Process !\n");
      continue;
    } else {
      //预分配
      for (int i = 0; i < r_am; i++) {
        TmpSystem->Allocation[rq.pnum][i] += rq.req[i];
        TmpSystem->Available[i] -= rq.req[i];
      }
    }
    if (is_safe(TmpSystem, r_am, p_am, safe_queue)) {
      state_apply(r_am, rq);
      printf("Success!\n");
      printf("The safe queue is :\n");
      for (int i = 0; i < p_am; i++) {
        printf("P%d ", safe_queue[i]);
      }
      printf("\n");
    } else {
      printf("找不到安全序列，资源分配回退。\n");
      printf("Suspend process\n");
      state_cp(r_am, p_am);
    }
  }
  return 1;
}
