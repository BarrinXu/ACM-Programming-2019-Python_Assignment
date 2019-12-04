#include <bigint.h>
Bigint& Bigint::clean()
{
    while(num.size()>1&&num.back()==0)
        num.pop_back();
    if(num.size()==1&&num.front()==0)
        id=1;
    return *this;
}
bool Bigint::operator<(const Bigint &b)const
{
    if(id!=b.id)
        return id<b.id;
    if(num.size()!=b.num.size())
        return num.size()*id<b.num.size()*b.id;
    for(int i=(int)num.size()-1; i>=0; i--)
        if(num[i]!=b.num[i])
            return num[i]*id<b.num[i]*b.id;
    return false;
}
bool Bigint::operator>(const Bigint &b)const
{
    return b<*this;
}
bool Bigint::operator<=(const Bigint &b)const
{
    return !(b<*this);
}
bool Bigint::operator>=(const Bigint &b)const
{
    return !(*this<b);
}
bool Bigint::operator==(const Bigint &b)const
{
    if(id!=b.id||num.size()!=b.num.size())
        return false;
    for(int i=(int)num.size()-1; i>=0; i--)
        if(num[i]!=b.num[i])
            return false;
    return true;
}
bool Bigint::operator!=(const Bigint &b)const
{
    return !(*this==b);
}
 Bigint::Bigint(int n)
{
    id=1;
    num.clear();
    do
    {
        num.push_back(n%base);
        n/=base;
    }
    while(n>0);
}
Bigint::Bigint(string s)
{
    id=1;
    num.clear();
    int ret,len=((int)s.size()-1)/width+1;
    for(int i=0; i<len; i++)
    {
        int ed=(int)s.size()-i*width;
        int st=max(0,ed-width);
        sscanf(s.substr(st,ed-st).c_str(),"%d",&ret);
        num.push_back(ret);
    }
}
Bigint::operator double() const
{
    double ret=num.back();
    for(int i=(int)num.size()-2; i>=0; i--)
        ret=ret*base+num[i];
    return ret;
}
Bigint Bigint::operator-()const
{
    Bigint ret=*this;
    ret.id=-id;
    return ret;
}
Bigint Bigint::operator+(const Bigint &b)const
{
    if(id!=b.id)
    {
        if(id==1)
            return *this-(-b);
        else
            return b-(-*this);
    }
    Bigint ret;
    ret.id=id,ret.num.clear();
    int tmp=0;
    for(int i=0; tmp||i<(int)num.size()||i<(int)b.num.size(); i++)
    {
        if(i<(int)num.size())
            tmp+=num[i];
        if(i<(int)b.num.size())
            tmp+=b.num[i];
        ret.num.push_back(tmp%base);
        tmp=tmp/base;
    }
    return ret.clean();
}
Bigint Bigint::operator-(const Bigint &b)const
{
    if(id!=b.id)
    {
        if(id==1)
            return *this+(-b);
        else
            return -((-*this)+b);
    }
    if(id==1&&*this<b)
        return -(b-*this);
    if(id==-1&&*this>=b)
        return (-b)-(-*this);
    Bigint ret;
    ret.id=id,ret.num.clear();
    int tmp=0;
    for(int i=0; tmp||i<(int)num.size()||i<(int)b.num.size(); i++)
    {
        tmp+=num[i];
        if(i<(int)b.num.size())
            tmp-=b.num[i];
        if(tmp<0)
        {
            ret.num.push_back(tmp+base);
            tmp=-1;
        }
        else
        {
            ret.num.push_back(tmp);
            tmp=0;
        }
    }
    return ret.clean();
}
Bigint Bigint::operator*(const Bigint &b)const
{
    vector<int>c(num.size()+b.num.size(),0);
    Bigint ret;
    ret.id=id*b.id;
    for(int i=0; i<(int)num.size(); i++)
        for(int j=0; j<(int)b.num.size(); j++)
        {
            ll tmp=c[i+j]+(ll)num[i]*b.num[j];
            c[i+j+1]+=tmp/base;
            c[i+j]=tmp%base;
        }
    ret.num=c;
    return ret.clean();
}
Bigint Bigint::operator/(const Bigint &b)const
{
    Bigint ret=0,tmp=0;
    Bigint nb=b;
    nb.id=tmp.id=1;
    ret.num.clear();
    ret.num.resize(num.size());
    for(int i=(int)num.size()-1; i>=0; i--)
    {
        tmp=tmp*base;
        tmp=tmp+num[i];
        ret.num[i]=cal(nb,tmp);
        tmp-=nb*ret.num[i];
    }
    ret.clean();
    ret.id=id*b.id;
    if(ret.id==-1&&tmp!=0)
        ret-=1;
    return ret.clean();
}
Bigint Bigint::operator%(const Bigint &b)const
{
    return *this-(*this/b)*b;
}
int Bigint::cal(const Bigint &a, const Bigint &m)const
{
    // search for ax<=m maximize x
    int ret=0;
    int l=0,r=base-1;
    while(l<=r)
    {
        int mid=(l+r)/2;
        if(a*mid<=m)
            ret=mid,l=mid+1;
        else
            r=mid-1;
    }
    return ret;
}
Bigint& Bigint::operator+=(const Bigint &b)
{
    *this=*this+b;
    return *this;
}
Bigint& Bigint::operator-=(const Bigint &b)
{
    *this=*this-b;
    return *this;
}
Bigint& Bigint::operator*=(const Bigint &b)
{
    *this=*this*b;
    return *this;
}
Bigint& Bigint::operator/=(const Bigint &b)
{
    *this=*this/b;
    return *this;
}
Bigint& Bigint::operator%=(const Bigint &b)
{
    *this=*this%b;
    return *this;
}
ostream& operator<<(ostream &out, const Bigint &x)
{
    if(x.id==-1)
        putchar('-');
    out<<x.num.back();
    for(int i=(int)x.num.size()-2; i>=0; i--)
    {
        int s[8],tmp=x.num[i];
        for(int j=0; j<8; j++)
        {
            s[j]=tmp%10;
            tmp/=10;
        }
        for(int j=7; j>=0; j--)
            out<<s[j];
    }
    return out;
}

