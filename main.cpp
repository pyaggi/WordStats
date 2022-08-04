#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <bit>
#include <tuple>
#include <chrono>
#include <mutex>
std::mutex pplock;
std::vector<std::string> words;
std::vector<uint32_t> signatures;
uint32_t signature(const std::string &str)
{
    uint32_t ret=0;
    for (auto &c:str)
        ret|=(1<<(c-'a'));
    return ret;
}
void reportTest(const char *algoname,const std::chrono::system_clock::time_point &start,const std::chrono::system_clock::time_point &start1,std::size_t found)
{
    auto end = std::chrono::system_clock::now();
    std::cout << "*** Algorithm "<<algoname<<" finished"<<std::endl;
    std::cout << "Finding combinations elapsed time:  "<< std::chrono::duration<double>(end-start1).count() << "s" << std::endl;
    std::cout << "Total elapsed time:  "<< std::chrono::duration<double>(end-start).count() << "s" << std::endl;
    std::cout<<"5x5 Combinations found:"<<found<<std::endl;


}
void algo_allCheck(std::ofstream &out,const std::chrono::system_clock::time_point &start)
{
    auto wc5d=words.size();
    std::vector<std::tuple<int,int,int,int,int>> found;
    auto start1 = std::chrono::system_clock::now();

    auto sigPtr0=signatures.data();
    for (auto c0=0;c0<wc5d;c0++)
    {
        auto sig0=*sigPtr0;
        sigPtr0++;
        auto sigPtr1=sigPtr0;
        for (auto c1=c0+1;c1<wc5d;c1++)
        {
            auto sig1=sig0|*sigPtr1;
            sigPtr1++;
            if (std::popcount(sig1)!=10)
                continue;
            auto sigPtr2=sigPtr1;
            for (auto c2=c1+1;c2<wc5d;c2++)
            {
                auto sig2=sig1|*sigPtr2;
                sigPtr2++;
                if (std::popcount(sig2)!=15)
                    continue;
                auto sigPtr3=sigPtr2;
                for (auto c3=c2+1;c3<wc5d;c3++)
                {
                    auto sig3=sig2|*sigPtr3;
                    sigPtr3++;
                    if (std::popcount(sig3)!=20)
                        continue;
                    auto sigPtr4=sigPtr3;
                    for (auto c4=c3+1;c4<wc5d;c4++,sigPtr4++)
                    {
                        auto sig4=sig3|*sigPtr4;
                        if (std::popcount(sig4)!=25)
                            continue;
                        found.push_back({c0,c1,c2,c3,c4});
                    }
                }
            }
        }
//        std::cout<<"*"<<std::flush;
    }
    std::cout<<std::endl;
    reportTest("Check all",start,start1,found.size());

    if (out.is_open())
    {
        for (auto &f:found)
            out<<words[std::get<0>(f)]<<" "<<words[std::get<1>(f)]<<" "<<words[std::get<2>(f)]<<" "<<words[std::get<3>(f)]<<" "<<words[std::get<4>(f)]<<std::endl;
    }

}
void algo_allCheck_par(std::ofstream &out,const std::chrono::system_clock::time_point &start)
{
    auto wc5d=words.size();
    auto start1 = std::chrono::system_clock::now();

    std::vector<std::tuple<int,int,int,int,int>> found;
    #pragma omp parallel
    {
        std::vector<std::tuple<int,int,int,int,int>> foundpp;
        #pragma omp for
        for (auto c0=0;c0<wc5d;c0++)
        {
            auto sigPtr0=&signatures[c0];
            auto sig0=*sigPtr0;
            sigPtr0++;
            auto sigPtr1=sigPtr0;
            for (auto c1=c0+1;c1<wc5d;c1++)
            {
                auto sig1=sig0|*sigPtr1;
                sigPtr1++;
                if (std::popcount(sig1)!=10)
                    continue;
                auto sigPtr2=sigPtr1;
                for (auto c2=c1+1;c2<wc5d;c2++)
                {
                    auto sig2=sig1|*sigPtr2;
                    sigPtr2++;
                    if (std::popcount(sig2)!=15)
                        continue;
                    auto sigPtr3=sigPtr2;
                    for (auto c3=c2+1;c3<wc5d;c3++)
                    {
                        auto sig3=sig2|*sigPtr3;
                        sigPtr3++;
                        if (std::popcount(sig3)!=20)
                            continue;
                        auto sigPtr4=sigPtr3;
                        for (auto c4=c3+1;c4<wc5d;c4++,sigPtr4++)
                        {
                            auto sig4=sig3|*sigPtr4;
                            if (std::popcount(sig4)!=25)
                                continue;
                            foundpp.push_back({c0,c1,c2,c3,c4});
                        }
                    }
                }
            }
//          #pragma master
//          std::cout<<"*"<<std::flush;
        }
        #pragma omp critical
        {
            found.insert(found.end(),foundpp.begin(),foundpp.end());
        }
    }
    std::cout<<std::endl;
    reportTest("Check all parallel",start,start1,found.size());

    if (out.is_open())
    {
        for (auto &f:found)
            out<<words[std::get<0>(f)]<<" "<<words[std::get<1>(f)]<<" "<<words[std::get<2>(f)]<<" "<<words[std::get<3>(f)]<<" "<<words[std::get<4>(f)]<<std::endl;
    }

}
void processList(std::ifstream &in,const std::chrono::system_clock::time_point &start)
{
    int wc=0,wc5=0;
    while (!in.eof())
    {
        std::string word;
        in>>word;
        wc++;
        if (word.size()!=5)
            continue;
        wc5++;
        auto sig=signature(word);
        if (std::popcount(sig)!=5)
            continue; //not all letters different
        signatures.push_back(sig);
        words.emplace_back(std::move(word));
    }
    auto end = std::chrono::system_clock::now();
    std::cout << "Processing list elapsed time:  "<< std::chrono::duration<double>(end-start).count() << "s" << std::endl;

    std::cout<<"Word count:"<<wc<<std::endl;
    std::cout<<"5 Letter Word count:"<<wc5<<std::endl;
    std::cout<<"5 Disinct Letter Word count:"<<words.size()<<std::endl;
}
struct GraphNode
{
    uint32_t sig;
    int word;
    int neibCount;
    int neibIdx;
};

