#include <astd/astd.h>
#include <iostream>
#include <thread>
#include <optional>
#include <variant>

Amazing::Ring<int32_t> ring;

struct NoDtor
{
    Amazing::Vector<int> v;
    Amazing::Map<int, Amazing::String> m;
};

class ProduceThread : public Amazing::Thread
{
private:
    void run(std::stop_token token) override
    {
        int32_t v[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        while (true)
        {
            if (token.stop_requested())
                return;

            ring.write(v, 10);
            std::cout << ring.size() << '\n';
        }
    }
};

class ConsumeThread : public Amazing::Thread
{
private:
    void run(std::stop_token token) override
    {
        Amazing::Vector<int32_t> v(10);
        while (true)
        {
            if (token.stop_requested())
                return;

            ring.read(v.data(), 10);
            std::cout << ring.size() << '\n';
        }
    }
};


void just()
{
    NoDtor* no_dtor = Amazing::Allocator<NoDtor>::allocate(1);

    no_dtor->v.emplace_back(4);
    no_dtor->v.emplace_back(5);
    no_dtor->v.emplace_back(6);

    no_dtor->m.emplace(3, "third");
    no_dtor->m.emplace(4, "four");
    no_dtor->m.emplace(5, "five");

    int32_t val[10] = { 1, 5, 7, 4, 9, 2, 3, 8, 6 };
    Amazing::heap_sort(val);

    uint32_t* v = nullptr;
    if (!no_dtor->v.empty())
    {
        v = static_cast<uint32_t*>(alloca(sizeof(uint32_t) * no_dtor->v.size()));
        for (int i = 0; i < no_dtor->v.size(); i++)
        {
            v[i] = no_dtor->v[i];
        }
    }

    for (int i = 0; i < no_dtor->v.size(); i++)
        v[i] = no_dtor->v[i];

    Amazing::Allocator<NoDtor>::deallocate(no_dtor);
}

void iii(int x)
{
    std::cout << x << '\n';
}

int main()
{
    ProduceThread produce_thread;
    ConsumeThread consume_thread;

    produce_thread.start();
    consume_thread.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    produce_thread.stop();
    consume_thread.stop();

    int hj = 5;
    Amazing::Functional<void(int)> td(iii);

    td(5);

    Amazing::Pair<int, NoDtor> pt;
    Amazing::Pair<int, NoDtor> qt(std::move(pt));

    just();
    int* pi = Amazing::Allocator<int>::allocate(1);
    // std::thread producer(&produce);
    // std::thread consumer(&consume);
    //
    // producer.join();
    // consumer.join();

    Amazing::String ss = Amazing::to_str(23);

    Amazing::HashMap<int, int> map;
    map.emplace(2, 4);
    map.emplace(3, 5);
    map.emplace(4, 6);

    int m1 = map[2];
    int m2 = map[3];
    int m3 = map[4];

    Amazing::String s("chr");
    Amazing::String f("fl");
    s += f;

    size_t pos = s.find("rfk");

    Amazing::Vector<int64_t> v;
    v.push_back(10);
    v.push_back(9);
    v.push_back(6);
    v.push_back(2);
    v.push_back(7);
    v.push_back(1);
    v.push_back(4);
    v.push_back(7);
    v.push_back(6);

    Amazing::Vector<double> d;
    d.push_back(8);
    d.push_back(10);
    d.push_back(9);
    d.push_back(6);
    d.push_back(2);
    d.push_back(7);
    d.push_back(1);
    d.push_back(4);
    d.push_back(7);
    d.push_back(6);


    Amazing::List<double> l;
    l.push_back(1);
    l.push_back(4);
    l.push_back(9);
    l.push_back(8);


    Amazing::Map<int64_t, uint32_t> m;
    m.emplace(1, 2);
    m.emplace(Amazing::Pair<int64_t, uint32_t>(34, 1));
    auto it = m.find(1);
    //m.insert(Amazing::Pair<int64_t, uint32_t>(32, 12));
    //m.insert(Amazing::Pair<int64_t, uint32_t>(2, 12));
    //m.insert(Amazing::Pair<int64_t, uint32_t>(2, 11));

    m.erase(1);

    m[2] = 14;

    m.clear();

    struct Tr
    {
        int v;
        double d;
    };

    Amazing::UniquePtr<Tr> p(1, 2);

    for (size_t i = 0; i < v.size(); i++)
        std::cout << v[i] << '\n';

    for (size_t i = 0; i < d.size(); i++)
        std::cout << d[i] << '\n';


    return 0;
}