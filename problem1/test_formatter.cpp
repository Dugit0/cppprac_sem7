#include "formatter.cpp"


int main() {
    {
        std::cout << "---------- TEST 1 ----------" << std::endl;
        std::string s = Format(5);
        std::cout << s << std::endl;
    }
    {
        std::cout << "---------- TEST 2 ----------" << std::endl;
        std::string s = TPrettyPrinter().Format("my int is ").Format(5).Str();
        std::cout << s << std::endl;
    }
    {
        std::cout << "---------- TEST 3 ----------" << std::endl;
        auto i = std::numeric_limits<int>::max();
        auto ui = std::numeric_limits<unsigned int>::max();
        auto ll = std::numeric_limits<long long>::max();
        auto ull = std::numeric_limits<unsigned long long>::max();
        std::string s = TPrettyPrinter().Format(i).Format(" ").Format(ui).Format(" ").Format(ll).Format(" ").Format(ull).Str();
        std::cout << s << std::endl;
    }
    {
        std::cout << "---------- TEST 4 ----------" << std::endl;
        std::vector<int> v1 {};
        std::vector<int> v2 {1, 4, 7, 9};
        std::string s = Format(v1) + "|" + Format(v2);
        std::cout << s << std::endl;
    }
    {
        std::cout << "---------- TEST 5 ----------" << std::endl;
        std::set<int> v {1, 4, 7, 9};
        std::string s = Format(v);
        std::cout << s << std::endl;
    }
    {
        std::cout << "---------- TEST 6 ----------" << std::endl;
        std::tuple<
            int,
            std::set<std::string>,
            std::vector<long long>,
            std::tuple<std::pair<unsigned int, long long>>
            > v {-100, {"asdf", "sdgsfg"}, {12312313, 241, -3123}, {{2, -123}}};
        std::string s = Format(v);
        std::cout << s << std::endl;
    }
    {
        std::cout << "---------- LAST TEST ----------" << std::endl;
        std::tuple<std::string, int, int> t = {"xyz", 1, 2};
        std::vector<std::pair<int, int> > v = {{1, 4}, {5, 6}};
        std::string s1 = TPrettyPrinter().Format("vector: ").Format(v).Str();
        // "vector: [(1, 4), (5, 6)]"
        std::cout << s1 << std::endl;
        std::string s2 = TPrettyPrinter().Format(t).Format(" ! ").Format(0).Str();
        // "(xyz, 1, 2) ! 0"
        std::cout << s2 << std::endl;
    }

}