void algo_graph(std::ofstream &out,const std::chrono::system_clock::time_point &start)
{
    std::vector<GraphNode> nodes;
    std::vector<int> neibs;


    auto wc5d=words.size();
    uint32_t *sigPtr0=signatures.data();
    uint32_t *sigPtr1;
    uint32_t sig0,sig1,sig2,sig3,sig4;
    std::size_t cntptr=0;
    auto start1 = std::chrono::system_clock::now();
    GraphNode nnode;
    for (int c0=0;c0<wc5d;c0++)
    {
        sig0=*sigPtr0;
        sigPtr0++;
        sigPtr1=sigPtr0;
        nnode.word=c0;
        nnode.neibCount=0;
        nnode.neibIdx=neibs.size();
        nnode.sig=sig0;
        for (int c1=c0+1;c1<wc5d;c1++,sigPtr1++)
        {
            sig1=sig0|*sigPtr1;
            if (std::popcount(sig1)==10)
            {
                nnode.neibCount++;
                neibs.push_back(c1);
            }
        }
        nodes.push_back(nnode);
    }

    std::vector<std::tuple<int,int,int,int,int>> found;
    for (auto &n:nodes)
    {
        sig0=n.sig;
        for (int c0=0,idx0=n.neibIdx;c0<n.neibCount;c0++,++idx0)
        {
            auto &n1=nodes[neibs[idx0]];
            sig1=sig0|n1.sig;
            for (int c1=0,idx1=n1.neibIdx;c1<n1.neibCount;c1++,++idx1)
            {
                auto &n2=nodes[neibs[idx1]];
                sig2=sig1|n2.sig;
                if (std::popcount(sig2)!=15)
                    continue;
                for (int c2=0,idx2=n2.neibIdx;c2<n2.neibCount;c2++,++idx2)
                {
                    auto &n3=nodes[neibs[idx2]];
                    sig3=sig2|n3.sig;
                    if (std::popcount(sig3)!=20)
                        continue;
                    for (int c3=0,idx3=n3.neibIdx;c3<n3.neibCount;c3++,++idx3)
                    {
                        auto &n4=nodes[neibs[idx3]];
                        sig4=sig3|n4.sig;
                        if (std::popcount(sig4)!=25)
                            continue;
                        found.push_back({n.word,n1.word,n2.word,n3.word,neibs[idx3]});
                    }
                }
            }
        }
//        std::cout<<"*"<<std::flush;
    }
    std::cout<<std::endl;
    reportTest("Graph",start,start1,found.size());
    if (out.is_open())
    {
        for (auto &f:found)
            out<<words[std::get<0>(f)]<<" "<<words[std::get<1>(f)]<<" "<<words[std::get<2>(f)]<<" "<<words[std::get<3>(f)]<<" "<<words[std::get<4>(f)]<<std::endl;
    }

}
void algo_graph_par(std::ofstream &out,const std::chrono::system_clock::time_point &start)
{
    std::vector<GraphNode> nodes;
    std::vector<int> neibs;


    auto wc5d=words.size();
    uint32_t *sigPtr0=signatures.data();
    uint32_t *sigPtr1;
    uint32_t sig0,sig1;
    std::size_t cntptr=0;
    auto start1 = std::chrono::system_clock::now();
    GraphNode nnode;
    for (int c0=0;c0<wc5d;c0++)
    {
        sig0=*sigPtr0;
        sigPtr0++;
        sigPtr1=sigPtr0;
        nnode.word=c0;
        nnode.neibCount=0;
        nnode.neibIdx=neibs.size();
        nnode.sig=sig0;
        for (int c1=c0+1;c1<wc5d;c1++,sigPtr1++)
        {
            sig1=sig0|*sigPtr1;
            if (std::popcount(sig1)==10)
            {
                nnode.neibCount++;
                neibs.push_back(c1);
            }
        }
        nodes.push_back(nnode);
    }
    std::vector<std::tuple<int,int,int,int,int>> found;
    #pragma omp parallel
    {
        std::vector<std::tuple<int,int,int,int,int>> foundpp;
        #pragma omp for
        for (int nc=0;nc<wc5d;nc++)
        {
            auto &n=nodes[nc];
            auto sig0=n.sig;
            for (int c0=0,idx0=n.neibIdx;c0<n.neibCount;c0++,++idx0)
            {
                auto &n1=nodes[neibs[idx0]];
                auto sig1=sig0|n1.sig;
                for (int c1=0,idx1=n1.neibIdx;c1<n1.neibCount;c1++,++idx1)
                {
                    auto &n2=nodes[neibs[idx1]];
                    auto sig2=sig1|n2.sig;
                    if (std::popcount(sig2)!=15)
                        continue;
                    for (int c2=0,idx2=n2.neibIdx;c2<n2.neibCount;c2++,++idx2)
                    {
                        auto &n3=nodes[neibs[idx2]];
                        auto sig3=sig2|n3.sig;
                        if (std::popcount(sig3)!=20)
                            continue;
                        for (int c3=0,idx3=n3.neibIdx;c3<n3.neibCount;c3++,++idx3)
                        {
                            auto &n4=nodes[neibs[idx3]];
                            auto sig4=sig3|n4.sig;
                            if (std::popcount(sig4)!=25)
                                continue;
                            foundpp.push_back({n.word,n1.word,n2.word,n3.word,neibs[idx3]});
                        }
                    }
                }
            }
//            #pragma master
//            std::cout<<"*"<<std::flush;
        }
        #pragma omp critical
        {
            found.insert(found.end(),foundpp.begin(),foundpp.end());
        }
    }
    std::cout<<std::endl;
    reportTest("Graph parallel",start,start1,found.size());
    if (out.is_open())
    {
        for (auto &f:found)
            out<<words[std::get<0>(f)]<<" "<<words[std::get<1>(f)]<<" "<<words[std::get<2>(f)]<<" "<<words[std::get<3>(f)]<<" "<<words[std::get<4>(f)]<<std::endl;
    }

}
int main(int argc, char *argv[])
{
    if  (argc<3)
    {
        std::cerr<<"Use: "<<argv[0]<<" word_list_file output_file [allcheck|graph|allcheck_par|graph_par]"<<std::endl;
        return -1;
    }
    bool allcheck=false;
    bool allcheck_par=false;
    bool graph=false;
    bool graph_par=false;
    for (int ac=3;ac<argc;ac++)
    {
        std::string_view a(argv[ac]);
        if (a=="allcheck")
            allcheck=true;
        else if (a=="allcheck_par")
            allcheck_par=true;
        else if (a=="graph")
            graph=true;
        else if (a=="graph_par")
            graph_par=true;
    }
    bool tcount=allcheck|graph|graph_par|allcheck_par;
    if (!tcount)
        graph_par=true;

    std::ifstream in;
    in.open(argv[1]);
    if (!in.is_open())
    {
        std::cerr<<"Can't open "<<argv[1]<<" for reading the word list"<<std::endl;
        return -1;
    }
    std::ofstream out;
    out.open(argv[2]);
    if (!out.is_open())
    {
        std::cerr<<"Can't open "<<argv[2]<<" for writing the 5x5 words combinations"<<std::endl;
        return -1;
    }
    auto start = std::chrono::system_clock::now();
    processList(in,start);
    if (allcheck)
    {
        std::cout<<"Perfoming all check algorithm"<<std::endl;
        algo_allCheck(out,start);
        out.close();
    }
    if (allcheck_par)
    {
        std::cout<<"Perfoming all check parallel algorithm"<<std::endl;
        algo_allCheck_par(out,start);
        out.close();
    }
    if (graph)
    {
        std::cout<<"Perfoming graph algorithm"<<std::endl;
        algo_graph(out,start);
        out.close();
    }
    if (graph_par)
    {
        std::cout<<"Perfoming graph paralell algorithm"<<std::endl;
        algo_graph_par(out,start);
        out.close();
    }

    return 0;
}
