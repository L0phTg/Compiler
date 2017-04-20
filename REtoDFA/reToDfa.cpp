#include <cstdio>
#include <cstdlib>
#include <queue>
#include <set>
#include <string>
#include <vector>
#include <list>
#include <iterator>
#include <algorithm>
#include <iostream>

enum State {NONE, ELEMENT, CHOOSE, CLOSURES};

int Num = 0;
struct NfaNode
{
    int flag;
    int accFlag;
    int nodeNum;
    char c;
    struct NfaNode *next;
    struct NfaNode *next2;
};

struct Nfa
{
    enum State state;
    struct NfaNode *startNode;
    struct NfaNode *endNode;
};

void Log_err(const char *s)
{
    fprintf(stderr, s);
    exit(0);
}

struct Nfa *Nfa_Closures(struct Nfa *old)
{
    struct NfaNode *node1 = (struct NfaNode *)malloc(sizeof(*node1)); node1->flag = 3; 
    struct NfaNode *node2 = (struct NfaNode *)malloc(sizeof(*node2)); node2->flag = 0; 
    node1->c = '#'; node2->c = '#'; 
    node1->nodeNum = Num++; node2->nodeNum = Num++;

    old->endNode->flag = 3; 

    old->endNode->next = node2;
    old->endNode->next2 = old->startNode;
    node1->next = old->startNode;   node1->next2 = node2;

    old->startNode = node1; old->endNode = node2;
    old->endNode->next = 0;
    return old;
}

struct Nfa *Nfa_Choose(struct Nfa *nfa, struct Nfa *old)
{
    struct NfaNode *node1 = (struct NfaNode *)malloc(sizeof(*node1)); node1->flag = 2;        node1->accFlag = 0;
    struct NfaNode *node2 = (struct NfaNode *)malloc(sizeof(*node2)); node2->flag = 0;        node2->accFlag = 2;
    node1->c = '#'; node2->c = '#';
    node1->nodeNum = Num++; node2->nodeNum = Num++;
    node2->next = 0;
    node1->next = nfa->startNode;   node1->next2 = old->startNode;
    nfa->endNode->next = node2; old->endNode->next = node2;

    nfa->startNode = node1;
    nfa->endNode = node2;
    return nfa;
}

struct Nfa *Nfa_Connect(struct Nfa *nfa, struct Nfa *old)
{
    old->endNode->next = nfa->startNode;
    old->endNode->c = '#';
    old->endNode = nfa->endNode;
    return old;
}



const char *iter;

