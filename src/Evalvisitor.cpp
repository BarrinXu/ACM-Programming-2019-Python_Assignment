//
// Created by barrin on 11/30/19.
//
//#include "bigint.cpp"
//#include "bigint.h"
#include "Evalvisitor.h"
#include <map>
map<string, Data>mp[2010];
map<string, Python3Parser::SuiteContext*>fsuite;
map<string, Python3Parser::ParametersContext*>fpa;
int dep=0;
vector<Data>paid;
void force_decode(string &s,Data &ret)
{
    mp[dep][s]=ret;
}
void decode(string &s,Data &ret)
{
    if(mp[dep].count(s))
        mp[dep][s]=ret;
    else if(mp[0].count(s))
        mp[0][s]=ret;
    else mp[dep][s]=ret;
}
Data decode(const string &s)
{
    if(mp[dep].count(s))
        return mp[dep][s];
    if(mp[0].count(s))
        return mp[0][s];
    cerr<<dep<<' '<<s<<endl;
    cerr<<"decode error"<<endl;
    return Data();
}
antlrcpp::Any EvalVisitor::visitFile_input(Python3Parser::File_inputContext *ctx)
{
    //cerr<<"visitFile_input_st"<<endl;
    for(int i=0; i<(int)ctx->stmt().size(); i++)
    {
        visitStmt(ctx->stmt()[i]);
    }
    return Data(true);
    //cerr<<"visitFile_input_ed"<<endl;
}

antlrcpp::Any EvalVisitor::visitFuncdef(Python3Parser::FuncdefContext *ctx)
{

    string s=ctx->NAME()->toString();
    fpa[s]=ctx->parameters();
    fsuite[s]=ctx->suite();
    return  Data(true);
}

antlrcpp::Any EvalVisitor::visitParameters(Python3Parser::ParametersContext *ctx)
{
    if(ctx->typedargslist()!= nullptr)
        return visitTypedargslist(ctx->typedargslist());
    vector<Data>v;
    return v;
}

antlrcpp::Any EvalVisitor::visitTypedargslist(Python3Parser::TypedargslistContext *ctx)
{
    vector<Data>v;
    int sz1=ctx->tfpdef().size(),sz2=ctx->test().size();
    Data tmp;
    for(int i=0; i<sz1-sz2; i++)
    {
        string s=ctx->tfpdef()[i]->NAME()->toString();
        force_decode(s,tmp);
        Data vip(s);
        vip.id=0;
        v.push_back(vip);
    }
    for(int i=sz1-sz2; i<sz1; i++)
    {
        string s=ctx->tfpdef()[i]->NAME()->toString();
        Data ret=visitTest(ctx->test()[i-(sz1-sz2)]);
        force_decode(s,ret);
        Data vip(s);
        vip.id=0;
        v.push_back(vip);
    }
    return v;
}

/*string getname(string s)
{
    if(fsuite.count(s))
        return s;
    string tmp;
    for(int i=1; i<=4; i++)
    {
        tmp+='0'+dep%10;
        dep/=10;
    }
    reverse(tmp.begin(),tmp.end());
    tmp=s+tmp;
    if(mp.count(tmp))
        return tmp;
    if(!mp.count(s+ori))
    return s;
}*/
antlrcpp::Any EvalVisitor::visitTfpdef(Python3Parser::TfpdefContext *ctx)
{

}

antlrcpp::Any EvalVisitor::visitStmt(Python3Parser::StmtContext *ctx)
{
    //cerr<<"visitStmt_st"<<endl;
    if(ctx->simple_stmt()!= nullptr)
    {
        return visitSimple_stmt(ctx->simple_stmt());
    }
    if(ctx->compound_stmt()!= nullptr)
    {
        return visitCompound_stmt(ctx->compound_stmt());
    }
    return 0;
}

antlrcpp::Any EvalVisitor::visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx)
{
    //cerr<<"visitSimple_stmt_st"<<endl;
    if(ctx->small_stmt()!= nullptr)
    {
        return visitSmall_stmt(ctx->small_stmt());
    }
    return Data(true);
    //cerr<<"visitSimple_stmt_ed"<<endl;
}

