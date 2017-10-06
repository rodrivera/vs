#ifndef __INTERVAL_TREE_H
#define __INTERVAL_TREE_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <memory>

#include <map>
#include <set>
#include "range-tree/RangeTree.h"

namespace RT = RangeTree;

template <class T, typename K = std::size_t>
class Interval {    
public:
    K start;
    K stop;
    T value;
    Interval(K s, K e, const T& v)
        : start(s)
        , stop(e)
        , value(v)
    { }
};

template <class T, typename K>
K intervalStart(const Interval<T,K>& i) {
    return i.start;
}

template <class T, typename K>
K intervalStop(const Interval<T,K>& i) {
    return i.stop;
}

template <class T, typename K>
  std::ostream& operator<<(std::ostream& out, Interval<T,K>& i) {
    out << "Interval(" << i.start << ", " << i.stop << "): " << i.value;
    return out;
}

template <class T, typename K = std::size_t>
class IntervalStartSorter {
public:
    bool operator() (const Interval<T,K>& a, const Interval<T,K>& b) {
        return a.start < b.start;
    }
};

template <class T, typename K = std::size_t>
class IntervalTree {

public:
    typedef Interval<T,K> interval;
    typedef std::vector<interval> intervalVector;
    typedef IntervalTree<T,K> intervalTree;

    intervalVector intervals;
    std::unique_ptr<intervalTree> left;
    std::unique_ptr<intervalTree> right;
    K center;

    RT::RangeTree<K,std::vector<interval*> >* rtree;

    IntervalTree<T,K>(void)
        : left(nullptr)
        , right(nullptr)
        , center(0)
    { }

private:
    std::unique_ptr<intervalTree> copyTree(const intervalTree& orig){
        return std::unique_ptr<intervalTree>(new intervalTree(orig));
    }
public:

    IntervalTree<T,K>(const intervalTree& other)
    :   intervals(other.intervals),
        left(other.left ? copyTree(*other.left) : nullptr),
        right(other.right ? copyTree(*other.right) : nullptr),
        center(other.center)
    {
    }

public:

    IntervalTree<T,K>& operator=(const intervalTree& other) {
        center = other.center;
        intervals = other.intervals;
        left = other.left ? copyTree(*other.left) : nullptr;
        right = other.right ? copyTree(*other.right) : nullptr;
        return *this;
    }

    // Note: changes the order of ivals
    IntervalTree<T,K>(
            intervalVector& ivals,
            std::size_t depth = 16,
            std::size_t minbucket = 64,
            K leftextent = 0,
            K rightextent = 0,
            std::size_t maxbucket = 512
            )
        : left(nullptr)
        , right(nullptr)
    {

        --depth;
        IntervalStartSorter<T,K> intervalStartSorter;
        if (depth == 0 || (ivals.size() < minbucket && ivals.size() < maxbucket)) {
            std::sort(ivals.begin(), ivals.end(), intervalStartSorter);
            intervals = ivals;
        } else {
            if (leftextent == 0 && rightextent == 0) {
                // sort intervals by start
              std::sort(ivals.begin(), ivals.end(), intervalStartSorter);
            }

            /********************/
            /**** RANGE-TREE ****/
            /********************/

            std::vector<RT::Point<K,std::vector<interval*> > > points = {};

            auto f = [](K a) { std::vector<K> b = {a}; return b;};

            std::map< K, std::vector<interval*> > m;

            for (auto i = ivals.begin(); i != ivals.end(); ++i)
            {
                const interval& tmp_i = *i;
                m[tmp_i.start].push_back(&(*i));
                m[tmp_i.stop].push_back(&(*i));
            }

            for (auto i = m.begin(); i != m.end(); ++i)
            {
                const std::pair<K,std::vector<interval*> >& tmp = *i;
                RT::Point<K,std::vector<interval*> > tmp_pL(f(tmp.first), tmp.second);
                points.push_back(tmp_pL);
            }
            rtree = new RT::RangeTree<K,std::vector<interval*> >(points);
            /********************/
            /********************/
            /********************/

            K leftp = 0;
            K rightp = 0;
            K centerp = 0;

            if (leftextent || rightextent) {
                leftp = leftextent;
                rightp = rightextent;
            } else {
                leftp = ivals.front().start;
                std::vector<K> stops;
                stops.resize(ivals.size());
                transform(ivals.begin(), ivals.end(), stops.begin(), intervalStop<T,K>);
                rightp = *max_element(stops.begin(), stops.end());
            }

            //centerp = ( leftp + rightp ) / 2;
            centerp = ivals.at(ivals.size() / 2).start;
            center = centerp;

            intervalVector lefts;
            intervalVector rights;

            for (typename intervalVector::const_iterator i = ivals.begin(); i != ivals.end(); ++i) {
                const interval& interval = *i;
                if (interval.stop < center) {
                    lefts.push_back(interval);
                } else if (interval.start > center) {
                    rights.push_back(interval);
                } else {
                    intervals.push_back(interval);
                }
            }

            if (!lefts.empty()) {
                left = std::unique_ptr<intervalTree>(new intervalTree(lefts, depth, minbucket, leftp, centerp));
            }
            if (!rights.empty()) {
                right = std::unique_ptr<intervalTree>(new intervalTree(rights, depth, minbucket, centerp, rightp));
            }
        }
    }