struct Nfa *ThompsonAlo(struct Nfa *old)                                // old = 0
{                                                                       // nfa->state = NONE
                                                                        // ThompsonAlo(nfa)
    struct Nfa *nfa = (struct Nfa *)malloc(sizeof(*nfa));                             //      old->state = NONE
    nfa->state = NONE; nfa->startNode = 0; nfa->endNode = 0;            //      nfa->state = NONE       startNode = 0       endNode = 0
    // if (old) printf("%d", old->state);                               //      a ---> nfa->state = ELEMENT     startNode = node1   endNode = node2
    while(*iter != '\0') {                                              //      | ---> nfa->state = CHOOSE
        // printf("%c", *iter);                                         //      b ---> nfa->state = ELEMENT     startNode = node3   endNode = node4
        // if (old) printf("%d", old->state);                           //      ) ---> exe NONE:
        switch(*iter++){                                                //      ret nfa    state = ELEMENT    startNode = node3    endNode = node4
            case '|':{                          // 处理外面的字符之间的|  // nfa->state = ELEMENT 
                nfa->state = CHOOSE;                                    // ThompsonAlo(nfa)
                break;                                                  //      old->state = ELEMENT
            }                                                           //      nfa->state = NONE
            case '(':{                         //     a |               //      ThompsonAlo(nfa)
                // printf("%d ",nfa->state);
                nfa = ThompsonAlo(nfa);        // (   b     //          old->state = NONE
                // printf("%d ", nfa->state);
                // 处理括号的*                                )结合          // input "fee|fie"
                break;                                                      // loop: 
            }                                                               //      0: fee|                
            case ')':{                                                      //      nfa->state = CHOOSE
                // printf("%x ", old->state);                               //      f
                // printf("%d ", nfa->state);
                if(*iter == '*') {                          // 处理反括号后的一位
                    nfa = Nfa_Closures(nfa);
                    nfa->state = ELEMENT;
                    iter++;
                }
                switch(old->state){
                case NONE:{
                    break;
                }
                case CHOOSE:{
                    nfa = Nfa_Choose(nfa, old);
                    nfa->state = ELEMENT;
                    break;
                }   
                default:{
                    nfa = Nfa_Connect(nfa, old);
                    nfa->state = ELEMENT;
                    break;
                } }
                return nfa;                     // 处理)和同一级的(和内容
            }
            case '*':{                          // 处理外面的字符之后的*
                nfa = Nfa_Closures(nfa);
                break;
            }
            default:{                           // 处理外面的字符
                struct Nfa *p = (struct Nfa *)malloc(sizeof(*p));
                p->state = ELEMENT;     p->startNode = 0; p->endNode = 0;
                int count = 0;
                do{
                    if (count++ != 0)
                        iter++;
                    struct NfaNode *node1 = (struct NfaNode *)malloc(sizeof(*node1)); node1->flag = 1;
                    struct NfaNode *node2 = (struct NfaNode *)malloc(sizeof(*node2)); node2->flag = 0;
                    node1->c = *(iter-1);   node2->c = '#';
                    node1->nodeNum = Num++;     node2->nodeNum = Num++; 
                    node1->next = node2;    node2->next = 0;
                    if (p->startNode == 0) {
                        p->startNode = node1; p->endNode = node2;
                    }
                    else {
                        p->endNode->next = node1;
                        p->endNode = node2;
                    }   
                }while(*iter >= 'a' && *iter <= 'z');
                if (nfa->startNode == 0) {
                    nfa = p;
                    // nfa->state = ELEMENT; nfa->startNode = node1; nfa->endNode = node2;
                }
                else {
                    // struct Nfa *p = (struct Nfa *)malloc(sizeof(*p));                                                     // 每次结合之前都要判断是否后一位是*
                    // p->state = ELEMENT; p->startNode = node1; p->endNode = node2; p->state = ELEMENT;
                    if(*iter == '*') {                          // 处理反括号后的一位
                        // printf("*");
                        p = Nfa_Closures(p);
                        p->state = ELEMENT;
                        iter++;
                    }
                    // while(*iter != '(' && *iter != ')' && *iter != '|' && *iter != '*' && *iter != '\0') {
                    //     p = Nfa_Connect(q, p);
                    //     iter++;
                    // }

                           // 处理外部的连接
                    switch(nfa->state) {
                        case CHOOSE:{
                            // printf("%d ", nfa->state);
                            nfa = Nfa_Choose(nfa, p);
                            nfa->state = ELEMENT;
                            // printf("%d ", nfa->state);
                            break;
                        }
                        default:{
                            nfa = Nfa_Connect(p, nfa);
                            nfa->state = ELEMENT;
                            break;
                        }
                    }
                }
                break;
            }
        }
        // printf("%d ", nfa->state);
    }
    return nfa;
}

struct Nfa *Inialize(const char *p)
{
    iter = p;
    struct Nfa *nfa = ThompsonAlo(0);
    nfa->endNode->next = 0;
    return nfa;
}

struct NfaNode *print_Nfa_node(struct NfaNode *node)                   // 闭包的第一个元素 flag == 3
{                                                           // 选择的第一个元素 flag == 2
    if (node->next == 0) {                                                                               //           0  1                 6   7
        printf("End Node: %d\n", node->nodeNum);                                                         //       4          5   12    10          11    13
        return 0;                                                                                        //           2  3                 8   9
    }
    if (node->flag == 0 || node->flag == 1) {
        while(1) {
            if (node->accFlag == 2)
                return node;
            if (node->flag == 2 || node->flag == 3)
                break;
            if (node->next == 0) {
                printf("End Node: %d\n", node->nodeNum);
                return 0;
            }
            printf("%5d    %5c    %5d\n", node->nodeNum, node->c, node->next->nodeNum);                 //                      3                 3
            node = node->next;
        }            
    }
    else if (node->flag == 3) {
        printf("%5d    %5c    %5d\n", node->nodeNum, node->c, node->next->nodeNum);  
        printf("%5d    %5c    %5d\n", node->nodeNum, node->c, node->next2->nodeNum);
        node = node->next;
    }
    else if (node->flag == 2) {
        printf("%5d    %5c    %5d\n", node->nodeNum, node->c, node->next->nodeNum);  
        printf("%5d    %5c    %5d\n", node->nodeNum, node->c, node->next2->nodeNum);        
        print_Nfa_node(node->next);                                                             // 
        node = print_Nfa_node(node->next2);
        if (node->next == 0){

        }
        else if(node->flag == 0 || node->flag == 1) {
            printf("%5d    %5c    %5d\n", node->nodeNum, node->c, node->next->nodeNum);                                                            //
            node = node->next;
        }
    }
    return print_Nfa_node(node);
}