antlrcpp::Any EvalVisitor::visitSmall_stmt(Python3Parser::Small_stmtContext *ctx)
{
    //cerr<<"visitSmall_stmt_st"<<endl;
    if(ctx->expr_stmt()!= nullptr)
    {
        return visitExpr_stmt(ctx->expr_stmt());
    }
    if(ctx->flow_stmt()!= nullptr)
    {
        return visitFlow_stmt(ctx->flow_stmt());
    }
    cerr<<"visitSmall_stmt error"<<endl;
}

antlrcpp::Any EvalVisitor::visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx)
{
    //cerr<<"visitExpr_stmt_st"<<endl;
    if(ctx->testlist().size()==1)
        visitTestlist(ctx->testlist()[0]);
    else if(ctx->augassign()!= nullptr)
    {
        vector<Data> id_name=visitTestlist(ctx->testlist()[0]);
        Data ret=decode(id_name[0].s);
        vector<Data>tmp=visitTestlist(ctx->testlist()[1]);
        Data vip=tmp[0];
        if(vip.id==0)
            vip=decode(vip.s);
        int id=visitAugassign(ctx->augassign());
        if(id==1)
            ret+=vip;
        else if(id==2)
            ret-=vip;
        else if(id==3)
            ret*=vip;
        else if(id==4)
            ret/=vip;
        else if(id==5)
            ret^=vip;
        else if(id==6)
            ret%=vip;
        else
            cerr<<"visitExpr_stmt error"<<endl;
        decode(id_name[0].s,ret);
        //return Data(true);
    }
    else
    {
        int sz=ctx->testlist().size();
        vector<Data>lft,rgt=visitTestlist(ctx->testlist()[sz-1]).as<vector<Data> >();
        for(int i=sz-2; i>=0; i--)
        {
            lft=visitTestlist(ctx->testlist()[i]).as<vector<Data> >();
           for(int i=0; i<lft.size(); i++)
           {
               Data a=lft[i],b=rgt[i];
               if(a.id==0)
               {
                   if(b.id==0)
                   {
                       Data ret=decode(b.s);
                       decode(a.s,ret);
                   }
                   else
                       {
                       decode(a.s,b);
                       }
               }
           }
           rgt=lft;
        }
    }
    return Data(true);
}

antlrcpp::Any EvalVisitor::visitAugassign(Python3Parser::AugassignContext *ctx)
{
    if(ctx->ADD_ASSIGN()!= nullptr)
        return 1;
    if(ctx->SUB_ASSIGN()!= nullptr)
        return 2;
    if(ctx->MULT_ASSIGN()!= nullptr)
        return 3;
    if(ctx->DIV_ASSIGN()!= nullptr)
        return 4;
    if(ctx->IDIV_ASSIGN()!= nullptr)
        return 5;
    if(ctx->MOD_ASSIGN()!= nullptr)
        return 6;
    cerr<<"visitAugassign error"<<endl;
    return 0;
}

antlrcpp::Any EvalVisitor::visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx)
{
    if(ctx->break_stmt()!=NULL)
        return Data(false,-1);
    if(ctx->continue_stmt()!=NULL)
        return Data(false,-2);
    if(ctx->return_stmt()!=NULL)
        return visitReturn_stmt(ctx->return_stmt());
    return Data(true);
}

antlrcpp::Any EvalVisitor::visitBreak_stmt(Python3Parser::Break_stmtContext *ctx)
{

}

antlrcpp::Any EvalVisitor::visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx)
{

}

antlrcpp::Any EvalVisitor::visitReturn_stmt(Python3Parser::Return_stmtContext *ctx)
{
    if(ctx->testlist()!= nullptr)
        return visitTestlist(ctx->testlist());
    vector<Data>v;
    return v;
}

antlrcpp::Any EvalVisitor::visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx)
{
    if(ctx->if_stmt()!= nullptr)
    {
        return visitIf_stmt(ctx->if_stmt());
    }
    else if(ctx->while_stmt()!= nullptr)
    {
        return visitWhile_stmt(ctx->while_stmt());
    }
    else if(ctx->funcdef()!= nullptr)
    {
        return visitFuncdef(ctx->funcdef());
    }
    return 0;
}

