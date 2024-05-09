#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 10

// 定义空闲分区的结构
typedef struct FreeBlock {
    int start_address;
    int length;
    struct FreeBlock *next;
} FreeBlock;

// 定义进程控制块
typedef struct {
    int start_address;
    int length;
} ProcessControlBlock;

// 定义进程和空闲分区的数组
ProcessControlBlock pcb[MAX_PROCESSES];
FreeBlock *free_block_list = NULL;

// 初始化空闲分区表
void initialize_free_blocks(int start_address, int length) {
    free_block_list = (FreeBlock *)malloc(sizeof(FreeBlock));
    free_block_list->start_address = start_address;
    free_block_list->length = length;
    free_block_list->next = NULL;
}

// 输出进程信息
void print_processes() {
    printf("==========================================\n");
    printf("进程号\t\t起始地址\t长度\n");
   //printf("==========================================\n");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (pcb[i].length > 0) {
            printf("%d\t\t%d\t\t%d\n", i, pcb[i].start_address, pcb[i].length);
        }
    }
    printf("==========================================\n");
}

// 输出空闲分区信息
void print_free_blocks() {
    printf("空闲区序号\t起始地址\t长度\n");
    FreeBlock *current = free_block_list;
    int index = 1;
    while (current) {
        printf("%d\t\t%d\t\t%d\n", index, current->start_address, current->length);
        index++;
        current = current->next;
    }
    printf("==========================================\n\n\n");
}

// 分配内存函数：首次适应算法
int alloc_mem(int len) {
    FreeBlock *current = free_block_list;
    FreeBlock *previous = NULL;

    while (current) {
        if (current->length >= len) {
            // 分配内存
            int start_address = current->start_address;
            current->start_address += len;
            current->length -= len;

            // 检查是否需要移除空闲分区
            if (current->length == 0) {
                if (previous) {
                    previous->next = current->next;
                } else {
                    free_block_list = current->next;
                }
                free(current);
            }

            return start_address;
        }

        previous = current;
        current = current->next;
    }

    // 没有找到合适的空闲分区
    return -1;
}

// 释放内存函数
void free_mem(int base, int len) {
    FreeBlock *new_free_block = (FreeBlock *)malloc(sizeof(FreeBlock));
    new_free_block->start_address = base;
    new_free_block->length = len;
    new_free_block->next = NULL;

    FreeBlock *current = free_block_list;
    FreeBlock *previous = NULL;

    // 寻找插入位置
    while (current && current->start_address < base) {
        previous = current;
        current = current->next;
    }

    // 将新的空闲分区插入链表中
    if (previous) {
        previous->next = new_free_block;
    } else {
        free_block_list = new_free_block;
    }
    new_free_block->next = current;

    // 合并前后空闲分区
    if (previous && previous->start_address + previous->length == base) {
        previous->length += len;
        previous->next = new_free_block->next;
        free(new_free_block);
        new_free_block = previous;
    }

    if (current && base + len == current->start_address) {
        new_free_block->length += current->length;
        new_free_block->next = current->next;
        free(current);
    }
}

int main() {
    // 输入初始的用户空间起始地址和长度
    int initial_start_address, initial_length;
    //printf("输入初始的用户空间起始地址和长度：");
    //scanf("%d %d", &initial_start_address, &initial_length);
	initial_start_address = 0;
	initial_length = 600;
    // 初始化空闲分区
    initialize_free_blocks(initial_start_address, initial_length);
	
	print_processes();
    print_free_blocks();
	char command;
    int process_id, size;

    while (1) {
        printf("输入命令：");
        scanf(" %c", &command);

        if (command == 'a') {
            // 分配内存
			//printf("输入n和size：");
            scanf("%d %d", &process_id, &size);
            int start_address = alloc_mem(size);
            if (start_address != -1) {
                pcb[process_id].start_address = start_address;
                pcb[process_id].length = size;
                //printf("动作：a %d %d\n", process_id, size);
                print_processes();
                print_free_blocks();
            } else {
                printf("无法分配内存。\n");
            }
        } else if (command == 'f') {
            // 释放内存
			//printf("输入n：");
            scanf("%d", &process_id);
            if (pcb[process_id].length > 0) {
                free_mem(pcb[process_id].start_address, pcb[process_id].length);
                pcb[process_id].start_address = 0;
                pcb[process_id].length = 0;
                //printf("动作：f %d\n", process_id);
                print_processes();
                print_free_blocks();
            } else {
                printf("进程 %d 未分配内存。\n", process_id);
            }
        } else if (command == 'e') {
            // 结束程序
            break;
        } else {
            printf("无效的命令。\n");
        }
    }

    // 释放内存
    while (free_block_list) {
        FreeBlock *temp = free_block_list;
        free_block_list = free_block_list->next;
        free(temp);
    }

    return 0;
}

