#include <algorithm>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>
typedef long long ll;
using namespace std;
class Bigint
{
public:
    int id;
    vector<int>num;
    static const int base=100000000;
    static const int width=8;

    Bigint& clean();
    bool operator<(const Bigint &b)const;
    bool operator>(const Bigint &b)const;
    bool operator<=(const Bigint &b)const;
    bool operator>=(const Bigint &b)const;
    bool operator==(const Bigint &b)const;
    bool operator!=(const Bigint &b)const;
    Bigint(int n=0);
    Bigint(string s);
    explicit operator double()const;
    Bigint operator-()const;

    Bigint operator+(const Bigint &b)const;
    Bigint operator-(const Bigint &b)const;
    Bigint operator*(const Bigint &b)const;
    Bigint operator/(const Bigint &b)const;
    Bigint operator%(const Bigint &b)const;

    int cal(const Bigint &a, const Bigint &m)const;

    Bigint& operator+=(const Bigint &b);
    Bigint& operator-=(const Bigint &b);
    Bigint& operator*=(const Bigint &b);
    Bigint& operator/=(const Bigint &b);
    Bigint& operator%=(const Bigint &b);
    string getstr()const;
    friend ostream& operator<<(ostream &out, const Bigint &x);
};
class Data
{
public:
    int id;
    //id=0 MingCheng
    //id=5 None
    bool f;//id=1
    string s;//id=2
    double db;//id=3
    Bigint bg;//id=4

    Data();

    Data toint()const;
    Data tobool()const;
    Data tofloat()const;
    Data tostr()const;
    explicit Data(bool a,int Type_);
    explicit Data(bool n);
    explicit Data(string n);
    explicit Data(double n);
    explicit Data(Bigint n);
    Data operator-()const;
    bool operator<(const Data &b)const;
    bool operator>(const Data &b)const;
    bool operator<=(const Data &b)const;
    bool operator>=(const Data &b)const;
    bool operator==(const Data &b)const;
    bool operator!=(const Data &b)const;

    Data operator+(const Data &b)const;
    Data operator-(const Data &b)const;
    Data operator*(const Data &b)const;
    Data operator/(const Data &b)const;
    Data operator^(const Data &b)const;//ZhengChu
    Data operator%(const Data &b)const;

    Data& operator+=(const Data &b);
    Data& operator-=(const Data &b);
    Data& operator*=(const Data &b);
    Data& operator/=(const Data &b);
    Data& operator^=(const Data &b);
    Data& operator%=(const Data &b);
    friend ostream& operator<<(ostream &out, const Data &x);

};