string Bigint::getstr() const
{
    string ret;
    if(id==-1)
        ret+='-';
    ret+=to_string(num.back());
    for(int i=(int)num.size()-2; i>=0; i--)
    {
        int s[8],tmp=num[i];
        for(int j=0; j<8; j++)
        {
            s[j]=tmp%10;
            tmp/=10;
        }
        for(int j=7; j>=0; j--)
            ret+='0'+s[j];
    }
    return ret;
}

Data::Data(bool n)
{
    id=1;
    f=n;
}
Data::Data(string n)
{
    id=2;
    s=n;
}
Data::Data(double n)
{
    id=3;
    db=n;
}
Data::Data(Bigint n)
{
    id=4;
    bg=n;
}

Data Data::operator-() const
{
    Data ret=*this;
    if(ret.id==1)
    {
        ret.id=4;
        ret.bg=-ret.f;
    }
    else if(ret.id==3)
        ret.db=-ret.db;
    else if(ret.id==4)
        ret.bg=-ret.bg;
    return ret;
}

bool Data::operator<(const Data &b)const
{
    if(id==2)
        return s<b.s;
    if(id==3||b.id==3)
    {
        double x,y;
        if(id==1)
            x=f;
        if(id==3)
            x=db;
        if(id==4)
            x=(double)bg;
        if(b.id==1)
            y=b.f;
        if(b.id==3)
            y=b.db;
        if(b.id==4)
            y=(double)b.bg;
        return x<y;
    }
    Bigint x,y;
    if(id==1)
        x=f;
    if(id==4)
        x=bg;
    if(b.id==1)
        y=b.f;
    if(b.id==4)
        y=b.bg;
    return x<y;
}
bool Data::operator>(const Data &b)const
{
    return b<*this;
}
bool Data::operator<=(const Data &b)const
{
    return !(b<*this);
}
bool Data::operator>=(const Data &b)const
{
    return !(*this<b);
}
bool Data::operator==(const Data &b)const
{
    if(id==2)
        return s==b.s;
    if(id==3||b.id==3)
    {
        double x,y;
        if(id==1)
            x=f;
        if(id==3)
            x=db;
        if(id==4)
            x=(double)bg;
        if(b.id==1)
            y=b.f;
        if(b.id==3)
            y=b.db;
        if(b.id==4)
            y=(double)b.bg;
        return x==y;
    }
    if(id==5||b.id==5)
    {
        return id==b.id;
    }
    Bigint x,y;
    if(id==1)
        x=f;
    if(id==4)
        x=bg;
    if(b.id==1)
        y=b.f;
    if(b.id==4)
        y=b.bg;
    return x==y;
}
bool Data::operator!=(const Data &b)const
{
    return !(*this==b);
}

Data Data::operator+(const Data &b) const
{
    if(id==3||b.id==3)
    {
        double x,y;
        if(id==1)
            x=f;
        if(id==3)
            x=db;
        if(id==4)
            x=(double)bg*bg.id;
        if(b.id==1)
            y=b.f;
        if(b.id==3)
            y=b.db;
        if(b.id==4)
            y=(double)b.bg*b.bg.id;
        return Data(x+y);
    }
    if(id==2&&b.id==2)
        return Data(s+b.s);
    Bigint x,y;
    if(id==1)
        x=f;
    if(id==4)
        x=bg;
    if(b.id==1)
        y=b.f;
    if(b.id==4)
        y=b.bg;
    return Data(x+y);
}

