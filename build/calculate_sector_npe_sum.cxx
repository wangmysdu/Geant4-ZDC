#include <vector>
#include <TFile.h>
#include <TTree.h>

void calculate_sector_npe_sum() {
    // 1. 打开 ROOT 文件并获取树
    TFile *file = TFile::Open("ZDC.root"); // 替换为实际文件名
    TTree *tree = (TTree*)file->Get("ZDC"); // 替换为实际树名

    // 2. 声明向量指针以绑定分支
    std::vector<int> *modNPE1 = nullptr; // sector0 (Mod.NPESec1)
    std::vector<int> *modNPE2 = nullptr; // sector1 (Mod.NPESec2)

    // 3. 绑定分支到向量指针
    tree->SetBranchAddress("Mod.NPESec1", &modNPE1);
    tree->SetBranchAddress("Mod.NPESec2", &modNPE2);

    // 4. 初始化总和变量
    Long64_t totalNPE_sector0 = 0;
    Long64_t totalNPE_sector1 = 0;

    // 5. 遍历所有事件
    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        tree->GetEntry(i); // 加载当前事件数据

        // 计算 sector0 的 NPE 总和（Mod.NPESec1 所有元素之和）
        for (auto &npe : *modNPE1) {
            totalNPE_sector0 += npe;
        }

        // 计算 sector1 的 NPE 总和（Mod.NPESec2 所有元素之和）
        for (auto &npe : *modNPE2) {
            totalNPE_sector1 += npe;
        }
    }

    // 6. 输出结果
    std::cout << "Total NPE for Sector0 (Mod.NPESec1): " << totalNPE_sector0 << std::endl;
    std::cout << "Total NPE for Sector1 (Mod.NPESec2): " << totalNPE_sector1 << std::endl;

    // 7. 关闭文件
    file->Close();
}