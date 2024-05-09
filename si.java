import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Scanner;

// 定义空闲分区
class FreePartition {
    int startAddress;
    int length;

    public FreePartition(int startAddress, int length) {
        this.startAddress = startAddress;
        this.length = length;
    }
}

// 定义进程控制块
class PCB {
    int processNumber;
    int startAddress;
    int length;

    public PCB(int processNumber, int startAddress, int length) {
        this.processNumber = processNumber;
        this.startAddress = startAddress;
        this.length = length;
    }
}

public class si {
    static List<FreePartition> freePartitions = new ArrayList<>();
    static List<PCB> pcbList = new ArrayList<>();
    static int userSpaceStart;
    static int userSpaceLength;

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        userSpaceStart = 0;
        userSpaceLength = 600;
        freePartitions.add(new FreePartition(userSpaceStart, userSpaceLength));
        displayMemoryStatus();

        while (true) {
            System.out.print("请输入命令：");
            String operation = scanner.next();
            if (operation.equals("e")) {
                break;
            }
            switch (operation) {
                case "a":
                    int processNumber = scanner.nextInt();
                    int size = scanner.nextInt();
                    allocMem(processNumber, size);
                    break;
                case "f":
                    int processToFree = scanner.nextInt();
                    freeMem(processToFree);
                    break;
                default:
                    System.out.println("无效的操作！");
                    break;
            }
            displayMemoryStatus();
        }
    }

    // 分配内存
    static void allocMem(int processNumber, int len) {
        FreePartition allocatedPartition = null;
        for (FreePartition partition : freePartitions) {
            if (partition.length >= len) {
                allocatedPartition = partition;
                break;
            }
        }

        if (allocatedPartition != null) {
            pcbList.add(new PCB(processNumber, allocatedPartition.startAddress, len));
            if (allocatedPartition.length == len) {
                freePartitions.remove(allocatedPartition);
            } else {
                allocatedPartition.startAddress += len;
                allocatedPartition.length -= len;
            }
        } else {
            System.out.println("无法满足分配请求！");
        }
    }

    // 释放内存
    static void freeMem(int processNumber) {
        PCB processToFree = null;
        for (PCB pcb : pcbList) {
            if (pcb.processNumber == processNumber) {
                processToFree = pcb;
                break;
            }
        }

        if (processToFree != null) {
            FreePartition newFreePartition = new FreePartition(processToFree.startAddress, processToFree.length);
            freePartitions.add(newFreePartition);
            Collections.sort(freePartitions, Comparator.comparingInt(o -> o.startAddress));
            mergeFreePartitions();
            pcbList.remove(processToFree);
        } else {
            System.out.println("未找到要释放的进程！");
        }
    }

    // 合并空闲分区
    static void mergeFreePartitions() {
        List<FreePartition> mergedPartitions = new ArrayList<>();
        mergedPartitions.add(freePartitions.get(0));

        for (int i = 1; i < freePartitions.size(); i++) {
            FreePartition currentPartition = freePartitions.get(i);
            FreePartition lastPartition = mergedPartitions.get(mergedPartitions.size() - 1);

            if (currentPartition.startAddress == lastPartition.startAddress + lastPartition.length) {
                lastPartition.length += currentPartition.length;
            } else {
                mergedPartitions.add(currentPartition);
            }
        }

        freePartitions = mergedPartitions;
    }

    // 显示内存状态
    static void displayMemoryStatus() {
        System.out.println("==========================================");
        System.out.println("进程号\t\t起始地址\t\t长度");
        for (PCB pcb : pcbList) {
            System.out.println(pcb.processNumber + "\t\t\t" + pcb.startAddress + "\t\t\t" + pcb.length);
        }
        System.out.println("==========================================");
        System.out.println("空闲区序号\t起始地址\t\t长度");
        for (int i = 0; i < freePartitions.size(); i++) {
            FreePartition partition = freePartitions.get(i);
            System.out.println((i + 1) + "\t\t\t" + partition.startAddress + "\t\t\t" + partition.length);
        }
        System.out.println("==========================================\n");
    }
}