    intervalVector findOverlapping(K start, K stop) const {
    	intervalVector ov;
        std::set<interval*> s;
        /********************/
        /********************/
        auto f = [](K a) { std::vector<K> b = {a}; return b;};
        auto g = [](bool a) { std::vector<bool> b = {a}; return b;};
        std::vector<RT::Point<K,std::vector<interval*> > > pp = rtree->pointsInRange(f(start), f(stop), g(true), g(true));
        for (auto i = pp.begin(); i != pp.end(); ++i)
        {
            const RT::Point<K,std::vector<interval*> >& tmp = *i;
            std::vector<interval*> ins = i->value();
            for (int i = 0; i < ins.size(); ++i)
            {
                s.insert(ins[i]);
            }
        }
        /********************/
        /********************/

    	this->findOverlapping(start, stop, ov);

        /********************/
        for (auto i = ov.begin(); i != ov.end(); ++i)
        {
            s.insert(&(*i));
        }
        intervalVector toReturn;
        for (auto i = s.begin(); i != s.end(); ++i)
        {
            toReturn.push_back(*(*i));
        }
        /********************/

    	return toReturn;
    }

    void findOverlapping(K start, K stop, intervalVector& overlapping) const {

        if (!intervals.empty() && ! (stop < intervals.front().start)) {
            for (typename intervalVector::const_iterator i = intervals.begin(); i != intervals.end(); ++i) {
                const interval& interval = *i;
                if (interval.stop >= start && interval.start <= stop) {
                    overlapping.push_back(interval);
                }
            }
        }

        if (left && start <= center) {
            left->findOverlapping(start, stop, overlapping);
        }

        if (right && stop >= center) {
            right->findOverlapping(start, stop, overlapping);
        }

    }

    intervalVector findContained(K start, K stop) const {
	intervalVector contained;
	this->findContained(start, stop, contained);
	return contained;
    }

    void findContained(K start, K stop, intervalVector& contained) const {
        if (!intervals.empty() && ! (stop < intervals.front().start)) {
            for (typename intervalVector::const_iterator i = intervals.begin(); i != intervals.end(); ++i) {
                const interval& interval = *i;
                if (interval.start >= start && interval.stop <= stop) {
                    contained.push_back(interval);
                }
            }
        }

        if (left && start <= center) {
            left->findContained(start, stop, contained);
        }

        if (right && stop >= center) {
            right->findContained(start, stop, contained);
        }

    }

    ~IntervalTree(void) = default;

};

#endif