antlrcpp::Any EvalVisitor::visitIf_stmt(Python3Parser::If_stmtContext *ctx)
{
    Data tmp=visitTest(ctx->test()[0]);
    if(tmp.id==0)
        tmp=decode(tmp.s);
    if(tmp.tobool().f)
        return visitSuite(ctx->suite()[0]);
    for(int i=1; i<(int)ctx->test().size(); i++)
    {
        tmp=visitTest(ctx->test()[i]);
        if(tmp.id==0)
            tmp=decode(tmp.s);
        if(tmp.tobool().f)
            return visitSuite(ctx->suite()[i]);
    }
    if(ctx->suite().size()>ctx->test().size())
        return visitSuite(ctx->suite()[ctx->suite().size()-1]);
    return Data(true);
}

antlrcpp::Any EvalVisitor::visitWhile_stmt(Python3Parser::While_stmtContext *ctx)
{
    while(1)
    {
        Data tmp=visitTest(ctx->test());
        if(tmp.id==0)
            tmp=decode(tmp.s);
        if(!tmp.tobool().f)
            break;
        auto ret=visitSuite(ctx->suite());
        if(ret.is<Data>())
        {
            if(ret.as<Data>().id==-1)
                break;
        }
    }
    return Data(true);
}

antlrcpp::Any EvalVisitor::visitSuite(Python3Parser::SuiteContext *ctx)
{
    if(ctx->simple_stmt()!= nullptr)
    {
        auto tmp=visitSimple_stmt(ctx->simple_stmt());
        if(tmp.is<Data>())
        {
            Data ret=tmp;
            if(ret.id==0)
                ret=decode(ret.s);
            return ret;
        }
        else if(tmp.is<vector<Data> >())
        {
            vector<Data> ret=tmp.as<vector<Data> >();
            for(int i=0; i<(int)ret.size(); i++)
                if(ret[i].id==0)
                    ret[i]=decode(ret[i].s);
            return ret;
        }
        else cerr<<"visitSuite error"<<endl;
    }
    for(int i=0; i<(int)ctx->stmt().size(); i++)
    {
        auto tmp=visitStmt(ctx->stmt()[i]);
        if(tmp.is<Data>())
        {
            if(tmp.as<Data>().id==-2)
                return Data(true);
            if(tmp.as<Data>().id==-1)
                return Data(false,-1);
        }
        else if(tmp.is<vector<Data> >())
        {
            //if(tmp.as<vector<Data> >()[0].id>=0)
            vector<Data> ret=tmp.as<vector<Data> >();
            for(int i=0; i<(int)ret.size(); i++)
                if(ret[i].id==0)
                    ret[i]=decode(ret[i].s);
            return ret;
            /*else if(tmp.as<vector<Data> >()[0].id==-2)
                return Data(true);
            else if(tmp.as<vector<Data> >()[0].id==-1)
                return Data(false,-1);*/
        }
        else cerr<<"visitSuite error"<<endl;
    }
    return Data(true);
}

antlrcpp::Any EvalVisitor::visitTest(Python3Parser::TestContext *ctx)
{
    //cerr<<"visitTest_st"<<endl;
    if(ctx->or_test()!=NULL)
    {
        return visitOr_test(ctx->or_test());
    }
}

antlrcpp::Any EvalVisitor::visitOr_test(Python3Parser::Or_testContext *ctx)
{
    //cerr<<"visitOr_test_st"<<endl;
    if(ctx->and_test().size()==1)
        return visitAnd_test(ctx->and_test()[0]);
    Data tmp=visitAnd_test(ctx->and_test()[0]).as<Data>();
    if(tmp.id==0)
        tmp=decode(tmp.s);
    if(tmp.tobool().f)
        return Data(true);
    for(int i=1; i<(int)ctx->and_test().size(); i++)
    {
        tmp=visitAnd_test(ctx->and_test()[i]).as<Data>();
        if(tmp.id==0)
            tmp=decode(tmp.s);
        if(tmp.tobool().f)
        {
            return Data(true);
        }
    }
    return Data(false);
}

antlrcpp::Any EvalVisitor::visitAnd_test(Python3Parser::And_testContext *ctx)
{
    //cerr<<"visitAnd_test_st"<<endl;
    if(ctx->not_test().size()==1)
        return visitNot_test(ctx->not_test()[0]);
    Data tmp=visitNot_test(ctx->not_test()[0]).as<Data>();
    if(tmp.id==0)
        tmp=decode(tmp.s);
    if(!tmp.tobool().f)
        return Data(false);
    for(int i=1; i<(int)ctx->not_test().size(); i++)
    {
        tmp=visitNot_test(ctx->not_test()[i]).as<Data>();
        if(tmp.id==0)
            tmp=decode(tmp.s);
        if(!tmp.tobool().f)
        {
            return Data(false);
        }
    }
    return Data(true);
}

