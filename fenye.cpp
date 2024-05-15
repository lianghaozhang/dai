#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// 定义结构体表示空闲块、页表和进程控制块
struct FreeBlock {
    size_t start;
    size_t size;
};

struct PageTable {
    int pageNumber;
    int blockNumber;
};

struct ProcessControlBlock {
    size_t startAddress;
    int tableLength;
    bool isExecuting;
    int processID; // 新增进程号字段
};

// 声明全局变量
vector<FreeBlock> freeBlocks;
vector<PageTable> pageTables;
vector<ProcessControlBlock> PCBs;
size_t memoryCapacity;
size_t pageSize;
int id = 2;

// 显示空闲块信息
void displayFreeBlocks() {

    cout << "空闲区序号\t起始地址\t长度" << endl;
    for (size_t i = 0; i < freeBlocks.size(); ++i) {
        cout << i + 1 << "\t\t" << freeBlocks[i].start << "\t\t" << freeBlocks[i].size << endl;
    }
    cout << "==========================================" << endl;
}

// 显示进程控制块信息
void displayPCBs() {
    cout << "==========================================" << endl;
    cout << "进程号\t\t起始地址\t长度" << endl;
    for (size_t i = 0; i < PCBs.size(); ++i) {
        cout << PCBs[i].processID << "\t\t" << PCBs[i].startAddress << "\t\t" << PCBs[i].tableLength << endl;
    }
    cout << "==========================================" << endl;
}

// 分配内存
void allocMem(size_t len) {
    if (len <= 0) {
        cout << "无效的内存请求大小" << endl;
        return;
    }

    // 遍历空闲块寻找合适的块进行分配
    size_t remaining = len;
    for (auto& block : freeBlocks) {
        if (block.size >= remaining) {
            PCBs.push_back({ block.start, static_cast<int>(remaining), false, id }); // 设置processID为id
            pageTables.push_back({ static_cast<int>(pageTables.size()), static_cast<int>(block.start / pageSize) });
            block.start += remaining;
            block.size -= remaining;
            remaining = 0;
            id++;
            break;
        }
        else {
            PCBs.push_back({ block.start, static_cast<int>(block.size), false, id }); // 设置processID为id
            pageTables.push_back({ static_cast<int>(pageTables.size()), static_cast<int>(block.start / pageSize) });
            remaining -= block.size;
            id++;
            block.size = 0;
        }
    }

    // 移除已使用的块
    freeBlocks.erase(remove_if(freeBlocks.begin(), freeBlocks.end(), [](const FreeBlock& block) {
        return block.size == 0;
        }), freeBlocks.end());

    if (remaining > 0) {
        cout << "内存分配失败。空间不足。" << endl;
    }
}

//// 释放内存
//void freeMem(int processID) {
//    auto it = find_if(PCBs.begin(), PCBs.end(), [=](const ProcessControlBlock& pcb) {
//        return pcb.processID == processID;
//        });
//
//    if (it != PCBs.end()) {
//        size_t index = distance(PCBs.begin(), it);
//        freeBlocks.push_back({ PCBs[index].startAddress, static_cast<size_t>(PCBs[index].tableLength) });
//        PCBs.erase(PCBs.begin() + index);
//        pageTables.erase(pageTables.begin() + index);
//    }
//    else {
//        cout << "未找到进程号为 " << processID << " 的进程" << endl;
//    }
//}

// 释放内存
void freeMem(size_t base) {
    auto it = find_if(PCBs.begin(), PCBs.end(), [=](const ProcessControlBlock& pcb) {
        return pcb.startAddress == base;
        });

    if (it != PCBs.end()) {
        size_t index = distance(PCBs.begin(), it);
        // 更新页表
        pageTables.erase(pageTables.begin() + index);

        freeBlocks.push_back({ PCBs[index].startAddress, static_cast<size_t>(PCBs[index].tableLength) });
        PCBs.erase(PCBs.begin() + index);
    }
    else {
        cout << "未找到起始地址为 " << base << " 的进程" << endl;
    }
}

int main() {
    // 初始化系统
    cout << "请输入内存容量和页大小: ";
    cin >> memoryCapacity >> pageSize;

    // 用户空间的起始地址和长度
    size_t userSpaceStart, userSpaceSize;
    cout << "请输入用户空间的起始地址和长度: ";
    cin >> userSpaceStart >> userSpaceSize;

    // 初始化空闲块
    freeBlocks.push_back({ userSpaceStart, userSpaceSize });
    displayPCBs();
    displayFreeBlocks();

    // 主循环
    char choice;
    do {
        cout << "选择操作 (a: 分配, f: 释放, e: 结束): ";
        cin >> choice;
        switch (choice) {
        case 'a': {
            size_t size;
            cout << "输入需要分配的大小: ";
            cin >> size;
            allocMem(size);
            displayPCBs();
            displayFreeBlocks();
            break;
        }
        case 'f': {
            int processID;
            cout << "输入要释放内存的起始地址: ";
            cin >> processID;
            freeMem(processID);
            displayPCBs();
            displayFreeBlocks();
            break;
        }
        case 'e':
            cout << "程序结束" << endl;
            break;
        default:
            cout << "无效的选择" << endl;
        }
    } while (choice != 'e');

    return 0;
}
