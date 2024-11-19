#pragma once
#include "Types.h"

#pragma region TypeList
template<typename... T>
struct TypeList;

template<typename T, typename U>
struct TypeList<T, U>
{
    using Head = T;
    using Tail = U;
};

template<typename T, typename... U>
struct TypeList<T, U...>
{
    using Head = T;
    using Tail = TypeList<U...>;
};
#pragma endregion

#pragma region Length
template<typename T>
struct Length;

// 컴파일 타임 안에서 계산하기위하여 enum을 사용.
// 사실상 템플릿을 재귀함수 호출하는 것과 비슷한 방식으로 진행하는 것.
template<>
struct Length<TypeList<>>
{
    enum { value = 0 };
};

template<typename T, typename... U>
struct Length<TypeList<T, U...>>
{
    enum { value = 1 + Length<TypeList<U...>>::value };
};

#pragma endregion

#pragma region TypeAt
template<typename TL, int32 index>
struct TypeAt;

template<typename Head, typename... Tail>
struct TypeAt<TypeList<Head, Tail...>, 0>
{
    // 0은 헤드를 반환
    using Result = Head;
};

template<typename Head, typename... Tail, int32 index>
struct TypeAt<TypeList<Head, Tail...>, index>
{
    // index -1을 넣어 재귀로 호출하면서 0이될 때 위의 0함수를 호출하게 됨.
    using Result = typename TypeAt<TypeList<Tail...>, index - 1>::Result;
};

#pragma endregion

#pragma region IndexOf
template<typename TL, typename T>
struct IndexOf;

template<typename... Tail, typename T>
struct IndexOf<TypeList<T, Tail...>, T>
{
    enum { value = 0 };
};

template<typename T>
struct IndexOf<TypeList<>, T>
{
    // 빈 TypeList를 받아 못찾았을 경우 -1을 리턴
    enum { value = -1 };
};

template<typename Head, typename... Tail, typename T>
struct IndexOf<TypeList<Head, Tail...>, T>
{
private:
    enum { temp = IndexOf<TypeList<Tail...>, T>::value };
public:
    enum { value = (temp == -1) ? -1 : temp + 1 };
};
#pragma endregion

#pragma region Conversion
template<typename From, typename To>
class Conversion
{
private:
    using Small = __int8;
    using Big = __int32;
    
    // 컴파일 단계에서 Test함수를 호출 할 때, 실행되는 함수를 토대로 Small, Big중 하나를 호출하고
    // 그 결과값을 토대로 결과를 판별함.
    static Small Test(const To&) { return 0; }
    static Big Test(...) { return 0; }
    static From MakeFrom() { return 0; }

public:

    enum
    {
        exists = sizeof(Test(MakeFrom())) == sizeof(Small)
    };
};

// ex)
// bool canConvert = Conversion<TestAAA, TestBBB>::exists
#pragma endregion

#pragma region TypeCast

template<int32 v>
struct Int2Type
{
    enum { value = v };
};

template<typename TL>
class TypeConversion
{
public:
    enum
    {
        length = Length<TL>::value
    };

    TypeConversion()
    {
        MakeTable(Int2Type<0>(), Int2Type<0>());
    }

    // 들어가는 인자에 따라 보정이되어, MakeTable이 여러개의 함수가 만들어짐.
    template<int32 i, int32 j>
    static void MakeTable(Int2Type<i>, Int2Type<j>)
    {
        using FromType = typename TypeAt<TL, i>::Result;
        using ToType = typename TypeAt<TL, j>::Result;

        if (Conversion<const FromType*, const ToType*>::exists)
            s_convert[i][j] = true;
        else
            s_convert[i][j] = false;

        // 재귀로 호출하여 j가 추가된 함수를 호출
        MakeTable(Int2Type<i>(), Int2Type<j + 1>());
    }

    // j가 멈춰야 될 때 아래의 함수를 호출
    // i에 1을 추가하고, 다시 위의 함수를 호출하여 j가 계속 증가
    template<int32 i>
    static void MakeTable(Int2Type<i>, Int2Type<length>)
    {
        MakeTable(Int2Type<i + 1>(), Int2Type<0>());
    }

    template<int j>
    static void MakeTable(Int2Type<length>, Int2Type<j>)
    {
        // 마지막에 도착했다면 아무것도 실행하지 않아 종료시킴.
    }

    static inline bool CanConvert(int32 from, int32 to)
    {
        static TypeConversion conversion;
        return s_convert[from][to];
    }

public:
    static bool s_convert[length][length];
};

template<typename TL>
bool TypeConversion<TL>::s_convert[length][length];

// C#의 is, as 와 비슷
template<typename To, typename From>
To TypeCast(From* ptr)
{
    if (ptr == nullptr)
        return nullptr;

    using TL = typename From::TL;

    // 포인터를 없애준 타입을 뱉음(remove_pointer_t)
    if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
        return static_cast<To>(ptr);

    return nullptr;
}

template<typename To, typename From>
shared_ptr<To> TypeCast(shared_ptr<From> ptr)
{
    if (ptr == nullptr)
        return nullptr;

    using TL = typename From::TL;

    // 포인터를 없애준 타입을 뱉음(remove_pointer_t)
    if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
        return static_pointer_cast<To>(ptr);

    return nullptr;
}

template<typename To, typename From>
bool CanCast(From* ptr)
{
    if (ptr == nullptr)
        return false;

    using TL = typename From::TL;
    return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value);
}

template<typename To, typename From>
bool CanCast(shared_ptr<From> ptr)
{
    if (ptr == nullptr)
        return false;

    using TL = typename From::TL;
    return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value);
}

#pragma endregion

#define DECLARE_TL      using TL = TL; int32 _typeId;
#define INIT_TL(Type)   _typeId = IndexOf<TL, Type>::value;