antlrcpp::Any EvalVisitor::visitNot_test(Python3Parser::Not_testContext *ctx)
{
    //cerr<<"visitNot_test_st"<<endl;
    if(ctx->not_test()!=nullptr)
    {
        Data ret=visitNot_test(ctx->not_test());
        if(ret.id==0)
            ret=decode(ret.s);
        ret=ret.tobool();
        ret.f^=1;
        return ret;
    }
    else
    {
        return visitComparison(ctx->comparison());
    }
}

antlrcpp::Any EvalVisitor::visitComparison(Python3Parser::ComparisonContext *ctx)
{
    //cerr<<"visitComparison_st"<<endl;
    int sz=ctx->arith_expr().size();
    if(sz==1)
        return visitArith_expr(ctx->arith_expr()[0]);
    Data lft=visitArith_expr(ctx->arith_expr()[0]);
    if(lft.id==0)
        lft=decode(lft.s);
    for(int i=1; i<(int)ctx->arith_expr().size(); i++)
    {
        Data rgt=visitArith_expr(ctx->arith_expr()[i]);
        if(rgt.id==0)
            rgt=decode(rgt.s);
        int id=visitComp_op(ctx->comp_op()[i-1]);
        if(id==1)
        {
            if(!(lft<rgt))
                return Data(false);
        }
        else if(id==2)
        {
            if(!(lft>rgt))
                return Data(false);
        }
        else if(id==3)
        {
            if(!(lft==rgt))
                return Data(false);
        }
        else if(id==4)
        {
            if(!(lft>=rgt))
                return Data(false);
        }
        else if(id==5)
        {
            if(!(lft<=rgt))
                return Data(false);
        }
        else if(id==6)
        {
            if(!(lft!=rgt))
                return Data(false);
        }
        lft=rgt;
    }
    return Data(true);
}

antlrcpp::Any EvalVisitor::visitComp_op(Python3Parser::Comp_opContext *ctx)
{
    if(ctx->LESS_THAN()!= nullptr)
        return 1;
    if(ctx->GREATER_THAN()!= nullptr)
        return 2;
    if(ctx->EQUALS()!= nullptr)
        return 3;
    if(ctx->GT_EQ()!= nullptr)
        return 4;
    if(ctx->LT_EQ()!= nullptr)
        return 5;
    if(ctx->NOT_EQ_2()!= nullptr)
        return 6;
    cerr<<"visitComp_op error"<<endl;
    return 0;
}

antlrcpp::Any EvalVisitor::visitArith_expr(Python3Parser::Arith_exprContext *ctx)
{
    if(ctx->term().size()==1)
        return visitTerm(ctx->term()[0]);
    vector<pair<int,int> >opt;
    for(int i=0; i<(int)ctx->ADD().size(); i++)
        opt.push_back(std::make_pair(ctx->ADD()[i]->getSymbol()->getTokenIndex(),1));
    for(int i=0; i<(int)ctx->MINUS().size(); i++)
        opt.push_back(std::make_pair(ctx->MINUS()[i]->getSymbol()->getTokenIndex(),2));
    std::sort(opt.begin(),opt.end());
    Data ret=visitTerm(ctx->term()[0]);
    if(ret.id==0)
        ret=decode(ret.s);
    for(int i=1; i<(int)ctx->term().size(); i++)
    {
        auto vipp=visitTerm(ctx->term()[i]);
        Data tmp;
        if(vipp.is<vector<Data> >())
            tmp=vipp.as<vector<Data> >()[0];
        else
            tmp=vipp;
        if(tmp.id==0)
            tmp=decode(tmp.s);
        if(opt[i-1].second==1)
                ret+=tmp;
        else if(opt[i-1].second==2)
                ret-=tmp;
    }
    return ret;
}

