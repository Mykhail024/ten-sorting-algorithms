#include <cstdlib>
#include <ctime>
#include <utility>
#include <random>

#include "sort.h"

std::vector<int> generateVec(const size_t &size, const int &min, const int &max)
{
    if(min > max) return {};
    std::vector<int> result(size);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);

    for(size_t i = 0; i < size; ++i) {
        result[i] = (dis(gen));
    }
    return result;
}

bool _isSorted(const std::vector<int> &vec)
{
    for(size_t i = 0; i < vec.size(); ++i) {
        if(vec[i] > vec[i+1]) {
            return false;
        }
    }
    return true;
}

const size_t bogoSort(std::vector<int> &vec)
{
    if(vec.size() < 2) return 0;
    srand(time(0));
    size_t count = 0;

    while(!_isSorted(vec)) {
        for(size_t i = 0; i < vec.size(); i++) {
            std::swap(vec[i], vec[rand() % vec.size()]);
            ++count;
        }
    }

    return count;
}

const size_t bozoSort(std::vector<int> &vec)
{
    if(vec.size() < 2) return 0;
    srand(time(0));
    size_t count = 0;

    while(!_isSorted(vec)) {
        std::swap(vec[rand() % vec.size()], vec[rand() % vec.size()]);
        ++count;
    }

    return count;
}

const size_t bubleSort(std::vector<int> &vec)
{
    if(vec.size() < 2) return 0;
    size_t count = 0;
    for(size_t i = 0; i < vec.size(); ++i) {
        for(size_t j = 0; j < vec.size() - i - 1; ++j) {
            if(vec[j] > vec[j+1]) {
                std::swap(vec[j], vec[j+1]);
                ++count;
            }
        }
    }
    return count;
}

const size_t insertionSort(std::vector<int> &vec)
{
    if(vec.size() < 2) return 0;
    size_t count = 0;
    int key, j;
    for(size_t i = 1; i < vec.size(); ++i) {
        key = vec[i];
        j = i-1;
        while(j >= 0 && vec[j] > key) {
            vec[j+1] = vec[j];
            --j;
            ++count;
        }
        vec[j+1] = key;
        ++count;
    }
    return count;
}

const size_t selectionSort(std::vector<int> &vec)
{
    if(vec.size() < 2) return 0;
    size_t count = 0;
    for(size_t i = 0; i < vec.size(); i++) {
        size_t min = i;
        for(size_t j = i+1; j < vec.size(); j++) {
            if(vec[min] > vec[j]) {
                min = j;
            }
        }
        if(min != i) {
            std::swap(vec[min], vec[i]);
            ++count;
        }
    }
    return count;
}

const size_t cycleSort(std::vector<int> &vec)
{
    if(vec.size() < 2) return 0;
    size_t count = 0;
    for(size_t i = 0; i < vec.size(); ++i) {
        int val = vec[i];
        size_t pos = i;

        for(size_t j = i+1; j < vec.size(); ++j) {
            if(vec[j] < val) {
                ++pos;
            }
        }

        if(pos == i) {
            continue;
        }

        while(val == vec[pos]) {
            ++pos;
        }

        std::swap(val, vec[pos]);
        ++count;

        while(pos != i) {
            pos = i;
            for(size_t k = i+1; k < vec.size(); ++k) {
                if(vec[k] < val) {
                    ++pos;
                }
            }
            while(val == vec[pos]) {
                ++pos;
            }
            std::swap(val, vec[pos]);
            ++count;
        }
    }
    return count;
}

const size_t _part(std::vector<int> &vec, const size_t &start, const size_t &end, size_t &count)
{
    auto pivot = vec[(start + end) / 2];
    size_t i = start;

    for(size_t j = start; j <= end - 1; ++j) {
        if(vec[j] <= pivot) {
            std::swap(vec[j], vec[i]);
            ++i;
            ++count;
        }
    }

    std::swap(vec[i], vec[end]);
    ++count;
    return i;
}

void _quickSort(std::vector<int> &vec, const size_t &start, const size_t &end, size_t &count)
{
    if(start < end) {
        const size_t partIndex = _part(vec, start, end, count);

        if (partIndex > 0) {
            _quickSort(vec, start, partIndex - 1, count);
        }
        if (partIndex < end) {
            _quickSort(vec, partIndex + 1, end, count);
        }
    }
}

