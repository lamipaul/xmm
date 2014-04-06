//
// utility.h
//
// Set of utilities
//
// Copyright (C) 2013 Ircam - Jules Françoise. All Rights Reserved.
// author: Jules Françoise
// contact: jules.francoise@ircam.fr
//

#ifndef rtml_utility_h
#define rtml_utility_h

#include <iostream>
#include <vector>
#include <sstream>

const double EPSILON_GAUSSIAN = 1.0e-40;

using namespace std;

#pragma mark -
#pragma mark Memory Allocation
template <typename T>
T* reallocate(T *src, unsigned int dim_src, unsigned int dim_dst) {
    T *dst = new T[dim_dst];
    
    if (!src) return dst;
    
    if (dim_dst > dim_src) {
        copy(src, src+dim_src, dst);
    } else {
        copy(src, src+dim_dst, dst);
    }
    delete[] src;
    return dst;
}

#pragma mark -
#pragma mark Centroid
template <typename T>
T centroid(T const *vect, int size) {
    T c(0.0);
    for (int i=0 ; i<size; i++) {
        c += vect[i] * T(i);
    }
    c /= T(size-1);
    return c;
}

template <typename T>
T centroid(vector<T> const &vect) {
    T c(0.0);
    for (unsigned int i=0 ; i<vect.size(); i++) {
        c += vect[i] * T(i);
    }
    c /= T(vect.size()-1);
    return c;
}

#pragma mark -
#pragma mark Gaussian Distribution
double gaussianProbabilityFullCovariance(const float *obs, std::vector<float>::iterator mean,
                                         double covarianceDeterminant,
                                         std::vector<float>::iterator inverseCovariance,
                                         int dimension);
double gaussianProbabilityFullCovariance_GestureSound(const float *obs_gesture,
                                                      const float *obs_sound,
                                                      std::vector<float>::iterator mean,
                                                      double covarianceDeterminant,
                                                      std::vector<float>::iterator inverseCovariance,
                                                      int dimension_gesture,
                                                      int dimension_sound);

#pragma mark -
#pragma mark Vector Utilies
//void vectorCopy(vector<float>::iterator dst_it, vector<float>::iterator src_it, int size);
//void vectorCopy(vector<double>::iterator dst_it, vector<double>::iterator src_it, int size);
void vectorMultiply(std::vector<float>::iterator dst_it, std::vector<float>::iterator src_it, int size);
void vectorMultiply(std::vector<double>::iterator dst_it, std::vector<double>::iterator src_it, int size);

#pragma mark -
#pragma mark File IO
const int MAX_STR_SIZE(4096);
void skipComments(istream *s);

#pragma mark -
#pragma mark Simple Ring Buffer
template <typename T, int channels>
class RingBuffer {
public:
	// Constructor
	RingBuffer(unsigned int length_ = 1)
    {
        length = length_;
        for (int c=0; c<channels; c++) {
            data[c].resize(length);
        }
        index = 0;
        full = false;
    }
    
	~RingBuffer()
    {
        for (int c=0; c<channels; c++) {
            data[c].clear();
        }
    }
    
    T operator()(unsigned int c, unsigned int i)
    {
        if (c >= channels)
            throw out_of_range("channel out of bounds");
        unsigned int m = full ? length : index;
        if (i >= m)
            throw out_of_range("index out of bounds");
        return data[c][i];
    }
	
	// methods
	void clear()
    {
        index = 0;
        full = false;
    }
    
    void push(T const value)
    {
        if (channels > 1)
            throw invalid_argument("You must pass a vector or array");
        data[0][index] = value;
        index++;
        if (index == length)
            full = true;
        index %= length;
    }
    
	void push(T const *value)
    {
        for (int c=0; c<channels; c++)
        {
            data[c][index] = value[c];
        }
        index++;
        if (index == length)
            full = true;
        index %= length;
    }
    
    void push(vector<T> const &value)
    {
        for (int c=0; c<channels; c++)
        {
            data[c][index] = value[c];
        }
        index++;
        if (index == length)
            full = true;
        index %= length;
    }
    
	unsigned int size() const
    {
        return length;
    }
    
    unsigned int size_t() const
    {
        return (full ? length : index);
    }
    
	void resize(unsigned int length_)
    {
        if (length_ == length) return;
        if (length_ > length) {
            full = false;
        } else if (index >= length_) {
            full = true;
            index = 0;
        }
        length = length_;
        for (int c=0; c<channels; c++) {
            data[c].resize(length);
        }
    }
    
    vector<T> mean() const
    {
        vector<T> _mean(channels, 0.0);
        int size = full ? length : index;
        for (int c=0; c<channels; c++)
        {
            for (int i=0; i<size; i++) {
                _mean[c] += data[c][i];
            }
            _mean[c] /= T(size);
        }
        return _mean;
    }
	
protected:
    vector<T> data[channels];
	unsigned int length;
	unsigned int index;
	bool full;
};


bool is_number(const string& s);
int to_int(const string& s);

#endif