antlrcpp::Any EvalVisitor::visitTerm(Python3Parser::TermContext *ctx)
{
    if(ctx->factor().size()==1)
        return visitFactor(ctx->factor()[0]);
    vector<pair<int,int> >opt;
    for(int i=0; i<(int)ctx->STAR().size(); i++)
        opt.push_back(std::make_pair(ctx->STAR()[i]->getSymbol()->getTokenIndex(),1));
    for(int i=0; i<(int)ctx->DIV().size(); i++)
        opt.push_back(std::make_pair(ctx->DIV()[i]->getSymbol()->getTokenIndex(),2));
    for(int i=0; i<(int)ctx->IDIV().size(); i++)
        opt.push_back(std::make_pair(ctx->IDIV()[i]->getSymbol()->getTokenIndex(),3));
    for(int i=0; i<(int)ctx->MOD().size(); i++)
        opt.push_back(std::make_pair(ctx->MOD()[i]->getSymbol()->getTokenIndex(),4));
    std::sort(opt.begin(),opt.end());
    Data ret=visitFactor(ctx->factor()[0]);
    if(ret.id==0)
        ret=decode(ret.s);
    for(int i=1; i<(int)ctx->factor().size(); i++)
    {
        Data tmp=visitFactor(ctx->factor()[i]);
        if(tmp.id==0)
            tmp=decode(tmp.s);
        if(opt[i-1].second==1)
            ret*=tmp;
        else if(opt[i-1].second==2)
            ret/=tmp;
        else if(opt[i-1].second==3)
            ret^=tmp;
        else if(opt[i-1].second==4)
            ret%=tmp;
    }
    return ret;
}

antlrcpp::Any EvalVisitor::visitFactor(Python3Parser::FactorContext *ctx)
{
    if(ctx->ADD()!=nullptr||ctx->MINUS()!=nullptr)
    {
        auto tmp=visitFactor(ctx->factor());
        Data ret;
        if(tmp.is<Data>())
            ret=tmp;
        else ret=tmp.as<vector<Data> >()[0];
        if(ret.id==0)
            ret=decode(ret.s);
        if(ctx->MINUS()!=nullptr)
            ret=-ret;
        return ret;
    }
    return visitAtom_expr(ctx->atom_expr());
}

antlrcpp::Any EvalVisitor::visitAtom_expr(Python3Parser::Atom_exprContext *ctx)
{
    if(ctx->trailer()== nullptr)
        return visitAtom(ctx->atom());
    string s=visitAtom(ctx->atom()).as<Data>().s;
    if(s=="print")
    {
        vector<Data>v=visitTrailer(ctx->trailer()).as<vector<Data> >();
        for(int i=0; i<(int)v.size(); i++)
        {
            if(v[i].id==0)
                cout<<decode(v[i].s)<<' ';
            else
                cout<<v[i]<<' ';
        }
        cout<<endl;
        return Data(true);
    }
    else if(s=="int")
    {
        vector<Data>v=visitTrailer(ctx->trailer()).as<vector<Data> >();
        for(int i=0; i<(int)v.size(); i++)
        {
            if(v[i].id==0)
                v[i]=decode(v[i].s).toint();
            else
                v[i]=v[i].toint();
        }
        return v;
    }
    else if(s=="float")
    {
        vector<Data>v=visitTrailer(ctx->trailer()).as<vector<Data> >();
        for(int i=0; i<(int)v.size(); i++)
        {
            if(v[i].id==0)
                v[i]=decode(v[i].s).tofloat();
            else
                v[i]=v[i].tofloat();
        }
        return v;
    }
    else if(s=="str")
    {
        vector<Data>v=visitTrailer(ctx->trailer()).as<vector<Data> >();
        for(int i=0; i<(int)v.size(); i++)
        {
            if(v[i].id==0)
                v[i]=decode(v[i].s).tostr();
            else
                v[i]=v[i].tostr();
        }
        return v;
    }
    else if(s=="bool")
    {
        vector<Data>v=visitTrailer(ctx->trailer()).as<vector<Data> >();
        for(int i=0; i<(int)v.size(); i++)
        {
            if(v[i].id==0)
                v[i]=decode(v[i].s).tobool();
            else
                v[i]=v[i].tobool();
        }
        return v;
    }

    dep++;
    vector<Data>list1=visitParameters(fpa[s]);
    dep--;
    vector<Data>v=visitTrailer(ctx->trailer());
    for(int i=0; i<(int)v.size(); i++)
    {
        if(v[i].id==-100)
            break;
        if(v[i].id==0)
            v[i]=decode(v[i].s);
        dep++;
        force_decode(list1[i].s,v[i]);
        dep--;
    }
    dep++;
    auto ret=visitSuite(fsuite[s]);
    if(ret.is<vector<Data> >()&&ret.as<vector<Data> >().size()==1)
        ret=ret.as<vector<Data> >()[0];
    mp[dep].clear();
    dep--;
    return ret;
    //cerr<<"visitAtom_expr_ed"<<endl;
}

