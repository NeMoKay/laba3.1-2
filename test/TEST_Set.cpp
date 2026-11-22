#include "Fixtures.hpp"

TEST_F(Set_Fixture, def_constructor){
    EXPECT_TRUE(empty_set->IsEmpty()) << "Ожидалось пустое множество. По факту IsEmpty() == false";
    EXPECT_EQ(empty_set->len(), 0) << "Ожидаемая мощность множества 0. По факту : " << empty_set->len();
    EXPECT_FALSE(empty_set->contains(42)) << "Пустое множество не должно содержать элемент 42";

    if (testing::Test::HasFailure()){
        std::cout << "\n---Текущее множество (пустое)---\n";
        std::cout << show_set(*empty_set);
    }
}

TEST_F(Set_Fixture, copy_and_move_constructors){
    Set<int> copy(*set_A);
    EXPECT_EQ(copy.len(), 5) << "Ожидаемая мощность копии 5. По факту : " << copy.len();
    EXPECT_TRUE(copy == *set_A) << "Ожидалось равенство копии и исходного множества";

    copy.insert(1000);
    EXPECT_EQ(set_A->len(), 5) << "Изменение копии повлияло на оригинал! Длина set_A : " << set_A->len();
    EXPECT_FALSE(set_A->contains(1000)) << "Элемент 1000 не должен присутствовать в set_A";

    Set<int> moved(std::move(copy));
    EXPECT_EQ(moved.len(), 6) << "Ожидаемая мощность перемещенного множества 6. По факту : " << moved.len();
    EXPECT_TRUE(copy.IsEmpty()) << "После перемещения исходное множество должно быть пустым";
}

TEST_F(Set_Fixture, uniqueness_and_remove){
    set_A->insert(42);
    set_A->insert(69);
    EXPECT_EQ(set_A->len(), 5) << "Дубликаты не должны увеличивать мощность множества. По факту : " << set_A->len();

    set_A->remove(42);
    EXPECT_FALSE(set_A->contains(42)) << "Элемент 42 должен быть удален из множества";
    EXPECT_EQ(set_A->len(), 4) << "Ожидаемая мощность множества 4. По факту : " << set_A->len();

    set_A->remove(9999);
    EXPECT_EQ(set_A->len(), 4) << "Удаление несуществующего элемента изменило мощность множества";

    if (testing::Test::HasFailure()){
        std::cout << "\n---Текущее множество set_A---\n";
        std::cout << show_set(*set_A);
    }
}

class SetContainsParamTest : public Set_Fixture,
                             public testing::WithParamInterface<std::tuple<int, bool>>{};

TEST_P(SetContainsParamTest, contains_check){
    auto [elem, expected] = GetParam();

    EXPECT_EQ(set_A->contains(elem), expected) << "Для элемента " << elem << " ожидалось contains == " << expected << ". По факту : " << set_A->contains(elem);

    if (testing::Test::HasFailure()){
        std::cout << "\n---Текущее множество set_A---\n";
        std::cout << show_set(*set_A);
    }
}

INSTANTIATE_TEST_SUITE_P(ContainsParams, SetContainsParamTest, testing::Values(
    std::make_tuple(42, true),
    std::make_tuple(52, true),
    std::make_tuple(69, true),
    std::make_tuple(228, true),
    std::make_tuple(0, false),
    std::make_tuple(-5, false),
    std::make_tuple(100, false)
));

TEST_F(Set_Fixture, subset_check){
    EXPECT_TRUE(set_B->If_Set_in(*set_A)) << "Ожидалось, что set_B является подмножеством set_A";
    EXPECT_FALSE(set_A->If_Set_in(*set_B)) << "Ожидалось, что set_A НЕ является подмножеством set_B";

    EXPECT_FALSE(set_C->If_Set_in(*set_A)) << "Множество{1, 2, 3} не должно входить в set_A";

    EXPECT_TRUE(empty_set->If_Set_in(*set_A)) << "Пустое множество должно входить в set_A";
    EXPECT_TRUE(empty_set->If_Set_in(*empty_set)) << "Пустое множество должно входить само в себя";

    if (testing::Test::HasFailure()){
        std::cout << "\n---set_A---\n" << show_set(*set_A);
        std::cout << "\n---set_B---\n" << show_set(*set_B);
        std::cout << "\n---set_C---\n" << show_set(*set_C);
    }
}

TEST_F(Set_Fixture, equality_different_insertion_order){
    Set<int> rev_C;
    rev_C.insert(3);
    rev_C.insert(2);
    rev_C.insert(1);

    EXPECT_TRUE(*set_C == rev_C) << "Множества{1, 2, 3} и{3, 2, 1} должны быть равны независимо от порядка вставки";
    EXPECT_FALSE(*set_C != rev_C) << "Оператор != вернул true для равных множеств с разным порядком вставки";

    EXPECT_TRUE(*set_A != *set_B) << "Множества set_A и set_B не должны быть равны";
    EXPECT_FALSE(*set_A == *set_B) << "Оператор == вернул true для разных множеств";

    if (testing::Test::HasFailure()){
        std::cout << "\n---set_C---\n" << show_set(*set_C);
        std::cout << "\n---rev_C---\n" << show_set(rev_C);
    }
}

TEST_F(Set_Fixture, different_types){
    EXPECT_EQ(set_str->len(), 3);
    EXPECT_TRUE(set_str->contains("красный"));
    EXPECT_FALSE(set_str->contains("желтый"));

    Set<Person> team;
    Person p1{1, "Анна", "Смирнова"};
    Person p2{2, "Борис", "Кузнецов"};
    team.insert(p1);
    team.insert(p2);
    team.insert(p1);

    EXPECT_EQ(team.len(), 2) << "Ожидалось 2 уникальных участника. По факту : " << team.len();
    EXPECT_TRUE(team.contains(p2));
}