void print_Nfa(struct Nfa *nfa)
{
    if (!nfa) Log_err("please input regex\n");
    if (nfa->startNode == 0) Log_err("Thompson parse error\n");

    struct NfaNode *iter = nfa->startNode;
    printf("nfa map: \n");
    printf("from    accept   to\n");
    print_Nfa_node(nfa->startNode);
    return ;
}


std::set<struct NfaNode *> eps_closure(struct NfaNode *node)
{
    std::set<struct NfaNode *> closure;
    std::queue<struct NfaNode *> Q;
    if (node == NULL) return closure;
    Q.push(node);
    while(!Q.empty()) {
        int FF = 0;
        struct NfaNode *q = Q.front();
        Q.pop();
        closure.insert(q);
        if (q->next == 0)
            continue;
        for (auto &i : closure)
            if (i == q->next) FF = 1;
        if (FF == 1) continue;  

        if (q->flag == 0)
            Q.push(q->next);
        if (q->flag == 2 || q->flag == 3) {
            Q.push(q->next);
            Q.push(q->next2);
        }
    }
    
    return closure;
}

// Q vector<set<struct NfaNode>
// WorkList set<struct NfaNode>
// D type mapNfa
class mapNfa {
public:
    std::set<struct NfaNode *> &operator()(std::set<struct NfaNode *>old, char c)
    {
        length++;
        edge.push_back(c);
        oldMap.push_back(old);
        newMap.resize(newMap.size() + 1);
        return *(newMap.rbegin());
    }

    size_t size()
    {
        return length;
    }

    const std::vector<std::set<struct NfaNode *>> &getOldMap()
    {
        return oldMap;
    }

    const std::vector<std::set<struct NfaNode *>> &getNewMap()
    {
        return newMap;
    }

    const std::vector<char> &getEdge()
    {
        return edge;
    }
private:
    std::vector<std::set<struct NfaNode *>> oldMap;
    std::vector<std::set<struct NfaNode *>> newMap;
    std::vector<char> edge;
    size_t length = 0;
};

struct NfaNode *delta(std::set<struct NfaNode *> q, char c)
{
    for(const auto &i : q)
        if (i->c == c)
            return i->next;

    return NULL;
}
                                                                                //               2  b  3
std::vector<std::set<struct NfaNode*>> Lex_SubSet(struct NfaNode* node, mapNfa &D)           // node == nfa->startNode           0 a  1   8   6     7    9
{
    std::vector<std::set<struct NfaNode*>> Q;                                     //               4  c  5
    std::queue<std::set<struct NfaNode*>> WorkList;
    // mapNfa D;
    
    struct NfaNode *iter = node;
    std::set<struct NfaNode *> q0 = eps_closure(iter);

    Q.push_back(q0);
    WorkList.push(q0);

    while(!WorkList.empty()) {
        std::set<struct NfaNode *> q = WorkList.front();
        WorkList.pop();
        for(char c = 'a'; c <= 'z'; c++) {
            std::set<struct NfaNode *> t = eps_closure(delta(q, c));

            if (t.empty()) continue;
            D(q, c) = t;
            auto i = Q.begin();
            for(; i != Q.end(); i++)
                if(*i == t) 
                    break;
            
            if(i == Q.end()) {
                Q.push_back(t); 
                WorkList.push(t);
            }
        }
    }


    return Q;
}

void print_Map_Dfa(mapNfa &dfa)
{
    printf("dfa Map: \n");
    for(size_t i = 0; i < dfa.size(); i++) {
        for (auto &i : (dfa.getOldMap())[i])
            std::cout << i->nodeNum << " ";
        std::cout << "   " << (dfa.getEdge())[i] << "    ";
        for (auto &i : (dfa.getNewMap())[i])
            std::cout << i->nodeNum << " ";
        std::cout << std::endl;
    }
}