Data Data::operator-(const Data &b) const
{
    if(id==3||b.id==3)
    {
        double x,y;
        if(id==1)
            x=f;
        if(id==3)
            x=db;
        if(id==4)
            x=(double)bg*bg.id;
        if(b.id==1)
            y=b.f;
        if(b.id==3)
            y=b.db;
        if(b.id==4)
            y=(double)b.bg*b.bg.id;
        return Data(x-y);
    }
    Bigint x,y;
    if(id==1)
        x=f;
    if(id==4)
        x=bg;
    if(b.id==1)
        y=b.f;
    if(b.id==4)
        y=b.bg;
    return Data(x-y);
}

Data Data::operator*(const Data &b) const
{
    if(id==3||b.id==3)
    {
        double x,y;
        if(id==1)
            x=f;
        if(id==3)
            x=db;
        if(id==4)
            x=(double)bg*bg.id;
        if(b.id==1)
            y=b.f;
        if(b.id==3)
            y=b.db;
        if(b.id==4)
            y=(double)b.bg*b.bg.id;
        return Data(x*y);
    }
    if(id==2)
    {
        string x;
        Bigint y;
        if(b.id==1)
            y=b.f;
        if(b.id==4)
            y=b.bg;
        //cerr<<y<<endl;
        while(true)
        {
            x+=s;
            y-=1;
            if(y==0)
                break;
        }
        return Data(x);
    }
    if(b.id==2)
        return b*(*this);
    Bigint x,y;
    if(id==1)
        x=f;
    if(id==4)
        x=bg;
    if(b.id==1)
        y=b.f;
    if(b.id==4)
        y=b.bg;
    return Data(x*y);
}

Data Data::operator/(const Data &b) const
{
    double x,y;
    if(id==1)
        x=f;
    if(id==3)
        x=db;
    if(id==4)
        x=(double)bg*bg.id;
    if(b.id==1)
        y=b.f;
    if(b.id==3)
        y=b.db;
    if(b.id==4)
        y=(double)b.bg*b.bg.id;
    return Data(x/y);
}

Data Data::operator^(const Data &b) const
{
    Bigint x,y;
    if(id==1)
        x=f;
    if(id==4)
        x=bg;
    if(b.id==1)
        y=b.f;
    if(b.id==4)
        y=b.bg;
    return Data(x/y);
}

Data Data::operator%(const Data &b) const
{
    //cerr<<*this<<' '<<b<<endl;
    Bigint x,y;
    if(id==1)
        x=f;
    if(id==4)
        x=bg;
    if(b.id==1)
        y=b.f;
    if(b.id==4)
        y=b.bg;
    return Data(x%y);
}
Data& Data::operator+=(const Data &b)
{
    *this=*this+b;
    return *this;
}
Data& Data::operator-=(const Data &b)
{
    *this=*this-b;
    return *this;
}
Data& Data::operator*=(const Data &b)
{
    *this=*this*b;
    return *this;
}
Data& Data::operator/=(const Data &b)
{
    *this=*this/b;
    return *this;
}
Data &Data::operator^=(const Data &b)
{
    *this=*this^b;
    return *this;
}

Data& Data::operator%=(const Data &b)
{
    *this=*this%b;
    return *this;
}

ostream &operator<<(ostream &out, const Data &x)
{
    if(x.id==1)
    {
        if(x.f)
            printf("True");
        else
            printf("False");
    }
    else if(x.id==2)
        cout<<x.s;
    else if(x.id==3)
        printf("%.6f",x.db);
    else if(x.id==4)
        cout<<x.bg;
    else if(x.id==5)
        printf("None");
    return out;
}

Data::Data() {
    id=1,f=true;
}

Data::Data(bool a, int Type_) {
    id=Type_,f=a;
}

Data Data::tobool() const
{
    if(id==1)
        return *this;
    if(id==2)
        return Data(!s.empty());
    if(id==3)
        return Data(db!=0);
    if(id==4)
        return Data(bg!=0);
    cerr<<"Data to bool error"<<endl;
}

Data Data::toint() const
{
    Data ret;
    ret.id=4;
    if(id==1)
        ret.bg=f;
    else if(id==2)
    {
        string tmp=s;
        ret.bg=s;
    }
    else if(id==3)
    {
        string tmp=std::to_string(db);
        tmp=tmp.substr(0,tmp.size()-7);
        ret.bg=tmp;
    }
    else if(id==4)
        ret=*this;
    else cerr<<"change into int error"<<endl;
    return ret;
}

Data Data::tofloat() const
{
    Data ret;
    ret.id=3;
    if(id==1)
        ret.db=f;
    else if(id==4)
        ret.db=(double)bg;
    else cerr<<"change into float error"<<endl;
    return ret;
}

Data Data::tostr() const
{
    Data ret;
    ret.id=2;
    if(id==1)
    {
        if(f)
            ret.s="True";
        else
            ret.s="False";
    }
    else if(id==4)
        ret.s=bg.getstr();
    else
        cerr<<"tostr error"<<endl;
    return ret;
}


