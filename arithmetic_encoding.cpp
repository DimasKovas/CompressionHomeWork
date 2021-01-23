/*
 * Template: github.com/DimasKovas/cp-templates
 */

#include <bits/stdc++.h>

using namespace std;

#define all(x) (x).begin(), (x).end()

random_device rnd_device;
mt19937 rnd(rnd_device());

#ifdef LOCAL
    #define err(...) cerr << "Line " << __LINE__ << ": ", _dbg(#__VA_ARGS__, __VA_ARGS__), cerr << endl

    #define t1 template <typename T>
    #define t2 template <typename T1, typename T2>

    string to_string(const string& s) { return '"' + s + '"'; }
    string to_string(const char* s) { return to_string(string(s)); }
    string to_string(bool b) { return b ? "true" : "false"; }
    string to_string(std::vector<bool>::reference b) { return to_string(bool(b)); }
    string to_string(char c) { return "'"s + c + "'"; }

    template <size_t N>
    string to_string(bitset<N> b) {
        stringstream s;
        s << b;
        return s.str();
    }

    // Container's functions should have forward declarations to allow nesting.
    t1 string to_string(T v);
    t2 string to_string(const pair<T1, T2>& p);

    t2 string to_string(const pair<T1, T2>& p) { return "{" + to_string(p.first) + ", " + to_string(p.second) + "}"; }

    t1 string to_string(T v) {
        bool first = true;
        string res = "[";
        for (const auto &x : v) {
            if (!first) res += ", ";
            first = false;
            res += to_string(x);
        }
        res += "]";
        return res;
    }
     
    pair<string_view, string_view> _dbg_split(string_view names) {
        int cnt = 0, pos = 0, q1 = false, q2 = false, bs = false;
        for (char c : names) {
            if (bs) bs = false;
            else if (c == '\\') bs = true;
            else if (q1) { if (c == '\'') q1 = false; }
            else if (q2) { if (c == '\"') q2 = false; }
            else if (c == '\'') q1 = true;
            else if (c == '\"') q2 = true;
            else if (cnt == 0 && c == ',') break;
            else if (c == '(' || c == '[' || c == '{') ++cnt;
            else if (c == ')' || c == ']' || c == '}') --cnt;
            ++pos;
        }
        auto name = names.substr(0, pos);
        name.remove_prefix(name.find_first_not_of(" \t\n"));
        name.remove_suffix((int)name.size() - 1 - name.find_last_not_of(" \t\n"));
        return {name, names.substr(min<int>(pos + 1, names.size()))};
    }

    void _dbg(string_view names) {}

    template <typename T1, typename ...T2>
    void _dbg(string_view names, const T1 &a, const T2& ...ar) {
        auto [name, rnames] = _dbg_split(names);
        auto val = to_string(a);
        if (val == name) { // constant
            cerr << val << "; ";
        } else {
            cerr << name << " = " << to_string(a) << "; ";
        }
        _dbg(rnames, ar...);
    }

    #undef t1
    #undef t2
#else
    #define err(...) ((void)42)
#endif

//--------------------------------REALIZATION---------------------------------\\

#define private public

using Byte = unsigned char;

string EncodeUint(uint32_t x) {
    string s;
    s.resize(sizeof(x));
    memcpy(s.data(), &x, sizeof(x));
    return s;
}

// returns how many bytes were read
int DecodeUint(uint32_t * result, const Byte * s) {
    memcpy(result, s, sizeof(uint32_t));
    return 4;
}

// Default value ~ 1.0, everything else can be derived
class Decimal
{
public:
    Decimal(int precision) : bits(precision, -1) { }

    Decimal(Decimal && other) = default;
    Decimal(const Decimal & other) = default;

    Decimal & operator = (Decimal && other) = default;
    Decimal & operator = (const Decimal & other) = default;

    int GetPrecision() const {
        return bits.size();
    }

    Decimal & operator /= (uint32_t x) {
        uint64_t carry = 0;
        for (int i = 0; i < GetPrecision(); ++i) {
            uint64_t curValue = (carry << 32) + bits[i];
            bits[i] = curValue / x;
            carry = curValue % x;
        }
        return *this;
    }

