#include <stdio.h>
#include <stdlib.h>

#define True 1
#define False 0
#define USED 1
#define UNUSED 0

int id_count = 1;
int mem_block_count = 0;
int mem_size = 0;
int mem_begin = 0;

typedef struct Node {
  int id;
  int size;
  int begin;
  int status;
  int is_first;
} node;

typedef struct Table {
  struct Table *next;
  node *n;
} MTable;

MTable Memory;

int insert_node(MTable *n, MTable *p) {
  p->next = n->next;
  n->next = p;
  return 1;
}

int left_mem(MTable m) {
  MTable *mm = &m;
  int res = 0;
  if (mm->next == NULL) {
    return mm->n->size;
  }
  do {
    if (mm->n->status == UNUSED) {
      res += mm->n->size;
    }
    mm = mm->next;
  } while (mm->next != NULL);
  if (mm->n->status == UNUSED) {
    res += mm->n->size;
  }
  return res;
}

void alc(int size_s, MTable *p) {
  if (p->n->size == size_s) {
    p->n->status = USED;
    // p->n->id = ++id_count;
    return;
  }
  MTable *tmp_Mtable = (MTable *)malloc(sizeof(MTable));
  tmp_Mtable->n = (node *)malloc(sizeof(node));
  tmp_Mtable->next = NULL;
  tmp_Mtable->n->size = p->n->size - size_s;
  tmp_Mtable->n->id = ++id_count;
  tmp_Mtable->n->status = UNUSED;
  tmp_Mtable->n->begin = p->n->begin + size_s;
  p->n->size = size_s;
  p->n->status = USED;
  insert_node(p, tmp_Mtable);
  mem_block_count++;
}

int Allocation(MTable *M) {
  int tmp_size = 0;
  int success = True;
  printf("请输入您要分配的内存大小： ");
  scanf("%d", &tmp_size);
  if (tmp_size > mem_size) {
    printf("分配内存大于总内存值，分配失败！\n\n");
    success = False;
    return success;
  }
  if (M->next == NULL) {
    if (M->n->status == USED) {
      success = False;
      return success;
    } else if (M->n->size < tmp_size) {
      printf("没有能够分配的内存块，分配失败！\n\n");
      success = False;
      return success;
    } else {
      alc(tmp_size, M);
      return success;
    }
  }
  while (M->next != NULL && (M->n->status == USED || M->n->size < tmp_size)) {
    M = M->next;
  }
  if (M->n->status == UNUSED && M->n->size >= tmp_size) {
    alc(tmp_size, M);
    return success;
  }
  if (M->next == NULL && (M->n->status == USED || M->n->size < tmp_size)) {
    printf("没有能够分配的内存块，分配失败！\n\n");
    success = False;
    return success;
  }
  return -1;
}

void Combine(MTable *m) {
  if (m->next == NULL || m->next->n->status == USED) {
    return;
  }
  MTable *p1 = m->next;
  m->n->size += m->next->n->size;
  m->next = m->next->next;
  // if (m->next->next != NULL) {
  //   m->next->next = NULL;
  // }
  free(p1->n);
  free(p1);
  Combine(m);
}

int Reclaim(MTable *m) {
  int tmp_id = -1;
  int success = True;
  printf("请输入您要释放的内存序号： ");
  scanf("%d", &tmp_id);
  //找到内存块位置
  MTable *p = m;
  while (p->n->id != tmp_id && p != NULL) {
    p = p->next;
  }
  if (p == NULL) {
    success = False;
    printf("找不到该块内存！\n\n");
    return False;
  }
  else {
    p->n->status = UNUSED;
  }
  p = m;
  while (p != NULL) {
    if (p->n->status == USED) {
      p = p->next;
      continue;
    }
    Combine(p);
    p = p->next;
  }
  return success;
}

void show_status() {
  int count = 0;
  MTable *M = &Memory;
  printf("--------------------------\n");
  while (M->next != NULL) {
    printf("第%d块\tID: %d\t大小: %d\t状态: %d\t起址: %d\n", ++count, M->n->id,
           M->n->size, M->n->status, M->n->begin);
    M = M->next;
    // count++;
  }
  printf("第%d块\tID: %d\t大小: %d\t状态: %d\t起址: %d\n", ++count, M->n->id,
         M->n->size, M->n->status, M->n->begin);
  printf("--------------------------\n");
  printf("\n\n");
}

int main() {
  Memory.next = NULL;
  Memory.n = (node *)malloc(sizeof(node));
  Memory.n->is_first = True;
  printf("Enter The size of Memory: ");
  scanf("%d", &mem_size);
  mem_block_count++;
  Memory.n->id = mem_block_count;
  Memory.n->size = mem_size;
  Memory.n->begin = mem_begin;
  Memory.n->status = UNUSED;
  Memory.n->begin = 0;
  int end_flag = False;
  MTable *mem = &Memory;
  printf("内存总大小： %d\n", mem_size);
  while (end_flag == False) {
    int opt_flag = -1;
    printf("请选择您的操作：\n1. 分配内存\n2. 释放内存\n3. "
           "退出\n4.查看内存详细状态\n\n");
    scanf("%d", &opt_flag);
    switch (opt_flag) {
    case 1:
      if (Allocation(mem)) {
        printf("分配成功！\n\n");
      };
      break;
    case 2:
      if (Reclaim(mem)) {
        printf("回收成功！\n\n");  
      };
      break;
    case 3:
      // 清理内存然后退出
      end_flag = True;
      break;
    case 4:
      show_status();
      break;
    default:
      printf("输入错误，请重新输入\n");
      break;
    }
  }
}