#include "Fixtures.hpp"

TEST_F(AVL_Fixture, def_constructor) {
    EXPECT_TRUE(empty_tree->IsEmpty()) << "Ожидалось пустое дерево. По факту IsEmpty() == false";
    EXPECT_EQ(empty_tree->len(), 0) << "Ожидаемая длина 0. По факту : " << empty_tree->len();
    EXPECT_THROW(empty_tree->search(42), std::invalid_argument) << "Ожидается std::invalid_argument при поиске в пустом дереве. По факту : исключение не выброшено";
    EXPECT_THROW(empty_tree->Get(0), std::invalid_argument) << "Ожидается std::invalid_argument при Get(0) в пустом дереве. По факту : исключение не выброшено";

    if (testing::Test::HasFailure()) {
        std::cout << "\n---Текущее дерево (пустое)---\n";
        std::cout << show_tree(*empty_tree);
    }
}

TEST_F(AVL_Fixture, copy_and_move_constructors) {
    AVLTree<int, int> copy(*tree_int);
    EXPECT_EQ(copy.len(), 5) << "Ожидаемая длина копии 5. По факту : " << copy.len();
    EXPECT_TRUE(copy == *tree_int) << "Ожидалось равенство копии и исходного дерева";

    copy.insert(999, 9990);
    EXPECT_EQ(tree_int->len(), 5) << "Изменение копии затронуло оригинал! Ожидалась длина 5, по факту : " << tree_int->len();
    EXPECT_FALSE(tree_int->key_be(999)) << "Ключ 999 не должен существовать в исходном дереве";

    AVLTree<int, int> moved(std::move(copy));
    EXPECT_EQ(moved.len(), 6) << "Ожидаемая длина перемещенного дерева 6. По факту : " << moved.len();
    EXPECT_TRUE(copy.IsEmpty()) << "После std::move исходное дерево должно быть пустым";

    if (testing::Test::HasFailure()) {
        std::cout << "\n---Исходное дерево---\n" << show_tree(*tree_int);
        std::cout << "\n---Перемещенное дерево---\n" << show_tree(moved);
    }
}

TEST_F(AVL_Fixture, assignment_operators) {
    AVLTree<int, int> assigned;
    assigned = *tree_int;
    EXPECT_TRUE(assigned == *tree_int) << "Ожидалось равенство после копирующего присваивания";
    EXPECT_FALSE(assigned != *tree_int) << "Оператор != вернул true для равных деревьев";

    assigned = assigned;
    EXPECT_EQ(assigned.len(), 5) << "Самоприсваивание повредило дерево. Длина : " << assigned.len();

    AVLTree<int, int> move_assigned;
    move_assigned = std::move(assigned);
    EXPECT_EQ(move_assigned.len(), 5) << "Ожидаемая длина после move-присваивания 5. По факту : " << move_assigned.len();
    EXPECT_TRUE(assigned.IsEmpty()) << "Исходный объект после move-присваивания должен стать пустым";
}

class AVLSearchParamTest : public AVL_Fixture,
                           public testing::WithParamInterface<std::tuple<int, int>> {};

TEST_P(AVLSearchParamTest, search_and_key_be) {
    auto [key, expected_val] = GetParam();

    EXPECT_TRUE(tree_int->key_be(key)) << "Ожидалось наличие ключа " << key << " в дереве";
    EXPECT_EQ(tree_int->search(key), expected_val) << "Для ключа " << key << " ожидалось значение " << expected_val << ". По факту : " << tree_int->search(key);

    if (testing::Test::HasFailure()) {
        std::cout << "\n---Текущее дерево tree_int---\n";
        std::cout << show_tree(*tree_int);
    }
}

INSTANTIATE_TEST_SUITE_P(SearchParams, AVLSearchParamTest, testing::Values(
    std::make_tuple(42, 420),
    std::make_tuple(52, 520),
    std::make_tuple(69, 690),
    std::make_tuple(67, 670),
    std::make_tuple(228, 2280)
));

class AVLGetByIndexParamTest : public AVL_Fixture,
                               public testing::WithParamInterface<std::tuple<int, int>> {};

