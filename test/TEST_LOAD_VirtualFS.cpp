#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <map>
#include <cstdio>
#include "VirtualFS.hpp"

const size_t VFS_LOAD_SIZE = 50000;

TEST(LoadTestVFS, CompareVirtualFSWithStdMap){
    const std::string bench_disk_file = "bench_vfs_payload.txt";
    {
        std::ofstream out(bench_disk_file);
        out << "benchmark payload";
    }

    std::vector<std::string> paths(VFS_LOAD_SIZE);
    for (size_t i = 0; i < VFS_LOAD_SIZE; ++i){
        paths[i] = "/dir/file_" + std::to_string(i) + ".txt";
    }

    std::mt19937 gen(42);
    std::shuffle(paths.begin(), paths.end(), gen);

    auto start_custom = std::chrono::high_resolution_clock::now();
    VirtualFS vfs;
    vfs.mkdir("/dir");
    for (size_t i = 0; i < VFS_LOAD_SIZE; ++i){
        vfs.touch(paths[i], bench_disk_file);
    }
    auto listed = vfs.ls("/dir");
    volatile size_t found_count = listed.size();
    (void)found_count;
    for (size_t i = 0; i < VFS_LOAD_SIZE; ++i){
        vfs.rm(paths[i]);
    }
    auto end_custom = std::chrono::high_resolution_clock::now();
    double time_custom = std::chrono::duration<double, std::milli>(end_custom - start_custom).count();

    auto start_standard = std::chrono::high_resolution_clock::now();
    std::map<std::string, VirtualFS::FSNode> std_fs;
    std_fs["/"] = VirtualFS::FSNode("/", "/", true);
    std_fs["/dir"] = VirtualFS::FSNode("dir", "/dir", true);
    for (size_t i = 0; i < VFS_LOAD_SIZE; ++i){
        std::ifstream file(bench_disk_file);
        if (file.is_open()){
            size_t pos = paths[i].find_last_of('/');
            std::string name = paths[i].substr(pos + 1);
            std_fs[paths[i]] = VirtualFS::FSNode(name, paths[i], false, bench_disk_file);
        }
    }
    std::vector<VirtualFS::FSNode> std_listed;
    const std::string prefix = "/dir/";
    for (const auto& [k, node] : std_fs){
        if (k == "/dir") continue;
        if (k.rfind(prefix, 0) == 0 && k.find('/', prefix.size()) == std::string::npos){
            std_listed.push_back(node);
        }
        else if (!std_listed.empty() && k.rfind(prefix, 0) != 0){
            break;
        }
    }
    volatile size_t std_found_count = std_listed.size();
    (void)std_found_count;
    for (size_t i = 0; i < VFS_LOAD_SIZE; ++i){
        std_fs.erase(paths[i]);
    }
    auto end_standard = std::chrono::high_resolution_clock::now();
    double time_standard = std::chrono::duration<double, std::milli>(end_standard - start_standard).count();

    std::remove(bench_disk_file.c_str());

    std::cout << "\n--- Сравнение VirtualFS и std::map (touch, ls и rm на " << VFS_LOAD_SIZE << " файлах) ---\n";
    std::cout << "Мой VirtualFS (AVL): " << time_custom << " ms\n";
    std::cout << "std::map (RB-tree):  " << time_standard << " ms\n";
}

