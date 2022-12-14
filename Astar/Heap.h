#pragma once

#include <iostream>
#include <functional>

template <class DataType, class SortPredicate>
class Heap
{
    SortPredicate m_compare;
    std::vector<DataType> m_heap;

    int leftChild(int index)
    {
        return (index + 1) * 2 - 1;
    }

    int rightChild(int index)
    {
        return (index + 1) * 2;
    }

    int parent(int index)
    {
        return (index - 1) / 2;
    }

    int compareChildIndex(int index)
    {
        if (leftChild(index) >= m_heap.size())
            return -1;
        if (rightChild(index) >= m_heap.size())
            return leftChild(index);
        return (m_compare(m_heap[leftChild(index)], m_heap[rightChild(index)])) ? leftChild(index) : rightChild(index);
    }

    void add_to_heap(DataType value)
    {
        m_heap.push_back(value);
        if (m_heap.size() == 1)
        {
            return;
        }
        //std::cout << "PUSH: Rebalancing heap\n";
        int currentIndex = m_heap.size() - 1;
        int parentIndex = parent(currentIndex);
        while (m_compare(m_heap[currentIndex], m_heap[parentIndex]))
        {
            //std::cout << "Swapping: " << m_heap[currentIndex] << " - " << m_heap[parentIndex] << std::endl;
            //std::swap(m_heap[currentIndex], m_heap[parentIndex]);
            DataType temp = m_heap[currentIndex];
            m_heap[currentIndex] = m_heap[parentIndex];
            m_heap[parentIndex] = temp;
            currentIndex = parentIndex;
            parentIndex = parent(currentIndex);
        }
        //std::cout << "PUSH: Rebalanced\n";
    }

    DataType remove_from_heap()
    {
        std::swap(m_heap[0], m_heap[m_heap.size() - 1]);
        DataType popped = m_heap.back();
        m_heap.pop_back();

        if (m_heap.size() == 0)
            return popped;

        int currentIndex = 0;
        bool swap{ true };
        //std::cout << "POP: rebalancing heap\n";
        while (swap)
        {
            swap = false;
            int largest = compareChildIndex(currentIndex);
            if (largest == -1)
                break;
            if (!m_compare(m_heap[currentIndex], m_heap[largest]))
            {
                swap = true;
                std::swap(m_heap[currentIndex], m_heap[largest]);
                currentIndex = largest;
            }
        }
        //std::cout << "POP: rebalanced\n";
        return popped;
    }

    int level(int points)
    {
        int counter{ 0 };
        while (points != 0)
        {
            counter++;
            points = points / 2;
        }
        return counter;
    }
public:
    Heap() {}

    void setPredicate(SortPredicate pred)
    {
        m_compare = pred;
    }

    void createHeap(std::vector<DataType> values)
    {
        for (auto& value : values)
        {
            add_to_heap(value);
        }
    }

    void print()
    {
        std::cout << "Printing heap:\n";
        int currentLevel{ 1 };
        int processedPoints{ 0 };
        for (int i = 0; i < m_heap.size(); i++)
        {
            processedPoints++;
            if (level(processedPoints) > currentLevel)
            {
                std::cout << std::endl;
                currentLevel++;
            }
            std::cout << m_heap[i] << ", ";

        }
        std::cout << "\nPrinted\n";
    }
    void printLine()
    {
        for (auto& value : m_heap)
            std::cout << value << ", ";
        std::cout << std::endl;
    }
    void push(DataType data)
    {
        add_to_heap(data);
    }

    DataType pop()
    {
        return remove_from_heap();
    }
};

//Heap<int, std::function<bool(int&, int&)>> testHeap;
//testHeap.setPredicate([](int& a, int& b) {return a < b; });
//testHeap.createHeap({ 2,5,7,1,3,6,9,4,8 });
//testHeap.print();