void print_Dfa_state(std::vector<std::set<struct NfaNode*>> Q, struct NfaNode *endNode)
{
    int endNum = endNode->nodeNum;
    printf("dfa state: \n");
    for (const auto &i : Q) {
        int flag = 0;
        for (const auto &j : i) {
            if (j->nodeNum == endNum)
                flag = 1;
            std::cout << j->nodeNum << " ";
        }
        std::cout << std::endl;
    }
}

class DfaNode {
public:
    void setId(int i)
    {
        id = i;
    }
    void setToId(int i)
    {
        toId.push_back(i);
    }
    void setAccChar(char c)
    {
        accChar.push_back(c);
    }
    void setEndState(int endS)
    {
        endState = endS;
    }
    const int &getEndState()
    {
        return endState;
    }
    const int &getId()
    {
        return id;
    }
    const std::vector<char> &getAccChar()
    {
        return accChar;
    }
    const std::vector<int> &getToId()
    {
        return toId;
    }
private:
    int id;
    std::vector<char> accChar;
    std::vector<int> toId;
    int endState;
};

std::vector<DfaNode*> createDfa(std::vector<std::set<struct NfaNode*>> Q, mapNfa D, struct NfaNode *endNode)
{
    int endNum = endNode->nodeNum;
    std::vector<DfaNode*> S;
    std::vector<DfaNode*> tmp;
    int id = 0;

    for(auto &q : Q) {                                          // inialize id  0, 1, 2, 3
        DfaNode *node = new(DfaNode); 
        node->setId(id++);
        for(auto &n : q)
            if (n->nodeNum == endNum) node->setEndState(1);
        tmp.push_back(node);
    }
    

    for(size_t i = 0; i < D.size(); i++) {
        for (size_t j = 0; j < Q.size(); j++) {
            if ((D.getOldMap())[i] == Q[j]) {
                char ff = (D.getEdge())[i];
                tmp[j]->setAccChar(ff);
                
                for(size_t k = 0; k < Q.size(); k++) {
                    if ((D.getNewMap())[i] == Q[k]) {
                        tmp[j]->setToId(k);
                        break;
                    }
                }
                break;
            }
        }    
    }
    std::vector<DfaNode*>::iterator it = S.begin();
    S.insert(it, tmp.begin(), tmp.end());

    // std::cout << std::endl;
    return S;   
}

void print_Dfa(std::vector<DfaNode*> &S)
{
    printf("dfa: \n");
    printf("state:    ");
    for(auto &s : S)
        std::cout << (*s).getId() << " ";
    std::cout << std::endl;
    for(auto &s : S)
        if ((*s).getEndState() == 1) 
            printf("EndNode: %d\n", (*s).getId());
    printf("from    accChar    to\n");
    for(auto &s : S) {
        if(!(*s).getAccChar().empty()) {
            for(size_t i = 0; i < (*s).getToId().size(); i++)
            std::cout << (*s).getId() << "\t" << (*s).getAccChar()[i] << "\t"
                    << (*s).getToId()[i] << std::endl;
        }
    }
}

DfaNode* split(std::vector<DfaNode*> &S)
{
    std::vector<DfaNode*> ele;
    std::set<int> idTotal;
    std::set<int> accept;               // to id
    std::set<char> accChar;
    // std::set<int> diff;
    
    for(auto &s : S)
        idTotal.insert((*s).getId());
    for(auto &s : S)
        for (auto &i : (*s).getToId())
            accept.insert(i);
    for(auto &s : S)
        accChar.insert((*s).getAccChar().begin(), (*s).getAccChar().end());

    // for(auto &v : accChar)                           print element
    //     std::cout << v << " ";
    // std::cout << std::endl;
    // for(auto &v : idTotal)
    //     std::cout << v << " ";
    // std::cout << std::endl;
    // for(auto &v : accept)
    //     std::cout << v << " ";
    // std::cout << std::endl;

    DfaNode *node = new(DfaNode);
    if (std::includes(idTotal.begin(), idTotal.end(), accept.begin(), accept.end())) {
        auto it = (*node).getToId().begin();
        (*node).setId(*(idTotal.begin()));
        for (size_t i = 0; i < accChar.size(); i++)
            (*node).setToId((*node).getId());

        (*node).setToId(*(idTotal.begin()));

        for (auto &i : accChar)
            (*node).setAccChar(i);
        
        S.clear();
        S.push_back(node);
    }
    else {
        
        int count = 0;
        for(char a = 'a'; a < 'z'; a++) {
            if (!accChar.count(a))
                continue;

            std::set<char> acC;
            for(std::vector<DfaNode*>::iterator i = S.begin(); i != S.end();) {
                if(!std::includes(idTotal.begin(), idTotal.end(), (**i).getToId().begin(), (**i).getToId().end())) {
                    // printf("%c", a);
                    if (count++ == 0) (*node).setId((**i).getId());
                    for(auto &c : (**i).getAccChar())
                        acC.insert(c);    
                    i = S.erase(i);
                }
                else
                    i++;
            }
            for(auto &c : acC)
                (*node).setAccChar(c);
        }       
    }
    return node;
}

