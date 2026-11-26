#include "Fixtures.hpp"

TEST_F(VirtualFS_Fixture, avl_compatibility_with_fsnode){
    AVLTree<std::string, VirtualFS::FSNode> node_tree;
    node_tree.insert("/", VirtualFS::FSNode("/", "/", true));
    node_tree.insert("/b_dir", VirtualFS::FSNode("b_dir", "/b_dir", true));
    node_tree.insert("/a_file.txt", VirtualFS::FSNode("a_file.txt", "/a_file.txt", false, disk_file_1));

    EXPECT_EQ(node_tree.len(), 3) << "Ожидаемая длина дерева узлов 3. По факту : " << node_tree.len();
    EXPECT_TRUE(node_tree.key_be("/a_file.txt")) << "Ключ /a_file.txt должен присутствовать в AVLTree";
    EXPECT_EQ(node_tree.search("/a_file.txt").real_path, disk_file_1) << "Неверный real_path в узле AVLTree";

    std::vector<std::string> iterated_paths;
    for (const auto& node : node_tree){
        iterated_paths.push_back(node.path);
    }
    std::vector<std::string> expected ={"/", "/a_file.txt", "/b_dir"};
    EXPECT_EQ(iterated_paths, expected) << "Range-based итератор AVLTree должен обходить FSNode в алфавитном порядке путей";

    if (testing::Test::HasFailure()){
        std::cout << "\n---Дерево узлов FSNode---\n";
        std::cout << show_tree(node_tree);
    }
}

TEST_F(VirtualFS_Fixture, def_constructor_and_pwd){
    EXPECT_EQ(empty_fs->pwd(), "/") << "Ожидалась корневая директория '/'. По факту : " << empty_fs->pwd();
    EXPECT_TRUE(empty_fs->ls().empty()) << "Корневая директория новой ФС должна быть пустой";

    if (testing::Test::HasFailure()){
        std::cout << "\n---Пустая ФС---\n";
        std::cout << show_fs(*empty_fs);
    }
}

TEST_F(VirtualFS_Fixture, mkdir_and_touch_overloads){
    fs->mkdir("/media");
    fs->touch("/media/note.txt", disk_file_1);
    fs->touch("/" + disk_file_auto);

    auto root_items = fs->ls("/");
    EXPECT_EQ(root_items.size(), 4) << "Ожидалось 4 элемента в корне (auto_disk.txt, docs, media, src). По факту : " << root_items.size();
    EXPECT_EQ(fs->cat("/" + disk_file_auto), "Автопривязка\n") << "Неверное содержимое файла при одноаргументном touch";
    EXPECT_EQ(fs->cat("/media/note.txt"), "Первая строка\nВторая строка\n") << "Неверное содержимое файла при двухаргументном touch";

    if (testing::Test::HasFailure()){
        std::cout << "\n---Текущая ФС после mkdir и touch---\n";
        std::cout << show_fs(*fs);
    }
}

TEST_F(VirtualFS_Fixture, cd_and_relative_paths){
    fs->cd("docs");
    EXPECT_EQ(fs->pwd(), "/docs") << "Ожидался путь /docs. По факту : " << fs->pwd();

    fs->cd("labs");
    EXPECT_EQ(fs->pwd(), "/docs/labs") << "Ожидался путь /docs/labs. По факту : " << fs->pwd();

    fs->cd(".");
    EXPECT_EQ(fs->pwd(), "/docs/labs") << "Переход в '.' не должен менять текущую директорию";

    fs->cd("");
    EXPECT_EQ(fs->pwd(), "/docs/labs") << "Переход по пустому пути не должен менять текущую директорию";

    fs->cd("..");
    EXPECT_EQ(fs->pwd(), "/docs") << "Ожидался возврат в /docs по '..'. По факту : " << fs->pwd();

    fs->cd("..");
    EXPECT_EQ(fs->pwd(), "/") << "Ожидался возврат в корень '/'. По факту : " << fs->pwd();

    fs->cd("..");
    EXPECT_EQ(fs->pwd(), "/") << "Переход '..' из корня должен оставлять в '/'";

    if (testing::Test::HasFailure()){
        std::cout << "\n---Текущая ФС при навигации cd---\n";
        std::cout << show_fs(*fs);
    }
}

TEST_F(VirtualFS_Fixture, ls_filtering_and_single_file){
    auto root_ls = fs->ls("/");
    ASSERT_EQ(root_ls.size(), 2) << "В корне должны отображаться только docs и src, без вложенных файлов";
    EXPECT_EQ(root_ls[0].name, "docs");
    EXPECT_TRUE(root_ls[0].is_directory);
    EXPECT_EQ(root_ls[1].name, "src");
    EXPECT_TRUE(root_ls[1].is_directory);

    auto docs_ls = fs->ls("/docs");
    ASSERT_EQ(docs_ls.size(), 2) << "В /docs должны быть только labs и readme.txt";
    EXPECT_EQ(docs_ls[0].name, "labs");
    EXPECT_TRUE(docs_ls[0].is_directory);
    EXPECT_EQ(docs_ls[1].name, "readme.txt");
    EXPECT_FALSE(docs_ls[1].is_directory);

    auto file_ls = fs->ls("/docs/readme.txt");
    ASSERT_EQ(file_ls.size(), 1) << "ls от файла должен возвращать вектор из 1 элемента";
    EXPECT_EQ(file_ls[0].path, "/docs/readme.txt");
    EXPECT_FALSE(file_ls[0].is_directory);

    if (testing::Test::HasFailure()){
        std::cout << "\n---Состояние ФС при проверке ls---\n";
        std::cout << show_fs(*fs);
    }
}

