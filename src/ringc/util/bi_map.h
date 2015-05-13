#ifndef RING_RINGC_UTIL_BI_MAP_H
#define RING_RINGC_UTIL_BI_MAP_H


#include <map>
#include <vector>
using namespace std;


namespace ring {
namespace ringc {


// BiMap is a data structure that manage a map from an object of type L to an object of type R
// and vice versa.
template <typename L, typename R>
class BiMap {
public:
	typedef typename std::map<L, R> LMap;
	typedef typename std::map<R, L> RMap;

	// Initialize container with default values for L and R.
	BiMap(L default_l, R default_r);
	virtual ~BiMap();

	// Add new entry, returns true if the new entry successfully added. or return false if there
	// were already the a entry that conatains one of each `lv` or `rv` in the map.
	bool add(L lv, R rv);

	// Check whether there is a mapping from `v` of type L to somthing.
	bool containsL(L v) const;

	// Check whether there is a mapping from `v` of type R to somthing.
	bool containsR(R v) const;

	// Returns mapped valued of type R by `v` of type L.
	// If there is no such mapping, returns default value for L.
	R fromL(L v);

	// Returns mapped valued of type L by `v` of type R.
	// If there is no such mapping, returns default value for R.
	L fromR(R v);

	// Returns the list that represents mapping from L to R.
	vector<pair<L, R> > toListL() const;

	// Returns the list that represents mapping form R to L.
	vector<pair<R, L> > toListR() const;

protected:
	 typedef typename LMap::const_iterator CLIterator;
	 typedef typename RMap::const_iterator CRIterator;

	 CLIterator beginL() const { return _l_to_r.begin(); }
	 CLIterator endL()   const { return _l_to_r.end(); }

	 CRIterator beginR() const { return _r_to_l.begin(); }
	 CRIterator endR()   const { return _r_to_l.end(); }

protected:
	LMap _l_to_r;
	RMap _r_to_l;
	L _default_l;
	R _default_r;
};


template <typename L, typename R>
BiMap<L, R>::BiMap(L default_l, R default_r)
		: _default_l(default_l)
		, _default_r(default_r) {
}


template <typename L, typename R>
BiMap<L, R>::~BiMap() {
}


template <typename L, typename R>
bool BiMap<L, R>::add(L lv, R rv) {
	if (containsL(lv) || containsR(rv)) return false;
	_l_to_r[lv] = rv;
	_r_to_l[rv] = lv;
	return true;
}


template <typename L, typename R>
bool BiMap<L, R>::containsL(L v) const {
	return _l_to_r.count(v) > 0;
}


template <typename L, typename R>
bool BiMap<L, R>::containsR(R v) const {
	return _r_to_l.count(v) > 0;
}


template <typename L, typename R>
R BiMap<L, R>::fromL(L v) {
	if (containsL(v)) {
		return _l_to_r[v];
	} else {
		return _default_r;
	}
}


template <typename L, typename R>
L BiMap<L, R>::fromR(R v) {
	if (containsR(v)) {
		return _r_to_l[v];
	} else {
		return _default_l;
	}
}


template <typename L, typename R>
vector<pair<L, R> > BiMap<L, R>::toListL() const {
	vector<pair<L, R> > res;
	for (auto it = beginL(); it != endL(); ++it) {
		res.push_back(*it);
	}
	return res;
}

#include <cstdio>
template <typename L, typename R>
vector<pair<R, L> > BiMap<L, R>::toListR() const {
	vector<pair<R, L> > res;
	for (auto it = beginR(); it != endR(); ++it) {
		res.push_back(*it);
	}
	return res;
}


} // namespace ringc
} // namespace ring


#endif