    Decimal & operator *= (uint32_t x) {
        uint64_t carry = 0;
        for (int i = GetPrecision() - 1; i >= 0; --i) {
            uint64_t curValue = x * 1ull * bits[i] + carry;
            bits[i] = static_cast<uint32_t>(curValue);
            carry = (curValue >> 32);
        }
        assert(carry == 0);
        return *this;
    }

    Decimal & operator += (const Decimal & other) {
        assert(GetPrecision() == other.GetPrecision());

        uint64_t carry = 0;
        for (int i = GetPrecision() - 1; i >= 0; --i) {
            uint64_t curValue = carry + bits[i] + other.bits[i];
            bits[i] = static_cast<uint32_t>(curValue);
            carry = (curValue >> 32);
        }
        return *this;
    }

    Decimal & operator -= (const Decimal & other) {
        assert(GetPrecision() == other.GetPrecision());
        if (other > *this) {
            bits.assign(bits.size(), 0);
            return * this;
        }
        assert(other <= *this);

        uint64_t carry = 0;
        for (int i = GetPrecision() - 1; i >= 0; --i) {
            uint32_t curValue = bits[i] - other.bits[i] - carry;
            carry = (carry + other.bits[i] > bits[i] ? 1 : 0);
            bits[i] = curValue;
        }

        assert(carry == 0);
        return *this;
    }

    friend bool operator < (const Decimal & a, const Decimal & b) {
        assert(a.GetPrecision() == b.GetPrecision());
        return a.bits < b.bits;
    }

    friend bool operator <= (const Decimal & a, const Decimal & b) {
        assert(a.GetPrecision() == b.GetPrecision());
        return a.bits <= b.bits;
    }

    friend bool operator > (const Decimal & a, const Decimal & b) {
        assert(a.GetPrecision() == b.GetPrecision());
        return a.bits > b.bits;
    }

    friend bool operator >= (const Decimal & a, const Decimal & b) {
        assert(a.GetPrecision() == b.GetPrecision());
        return a.bits >= b.bits;
    }

    friend Decimal operator + (Decimal a, const Decimal & b) {
        return a += b;
    }

    friend Decimal operator - (Decimal a, const Decimal & b) {
        return a -= b;
    }

    friend Decimal operator * (Decimal a, uint32_t x) {
        return a *= x;
    }

    friend Decimal operator / (Decimal a, uint32_t x) {
        return a /= x;
    }

    string Encode() {
        string result = EncodeUint(bits.size());

        for (uint32_t b : bits) {
            result += EncodeUint(b);
        }
        return result;
    }

    int Decode(const Byte * data) {
        uint32_t size;
        data += DecodeUint(&size, data);
        err(size);
        bits.resize(size);
        for (int i = 0; i < size; ++i) {
            data += DecodeUint(&bits[i], data);
        }
        return (size + 1) * sizeof(uint32_t);
    }

    uint32_t GetComponent(int i) {
        assert(i >= 0 && i < GetPrecision());
        return bits[i];
    }

private:
    vector<uint32_t> bits;
};

string readInput() {
    string result;
    int c;
    while ((c = cin.get()) != EOF) {
        result.push_back(c);
    }
    return result;
}

class ArithmeticEncodingTable
{
public:

    string Encode() {
        string result = EncodeUint(entries.size());
        for (const auto & [weight, symbol] : entries) {
            result += EncodeUint(weight);
            result += symbol;
        }
        return result;
    }

    int Decode(const Byte * data) {
        uint32_t size;
        data += DecodeUint(&size, data);
        entries.resize(size);
        for (int i = 0; i < size; ++i) {
            data += DecodeUint(&entries[i].weight, data);
            entries[i].symbol = *data;
            ++data;
        }
        return size * (sizeof(uint32_t) + sizeof(Byte)) + sizeof(uint32_t);
    }

