#include "sort.h"
#include <cstdlib>
#include <ctime>

std::vector<int> generateVec(const size_t &size, const int &min, const int &max)
{
    std::vector<int> result(size);

    srand(time(0));

    for(size_t i = 0; i < size; i++) {
        result.push_back(rand() % (max - min + 1) + min);
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
    if(vec.empty() || vec.size() < 2) return 0;
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
    if(vec.empty() || vec.size() < 2) return 0;
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
    if(vec.empty() || vec.size() < 2) return 0;
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
    if(vec.empty() || vec.size() < 2) return 0;
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

const size_t selectSort(std::vector<int> &vec)
{
    if(vec.empty() || vec.size() < 2) return 0;
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
    if(vec.empty() || vec.size() < 2) return 0;
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
    auto pivot = vec[end];
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

        _quickSort(vec, start, partIndex-1, count);
        _quickSort(vec, partIndex+1, end, count);
    }
}

const size_t quickSort(std::vector<int> &vec)
{
    if(vec.empty() || vec.size() < 2) return 0;
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
    if(vec.empty() || vec.size() < 2) return 0;
    size_t count = 0;
    _mergeSort(vec, 0, vec.size()-1, count);
    return count;
}
