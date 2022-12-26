#include <iostream>
#include <vector>
#include <string>


class BigInt {
public:
    BigInt() {};

    explicit BigInt(std::string number) {
        std::string num;
        if (number[0] == '-') {
            number = number.substr(1);
            sign = true;
        } else {
            sign = false;
        }
        for (int i = number.length(); i > 0; i -= 9) {
            numbers_array.push_back(atoi((i < 9 ? number.substr(0, i) : number.substr(i - 9, 9)).c_str()));
        }
    }

    explicit BigInt(int number) {
        if (number < 0) {
            sign = true;
            number = number;
        } else {
            sign = false;
        }
        while (number != 0) {
            numbers_array.push_back(number % base);
            number /= base;
        };
    }

    BigInt(const std::string &number, std::vector<int> &c) {
        for (int i = number.length(); i > 0; i -= 9) {
            c.push_back(atoi((i < 9 ? number.substr(0, i) : number.substr(i - 9, 9)).c_str()));
        }
    }

    void Print();

    void ShiftRight();

    bool CheckZero(BigInt Number);

    BigInt NOD(BigInt num, BigInt den);

    //унарные знаки
    BigInt operator+() const {
        return BigInt(*this);
    }

    BigInt operator-() const {
        BigInt c(*this);
        c.sign = !c.sign;
        return c;
    }

    // проверка ==
    friend bool operator==(const BigInt &left, const BigInt &right) {
        if (left.sign != right.sign) {
            return false;
        }
        if (left.numbers_array.empty()) { // есть конструктор по умолчанию по этому проверка на пустоту
            if (right.numbers_array.empty() || (right.numbers_array.size() == 1 && right.numbers_array[0] == 0)) {
                return true;
            } else {
                return false;
            }
        }
        if (right.numbers_array.empty()) {
            if (left.numbers_array.size() == 1 && left.numbers_array[0] == 0) {
                return true;
            } else {
                return false;
            }
        }
        if (left.numbers_array.size() != right.numbers_array.size()) {
            return false;
        }
        for (size_t i = 0; i < left.numbers_array.size(); ++i) {
            if (left.numbers_array[i] != right.numbers_array[i]) {
                return false;
            }
        }
        return true;
    }

    // оператор сравнения <
    friend bool operator<(const BigInt &left, const BigInt &right) {
        if (left == right) {
            return false;
        }
        if (left.sign) {
            if (right.sign) {
                return ((-right) < (-left));
            } else {
                return true;
            }
        } else if (right.sign) {
            return false;
        } else {
            if (left.numbers_array.size() != right.numbers_array.size()) {
                return left.numbers_array.size() < right.numbers_array.size();
            } else {
                for (long long i = left.numbers_array.size() - 1; i >= 0; --i) {
                    if (left.numbers_array[i] != right.numbers_array[i])
                        return left.numbers_array[i] < right.numbers_array[i];
                }
                return false;
            }
        }
    }


    // Сравнение <=
    friend bool operator<=(const BigInt &left, const BigInt &right) {
        if (left == right) {
            return true;
        }
        if (left < right) {
            return true;
        } else {
            return false;
        }

    }

    // сложение
    friend BigInt operator+(BigInt left, const BigInt &right) {
        if (left.sign) {
            if (right.sign) {
                return -(-left + (-right));
            } else {
                return right - (-left);
            }
        } else if (right.sign) {
            return left - (-right);
        }
        int carry = 0;
        for (size_t i = 0; i < std::max(left.numbers_array.size(), right.numbers_array.size()) || carry != 0; ++i) {
            if (i == left.numbers_array.size()) {
                left.numbers_array.push_back(0); // добавляем 0, чтобы было возможно сложение, если left < right
            }
            left.numbers_array[i] += carry + (i < right.numbers_array.size() ? right.numbers_array[i] : 0);
            if (left.numbers_array[i] >= BigInt::base) {
                carry = left.numbers_array[i];
            }
            if (carry != 0) {
                left.numbers_array[i] -= BigInt::base;
            }
        }
        return left;
    }

    // вычитание
    friend BigInt operator-(BigInt left, const BigInt &right) {
        if (right.sign) {
            return left + (-right);
        } else if (left.sign) {
            return -(-left + right);
        } else if (left < right) {
            return -(right - left);
        }
        int carry = 0;
        for (size_t i = 0; i < right.numbers_array.size() || carry; ++i) {
            left.numbers_array[i] -= carry + (i < right.numbers_array.size() ? right.numbers_array[i] : 0);
            carry = left.numbers_array[i] < 0;
            if (carry) {
                left.numbers_array[i] += base;
            }
        }
        while (left.numbers_array.size() > 1 && left.numbers_array.back() == 0) {
            left.numbers_array.pop_back();
        }
        return left;
    }

    friend BigInt operator*(const BigInt &left, const BigInt &right) {
        BigInt result("0");
        result.numbers_array.resize(left.numbers_array.size() + right.numbers_array.size());
        for (size_t i = 0; i < left.numbers_array.size(); ++i) {
            int carry = 0;
            for (size_t j = 0; j < right.numbers_array.size() || carry != 0; ++j) {
                long long cur = result.numbers_array[i + j] + left.numbers_array[i] * 1LL * (j < right.numbers_array.size() ? right.numbers_array[j] : 0) + carry;
                result.numbers_array[i + j] = cur % base;
                carry = cur / base;
            }
        }
        while (result.numbers_array.size() > 1 && result.numbers_array.back() == 0) {
            result.numbers_array.pop_back();
        }
        if (left.sign != right.sign) {
            result.sign = true;
        }
        if (left.sign && right.sign) {
            result.sign = false;
        }
        return result;
    }