TEST(LoadTestVFS, CompareInputOrders){
    const size_t N = 25000;
    const std::string bench_disk_file = "bench_vfs_orders.txt";
    {
        std::ofstream out(bench_disk_file);
        out << "order test";
    }

    std::vector<std::string> sorted_paths(N);
    for (size_t i = 0; i < N; ++i){
        sorted_paths[i] = "/dir/file_" + std::to_string(100000 + i) + ".txt";
    }

    std::vector<std::string> reversed_paths = sorted_paths;
    std::reverse(reversed_paths.begin(), reversed_paths.end());

    std::vector<std::string> random_paths = sorted_paths;
    std::mt19937 gen(42);
    std::shuffle(random_paths.begin(), random_paths.end(), gen);

    auto measure_build = [&](const std::vector<std::string>& input){
        auto t1 = std::chrono::high_resolution_clock::now();
        VirtualFS vfs;
        vfs.mkdir("/dir");
        for (const auto& p : input){
            vfs.touch(p, bench_disk_file);
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(t2 - t1).count();
    };

    double t_sorted = measure_build(sorted_paths);
    double t_reversed = measure_build(reversed_paths);
    double t_random = measure_build(random_paths);

    std::remove(bench_disk_file.c_str());

    std::cout << "\n--- Построение VirtualFS на " << N << " файлах (разный порядок путей) ---\n";
    std::cout << "Отсортированные пути:          " << t_sorted << " ms\n";
    std::cout << "Обратно отсортированные пути:  " << t_reversed << " ms\n";
    std::cout << "Случайный порядок путей:       " << t_random << " ms\n";
}

TEST(LoadTestVFS, ExportVFSBenchmarkForPlot){
    const std::string bench_disk_file = "bench_vfs_plot.txt";
    {
        std::ofstream out(bench_disk_file);
        out << "plot payload";
    }

    std::vector<size_t> sizes = {1000, 2500, 5000, 10000, 25000, 50000};
    std::ofstream csv("benchmark_vfs.csv");
    csv << "N,VFS_Touch_ms,VFS_Ls_ms,VFS_Rm_ms,StdMap_Touch_ms,StdMap_Ls_ms,StdMap_Rm_ms\n";

    std::cout << "\n--- Замеры зависимости времени VirtualFS от N (запись в benchmark_vfs.csv) ---\n";
    for (size_t n : sizes){
        std::vector<std::string> paths(n);
        for (size_t i = 0; i < n; ++i){
            paths[i] = "/dir/file_" + std::to_string(i) + ".txt";
        }
        std::mt19937 gen(42);
        std::shuffle(paths.begin(), paths.end(), gen);

        VirtualFS vfs;
        vfs.mkdir("/dir");
        auto t1 = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < n; ++i) vfs.touch(paths[i], bench_disk_file);
        auto t2 = std::chrono::high_resolution_clock::now();
        auto items = vfs.ls("/dir");
        volatile size_t cnt1 = items.size();
        (void)cnt1;
        auto t3 = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < n; ++i) vfs.rm(paths[i]);
        auto t4 = std::chrono::high_resolution_clock::now();

        std::map<std::string, VirtualFS::FSNode> mp;
        mp["/"] = VirtualFS::FSNode("/", "/", true);
        mp["/dir"] = VirtualFS::FSNode("dir", "/dir", true);
        auto s1 = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < n; ++i){
            std::ifstream f(bench_disk_file);
            if (f.is_open()){
                size_t pos = paths[i].find_last_of('/');
                mp[paths[i]] = VirtualFS::FSNode(paths[i].substr(pos + 1), paths[i], false, bench_disk_file);
            }
        }
        auto s2 = std::chrono::high_resolution_clock::now();
        std::vector<VirtualFS::FSNode> mp_items;
        const std::string prefix = "/dir/";
        for (const auto& [k, node] : mp){
            if (k == "/dir") continue;
            if (k.rfind(prefix, 0) == 0 && k.find('/', prefix.size()) == std::string::npos){
                mp_items.push_back(node);
            }
            else if (!mp_items.empty() && k.rfind(prefix, 0) != 0){
                break;
            }
        }
        volatile size_t cnt2 = mp_items.size();
        (void)cnt2;
        auto s3 = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < n; ++i) mp.erase(paths[i]);
        auto s4 = std::chrono::high_resolution_clock::now();

        double vfs_ins = std::chrono::duration<double, std::milli>(t2 - t1).count();
        double vfs_ls  = std::chrono::duration<double, std::milli>(t3 - t2).count();
        double vfs_rem = std::chrono::duration<double, std::milli>(t4 - t3).count();

        double std_ins = std::chrono::duration<double, std::milli>(s2 - s1).count();
        double std_ls  = std::chrono::duration<double, std::milli>(s3 - s2).count();
        double std_rem = std::chrono::duration<double, std::milli>(s4 - s3).count();

        csv << n << "," << vfs_ins << "," << vfs_ls << "," << vfs_rem << ","
            << std_ins << "," << std_ls << "," << std_rem << "\n";

        std::cout << "N = " << n
                  << " | VFS(touch/ls/rm): " << vfs_ins << " / " << vfs_ls << " / " << vfs_rem << " ms"
                  << " | std::map: " << std_ins << " / " << std_ls << " / " << std_rem << " ms\n";
    }
    csv.close();
    std::remove(bench_disk_file.c_str());
}