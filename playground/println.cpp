#include <type_traits>
#include <iostream>
#include <exception>
#include <tuple>


// from  stackoverflow http://stackoverflow.com/questions/21062864/optimal-way-to-access-stdtuple-element-in-runtime-by-index
struct Func {
    template<class T>
    void operator()(T p)
    {
        std::cout << p;
//        std::cout << "'" << __PRETTY_FUNCTION__ << " : " << p << "'";
    }
};


template<int... Is> struct seq {};
template<int N, int... Is> struct gen_seq : gen_seq<N-1, N-1, Is...> {};
template<int... Is> struct gen_seq<0, Is...> : seq<Is...> {};

template<int N, class T, class F>
void apply_one(T& p, F func)
{
    func( std::get<N>(p) );
}

template<class T, class F, int... Is>
void apply(T& p, int index, F func, seq<Is...>)
{
    using FT = void(T&, F);
    static constexpr FT* arr[] = { &apply_one<Is, T, F>... };
    arr[index](p, func);
}

template<class T, class F>
void apply(T& p, int index, F func)
{
    apply(p, index, func, gen_seq<std::tuple_size<T>::value> {});
}

template<typename Tup>
void print_tuple_value(Tup & all_args, const int index)
{
    if (index < std::tuple_size<Tup>::value)
        apply(all_args, index, Func{});
}

template<typename Tup, typename... Args>
void print_helper(Tup & all_args, const char *s)
{
    int cur_index = 0;
    int dyn_index = 0;

    if (s == nullptr) return;
    while (*s) {

        if (*s == '\\') {
            ++s;
        } else if (*s == '{') {

            // number
            if (isdigit(s[1])) {
                int i = 2;
                while (isdigit(s[i]))
                    i++;
                if (s[i] == '}') {
                    dyn_index = atoi(&s[1]);
                    print_tuple_value(all_args, dyn_index);
                    s = &s[i + 1];
                    continue;
                }
            }

            if (*(s + 1) == '}') {
                print_tuple_value(all_args, cur_index++);
                s += 2;
                continue;
            }
        }

        std::cout << *s++;
    }
}


template<typename... Args>
void print(const char *s, Args... args)
{
    std::tuple<Args...> all_args(args...);
    print_helper(all_args, s);
}

void println(const char *s)
{
    std::cout << s << "\n";
}

template<typename... Args>
void println(const char *s, Args... args)
{
    print(s, args...);
    std::cout << "\n";
}

int main(int ac, char * av[])
{

    float f = 1.0;
    double d = 5.6;
    void * p = reinterpret_cast<void*>(0xdeadbeef);
    println("f = {}, b = {}, {}, {}, {2048}", f, 2, d, p, std::string("hello world !"));
    println("f = arg{} arg{} arg{} arg{} arg{} arg{} @3{3} @1:{1} b", 1, 2, 3, 4);
    return 0;
}