    friend BigInt operator*(BigInt left, const int &right) {
        int carry = 0;
        for (size_t i = 0; i < left.numbers_array.size() || carry; ++i) {
            if (i == left.numbers_array.size()) {
                left.numbers_array.push_back(0);
            }
            long long cur = carry + left.numbers_array[i] * 1ll * right;
            left.numbers_array[i] = int(cur % base);
            carry = int(cur / base);
        }
        while (left.numbers_array.size() > 1 && left.numbers_array.back() == 0) {
            left.numbers_array.pop_back();
        }
        return left;
    }

    friend BigInt operator/(const BigInt &left, const BigInt &right) {
        if (right.numbers_array.size() == 1 and right.numbers_array[0] == 0) {
            std::cout << "divide by 0";
            exit(-1); // защита от любителей делить на 0
        }
        BigInt temp = right;
        temp.sign = false;
        BigInt result(0);
        BigInt current(0);
        result.numbers_array.resize(left.numbers_array.size());
        for (long long i = left.numbers_array.size() - 1; i >= 0; --i) {
            current.ShiftRight();
            current.numbers_array[0] = left.numbers_array[i];
            while (current.numbers_array.size() > 1 && current.numbers_array.back() == 0) {
                current.numbers_array.pop_back();
            }
            int x = 0;
            int y = 0;
            int bbase = base;
            while (y <= bbase) {
                int m = (y + bbase) / 2;
                BigInt t = temp * m;
                if (t <= current) {
                    x = m;
                    y = m + 1;
                } else {
                    bbase = m - 1;
                }
            }
            result.numbers_array[i] = x;
            current = current - temp * x;
        }
        if (left.sign != right.sign) {
            result.sign = true;
        }
        while (result.numbers_array.size() > 1 && result.numbers_array.back() == 0) {
            result.numbers_array.pop_back();
        }
        return result;
    }

private:
    static const int base = 1000000000;
    std::vector<int> numbers_array;
    bool sign;
};

void BigInt::Print() {
    if (sign) {
        std::cout << "-";
    }
    std::cout << (numbers_array.empty() ? 0 : numbers_array.back());
    for (int i = numbers_array.size() - 2; i >= 0; --i) {
        printf("%09d", numbers_array[i]);
    }
    std::cout << std::endl;
}

void BigInt::ShiftRight() {
    if (numbers_array.size() == 0) {
        numbers_array.push_back(0);
        return;
    }
    numbers_array.push_back(numbers_array[numbers_array.size() - 1]);
    for (size_t i = numbers_array.size() - 2; i > 0; --i) {
        numbers_array[i] = numbers_array[i - 1];
    }
    numbers_array[0] = 0;
}

bool BigInt::CheckZero(BigInt Number) {  // Проверка на 0.
    if (Number.numbers_array.size() == 1 and Number.numbers_array[0] == 0) {
        return true;
    } else {
        return false;
    }
}

BigInt BigInt::NOD(BigInt num, BigInt den) { //  Нахождение НОДа для сокращения дробей
    BigInt temp1 = num;
    BigInt temp2 = den;
    temp1.sign = false;
    temp2.sign = false;
    while (!(temp1 == temp2)) {
        if (temp1 < temp2) {
            temp2 = temp2 - temp1;
        } else {
            temp1 = temp1 - temp2;
        }
    }
    return temp1;
}

class LongDouble { //  Дроби
public:
    LongDouble(const std::string &ch, const std::string &zn) {
        BigInt num1(ch);
        BigInt den1(zn);
        num = num1;
        den = den1;
    }

    BigInt num;
    BigInt den;

    void Print();

    friend LongDouble operator+(LongDouble left, const LongDouble &right) {
        left.num = left.num * right.den + right.num * left.den;
        left.den = left.den * right.den;
        return left;
    }

    friend LongDouble operator-(LongDouble left, const LongDouble &right) {
        left.num = left.num * right.den - right.num * left.den;
        left.den = left.den * right.den;
        return left;
    }

    friend LongDouble operator*(LongDouble left, const LongDouble &right) {
        left.num = left.num * right.num;
        left.den = left.den * right.den;
        return left;
    }

    friend LongDouble operator/(LongDouble left, const LongDouble &right) {
        left.num = left.num * right.den;
        left.den = left.den * right.num;
        return left;
    }
};

void LongDouble::Print() {
    if (num.CheckZero(num)) {
        std::cout << "0" << std::endl;
        return;
    }
    BigInt c;
    c = c.NOD(num, den);
    num = num / c;
    num.Print();
    std::cout << "/" << std::endl;
    den = den / c;
    den.Print();
    std::cout << std::endl;
}

int main() {
    std::string enternum1;
    std::string enternum2;
    std::cin >> enternum1 >> enternum2;
    BigInt num1(enternum1);
    BigInt num2(enternum2);
    num1.Print();
    BigInt sum = num1 + num2;
    sum.Print();
    BigInt z = num1 - num2;
    z.Print();
    BigInt q = num1 * num2;
    q.Print();
    z = num1 / num2;
    z.Print();
    std::string enterden1;
    std::string enterden2;
    std::cin >> enterden1 >> enterden2;
    LongDouble fract1(enternum1, enterden1);
    LongDouble fract2(enternum2, enterden2);
    fract1 = fract1 * fract2;
    fract1.Print();
    fract1 = fract1 / fract2;
    fract1.Print();
    fract1 = fract1 - fract2;
    fract1.Print();
    fract1 = fract1 + fract2;
    fract1.Print();
    return 0;
}