TEST_P(AVLGetByIndexParamTest, get_by_sorted_index) {
    auto [index, expected_val] = GetParam();

    EXPECT_EQ(tree_int->Get(index), expected_val) << "Для индекса " << index << " ожидалось значение " << expected_val << ". По факту : " << tree_int->Get(index);

    if (testing::Test::HasFailure()) {
        std::cout << "\n---Текущее дерево tree_int---\n";
        std::cout << show_tree(*tree_int);
    }
}

INSTANTIATE_TEST_SUITE_P(GetIndexParams, AVLGetByIndexParamTest, testing::Values(
    std::make_tuple(0, 420),
    std::make_tuple(1, 520),
    std::make_tuple(2, 670),
    std::make_tuple(3, 690),
    std::make_tuple(4, 2280)
));

class AVLInvalidArgParamTest : public AVL_Fixture,
                               public testing::WithParamInterface<std::tuple<int, int, std::string>> {};

TEST_P(AVLInvalidArgParamTest, invalid_arguments_throw) {
    auto [missing_key, bad_index, bad_order] = GetParam();

    EXPECT_FALSE(tree_int->key_be(missing_key)) << "Ключ " << missing_key << " не должен присутствовать в дереве";
    EXPECT_THROW(tree_int->search(missing_key), std::invalid_argument) << "Ожидается std::invalid_argument при search(" << missing_key << "). По факту : исключение не выброшено";
    EXPECT_THROW(tree_int->Get(bad_index), std::invalid_argument) << "Ожидается std::invalid_argument при Get(" << bad_index << "). По факту : исключение не выброшено";
    EXPECT_THROW(tree_int->bypass(bad_order, [](const int&, const int&) {}), std::invalid_argument) << "Ожидается std::invalid_argument для строки обхода \"" << bad_order << "\". По факту : исключение не выброшено";

    if (testing::Test::HasFailure()) {
        std::cout << "\n---Текущее дерево tree_int при проверке исключений---\n";
        std::cout << show_tree(*tree_int);
    }
}

INSTANTIATE_TEST_SUITE_P(InvalidArgParams, AVLInvalidArgParamTest, testing::Values(
    std::make_tuple(-100, -1, ""),
    std::make_tuple(0, 5, "KKP"),
    std::make_tuple(1000, 100, "ABC"),
    std::make_tuple(999, 99, "KLPP")
));

TEST_F(AVL_Fixture, balancing_all_rotations) {
    AVLTree<int, int> ll;
    ll.insert(30, 30);
    ll.insert(20, 20);
    ll.insert(10, 10);
    EXPECT_EQ(ll.Get(0), 10);
    EXPECT_EQ(ll.Get(1), 20);
    EXPECT_EQ(ll.Get(2), 30);

    AVLTree<int, int> rr;
    rr.insert(10, 10);
    rr.insert(20, 20);
    rr.insert(30, 30);
    EXPECT_TRUE(ll == rr) << "После балансировки LL и RR деревья с {10, 20, 30} должны иметь идентичную форму!";

    AVLTree<int, int> lr;
    lr.insert(30, 30);
    lr.insert(10, 10);
    lr.insert(20, 20);
    EXPECT_TRUE(ll == lr) << "После LR-балансировки дерево {30, 10, 20} должно совпасть с эталоном";

    AVLTree<int, int> rl;
    rl.insert(10, 10);
    rl.insert(30, 30);
    rl.insert(20, 20);
    EXPECT_TRUE(ll == rl) << "После RL-балансировки дерево {10, 30, 20} должно совпасть с эталоном";

    if (testing::Test::HasFailure()) {
        std::cout << "\n---LL---\n" << show_tree(ll);
        std::cout << "\n---RR---\n" << show_tree(rr);
        std::cout << "\n---LR---\n" << show_tree(lr);
        std::cout << "\n---RL---\n" << show_tree(rl);
    }
}

