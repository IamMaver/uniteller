#include <iostream>
#include <mutex>
#include <vector>

namespace uniteller {

    using namespace std;

    class IdentificatorMaker {

    private:

        unsigned long long _number;

        mutable std::mutex _mutex;

        std::string get() const;

    public:

        bool try_set_id(unsigned long long number);

        std::string inc_and_get_id();

        IdentificatorMaker() : _number{0} {}

    };

    string IdentificatorMaker::get() const {
        string result;
        string char_array = "ABCEHIKLNOPRSTUWXYZ";
        string number_array = "123456789";
        size_t size_of_group = char_array.size() * number_array.size();
        constexpr size_t MAX_GROUP_COUNT = 10;
        unsigned long long remaining = _number+1;
        unsigned long long groups[MAX_GROUP_COUNT] = {};
        size_t group_count = 0;

        do {
            groups[group_count++] = (remaining-1) % size_of_group;
            remaining = (remaining-1) / size_of_group;
        } while (remaining > 0);

        for (size_t i = group_count - 1;; i--) {
            result += char_array[groups[i] / number_array.size()];
            result += number_array[groups[i] % number_array.size()];
            if (i == 0) { break; }
            result += "-";
        }

        return result;
    }

    string IdentificatorMaker::inc_and_get_id() {
        std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
        if (!lock) {
            return "";
        }
        _number++;
        return get();
    }

    bool IdentificatorMaker::try_set_id(unsigned long long number) {
        std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
        if (!lock) {
            return false;
        }
        _number = number;
        return true;
    }
}

using namespace uniteller;

int main() {
    IdentificatorMaker id_maker;

    for (size_t i = 0; i < 175; i++) {
        cout << i << "  ";
        id_maker.try_set_id(i);
        cout << id_maker.inc_and_get_id() << endl;
    }
    for (size_t i = 29235; i < 29545; i++) {
        cout << i << "  ";
        id_maker.try_set_id(i);
        cout << id_maker.inc_and_get_id() << endl;
    }

    return 0;
}