antlrcpp::Any EvalVisitor::visitTrailer(Python3Parser::TrailerContext *ctx)
{
    if(ctx->arglist()!= nullptr)
        return visitArglist(ctx->arglist());
    vector<Data>v;
    return v;
    //cerr<<"No Arglist"<<endl;

}
Data get_num(const string &b)
{
    Data ret;
    bool is_db=false;
    for(int i=0; i<(int)b.size(); i++)
        if(b[i]=='.')
        {
            is_db=true;
            break;
        }
    if(is_db)
    {
        ret.id=3;
        sscanf(b.c_str(),"%lf",&ret.db);
    }
    else
    {
        ret.id=4;
        ret.bg=b;
    }
    return ret;
}
antlrcpp::Any EvalVisitor::visitAtom(Python3Parser::AtomContext *ctx)
{

    //cerr<<"visitAtom"<<endl;
    if(ctx->NAME()!=nullptr)
    {
        Data ret(ctx->NAME()->toString());
        ret.id=0;
        return ret;
    }
    if(ctx->NUMBER()!=nullptr)
    {
        Data ret=get_num(ctx->NUMBER()->toString());
        return ret;
    }
    if(!ctx->STRING().empty())
    {
        string s;
        for(int i=0; i<(int)ctx->STRING().size(); i++)
            s+=ctx->STRING()[i]->toString();
        s=s.substr(1,s.size()-2);
        Data ret(s);
        return ret;
    }
    if(ctx->NONE()!=nullptr)
    {
        Data ret;
        ret.id=5;
        return ret;
    }
    if(ctx->TRUE()!=nullptr)
    {
        //return true;
        Data ret;
        ret.id=1,ret.f=true;
        return ret;
    }
    if(ctx->FALSE()!=nullptr)
    {
        //return false;
        Data ret;
        ret.id=1,ret.f=false;
        return ret;
    }
    if(ctx->test()!= nullptr)
        return visitTest(ctx->test());
}

antlrcpp::Any EvalVisitor::visitTestlist(Python3Parser::TestlistContext *ctx)
{
    //cerr<<"visitTestlist_st"<<endl;
    vector<Data>v;
    for(int i=0; i<(int)ctx->test().size(); i++)
    {
        auto tmp=visitTest(ctx->test()[i]);
        if(tmp.is<Data>())
            v.push_back(tmp);
        else if(tmp.is<vector<Data> >())
        {
            vector<Data>tt=tmp;
            for(int j=0; j<(int)tt.size(); j++)
                v.push_back(tt[j]);
        }
        else cerr<<"visitTestlist error"<<endl;
    }
    return v;
}

antlrcpp::Any EvalVisitor::visitArglist(Python3Parser::ArglistContext *ctx)
{
    vector<Data>v;
    for(int i=0; i<(int)ctx->argument().size(); i++)
    {
        auto tmp=visitArgument(ctx->argument()[i]);
        if(tmp.is<Data>())
            v.push_back(tmp);
        else if(tmp.is<vector<Data> >())
        {
            vector<Data>tt=tmp;
            for(int j=0; j<(int)tt.size(); j++)
                v.push_back(tt[j]);
        }
        else cerr<<"visitArglist error"<<endl;
    }
    return v;
}

antlrcpp::Any EvalVisitor::visitArgument(Python3Parser::ArgumentContext *ctx)
{
    if(ctx->NAME()== nullptr)
    {
        return visitTest(ctx->test());
    }
    else
    {
        Data tmp=visitTest(ctx->test());
        string s=ctx->NAME()->toString();
        dep++;
        force_decode(s,tmp);
        dep--;
        Data ret(s);
        ret.id=-100;
        return ret;
    }
}