TEST_F(AVL_Fixture, insert_overwrite_and_remove) {
    tree_int->insert(69, 9999);
    EXPECT_EQ(tree_int->len(), 5) << "Ожидаемая длина 5 при обновлении ключа. По факту : " << tree_int->len();
    EXPECT_EQ(tree_int->search(69), 9999) << "Ожидалось обновленное значение 9999. По факту : " << tree_int->search(69);

    tree_int->remove(42);
    EXPECT_FALSE(tree_int->key_be(42)) << "Ключ 42 должен быть удален";
    EXPECT_EQ(tree_int->len(), 4) << "Ожидаемая длина 4. По факту : " << tree_int->len();

    tree_int->remove(67);
    tree_int->remove(69);
    EXPECT_EQ(tree_int->len(), 2) << "Ожидаемая длина 2. По факту : " << tree_int->len();
    EXPECT_EQ(tree_int->Get(0), 520) << "Ожидаемый первый элемент 520. По факту : " << tree_int->Get(0);
    EXPECT_EQ(tree_int->Get(1), 2280) << "Ожидаемый второй элемент 2280. По факту : " << tree_int->Get(1);

    tree_int->remove(123456);
    EXPECT_EQ(tree_int->len(), 2) << "Удаление отсутствующего ключа изменило размер дерева";

    if (testing::Test::HasFailure()) {
        std::cout << "\n---Исходное дерево---\n" << show_tree(*orig_tree_int);
        std::cout << "\n---Текущее дерево после удалений---\n" << show_tree(*tree_int);
    }
}

class AVLBypassParamTest : public AVL_Fixture,
                           public testing::WithParamInterface<std::tuple<std::string, std::vector<int>>> {};

TEST_P(AVLBypassParamTest, all_six_traversals) {
    auto [order, expected_keys] = GetParam();

    AVLTree<int, int> t;
    t.insert(20, 20);
    t.insert(10, 10);
    t.insert(30, 30);

    std::vector<int> actual_keys;
    t.bypass(order, [&actual_keys](const int& k, const int&) {
        actual_keys.push_back(k);
    });

    EXPECT_EQ(actual_keys, expected_keys) << "Неверный порядок ключей при обходе \"" << order << "\"";

    if (testing::Test::HasFailure()) {
        std::cout << "\n---Тестируем обход " << order << "---\n";
        std::cout << show_tree(t);
    }
}

INSTANTIATE_TEST_SUITE_P(BypassParams, AVLBypassParamTest, testing::Values(
    std::make_tuple("KLP", std::vector<int>{20, 10, 30}),
    std::make_tuple("KPL", std::vector<int>{20, 30, 10}),
    std::make_tuple("LKP", std::vector<int>{10, 20, 30}),
    std::make_tuple("LPK", std::vector<int>{10, 30, 20}),
    std::make_tuple("PLK", std::vector<int>{30, 10, 20}),
    std::make_tuple("PKL", std::vector<int>{30, 20, 10})
));

TEST_F(AVL_Fixture, subtree_extract_and_contains) {
    AVLTree<int, int> base;
    for (int x : {40, 20, 60, 10, 30, 50, 70}) {
        base.insert(x, x * 10);
    }

    AVLTree<int, int>* sub = base.TakeSubTree(60);
    ASSERT_NE(sub, nullptr) << "TakeSubTree(60) вернул nullptr для существующего узла";
    EXPECT_EQ(sub->len(), 3) << "Ожидаемый размер поддерева 3. По факту : " << sub->len();
    EXPECT_TRUE(base.IfInSubTree(*sub)) << "Извлеченное поддерево обязано входить в исходное дерево";

    sub->insert(70, 999999);
    EXPECT_FALSE(base.IfInSubTree(*sub)) << "После изменения значения узла поддерево не должно входить в исходное";

    AVLTree<int, int>* missing_sub = base.TakeSubTree(777);
    EXPECT_EQ(missing_sub, nullptr) << "TakeSubTree(777) должен вернуть nullptr для несуществующего ключа";

    EXPECT_TRUE(base.IfInSubTree(*empty_tree)) << "Пустое дерево должно считаться входящим в любое дерево";

    delete sub;
}

