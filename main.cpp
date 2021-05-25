#include <QCoreApplication>
#include<QStack>
#include<QList>
#include<QDebug>
class IObserver{
public:
    virtual ~IObserver(){};
    virtual void Update(int numNode) = 0;
};

class Node
{
public:
    Node(int value)
    {
        _value = value;
    }
    int _value;
    QList<Node*> adjacentNodes;
};

class Graph
{
private:
    QList<Node*> Nodes;
public:
    Graph()
    {
        for(int i = 1; i <= 8; i++)
            Nodes.append(new Node(i));

        Nodes.at(0)->adjacentNodes.append(Nodes.at(1));
        Nodes.at(0)->adjacentNodes.append(Nodes.at(2));
        Nodes.at(1)->adjacentNodes.append(Nodes.at(2));
        Nodes.at(2)->adjacentNodes.append(Nodes.at(3));
        Nodes.at(3)->adjacentNodes.append(Nodes.at(4));
        Nodes.at(4)->adjacentNodes.append(Nodes.at(5));
        Nodes.at(5)->adjacentNodes.append(Nodes.at(6));
        Nodes.at(6)->adjacentNodes.append(Nodes.at(7));
    }
    ~Graph()
    {
        for(int i = 0; i < Nodes.length(); i++)
            delete Nodes.at(i);
    }
    class Iterator
    {
    public:
        Iterator(Graph* graph)
        {
            _graph = graph;
        }
        ~Iterator()
        {

        }
        void Attach(IObserver *observer)
        {
            _observer = observer;
        }
        void Detach()
        {
            _observer = nullptr;
        }
        void Notify()
        {
            _observer->Update(GetCurrentNode()->_value);
        }
        Node* First()
        {
            SetPath();
            _currentNodeNumber = 0;
            return _path.at(_currentNodeNumber);
        }
        void Next()
        {
            Notify();
            _currentNodeNumber++;
        }
        bool isDone()
        {
            return _currentNodeNumber == _path.length();
        }
        Node* GetCurrentNode()
        {
            return _path.at(_currentNodeNumber);
        }
    private:
        int _currentNodeNumber;
        IObserver* _observer;
        Graph* _graph;
        QList<Node*> _usedNode;
        QList<Node*> _path;
        void SetPath()
        {
            QStack<Node*> stack;
            stack.push_back(_graph->Nodes.at(0));

            while(!stack.empty())
            {
                Node* node;
                node = stack.top();
                stack.pop();
                if(NodeIsUsed(node))
                {
                    continue;
                }
                _usedNode.push_back(node);
                for(int i = 0; i < node->adjacentNodes.length(); i++)
                {
                    Node* nextNode = node->adjacentNodes.at(i);
                    if(!NodeIsUsed(nextNode))
                    {
                        stack.push(nextNode);
                    }
                }
                _path.push_back(node);
            }
        }

        bool NodeIsUsed(Node* node)
        {
            for(int i = 0; i < _usedNode.length(); i++)
            {
                if(node == _usedNode.at(i))
                {
                    return true;
                }
            }
            return false;
        }
    };

    Iterator* CreateIterator()
    {
        return new Iterator(this);
    }

    friend class Iterator;
};

class Observer : public IObserver{
public:
    Observer(Graph::Iterator &iterator) : _iterator(iterator)
    {
        this->_iterator.Attach(this);
    }
    void Update(int numNode) override
    {
        qDebug() << "Node" << numNode << "is passed";
    }
    void RemoveMe()
    {
        _iterator.Detach();
    }
    void PrintInfo()
    {
        qDebug() << "";
    }
private:
    Graph::Iterator &_iterator;
    QString _message;
};

int main()
{

    Graph* graph = new Graph();
    Graph::Iterator* it = graph->CreateIterator();

    Observer* observer = new Observer(*it);

    it->First();
    while(!it->isDone())
    {
        it->Next();
    }

    delete observer;
    delete it;
    delete graph;


    return 0;
}
