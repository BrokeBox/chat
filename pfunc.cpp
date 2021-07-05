#include <map>
#include <iostream>
#include <string>

void fooi(int arg) { std::cout << arg << std::endl; }
void food(double arg) { std::cout << arg << std::endl; }
void foos(std::string arg) { std::cout << arg << std::endl; }

template<typename T>
using callback = void (*)(T);

class FuncBase {
    public:
    virtual ~FuncBase(){}
    template<typename T> callback<T> get() const;
    template<typename T> void setFunc(callback<T> rhs);
};

template<typename T>
class Func : public FuncBase {
public:
    ~Func(){ pfunc = nullptr; }
    Func(callback<T> rhs) : pfunc(rhs) {}
    callback<T> get() const { return pfunc; }
    void setFunc(callback<T> rhs) { pfunc = rhs; };

private:
    callback<T> pfunc;
};

template<class T> callback<T> FuncBase::get() const
{ return dynamic_cast<const Func<T>&>(*this).get(); }

template<class T> void FuncBase::setFunc(callback<T> rhs)
{ return dynamic_cast<Func<T>&>(*this).setFunc(rhs); }

int main(void) {
    std::map<int, FuncBase*> pfuncs;

    callback<int> myfun = &fooi;

    Func<int> intfunc(fooi);
    Func<double> dblfunc(food);
    Func<std::string> strfunc(foos);

    pfuncs[1] = &intfunc;
    pfuncs[2] = &dblfunc;
    pfuncs[3] = &strfunc;

    pfuncs[1]->get<int>()(1);
}