const size_t quickSort(std::vector<int> &vec)
{
    if(vec.size() < 2) return 0;
    size_t count = 0;
    _quickSort(vec, 0, vec.size()-1, count);
    return count;
}

void _merge(std::vector<int> &vec, const size_t &left, const size_t &mid, const size_t &right, size_t &count)
{
    std::vector<int> arr(vec.begin(), vec.end());

    size_t i = left;
    size_t j = mid + 1;

    for(size_t k = left; k <= right; ++k) {
        if(i <= mid && (j > right || arr[i] < arr[j])) {
            vec[k] = arr[i];
            ++i;
            ++count;
        } else {
            vec[k] = arr[j];
            ++j;
            ++count;
        }
    }
}

void _mergeSort(std::vector<int> &vec, const size_t &left, const size_t &right, size_t &count)
{
    if(left < right) {
        const size_t mid = (left + right) / 2;
        _mergeSort(vec, left, mid, count);
        _mergeSort(vec, mid + 1, right, count);
        _merge(vec, left, mid, right, count);
    }
}

const size_t mergeSort(std::vector<int> &vec)
{
    if(vec.size() < 2) return 0;
    size_t count = 0;
    _mergeSort(vec, 0, vec.size()-1, count);
    return count;
}

const size_t gnomeSort(std::vector<int> &vec)
{
    if(vec.size() < 2) return 0;
    size_t count = 0;

    size_t i = 0;
    size_t n = vec.size();
    while(i < n) {
        if(i == 0) ++i;
        if(vec[i] >= vec[i-1]) ++i;
        else {
            std::swap(vec[i], vec[i-1]);
            --i;
            ++count;
        }
    }

    return count;
}

void _stoogeSort(std::vector<int> &vec, const size_t &l, const  size_t &r, size_t &count)
{
    if(l >= r) return;

    if(vec[l] > vec[r]) {
        std::swap(vec[l], vec[r]);
        ++count;
    }

    if(r - l + 1 > 2) {
        size_t t = (r - l + 1) / 3;

        _stoogeSort(vec, l, r-t, count);
        _stoogeSort(vec, l + t, r, count);
        _stoogeSort(vec, l, r-t, count);

    }
}

const size_t stoogeSort(std::vector<int> &vec)
{
    if(vec.size() < 2) return 0;
    size_t count = 0;

    _stoogeSort(vec, 0, vec.size()-1, count);

    return count;
}

const size_t pigeonholeSort(std::vector<int> &vec)
{

    if(vec.size() < 2) return 0;
    size_t count = 0;

    int min = vec[0], max = vec[0];
    size_t n = vec.size();
    for(size_t i = 1; i < n; ++i) {
        if(vec[i] < min) min = vec[i];
        if(vec[i] > max) max = vec[i];
    }

    int range = max - min + 1;

    std::vector<std::vector<int>> holes(range);

    for(int i = 0; i < n; ++i) {
        holes[vec[i] - min].push_back(vec[i]);
    }

    size_t index = 0;
    for(size_t i = 0; i < range; i++) {
        std::vector<int>::iterator it;
        for(it = holes[i].begin(); it != holes[i].end(); ++it) {
            vec[index++] = *it;
            ++count;
        }
    }

    return count;
}

void _heapify(std::vector<int> &vec, const size_t &N, const size_t &i, size_t &count)
{
    size_t largest = i;
    size_t l = 2 * i + 1;
    size_t r = 2 * i + 2;

    if(l < N && vec[l] > vec[largest]) largest = l;
    if(r < N && vec[r] > vec[largest]) largest = r;

    if(largest != i) {
        std::swap(vec[i], vec[largest]);
        ++count;

        _heapify(vec, N, largest, count);
    }
}

const size_t heapSort(std::vector<int> &vec)
{
    const size_t N = vec.size();
    if(N < 2) return 0;

    size_t count = 0;

    for(long long i = N / 2 - 1; i >= 0; --i) _heapify(vec, N, i, count);

    for(long long i = N - 1; i > 0; --i) {
        std::swap(vec[0], vec[i]);
        ++count;

        _heapify(vec, static_cast<size_t>(i), 0, count);
    }

    return count;
}
