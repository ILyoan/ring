#ifndef RING_RINGC_UTIL_TABLE_MAP_H
#define RING_RINGC_UTIL_TABLE_MAP_H


#include <vector>
#include <map>
using namespace std;


namespace ring {
namespace ringc {


// TableMap is a data structure that manage a table from numberical id to template object and
// vice versa.
template <typename ID, typename T>
class TableMap {
public:
	TableMap(T first);
	virtual ~TableMap();

	// Returns the number of elements on this table map.
	int length() const;

	// Adds a value to the table and returns its id.
	// If the value already exists, returns the id.
	ID add(const T & v);

	// Returns ture if the table contains the value.
	bool contains(const T & v) const;

	// Returns id for the value, -1 if there is not the value.
	ID id(const T & v) const;

	// Returns value for the id, or first element if there is no such id.
	const T & value(ID id) const;

	void printEntries(FILE* fd);

	typedef typename std::vector<T>::iterator it_type;
	it_type begin() { return _id_to_value.begin(); }
	it_type end() { return _id_to_value.end(); }

protected:
	vector<T> _id_to_value;
	map<T, ID> _value_to_id;
};


template <typename ID, typename T>
TableMap<ID, T>::TableMap(T first) {
	add(first);
}


template <typename ID, typename T>
TableMap<ID, T>::~TableMap() {
}


template <typename ID, typename T>
int TableMap<ID, T>::length() const {
	return _id_to_value.size();
}


template <typename ID, typename T>
ID TableMap<ID, T>::add(const T & v) {
	ID res = id(v);
	if (res.none()) {
		_id_to_value.push_back(v);
		res = _value_to_id[v] = ID(length() - 1);
	}
	return res;
}


template <typename ID, typename T>
bool TableMap<ID, T>::contains(const T & v) const {
	return _value_to_id.count(v) > 0;
}


template <typename ID, typename T>
ID TableMap<ID, T>::id(const T & v) const {
	auto it = _value_to_id.find(v);
	if (it == _value_to_id.end()) {
		return ID();
	} else {
		return it->second;
	}
}


template <typename ID, typename T>
const T & TableMap<ID, T>::value(ID id) const {
	if (id.value() < 0 || id.value() >= length()) {
		return _id_to_value[0];
	} else {
		return _id_to_value[id.value()];
	}
}


} // namespace ringc
} // namespace ring


#endif