    void Build(const string & data) {
        vector<uint32_t> count(256);
        for (Byte b : data) {
            ++count[b];
        }

        entries.clear();

        for (int i = 0; i < 256; ++i) {
            if (count[i] > 0) {
                entries.push_back({static_cast<uint32_t>(-1) * 1ull * count[i] / data.size(), Byte(i)});
            }
        }
    }

    uint32_t GetWeight(Byte b) {
        if (weight_cache[b] == -1) {
            weight_cache[b] = 0;
            for (const auto & [weight, symbol] : entries) {
                if (symbol == b) {
                    weight_cache[b] = weight;
                    break;
                } 
            }
        }
        return weight_cache[b];
    }

    uint32_t GetPrefixWeight(Byte b) {
        if (prefix_cache[b] == -1) {
            prefix_cache[b] = 0;
            for (const auto & [weight, symbol] : entries) {
                if (symbol == b) {
                    break;
                }
                prefix_cache[b] += weight;
            }
        }
        return prefix_cache[b];
    }

    const string & GetSymbols() {
        if (symbols_cache.empty()) {
            for (const auto & [weight, symbol] : entries) {
                symbols_cache += symbol;
            }
        }
        return symbols_cache;
    }

    struct Entry {
        uint32_t weight;
        Byte symbol;
    };

private:
    vector<Entry> entries;

    vector<int64_t> weight_cache = vector<int64_t>(256, -1);
    vector<int64_t> prefix_cache = vector<int64_t>(256, -1);
    string symbols_cache;

};

const int N = 10000;

class ArithmeticEncoder
{
public:
    string Encode(const string & data) {
        ArithmeticEncodingTable table;
        table.Build(data);
        Decimal bottom(N);
        bottom *= 0;
        Decimal length(N);

        for (Byte b : data) {
            bottom += (Decimal(length) / static_cast<uint32_t>(-1) * table.GetPrefixWeight(b));
            length /= static_cast<uint32_t>(-1);
            length *= table.GetWeight(b); 
        }

        return EncodeUint(data.size()) + table.Encode() + bottom.Encode();
    }

    string Decode(const string & data) {
        const Byte * bytes = reinterpret_cast<const Byte *>(data.data());

        uint32_t dataSize;
        bytes += DecodeUint(&dataSize, bytes);

        ArithmeticEncodingTable table;
        bytes += table.Decode(bytes);

        Decimal bottom(N);
        bytes += bottom.Decode(bytes);

        string result;
        result.reserve(dataSize);

        for (int i = 0; i < dataSize; ++i) {
            bool found = false;
            for (Byte b : table.GetSymbols()) {
                if (bottom.GetComponent(0) < table.GetPrefixWeight(b) + table.GetWeight(b)) {
                    result += b;

                    Decimal cur(N);
                    cur /= static_cast<uint32_t>(-1);
                    cur *= table.GetPrefixWeight(b);
                    bottom -= cur;

                    bottom /= table.GetWeight(b);
                    bottom *= static_cast<uint32_t>(-1);

                    found = true;
                    break;
                }
            }
            assert(found);
        }

        return result;
    }
};

int Score(const string & a, const string & b) {
    for (int i = 0; i < a.size() && i < b.size(); ++i) {
        if (a[i] != b[i]) {
            return i;
        }
    }
    return a.size();
}

int main(int argc, char ** argv) {
    assert(argc >= 2);

    string param(argv[1]);
    if (param == "e") {
        auto input = readInput();
        auto encoded = ArithmeticEncoder().Encode(input);
        err(input.size(), encoded.size());
        cout << encoded;
    } else if (param == "d") {
        auto input = readInput();
        auto decoded = ArithmeticEncoder().Decode(input);
        cout << decoded;
    } else if (param == "t") {
        auto input = readInput();
        auto encoded = ArithmeticEncoder().Encode(input);
        err(input.size(), encoded.size());
        auto decoded = ArithmeticEncoder().Decode(encoded);
        if (input.size() < 30) {
            err(input, decoded);
        }
        err(Score(input, decoded));
        assert(input == decoded);
    } else {
        throw 42;
    }

    return 0;
}