void hopcroft(std::vector<DfaNode*> &dfa)
{
    std::vector<DfaNode*> S = dfa;                                     // really dfa
    std::vector<DfaNode*> A;                                           // accept
    std::vector<DfaNode*> N;                                           // none ...
    std::list<DfaNode*> nDfa;
    std::list<DfaNode*> aDfa;
    for(auto &s : S) {
        // std::cout << (*s).getEndState() << " ";
        if ((*s).getEndState() == 1)
            A.push_back(s);
        else
            N.push_back(s);
    }
    dfa.clear();

    int flag = S.size();
    int oldId;
    // printf("%d\n", A.size());
    int count = 0;    
    while(flag != A.size()) {
        if(A.size() == 1) {
            aDfa.push_front(A[0]);
            break;
        }
        flag = A.size();
        DfaNode *p = split(A);
        (*p).setEndState(1);
        
        aDfa.push_front(p);
        if (p == A[0]) break;
    }

    // printf("%d\n", aDfa.size());
    oldId = (*(A[0])).getId();
    flag = S.size();
    // for(auto &v : N)
    //     printf("%d ", (*v).getId());
    while(flag != N.size()) {
        if (N.size() == 1) {
            nDfa.push_front(N[0]);
            break;
        }
        flag = N.size();
        DfaNode *p = split(N);
        // printf("%d ", N.size());
        (*p).setToId(oldId);
        (*p).setEndState(0);
        nDfa.push_front(p);
        oldId = (*p).getId();
    }  
    // std::cout << nDfa.size() << std::endl;


    // printf("%d\n", aDfa.size());
    dfa.insert(dfa.begin(), nDfa.begin(), nDfa.end());
    // printf("%d %c %d \n", (*(dfa[0])).getId());
    dfa.insert(dfa.end(), aDfa.begin(), aDfa.end());
    // printf("%d\n", dfa.size());
}

void print_Min_Dfa(std::vector<DfaNode*> &dfa)
{
    printf("MinDfa: \n");
    printf("state: ");
    for(auto &v : dfa) {
        printf("%d ", (*v).getId());
    }
    printf("\n");
    for(auto &v : dfa) {
        if ((*v).getEndState() == 1)
            printf("EndState: %d\n", (*v).getId());
    }
    printf("from    accChar    to\n");
    for(auto &v : dfa) {
        for(size_t i = 0; i < (*v).getAccChar().size(); i++)
            printf("%d    %c    %d\n", (*v).getId(), (*v).getAccChar()[i], (*v).getToId()[0]);
    }
}

int main()
{
    static const char *regex = "(a|b)((c|d)*)";
    printf("Testing regex: %s\n", regex);
    printf("compile starting...\n");
    struct Nfa *nfa = Inialize(regex);                          // 将转换后的Nfa保存到nfa中                                         
    print_Nfa(nfa);
    mapNfa D;
    std::vector<std::set<struct NfaNode*>> Q = Lex_SubSet(nfa->startNode, D);
    print_Dfa_state(Q, nfa->endNode);
    print_Map_Dfa(D);

    
    std::vector<DfaNode*> dfa = createDfa(Q, D, nfa->endNode);

    print_Dfa(dfa);
    hopcroft(dfa);
    print_Min_Dfa(dfa);

    return 0;
}