TEST_F(VirtualFS_Fixture, cat_reads_real_file){
    std::string content = fs->cat("/docs/readme.txt");
    EXPECT_EQ(content, "Первая строка\nВторая строка\n") << "Неверное содержимое файла из cat";

    fs->cd("/docs/labs");
    EXPECT_EQ(fs->cat("report.txt"), "Секретный отчет\n") << "cat должен поддерживать относительные пути";

    if (testing::Test::HasFailure()){
        std::cout << "\n---Состояние ФС при проверке cat---\n";
        std::cout << show_fs(*fs);
    }
}

TEST_F(VirtualFS_Fixture, mv_preserves_physical_file){
    fs->mv("/docs/readme.txt", "/src/moved_readme.txt");

    EXPECT_EQ(fs->ls("/docs").size(), 1) << "После mv старый файл должен исчезнуть из /docs";
    EXPECT_EQ(fs->cat("/src/moved_readme.txt"), "Первая строка\nВторая строка\n") << "Содержимое перемещенного файла должно сохраниться";

    std::ifstream real_check(disk_file_1);
    EXPECT_TRUE(real_check.is_open()) << "Перемещение в виртуальной ФС не должно удалять или перемещать реальный файл на диске!";

    if (testing::Test::HasFailure()){
        std::cout << "\n---Состояние ФС после mv---\n";
        std::cout << show_fs(*fs);
    }
}

TEST_F(VirtualFS_Fixture, rm_preserves_physical_file){
    fs->rm("/docs/labs/report.txt");
    EXPECT_TRUE(fs->ls("/docs/labs").empty()) << "После удаления файла директория /docs/labs должна стать пустой";

    fs->rm("/docs/labs");
    EXPECT_EQ(fs->ls("/docs").size(), 1) << "Пустая директория /docs/labs должна удаляться без ошибок";

    std::ifstream real_check(disk_file_2);
    EXPECT_TRUE(real_check.is_open()) << "Удаление из виртуальной ФС не должно удалять реальный файл с диска!";

    if (testing::Test::HasFailure()){
        std::cout << "\n---Состояние ФС после rm---\n";
        std::cout << show_fs(*fs);
    }
}

class VirtualFSInvalidPathParamTest : public VirtualFS_Fixture,
                                      public testing::WithParamInterface<std::string>{};

TEST_P(VirtualFSInvalidPathParamTest, missing_paths_throw){
    std::string bad_path = GetParam();

    EXPECT_THROW(fs->cd(bad_path), std::invalid_argument) << "Ожидается std::invalid_argument при cd(\"" << bad_path << "\")";
    EXPECT_THROW(fs->ls(bad_path), std::invalid_argument) << "Ожидается std::invalid_argument при ls(\"" << bad_path << "\")";
    EXPECT_THROW(fs->cat(bad_path), std::invalid_argument) << "Ожидается std::invalid_argument при cat(\"" << bad_path << "\")";
    EXPECT_THROW(fs->rm(bad_path), std::invalid_argument) << "Ожидается std::invalid_argument при rm(\"" << bad_path << "\")";

    if (testing::Test::HasFailure()){
        std::cout << "\n---Состояние ФС при проверке некорректного пути \"" << bad_path << "\"---\n";
        std::cout << show_fs(*fs);
    }
}

INSTANTIATE_TEST_SUITE_P(BadPaths, VirtualFSInvalidPathParamTest, testing::Values(
    "/non_existent",
    "/docs/missing.txt",
    "/unknown_dir/sub"
));

TEST_F(VirtualFS_Fixture, invalid_operations_throw){
    EXPECT_THROW(fs->mkdir("/docs"), std::invalid_argument) << "Повторное создание /docs должно бросать исключение";
    EXPECT_THROW(fs->mkdir("/no_parent/child"), std::invalid_argument) << "Создание папки без родителя должно бросать исключение";
    EXPECT_THROW(fs->mkdir("/docs/readme.txt/sub"), std::invalid_argument) << "Создание узла внутри файла должно бросать исключение";

    EXPECT_THROW(fs->touch("/docs/ghost.txt", "no_such_real_file_12345.txt"), std::invalid_argument) << "Привязка несуществующего реального файла должна бросать исключение";
    EXPECT_THROW(fs->touch("/docs/readme.txt", disk_file_1), std::invalid_argument) << "Дубликат виртуального пути должен бросать исключение";

    EXPECT_THROW(fs->cat("/docs"), std::invalid_argument) << "cat от директории должен бросать исключение";
    EXPECT_THROW(fs->cd("/docs/readme.txt"), std::invalid_argument) << "cd в файл должен бросать исключение";

    EXPECT_THROW(fs->rm("/"), std::invalid_argument) << "Удаление корня '/' должно бросать исключение";
    EXPECT_THROW(fs->rm("/docs"), std::invalid_argument) << "Удаление непустой директории должно бросать исключение";

    EXPECT_THROW(fs->mv("/docs", "/docs_new"), std::invalid_argument) << "Перемещение директории должно бросать исключение";
    EXPECT_THROW(fs->mv("/missing.txt", "/new.txt"), std::invalid_argument) << "Перемещение несуществующего файла должно бросать исключение";

    if (testing::Test::HasFailure()){
        std::cout << "\n---Состояние ФС при проверке исключений---\n";
        std::cout << show_fs(*fs);
    }
}