TEST_F(AVL_Fixture, map_where_reduce) {
    AVLTree<int, int>* mapped = tree_int->map(double_val);
    EXPECT_EQ(mapped->len(), 5) << "map не должен менять количество элементов. По факту : " << mapped->len();
    EXPECT_EQ(mapped->search(42), 840) << "Ожидалось 420 * 2 = 840. По факту : " << mapped->search(42);
    EXPECT_EQ(mapped->search(228), 4560) << "Ожидалось 2280 * 2 = 4560. По факту : " << mapped->search(228);

    AVLTree<int, int>* filtered = tree_int->where(is_big);

    AVLTree<int, int> keys_only;
    for (int x : keys_int) keys_only.insert(x);
    AVLTree<int, int>* big_keys = keys_only.where(is_big);
    EXPECT_EQ(big_keys->len(), 3) << "Ожидалось 3 элемента > 60. По факту : " << big_keys->len();
    EXPECT_FALSE(big_keys->key_be(42)) << "Элемент 42 должен быть отфильтрован";
    EXPECT_TRUE(big_keys->key_be(69)) << "Элемент 69 должен остаться после where";

    int total = tree_int->reduce(sum_func, 0);
    int expected_sum = 420 + 520 + 690 + 670 + 2280;
    EXPECT_EQ(total, expected_sum) << "Ожидаемая сумма " << expected_sum << ". По факту : " << total;

    AVLTree<int, int> seq_123;
    seq_123.insert(1);
    seq_123.insert(2);
    seq_123.insert(3);
    EXPECT_EQ(seq_123.reduce(lab_reduce_func, 4), 144) << "Контрольный пример reduce из методички должен давать 132";

    delete mapped;
    delete filtered;
    delete big_keys;
}

TEST_F(AVL_Fixture, iterator_traversal_and_exceptions) {
    auto empty_it = empty_tree->GetIterator();
    EXPECT_FALSE(empty_it.HasNext()) << "У пустого дерева итератор не должен иметь элементов";
    EXPECT_FALSE(empty_it.Next()) << "Next() на пустом итераторе должен вернуть false";
    EXPECT_THROW(empty_it.GetCurrentItem(), std::invalid_argument) << "Ожидается std::invalid_argument при GetCurrentItem() на пустом итераторе";
    EXPECT_THROW(empty_it.GetCurrentKey(), std::invalid_argument) << "Ожидается std::invalid_argument при GetCurrentKey() на пустом итераторе";

    std::vector<int> iterated_keys;
    for (auto it = tree_int->GetIterator(); it.HasNext(); it.Next()) {
        iterated_keys.push_back(it.GetCurrentKey());
    }
    std::vector<int> expected = {42, 52, 67, 69, 228};
    EXPECT_EQ(iterated_keys, expected) << "Итератор должен обходить дерево в отсортированном порядке (LKP)";
}

TEST_F(AVL_Fixture, different_data_types) {
    EXPECT_EQ(tree_double->len(), 3);
    EXPECT_DOUBLE_EQ(tree_double->search(1.5), 10.5);

    EXPECT_EQ(tree_str->len(), 3);
    EXPECT_EQ(tree_str->search("a"), "первый");
    EXPECT_EQ(tree_str->Get(2), "третий");

    using FuncPtr = int (*)(int);
    AVLTree<int, FuncPtr> func_tree;
    func_tree.insert(0, &inc1);
    func_tree.insert(1, &inc2);
    func_tree.insert(2, &inc3);

    EXPECT_EQ(func_tree.search(0)(0), 1) << "Ожидался вызов inc1(0) == 1";
    EXPECT_EQ(func_tree.search(1)(0), 2) << "Ожидался вызов inc2(0) == 2";
    EXPECT_EQ(func_tree.search(2)(0), 3) << "Ожидался вызов inc3(0) == 3";

    AVLTree<Person, std::string> persons;
    Person p1{101, "Иван", "Иванов"};
    Person p2{202, "Петр", "Петров"};
    persons.insert(p2, "Группа Б25-521");
    persons.insert(p1, "Группа Б25-522");

    EXPECT_EQ(persons.len(), 2);
    EXPECT_TRUE(persons.key_be(p1));
    EXPECT_EQ(persons.search(p1), "Группа Б25-522");
    EXPECT_EQ(persons.Get(0), "Группа Б25-522");
}